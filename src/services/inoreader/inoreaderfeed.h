// For license of this file, see <object-root-folder>/LICENSE.md.

#ifndef INOREADERFEED_H
#define INOREADERFEED_H

#include "services/abstract/feed.h"

class InoreaderServiceRoot;

class InoreaderFeed : public Feed {
  public:
    explicit InoreaderFeed(RootItem* parent = nullptr);
    explicit InoreaderFeed(const QSqlRecord& record);

    InoreaderServiceRoot* serviceRoot() const;

  private:
    QList<Message> obtainNewMessages(bool* error_during_obtaining);
};

#endif // INOREADERFEED_H
