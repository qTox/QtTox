#ifndef _Q_TOX_MESSENGER_H_
#define _Q_TOX_MESSENGER_H_

struct Tox;

namespace QtTox
{

class Messenger
{
    Messenger(struct Tox* tox);

    enum class ErrFriendGetPublicKey
    {
        Ok,
        FriendNotFound,
    };
    QByteArray getFriendPublicKey(uint32_t friendNum, ErrFriendGetPublicKey* err = nullptr) const;

    enum class ErrFriendGetLastOnline
    {
        Ok,
        FriendNotFound,
    };
    uint64_t getFriendLastOnline(uint32_t friendNum, ErrFriendGetLastOnline* err = nullptr) const;

    enum class ErrFriendQuery
    {
        Ok,
        Null,
        FriendNotFound,
    };

    QString getFriendName(uint32_t friendNum, ErrFriendQuery* err = nullptr) const;
    Q_SIGNAL void friendNameChanged(uint32_t friendNum, const QString& name);

    QString getFriendStatusMessage(uint32_t friendNum, ErrFriendQuery* err = nullptr) const;
    Q_SIGNAL void friendStatusMessageChanged(uint32_t friendNum, const QString& name);

    UserStatus getFriendStatus(uint32_t friendNum, ErrFriendQuery* err = nullptr) const;
    Q_SIGNAL void friendStatusChanged(uint32_t friendNum, UserStatus status);

    Connection getFriendConnectionStatus(uint32_t friendNum, ErrFriendQuery* err = nullptr) const;
    Q_SIGNAL void friendConnectionStatusChanged(uint32_t friendNum, Connection status);

    bool getFriendTyping(uint32_t friendNum, ErrFriendQuery* err = nullptr) const;
    Q_SIGNAL void friendTypingChanged(uint32_t friendNum, bool typing);

    enum class ErrSetTyping
    {
        Ok,
        FriendNotFound,
    };

    bool setSelfTyping(uint32_t friendNum, bool typing, ErrSetTyping* err = nullptr);

    enum class ErrFriendSendMessage
    {
        Ok,
        Null,
        FriendNotFound,
        FriendNotConnected,
        Sendq,
        TooLong,
        Empty,
    };

    uint32_t friendSendMessage(uint32_t friendNum, MessageType type, const QString& message,
            ErrFriendSendMessage* err = nullptr);

    Q_SIGNAL void friendReceiptReaded(uint32_t friendNum, uint32_t messageId);
    Q_SIGNAL void friendMessage(uint32_t friendNum, MessageType type, const QString& message);

private:
    struct Tox* tox;
};

}

#endif // _Q_TOX_MESSENGER_H_
