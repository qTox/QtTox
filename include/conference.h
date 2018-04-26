#ifndef _QT_TOX_CONFERENCE_H
#define _QT_TOX_CONFERENCE_H

#include <QObject>

struct Tox;

namespace QtTox
{

class Conference : public QObject
{
    Q_OBJECT
public:

    enum class Type
    {
        Text,
        AV,
    };

    Q_SIGNAL void messageReceived(uint32_t conferenceNum, uint32_t peerNum,
            MessageType type, const QString& message);
    Q_SIGNAL void titleChanged(uint32_t conferenceNum, uint32_t peerNum, const QString& title);
    Q_SIGNAL void peerNameChanged(uint32_t conferenceNum, uint32_t peerNum, const QString& name);

    Q_SIGNAL void peerListChanged(uint32_t conferenceNum);

    enum class ErrPeerQuery
    {
        Ok,
        ConferenceNotFound,
        PeerNotFound,
        NoConnection,
    };

    uint32_t getPeerCount(uint32_t conferenceNum, ErrConferencePeerQuery* err = nullptr) const;
    QString getPeerName(uint32_t conferenceNum, uint32_t peerNum, const QString& name) const;
    QByteArray getPeerPublicKey(uint32_t conferenceNum, uint32_t peerNum, ErrConferencePeerQuery* err = nullptr) const;
    bool peerNumberIsOurs(uint32_t conferenceNum, uint32_t peerNum, ErrConferencePeerQuery* err = nullptr) const;

    enum class ErrInvite
    {
        Ok,
        ConferenceNotFound,
        FailSend,
    };

    bool invite(uint32_t friendNum, uint32_t conferenceNum, ErrInvite* err = nullptr);

    enum class ErrJoin
    {
        Ok,
        InvalidLength,
        WrongType,
        FriendNotFound,
        Duplicate,
        InitFail,
        FailSend,
    };

    uint32_t join(uint32_t friendNum, const QByteArray& cookie, ErrJoin* err = nullptr);

    enum class ErrSendMessage
    {
        Ok,
        ConferenceNotFound,
        TooLong,
        NoConnection,
        FailSend,
    };

    bool sendMessage(uint32_t conferenceNum, MessageType type, const Qtring& message,
            ErrSendMessage* err = nullptr);


    enum class ErrTitle
    {
        Ok,
        NotFound,
        InvalidLength,
        FailSend,
    };

    QString getTitle(uint32_t conferenceNum, ErrTitle* err = nullptr) const;
    bool setTitle(uint32_t conferenceNum, const QString& title, ErrTitle* err = nullptr);

    enum class ErrGetType
    {
        Ok,
        ConferenceNotFound,
    };

    Type getType(uint32_t conferenceNum, ErrGetType* err = nullptr);

private:
    struct Tox* tox;
}
#endif // _QT_TOX_CONFERENCE_H
