#ifndef _QT_TOX_CHAT_LIST_H_
#define _QT_TOX_CHAT_LIST_H_

#include "conferencetype.h"

#include <QObject>

struct Tox;

namespace QtTox
{

class ChatList : public QObject
{
    Q_OBJECT

public:

    // Friend list management

    enum class ErrFriendAdd
    {
        Ok,
        Null,
        TooLong,
        NoMessage,
        OwnKey,
        AlreadySent,
        BadChecksum,
        SetNewNospam,
        Malloc
    };

    uint32_t friendAdd(const QByteArray& address, const QString& message, ErrFriendAdd* err = nullptr);
    uint32_t friendAddNorequest(const QByteArray& address, ErrFriendAdd* err = nullptr);
    Q_SIGNAL void friendRequest(const QByteArray& publicKey, const QString& message);
    
    enum class ErrFriendDelete
    {
        Ok,
        FriendNotFound,
    };

    bool friendDelete(uint32_t friendNum, ErrFriendDelete* err = nullptr);

    enum class ErrFriendByPublicKey
    {
        Ok,
        Null,
        NotFound,
    };
    uint32_t friendByPublicKey(const QByteArray& publicKey, ErrFriendByPublicKey* err = nullptr) const;

    bool friendExists(uint32_t friendNumer) const;
    QVector<uint32_t> getFriendList() const;

    // Conference list management
    
    enum class ErrConferenceNew
    {
        Ok,
        Init,
    };
    uint32_t conferenceNew(ErrConferenceNew* err = nullptr);
    Q_SIGNAL void invited(uint32_t conferenceNum, ConferenceType type);

    enum class ErrConferenceDelete
    {
        Ok,
        ConferenceNotFound,
    };

    bool conferenceDelete(uint32_t conferenceNum, ErrConferenceDelete* err = nullptr);

    // Get list of conference numbers
    QVector<uint32_t> getChatlist() const;

private:
    struct Tox* tox;
};

}

#endif // _QT_TOX_CHAT_LIST_H_
