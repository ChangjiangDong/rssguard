// For license of this file, see <object-root-folder>/LICENSE.md.

#ifndef APPLICATIONEXCEPTION_H
#define APPLICATIONEXCEPTION_H

#include <QString>

class ApplicationException {
  public:
    explicit ApplicationException(const QString& message = QString());
    virtual ~ApplicationException();

    QString message() const;

  private:
    QString m_message;
};

#endif // APPLICATIONEXCEPTION_H
