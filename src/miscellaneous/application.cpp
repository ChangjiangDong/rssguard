// This file is part of RSS Guard.
//
// Copyright (C) 2011-2017 by Martin Rotter <rotter.martinos@gmail.com>
//
// RSS Guard is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// RSS Guard is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with RSS Guard. If not, see <http://www.gnu.org/licenses/>.

#include "miscellaneous/application.h"

#include "miscellaneous/iconfactory.h"
#include "miscellaneous/iofactory.h"
#include "miscellaneous/mutex.h"
#include "miscellaneous/feedreader.h"
#include "gui/feedsview.h"
#include "gui/feedmessageviewer.h"
#include "gui/messagebox.h"
#include "gui/statusbar.h"
#include "gui/dialogs/formmain.h"
#include "exceptions/applicationexception.h"

#include "services/abstract/serviceroot.h"
#include "services/standard/standardserviceroot.h"
#include "services/standard/standardserviceentrypoint.h"
#include "services/tt-rss/ttrssserviceentrypoint.h"
#include "services/owncloud/owncloudserviceentrypoint.h"
#include "network-web/webfactory.h"

#include <QSessionManager>
#include <QProcess>

#if defined(USE_WEBENGINE)
#include "network-web/urlinterceptor.h"
#include "network-web/networkurlinterceptor.h"
#include "network-web/adblock/adblockicon.h"
#include "network-web/adblock/adblockmanager.h"
#include "network-web/rssguardschemehandler.h"

#include <QWebEngineProfile>
#include <QWebEngineDownloadItem>
#include <QWebEngineScript>
#include <QWebEngineScriptCollection>
#endif

Application::Application(const QString& id, int& argc, char** argv)
	: QtSingleApplication(id, argc, argv),

#if defined(USE_WEBENGINE)
	  m_urlInterceptor(new NetworkUrlInterceptor(this)),
#endif

	  m_feedReader(nullptr),
    m_updateFeedsLock(new Mutex()), m_userActions(QList<QAction*>()), m_mainForm(nullptr),
    m_trayIcon(nullptr), m_settings(Settings::setupSettings(this)), m_webFactory(new WebFactory(this)),
    m_system(new SystemFactory(this)), m_skins(new SkinFactory(this)),
    m_localization(new Localization(this)), m_icons(new IconFactory(this)),
    m_database(new DatabaseFactory(this)), m_downloadManager(nullptr), m_shouldRestart(false) {
	connect(this, &Application::aboutToQuit, this, &Application::onAboutToQuit);
	connect(this, &Application::commitDataRequest, this, &Application::onCommitData);
	connect(this, &Application::saveStateRequest, this, &Application::onSaveState);

#if defined(USE_WEBENGINE)
	connect(QWebEngineProfile::defaultProfile(), &QWebEngineProfile::downloadRequested, this, &Application::downloadRequested);

	QWebEngineProfile::defaultProfile()->setRequestInterceptor(m_urlInterceptor);
	m_urlInterceptor->loadSettings();
  QWebEngineProfile::defaultProfile()->installUrlSchemeHandler(
      QByteArray(APP_LOW_NAME),
      new RssGuardSchemeHandler(QWebEngineProfile::defaultProfile()));
#endif
}

Application::~Application() {
	qDebug("Destroying Application instance.");
}

FeedReader* Application::feedReader() {
	return m_feedReader;
}

QList<QAction*> Application::userActions() {
	if (m_mainForm != nullptr && m_userActions.isEmpty()) {
		m_userActions = m_mainForm->allActions();

#if defined(USE_WEBENGINE)
		m_userActions.append(AdBlockManager::instance()->adBlockIcon());
#endif
	}

	return m_userActions;
}

bool Application::isFirstRun() {
	return settings()->value(GROUP(General), SETTING(General::FirstRun)).toBool();
}

bool Application::isFirstRun(const QString& version) {
	if (version == APP_VERSION) {
		// Check this only if checked version is equal to actual version.
		return settings()->value(GROUP(General), QString(General::FirstRun) + QL1C('_') + version, true).toBool();
	}
	else {
		return false;
	}
}

WebFactory* Application::web() {
	return m_webFactory;
}

SystemFactory* Application::system() {
	return m_system;
}

SkinFactory* Application::skins() {
	return m_skins;
}

Localization* Application::localization() {
	return m_localization;
}

DatabaseFactory* Application::database() {
	return m_database;
}

void Application::eliminateFirstRun() {
	settings()->setValue(GROUP(General), General::FirstRun, false);
}

void Application::eliminateFirstRun(const QString& version) {
	settings()->setValue(GROUP(General), QString(General::FirstRun) + QL1C('_') + version, false);
}

void Application::setFeedReader(FeedReader* feed_reader) {
	m_feedReader = feed_reader;
	connect(m_feedReader, &FeedReader::feedUpdatesStarted, this, &Application::onFeedUpdatesStarted);
	connect(m_feedReader, &FeedReader::feedUpdatesProgress, this, &Application::onFeedUpdatesProgress);
	connect(m_feedReader, &FeedReader::feedUpdatesFinished, this, &Application::onFeedUpdatesFinished);
}

IconFactory* Application::icons() {
	return m_icons;
}

DownloadManager* Application::downloadManager() {
	if (m_downloadManager == nullptr) {
		m_downloadManager = new DownloadManager();
		connect(m_downloadManager, &DownloadManager::downloadFinished, mainForm()->statusBar(), &StatusBar::clearProgressDownload);
		connect(m_downloadManager, &DownloadManager::downloadProgressed, mainForm()->statusBar(), &StatusBar::showProgressDownload);
	}

	return m_downloadManager;
}

Settings* Application::settings() {
	return m_settings;
}

Mutex* Application::feedUpdateLock() {
	return m_updateFeedsLock.data();
}

FormMain* Application::mainForm() {
	return m_mainForm;
}

QWidget* Application::mainFormWidget() {
	return m_mainForm;
}

void Application::setMainForm(FormMain* main_form) {
	m_mainForm = main_form;
}

QString Application::configFolder() {
	return IOFactory::getSystemFolder(QStandardPaths::GenericConfigLocation);
}

QString Application::userDataAppFolder() {
	// In "app" folder, we would like to separate all user data into own subfolder,
	// therefore stick to "data" folder in this mode.
	return applicationDirPath() + QDir::separator() + QSL("data");
}

QString Application::userDataFolder() {
	if (settings()->type() == SettingsProperties::Portable) {
    return userDataAppFolder();
	}
	else {
    return userDataHomeFolder();
	}
}

QString Application::userDataHomeFolder() {
	// Fallback folder.
  const QString home_folder = homeFolder() + QDir::separator() + QSL(APP_LOW_H_NAME) + QDir::separator() + QSL("data");

	if (QDir().exists(home_folder)) {
		return home_folder;
	}
	else {
    return configFolder() + QDir::separator() + QSL(APP_NAME);
	}
}

QString Application::tempFolder() {
	return IOFactory::getSystemFolder(QStandardPaths::TempLocation);
}

QString Application::documentsFolder() {
	return IOFactory::getSystemFolder(QStandardPaths::DocumentsLocation);
}

QString Application::homeFolder() {
	return IOFactory::getSystemFolder(QStandardPaths::HomeLocation);
}

void Application::backupDatabaseSettings(bool backup_database, bool backup_settings,
                                         const QString& target_path, const QString& backup_name) {
	if (!QFileInfo(target_path).isWritable()) {
		throw ApplicationException(tr("Output directory is not writable."));
	}

	if (backup_settings) {
		settings()->sync();

		if (!IOFactory::copyFile(settings()->fileName(), target_path + QDir::separator() + backup_name + BACKUP_SUFFIX_SETTINGS)) {
			throw ApplicationException(tr("Settings file not copied to output directory successfully."));
		}
	}

	if (backup_database &&
	        (database()->activeDatabaseDriver() == DatabaseFactory::SQLITE ||
	         database()->activeDatabaseDriver() == DatabaseFactory::SQLITE_MEMORY)) {
		// We need to save the database first.
		database()->saveDatabase();

		if (!IOFactory::copyFile(database()->sqliteDatabaseFilePath(), target_path + QDir::separator() + backup_name + BACKUP_SUFFIX_DATABASE)) {
			throw ApplicationException(tr("Database file not copied to output directory successfully."));
		}
	}
}

void Application::restoreDatabaseSettings(bool restore_database, bool restore_settings,
                                          const QString& source_database_file_path, const QString& source_settings_file_path) {
	if (restore_database) {
		if (!qApp->database()->initiateRestoration(source_database_file_path)) {
			throw ApplicationException(tr("Database restoration was not initiated. Make sure that output directory is writable."));
		}
	}

	if (restore_settings) {
		if (!qApp->settings()->initiateRestoration(source_settings_file_path)) {
			throw ApplicationException(tr("Settings restoration was not initiated. Make sure that output directory is writable."));
		}
	}
}

void Application::processExecutionMessage(const QString& message) {
	qDebug("Received '%s' execution message from another application instance.", qPrintable(message));
	const QStringList messages = message.split(ARGUMENTS_LIST_SEPARATOR);

	if (messages.contains(APP_QUIT_INSTANCE)) {
		quit();
	}
	else {
		foreach (const QString& msg, messages) {
			if (msg == APP_IS_RUNNING) {
				showGuiMessage(APP_NAME, tr("Application is already running."), QSystemTrayIcon::Information);
				mainForm()->display();
			}
			else if (msg.startsWith(QL1S(URI_SCHEME_FEED_SHORT))) {
				// Application was running, and someone wants to add new feed.
				StandardServiceRoot* root = qApp->feedReader()->feedsModel()->standardServiceRoot();

				if (root != nullptr) {
					root->checkArgumentForFeedAdding(msg);
				}
				else {
					showGuiMessage(tr("Cannot add feed"),
					               tr("Feed cannot be added because standard RSS/ATOM account is not enabled."),
					               QSystemTrayIcon::Warning, qApp->mainForm(),
					               true);
				}
			}
		}
	}
}

SystemTrayIcon* Application::trayIcon() {
	if (m_trayIcon == nullptr) {
		m_trayIcon = new SystemTrayIcon(APP_ICON_PATH, APP_ICON_PLAIN_PATH, m_mainForm);
		connect(m_trayIcon, &SystemTrayIcon::shown, m_feedReader->feedsModel(), &FeedsModel::notifyWithCounts);
		connect(m_feedReader->feedsModel(), &FeedsModel::messageCountsChanged, m_trayIcon, &SystemTrayIcon::setNumber);
	}

	return m_trayIcon;
}

#if defined(USE_WEBENGINE)
NetworkUrlInterceptor* Application::urlIinterceptor() {
	return m_urlInterceptor;
}
#endif

void Application::showTrayIcon() {
	qDebug("Showing tray icon.");
	trayIcon()->show();
}

void Application::deleteTrayIcon() {
	if (m_trayIcon != nullptr) {
		qDebug("Disabling tray icon, deleting it and raising main application window.");
		m_mainForm->display();
		delete m_trayIcon;
		m_trayIcon = nullptr;
		// Make sure that application quits when last window is closed.
		setQuitOnLastWindowClosed(true);
	}
}

void Application::showGuiMessage(const QString& title, const QString& message,
                                 QSystemTrayIcon::MessageIcon message_type, QWidget* parent,
                                 bool show_at_least_msgbox, std::function<void()> functor) {
	if (SystemTrayIcon::areNotificationsEnabled() && SystemTrayIcon::isSystemTrayActivated()) {
		trayIcon()->showMessage(title, message, message_type, TRAY_ICON_BUBBLE_TIMEOUT, functor);
	}
	else if (show_at_least_msgbox) {
		// Tray icon or OSD is not available, display simple text box.
		MessageBox::show(parent, (QMessageBox::Icon) message_type, title, message);
	}
	else {
		qDebug("Silencing GUI message: '%s'.", qPrintable(message));
	}
}

void Application::onCommitData(QSessionManager& manager) {
	qDebug("OS asked application to commit its data.");
	manager.setRestartHint(QSessionManager::RestartNever);
	manager.release();
}

void Application::onSaveState(QSessionManager& manager) {
	qDebug("OS asked application to save its state.");
	manager.setRestartHint(QSessionManager::RestartNever);
	manager.release();
}

void Application::onAboutToQuit() {
	eliminateFirstRun();
	eliminateFirstRun(APP_VERSION);
#if defined(USE_WEBENGINE)
	AdBlockManager::instance()->save();
#endif
	// Make sure that we obtain close lock BEFORE even trying to quit the application.
	const bool locked_safely = feedUpdateLock()->tryLock(4 * CLOSE_LOCK_TIMEOUT);
	processEvents();
	qDebug("Cleaning up resources and saving application state.");
#if defined(Q_OS_WIN)
	system()->removeTrolltechJunkRegistryKeys();
#endif
	qApp->feedReader()->quit();
	database()->saveDatabase();

	if (mainForm() != nullptr) {
		mainForm()->saveSize();
	}

	if (locked_safely) {
		// Application obtained permission to close in a safe way.
		qDebug("Close lock was obtained safely.");
		// We locked the lock to exit peacefully, unlock it to avoid warnings.
		feedUpdateLock()->unlock();
	}
	else {
		// Request for write lock timed-out. This means
		// that some critical action can be processed right now.
		qDebug("Close lock timed-out.");
	}

	// Now, we can check if application should just quit or restart itself.
	if (m_shouldRestart) {
		finish();
		qDebug("Killing local peer connection to allow another instance to start.");

		// TODO: Start RSS Guard with sleep before it cross-platform way if possible.
		// sleep 5 && "<rssguard-start>".
		if (QProcess::startDetached(QString("\"") + QDir::toNativeSeparators(applicationFilePath()) + QString("\""))) {
			qDebug("New application instance was started.");
		}
		else {
			qWarning("New application instance was not started successfully.");
		}
	}
}

void Application::restart() {
	m_shouldRestart = true;
	quit();
}

#if defined(USE_WEBENGINE)
void Application::downloadRequested(QWebEngineDownloadItem* download_item) {
	downloadManager()->download(download_item->url());
	download_item->cancel();
	download_item->deleteLater();
}
#endif

void Application::onFeedUpdatesStarted() {
}

void Application::onFeedUpdatesProgress(const Feed* feed, int current, int total) {
	Q_UNUSED(feed)
	Q_UNUSED(current)
	Q_UNUSED(total)
}

void Application::onFeedUpdatesFinished(FeedDownloadResults results) {
	if (!results.updatedFeeds().isEmpty()) {
		// Now, inform about results via GUI message/notification.
		qApp->showGuiMessage(tr("New messages downloaded"), results.overview(10), QSystemTrayIcon::NoIcon, 0, false);
	}
}
