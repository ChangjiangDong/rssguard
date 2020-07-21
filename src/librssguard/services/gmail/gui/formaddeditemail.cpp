// For license of this file, see <project-root-folder>/LICENSE.md.

#include "services/gmail/gui/formaddeditemail.h"

#include "3rd-party/mimesis/mimesis.hpp"
#include "exceptions/applicationexception.h"
#include "gui/guiutilities.h"
#include "gui/messagebox.h"
#include "miscellaneous/application.h"
#include "miscellaneous/iconfactory.h"
#include "services/gmail/gmailserviceroot.h"
#include "services/gmail/gui/emailrecipientcontrol.h"
#include "services/gmail/network/gmailnetworkfactory.h"

FormAddEditEmail::FormAddEditEmail(GmailServiceRoot* root, QWidget* parent) : QDialog(parent), m_root(root) {
  m_ui.setupUi(this);

  GuiUtilities::applyDialogProperties(*this, qApp->icons()->fromTheme(QSL("mail-message-new")));

  m_ui.m_layoutAdder->setMargin(0);
  m_ui.m_layoutAdder->setContentsMargins(0, 0, 0, 0);

  m_ui.m_btnAdder->setIcon(qApp->icons()->fromTheme(QSL("list-add")));
  m_ui.m_btnAdder->setToolTip(tr("Add new recipient."));

  connect(m_ui.m_btnAdder, &PlainToolButton::clicked, this, [=]() {
    addRecipientRow();
  });

  connect(m_ui.m_buttonBox->button(QDialogButtonBox::StandardButton::Ok),
          &QPushButton::clicked,
          this,
          &FormAddEditEmail::onOkClicked);
}

void FormAddEditEmail::execForAdd() {
  addRecipientRow();
  exec();
}

void FormAddEditEmail::removeRecipientRow() {
  auto* sndr = static_cast<EmailRecipientControl*>(sender());

  m_ui.m_layout->takeRow(sndr);
  m_recipientControls.removeOne(sndr);

  sndr->deleteLater();
}

void FormAddEditEmail::onOkClicked() {
  Mimesis::Message msg;
  auto username = m_root->network()->username();

  if (!username.endsWith(QSL("@gmail.com"))) {
    username += QSL("@gmail.com");
  }

  msg["From"] = username.toStdString();

  auto recipients = recipientControls();
  QStringList rec_to, rec_cc, rec_bcc, rec_repl;

  for (EmailRecipientControl* ctrl : recipients) {
    switch (ctrl->recipientType()) {
      case RecipientType::Cc:
        rec_cc << ctrl->recipientAddress();
        break;

      case RecipientType::To:
        rec_to << ctrl->recipientAddress();
        break;

      case RecipientType::Bcc:
        rec_bcc << ctrl->recipientAddress();
        break;

      case RecipientType::ReplyTo:
        rec_repl << ctrl->recipientAddress();
        break;
    }
  }

  if (!rec_cc.isEmpty()) {
    msg["Cc"] = rec_cc.join(',').toStdString();
  }

  if (!rec_to.isEmpty()) {
    msg["To"] = rec_to.join(',').toStdString();
  }

  if (!rec_bcc.isEmpty()) {
    msg["Bcc"] = rec_bcc.join(',').toStdString();
  }

  if (!rec_repl.isEmpty()) {
    msg["Reply-To"] = rec_repl.join(',').toStdString();
  }

  msg["Subject"] = m_ui.m_txtSubject->text().toStdString();
  msg.set_plain(m_ui.m_txtMessage->toPlainText().toStdString());

  if (m_originalMessage == nullptr) {
    // Send completely new message.
  }
  else {
    // TODO: Reply to existing message.
  }

  try {
    m_root->network()->sendEmail(msg);
    accept();
  }
  catch (const ApplicationException& ex) {
    MessageBox::show(this, QMessageBox::Icon::Critical,
                     tr("E-mail NOT sent"), tr("Your e-mail message wasn't sent."),
                     QString(),
                     ex.message());
  }
}

void FormAddEditEmail::addRecipientRow(const QString& recipient) {
  auto* mail_rec = new EmailRecipientControl(recipient, this);

  connect(mail_rec, &EmailRecipientControl::removalRequested, this, &FormAddEditEmail::removeRecipientRow);

  m_ui.m_layout->insertRow(m_ui.m_layout->count() - 5, mail_rec);
}

QList<EmailRecipientControl*> FormAddEditEmail::recipientControls() const {
  QList<EmailRecipientControl*> list;

  for (int i = 0; i < m_ui.m_layout->count(); i++) {
    auto* wdg = qobject_cast<EmailRecipientControl*>(m_ui.m_layout->itemAt(i)->widget());

    if (wdg != nullptr) {
      list.append(wdg);
    }
  }

  return list;
}
