// For license of this file, see <project-root-folder>/LICENSE.md.

#ifndef FORMADDEDITEMAIL_H
#define FORMADDEDITEMAIL_H

#include <QDialog>

#include "ui_formaddeditemail.h"

namespace Ui {
  class FormAddEditEmail;
}

class GmailServiceRoot;
class Message;
class EmailRecipientControl;

class FormAddEditEmail : public QDialog {
  Q_OBJECT

  public:
    explicit FormAddEditEmail(GmailServiceRoot* root, QWidget* parent = nullptr);

  public slots:
    void execForAdd();

  private slots:
    void removeRecipientRow();
    void onOkClicked();
    void addRecipientRow(const QString& recipient = QString());

  private:
    QList<EmailRecipientControl*> recipientControls() const;

  private:
    GmailServiceRoot* m_root;

    Ui::FormAddEditEmail m_ui;
    QList<EmailRecipientControl*> m_recipientControls;
    Message* m_originalMessage;
};

#endif // FORMADDEDITEMAIL_H
