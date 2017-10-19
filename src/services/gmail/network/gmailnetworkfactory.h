// For license of this file, see <object-root-folder>/LICENSE.md.

#ifndef GMAILNETWORKFACTORY_H
#define GMAILNETWORKFACTORY_H

#include <QObject>

#include "core/message.h"

#include "services/abstract/feed.h"
#include "services/abstract/rootitem.h"

#include <QNetworkReply>

class RootItem;
class GmailServiceRoot;
class OAuth2Service;

class GmailNetworkFactory : public QObject {
  Q_OBJECT

  public:
    explicit GmailNetworkFactory(QObject* parent = nullptr);

    void setService(GmailServiceRoot* service);

    OAuth2Service* oauth() const;

    QString userName() const;
    void setUsername(const QString& username);

    // Gets/sets the amount of messages to obtain during single feed update.
    int batchSize() const;
    void setBatchSize(int batch_size);

    // Returns tree of feeds/categories.
    // Top-level root of the tree is not needed here.
    // Returned items do not have primary IDs assigned.
    //RootItem* feedsCategories();

    QList<Message> messages(const QString& stream_id, Feed::Status& error);
    void markMessagesRead(RootItem::ReadStatus status, const QStringList& custom_ids, bool async = true);
    void markMessagesStarred(RootItem::Importance importance, const QStringList& custom_ids, bool async = true);

  private slots:
    void onTokensError(const QString& error, const QString& error_description);
    void onAuthFailed();

  private:
    bool obtainAndDecodeFullMessages(const QList<Message>& lite_messages);
    QList<Message> decodeLiteMessages(const QString& messages_json_data, const QString& stream_id, QString& next_page_token);

    //RootItem* decodeFeedCategoriesData(const QString& categories);

    void initializeOauth();

  private:
    GmailServiceRoot* m_service;
    QString m_username;
    int m_batchSize;
    OAuth2Service* m_oauth2;
};

#endif // GMAILNETWORKFACTORY_H
