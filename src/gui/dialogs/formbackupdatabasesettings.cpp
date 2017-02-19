// This file is part of RSS Guard.
//
// Copyright (C) 2011-2016 by Martin Rotter <rotter.martinos@gmail.com>
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

#include "gui/dialogs/formbackupdatabasesettings.h"

#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "exceptions/applicationexception.h"

#include <QDialogButtonBox>
#include <QPushButton>
#include <QCheckBox>
#include <QFileDialog>
#include <QDateTime>


FormBackupDatabaseSettings::FormBackupDatabaseSettings(QWidget *parent) : QDialog(parent), m_ui(new Ui::FormBackupDatabaseSettings) {
  m_ui->setupUi(this);
  m_ui->m_txtBackupName->lineEdit()->setPlaceholderText(tr("Common name for backup files"));

  setWindowIcon(qApp->icons()->fromTheme(QSL("document-export")));
  setWindowFlags(Qt::MSWindowsFixedSizeDialogHint | Qt::Dialog | Qt::WindowSystemMenuHint);

  connect(m_ui->m_checkBackupDatabase, &QCheckBox::toggled, this, &FormBackupDatabaseSettings::checkOkButton);
  connect(m_ui->m_checkBackupSettings, &QCheckBox::toggled, this, &FormBackupDatabaseSettings::checkOkButton);
  connect(m_ui->m_buttonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, this, &FormBackupDatabaseSettings::performBackup);
  connect(m_ui->m_txtBackupName->lineEdit(), &BaseLineEdit::textChanged, this, &FormBackupDatabaseSettings::checkBackupNames);
  connect(m_ui->m_txtBackupName->lineEdit(), &BaseLineEdit::textChanged, this, &FormBackupDatabaseSettings::checkOkButton);
  connect(m_ui->m_btnSelectFolder, &QPushButton::clicked, this, &FormBackupDatabaseSettings::selectFolderInitial);

  selectFolder(qApp->getDocumentsFolderPath());
  m_ui->m_txtBackupName->lineEdit()->setText(QString(APP_LOW_NAME) + QL1S("_") + QDateTime::currentDateTime().toString(QSL("yyyyMMddHHmm")));
  m_ui->m_lblResult->setStatus(WidgetWithStatus::Warning, tr("No operation executed yet."), tr("No operation executed yet."));

  if (qApp->database()->activeDatabaseDriver() != DatabaseFactory::SQLITE &&
      qApp->database()->activeDatabaseDriver() != DatabaseFactory::SQLITE_MEMORY) {
    m_ui->m_checkBackupDatabase->setDisabled(true);
  }
}

FormBackupDatabaseSettings::~FormBackupDatabaseSettings() {
  qDebug("Destroying FormBackupDatabaseSettings instance.");
}

void FormBackupDatabaseSettings::performBackup() {
  try {
    qApp->backupDatabaseSettings(m_ui->m_checkBackupDatabase->isChecked(), m_ui->m_checkBackupSettings->isChecked(),
                                 m_ui->m_lblSelectFolder->label()->text(), m_ui->m_txtBackupName->lineEdit()->text());
    m_ui->m_lblResult->setStatus(WidgetWithStatus::Ok,
                                 tr("Backup was created successfully and stored in target directory."),
                                 tr("Backup was created successfully."));
  }
  catch (const ApplicationException &ex) {
    m_ui->m_lblResult->setStatus(WidgetWithStatus::Error, ex.message(), tr("Backup failed."));
  }
}

void FormBackupDatabaseSettings::selectFolderInitial() {
  selectFolder();
}

void FormBackupDatabaseSettings::selectFolder(QString path) {
  if (path.isEmpty()) {
    path = QFileDialog::getExistingDirectory(this, tr("Select destination directory"), m_ui->m_lblSelectFolder->label()->text());
  }

  if (!path.isEmpty()) {
    m_ui->m_lblSelectFolder->setStatus(WidgetWithStatus::Ok, QDir::toNativeSeparators(path),
                                       tr("Good destination directory is specified."));
  }
}

void FormBackupDatabaseSettings::checkBackupNames(const QString &name) {
  if (name.simplified().isEmpty()) {
    m_ui->m_txtBackupName->setStatus(WidgetWithStatus::Error, tr("Backup name cannot be empty."));
  }
  else {
    m_ui->m_txtBackupName->setStatus(WidgetWithStatus::Ok, tr("Backup name looks okay."));
  }
}

void FormBackupDatabaseSettings::checkOkButton() {
  m_ui->m_buttonBox->button(QDialogButtonBox::Ok)->setDisabled(m_ui->m_txtBackupName->lineEdit()->text().simplified().isEmpty() ||
                                                               m_ui->m_lblSelectFolder->label()->text().simplified().isEmpty() ||
                                                               (!m_ui->m_checkBackupDatabase->isChecked() &&
                                                                !m_ui->m_checkBackupSettings->isChecked()));
}
