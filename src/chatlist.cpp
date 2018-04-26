#include "chatlist.h"
#include "toxstring.h"

#include <tox/tox.h>

#include <QMap>
#include <QVector>

namespace
{
template<class ToxErr, class Err>
void fillError(ToxErr toxErr, Err* err, const QMap<ToxErr, Err>& map)
{
    if (!err) {
        return;
    }

    *err = map[toxErr];
}

using ErrFriendAdd = QtTox::ChatList::ErrFriendAdd;
void fillErrFriendAdd(TOX_ERR_FRIEND_ADD toxErr, ErrFriendAdd* err)
{
    fillError(toxErr, err, {
        { TOX_ERR_FRIEND_ADD_OK,             ErrFriendAdd::Ok },
        { TOX_ERR_FRIEND_ADD_NULL,           ErrFriendAdd::Null },
        { TOX_ERR_FRIEND_ADD_TOO_LONG,       ErrFriendAdd::TooLong },
        { TOX_ERR_FRIEND_ADD_NO_MESSAGE,     ErrFriendAdd::NoMessage },
        { TOX_ERR_FRIEND_ADD_OWN_KEY,        ErrFriendAdd::OwnKey },
        { TOX_ERR_FRIEND_ADD_ALREADY_SENT,   ErrFriendAdd::AlreadySent },
        { TOX_ERR_FRIEND_ADD_BAD_CHECKSUM,   ErrFriendAdd::BadChecksum },
        { TOX_ERR_FRIEND_ADD_SET_NEW_NOSPAM, ErrFriendAdd::SetNewNospam },
        { TOX_ERR_FRIEND_ADD_MALLOC,         ErrFriendAdd::Malloc },
    });
}

using ErrFriendDelete = QtTox::ChatList::ErrFriendDelete;
void fillErrFriendDelete(TOX_ERR_FRIEND_DELETE toxErr, ErrFriendDelete* err)
{
    fillError(toxErr, err, {
        { TOX_ERR_FRIEND_DELETE_OK,               ErrFriendDelete::Ok },
        { TOX_ERR_FRIEND_DELETE_FRIEND_NOT_FOUND, ErrFriendDelete::FriendNotFound },
    });
}

using ErrFriendByPK = QtTox::ChatList::ErrFriendByPublicKey;
void fillErrFriendByPublicKey(TOX_ERR_FRIEND_BY_PUBLIC_KEY toxErr,
        ErrFriendByPK* err)
{
    fillError(toxErr, err, {
        { TOX_ERR_FRIEND_BY_PUBLIC_KEY_OK,        ErrFriendByPK::Ok },
        { TOX_ERR_FRIEND_BY_PUBLIC_KEY_NULL,      ErrFriendByPK::Null },
        { TOX_ERR_FRIEND_BY_PUBLIC_KEY_NOT_FOUND, ErrFriendByPK::NotFound },
    });
}

using ErrConferenceDelete = QtTox::ChatList::ErrConferenceDelete;
void fillErrConferenceDelete(TOX_ERR_CONFERENCE_DELETE toxErr,
        ErrConferenceDelete* err)
{
    fillError(toxErr, err, {
        { TOX_ERR_CONFERENCE_DELETE_OK, ErrConferenceDelete::Ok },
        { TOX_ERR_CONFERENCE_DELETE_CONFERENCE_NOT_FOUND,
          ErrConferenceDelete::ConferenceNotFound },
    });
}

using ErrConferenceNew = QtTox::ChatList::ErrConferenceNew;
void fillErrConferenceNew(TOX_ERR_CONFERENCE_NEW toxErr, ErrConferenceNew* err)
{
    fillError(toxErr, err, {
        { TOX_ERR_CONFERENCE_NEW_OK,   ErrConferenceNew::Ok },
        { TOX_ERR_CONFERENCE_NEW_INIT, ErrConferenceNew::Init },
    });
}

const uint8_t* data(const QByteArray& arr)
{
    return static_cast<const uint8_t*>(static_cast<const void*>(arr.data()));
}

}

namespace QtTox
{

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
