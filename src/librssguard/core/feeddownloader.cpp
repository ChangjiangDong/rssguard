// For license of this file, see <project-root-folder>/LICENSE.md.

#include "core/feeddownloader.h"

#include "core/messagefilter.h"
#include "definitions/definitions.h"
#include "exceptions/filteringexception.h"
#include "miscellaneous/application.h"
#include "services/abstract/cacheforserviceroot.h"
#include "services/abstract/feed.h"

#include <QDebug>
#include <QJSEngine>
#include <QMutexLocker>
#include <QRegularExpression>
#include <QString>
#include <QThread>
#include <QUrl>

FeedDownloader::FeedDownloader()
  : QObject(), m_mutex(new QMutex()), m_feedsUpdated(0), m_feedsOriginalCount(0) {
  qRegisterMetaType<FeedDownloadResults>("FeedDownloadResults");
}

FeedDownloader::~FeedDownloader() {
  m_mutex->tryLock();
  m_mutex->unlock();
  delete m_mutex;
  qDebug("Destroying FeedDownloader instance.");
}

bool FeedDownloader::isUpdateRunning() const {
  return !m_feeds.isEmpty();
}

void FeedDownloader::updateAvailableFeeds() {
  for (const Feed* feed : m_feeds) {
    auto* cache = dynamic_cast<CacheForServiceRoot*>(feed->getParentServiceRoot());

    if (cache != nullptr) {
      qDebug("Saving cache for feed with DB ID %d and title '%s'.", feed->id(), qPrintable(feed->title()));
      cache->saveAllCachedData(false);
    }
  }

  while (!m_feeds.isEmpty()) {
    updateOneFeed(m_feeds.takeFirst());
  }
}

void FeedDownloader::updateFeeds(const QList<Feed*>& feeds) {
  QMutexLocker locker(m_mutex);

  if (feeds.isEmpty()) {
    qDebug("No feeds to update in worker thread, aborting update.");
  }
  else {
    qDebug().nospace() << "Starting feed updates from worker in thread: \'" << QThread::currentThreadId() << "\'.";
    m_feeds = feeds;
    m_feedsOriginalCount = m_feeds.size();
    m_results.clear();
    m_feedsUpdated = 0;

    // Job starts now.
    emit updateStarted();

    updateAvailableFeeds();
  }

  finalizeUpdate();
}

void FeedDownloader::stopRunningUpdate() {
  m_feeds.clear();
  m_feedsOriginalCount = m_feedsUpdated = 0;
}

void FeedDownloader::updateOneFeed(Feed* feed) {
  qDebug().nospace() << "Downloading new messages for feed ID "
                     << feed->customId() << " URL: " << feed->url() << " title: " << feed->title() << " in thread: \'"
                     << QThread::currentThreadId() << "\'.";

  bool error_during_obtaining = false;
  QElapsedTimer tmr; tmr.start();
  QList<Message> msgs = feed->obtainNewMessages(&error_during_obtaining);

  qDebug().nospace() << "Downloaded " << msgs.size() << " messages for feed ID "
                     << feed->customId() << " URL: " << feed->url() << " title: " << feed->title() << " in thread: \'"
                     << QThread::currentThreadId() << "\'. Operation took " << tmr.nsecsElapsed() / 1000 << " microseconds.";

  // Now, sanitize messages (tweak encoding etc.).
  for (auto& msg : msgs) {
    // Also, make sure that HTML encoding, encoding of special characters, etc., is fixed.
    msg.m_contents = QUrl::fromPercentEncoding(msg.m_contents.toUtf8());
    msg.m_author = msg.m_author.toUtf8();

    // Sanitize title. Remove newlines etc.
    msg.m_title = QUrl::fromPercentEncoding(msg.m_title.toUtf8())

                  // Replace all continuous white space.
                  .replace(QRegularExpression(QSL("[\\s]{2,}")), QSL(" "))

                  // Remove all newlines and leading white space.
                  .remove(QRegularExpression(QSL("([\\n\\r])|(^\\s)")));
  }

  if (!feed->messageFilters().isEmpty()) {
    tmr.restart();

    bool is_main_thread = QThread::currentThread() == qApp->thread();
    QSqlDatabase database = is_main_thread ?
                            qApp->database()->connection(metaObject()->className()) :
                            qApp->database()->connection(QSL("feed_upd"));

    // Perform per-message filtering.
    QJSEngine filter_engine;

    // Create JavaScript communication wrapper for the message.
    MessageObject msg_obj(&database, feed->customId(), feed->getParentServiceRoot()->accountId());

    // Register the wrapper.
    auto js_object = filter_engine.newQObject(&msg_obj);

    filter_engine.installExtensions(QJSEngine::Extension::ConsoleExtension);
    filter_engine.globalObject().setProperty("msg", js_object);

    qDebug().nospace() << "Setting up JS evaluation took " << tmr.nsecsElapsed() / 1000 << " microseconds.";

    for (int i = 0; i < msgs.size(); i++) {
      tmr.restart();

      // Attach live message object to wrapper.
      msg_obj.setMessage(&msgs[i]);
      qDebug().nospace() << "Hooking message took " << tmr.nsecsElapsed() / 1000 << " microseconds.";

      auto feed_filters = feed->messageFilters();

      for (int j = 0; j < feed_filters.size(); j++) {
        QPointer<MessageFilter> filter = feed_filters.at(j);

        if (filter.isNull()) {
          qWarning("Message filter was probably deleted, removing its pointer from list of filters.");
          feed_filters.removeAt(j--);
          continue;
        }

        MessageFilter* msg_filter = filter.data();

        tmr.restart();

        try {
          FilteringAction decision = msg_filter->filterMessage(&filter_engine);

          qDebug().nospace() << "Running filter script, it took " << tmr.nsecsElapsed() / 1000 << " microseconds.";

          switch (decision) {
            case FilteringAction::Accept:

              // Message is normally accepted, it could be tweaked by the filter.
              continue;

            case FilteringAction::Ignore:

              // Remove the message, we do not want it.
              msgs.removeAt(i--);
              break;
          }
        }
        catch (const FilteringException& ex) {
          qCritical("Error when evaluating filtering JS function: '%s'. Accepting message.", qPrintable(ex.message()));
          continue;
        }

        // If we reach this point. Then we ignore the message which is by now
        // already removed, go to next message.
        break;
      }
    }
  }

  m_feedsUpdated++;

  // Now make sure, that messages are actually stored to SQL in a locked state.
  qDebug().nospace() << "Saving messages of feed ID "
                     << feed->customId() << " URL: " << feed->url() << " title: " << feed->title() << " in thread: \'"
                     << QThread::currentThreadId() << "\'.";

  int updated_messages = feed->updateMessages(msgs, error_during_obtaining);

  qDebug("%d messages for feed %s stored in DB.", updated_messages, qPrintable(feed->customId()));

  if (updated_messages > 0) {
    m_results.appendUpdatedFeed(QPair<QString, int>(feed->title(), updated_messages));
  }

  qDebug("Made progress in feed updates, total feeds count %d/%d (id of feed is %d).", m_feedsUpdated, m_feedsOriginalCount, feed->id());
  emit updateProgress(feed, m_feedsUpdated, m_feedsOriginalCount);
}

void FeedDownloader::finalizeUpdate() {
  qDebug().nospace() << "Finished feed updates in thread: \'" << QThread::currentThreadId() << "\'.";
  m_results.sort();

  // Update of feeds has finished.
  // NOTE: This means that now "update lock" can be unlocked
  // and feeds can be added/edited/deleted and application
  // can eventually quit.
  emit updateFinished(m_results);
}

QString FeedDownloadResults::overview(int how_many_feeds) const {
  QStringList result;

  for (int i = 0, number_items_output = qMin(how_many_feeds, m_updatedFeeds.size()); i < number_items_output; i++) {
    result.append(m_updatedFeeds.at(i).first + QSL(": ") + QString::number(m_updatedFeeds.at(i).second));
  }

  QString res_str = result.join(QSL("\n"));

  if (m_updatedFeeds.size() > how_many_feeds) {
    res_str += QObject::tr("\n\n+ %n other feeds.", nullptr, m_updatedFeeds.size() - how_many_feeds);
  }

  return res_str;
}

void FeedDownloadResults::appendUpdatedFeed(const QPair<QString, int>& feed) {
  m_updatedFeeds.append(feed);
}

void FeedDownloadResults::sort() {
  std::sort(m_updatedFeeds.begin(), m_updatedFeeds.end(), [](const QPair<QString, int>& lhs, const QPair<QString, int>& rhs) {
    return lhs.second > rhs.second;
  });
}

void FeedDownloadResults::clear() {
  m_updatedFeeds.clear();
}

QList<QPair<QString, int>> FeedDownloadResults::updatedFeeds() const {
  return m_updatedFeeds;
}
