#ifndef _QT_TOX_SELF_H_
#define _QT_TOX_SELF_H_

struct Tox;

namespace QtTox
{

class Self : public QObject
{
    Q_OBJECT

public:
    Connection getSelfConnectionStatus() const;
    Q_SIGNAL void selfConnectionStatusChanged(Connection status);

    QByteArray getSelfAddress() const;

    QByteArray getNospam() const;
    void setNospam(const QByteArray& nospam);

    QByteArray getSelfPublicKey() const;
    QByteArray getSelfSecretKey() const;

    // User-visible client information (nickname/status)

    enum class ErrSetInfo
    {
        Ok,
        Null,
        TooLong
    };

    bool setSelfName(const QString& name, ErrSetInfo* err = nullptr);
    QString getSelfName() const;

    bool setSelfStatusMessage(const QString& statusMessage, ErrSetInfo* err = nullptr)
    QString getSelfStatusMessage() const;

    bool setSelfStatus(UserStatus status);
    UserStatus getSelfStatus() const;

private:
    struct Tox* tox;
}

#endif // _QT_TOX_SELF_H_
