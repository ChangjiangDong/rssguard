// This file is part of RSS Guard.
//
// Copyright (C) 2011-2015 by Martin Rotter <rotter.martinos@gmail.com>
// Copyright (C) 2010-2014 by David Rosca <nowrep@gmail.com>
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

#ifndef ADBLOCKDIALOG_H
#define ADBLOCKDIALOG_H

#include <QDialog>

#include "ui_adblockdialog.h"


namespace Ui {
  class AdBlockDialog;
}

class AdBlockSubscription;
class AdBlockTreeWidget;
class AdBlockManager;
class AdBlockRule;

class AdBlockDialog : public QDialog {
    Q_OBJECT

  public:
    // Constructors.
    explicit AdBlockDialog(QWidget *parent = 0);
    virtual ~AdBlockDialog();

    void showRule(const AdBlockRule *rule) const;

  private slots:
    void addRule();
    void removeRule();

    void addSubscription();
    void removeSubscription();

    void currentChanged(int index);
    void filterString(const QString &string);
    void enableAdBlock(bool state);

    void aboutToShowMenu();
    void learnAboutRules();

    void loadSubscriptions();
    void load();

  protected:
    void closeEvent(QCloseEvent *event);

  private:
    void setupMenu();
    void createConnections();

    Ui::AdBlockDialog *m_ui;
    AdBlockManager* m_manager;
    AdBlockTreeWidget* m_currentTreeWidget;
    AdBlockSubscription* m_currentSubscription;

    QAction* m_actionAddRule;
    QAction* m_actionRemoveRule;
    QAction* m_actionAddSubscription;
    QAction* m_actionRemoveSubscription;

    bool m_loaded;
    bool m_useLimitedEasyList;
};

#endif // ADBLOCKDIALOG_H
