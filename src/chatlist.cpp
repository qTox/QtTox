#include "chatlist.h"

#include "datahelper.h"
#include "fillerror.h"
#include "services.h"
#include "toxstring.h"

#include <tox/tox.h>

#include <QMap>
#include <QVector>

namespace
{

template<class ToxErr, class Err>
void fillErrFriendAdd(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_FRIEND_ADD_##toxName, QtTox::ChatList::ErrFriendAdd::qtName }
    fillError(toxErr, err, {
        ERR(OK,             Ok),
        ERR(NULL,           Null),
        ERR(TOO_LONG,       TooLong),
        ERR(NO_MESSAGE,     NoMessage),
        ERR(OWN_KEY,        OwnKey),
        ERR(ALREADY_SENT,   AlreadySent),
        ERR(BAD_CHECKSUM,   BadChecksum),
        ERR(SET_NEW_NOSPAM, SetNewNospam),
        ERR(MALLOC,         Malloc),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrFriendDelete(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_FRIEND_DELETE_##toxName, QtTox::ChatList::ErrFriendDelete::qtName }
    fillError(toxErr, err, {
        ERR(OK,               Ok),
        ERR(FRIEND_NOT_FOUND, FriendNotFound),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrFriendByPublicKey(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_FRIEND_BY_PUBLIC_KEY_##toxName, Err::qtName }
    fillError(toxErr, err, {
        ERR(OK,        Ok),
        ERR(NULL,      Null),
        ERR(NOT_FOUND, NotFound),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrConferenceDelete(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_CONFERENCE_DELETE_##toxName, Err::qtName }
    fillError(toxErr, err, {
        ERR(OK,                   Ok),
        ERR(CONFERENCE_NOT_FOUND, ConferenceNotFound),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrConferenceNew(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_CONFERENCE_NEW_##toxName, Err::qtName }
    fillError(toxErr, err, {
        ERR(OK,   Ok),
        ERR(INIT, Init),
    });
#undef ERR
}

void onConferenceInvite(struct Tox*, uint32_t friendNum,
        TOX_CONFERENCE_TYPE toxType, const uint8_t* toxCookie, size_t length,
        void* payload)
{
    auto services = static_cast<QtTox::Services*>(payload);
    const auto ptr = static_cast<const char*>(static_cast<const void*>(toxCookie));
    const auto cookie = QByteArray(ptr, length);
    const auto map = QMap<TOX_CONFERENCE_TYPE, QtTox::ConferenceType> {
        { TOX_CONFERENCE_TYPE_TEXT, QtTox::ConferenceType::Text },
        { TOX_CONFERENCE_TYPE_AV,   QtTox::ConferenceType::AV },
    };
    const auto type = map[toxType];
    emit services->chatList->conferenceInviteReceived(friendNum, type, cookie);
}

void onFriendRequest(struct Tox*, const uint8_t* cFriendPk,
        const uint8_t* cMessage, size_t cMessageSize, void* payload)
{
    auto services = static_cast<QtTox::Services*>(payload);
    const auto message = ToxString(cMessage, cMessageSize).getQString();
    const auto ptr = static_cast<const char*>(static_cast<const void*>(cFriendPk));
    const auto publicKey = QByteArray(ptr, TOX_PUBLIC_KEY_SIZE);
    emit services->chatList->friendRequestReceived(publicKey, message);
}

}

namespace QtTox
{

ChatList::ChatList(struct Tox* tox)
    : tox{tox}
{
    tox_callback_friend_request(tox, onFriendRequest);
    tox_callback_conference_invite(tox, onConferenceInvite);
}

uint32_t ChatList::friendAdd(const QByteArray& address, const QString& message, ErrFriendAdd* err)
{
    TOX_ERR_FRIEND_ADD toxErr;
    const auto cMessage = ToxString{message};
    const auto friendNum = tox_friend_add(tox, data(address), cMessage.data(),
            cMessage.size(), &toxErr);

    fillErrFriendAdd(toxErr, err);
    return friendNum;
}

uint32_t ChatList::friendAddNorequest(const QByteArray& address, ErrFriendAdd* err)
{
    TOX_ERR_FRIEND_ADD toxErr;
    const auto friendNum = tox_friend_add_norequest(tox, data(address),
            &toxErr);

    fillErrFriendAdd(toxErr, err);
    return friendNum;
}

bool ChatList::friendDelete(uint32_t friendNum, ErrFriendDelete* err)
{
    TOX_ERR_FRIEND_DELETE toxErr;
    const auto success = tox_friend_delete(tox, friendNum, &toxErr);
    fillErrFriendDelete(toxErr, err);
    return success;
}

uint32_t ChatList::friendByPublicKey(const QByteArray& publicKey, ErrFriendByPublicKey* err) const
{
    TOX_ERR_FRIEND_BY_PUBLIC_KEY toxErr;
    const auto friendNum = tox_friend_by_public_key(tox, data(publicKey),
            &toxErr);
    fillErrFriendByPublicKey(toxErr, err);
    return friendNum;
}

bool ChatList::friendExists(uint32_t friendNum) const
{
    return tox_friend_exists(tox, friendNum);
}

QVector<uint32_t> ChatList::getFriendList() const
{
    const auto size = tox_self_get_friend_list_size(tox);
    auto friends = QVector<uint32_t>();
    friends.resize(size);
    tox_self_get_friend_list(tox, friends.data());
    return friends;
}

uint32_t ChatList::conferenceNew(ErrConferenceNew* err)
{
    TOX_ERR_CONFERENCE_NEW toxErr;
    const auto conferenceNum = tox_conference_new(tox, &toxErr);
    fillErrConferenceNew(toxErr, err);
    return conferenceNum;
}

bool ChatList::conferenceDelete(uint32_t conferenceNum, ErrConferenceDelete* err)
{
    TOX_ERR_CONFERENCE_DELETE toxErr;
    const auto success = tox_conference_delete(tox, conferenceNum, &toxErr);
    fillErrConferenceDelete(toxErr, err);
    return success;
}

QVector<uint32_t> ChatList::getChatlist() const
{
    const auto size = tox_conference_get_chatlist_size(tox);
    auto chats = QVector<uint32_t>{};
    chats.resize(size);
    tox_conference_get_chatlist(tox, chats.data());
    return chats;
}

}
