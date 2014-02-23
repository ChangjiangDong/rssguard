#include "core/webbrowsernetworkaccessmanager.h"

#include <QNetworkReply>


WebBrowserNetworkAccessManager::WebBrowserNetworkAccessManager(QObject *parent)
  : BaseNetworkAccessManager(parent) {
  connect(this, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
          this, SLOT(onSslErrors(QNetworkReply*,QList<QSslError>)));
  connect(this, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
          this, SLOT(onAuthenticationRequired(QNetworkReply*,QAuthenticator*)));
}

WebBrowserNetworkAccessManager::~WebBrowserNetworkAccessManager() {
  qDebug("Destroying WebBrowserNetworkAccessManager instance.");
}

void WebBrowserNetworkAccessManager::onSslErrors(QNetworkReply *reply,
                                                 const QList<QSslError> &error) {
  qDebug("SSL errors for '%s': '%s' (code %d).",
         qPrintable(reply->url().toString()),
         qPrintable(reply->errorString()),
         (int) reply->error());

  reply->ignoreSslErrors(error);
}

void WebBrowserNetworkAccessManager::onAuthenticationRequired(QNetworkReply *reply,
                                                              QAuthenticator *authenticator) {
  Q_UNUSED(authenticator);

  // Authentication is required but this feed does not contain it.
  qDebug("URL '%s' requested authentication but username/password is not available.",
         qPrintable(reply->url().toString()));
}
