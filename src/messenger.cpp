#include "messenger.h"

#include "datahelper.h"
#include "fillerror.h"
#include "toxstring.h"

#include <QMap>

#include <tox/tox.h>

namespace
{
template<class ToxErr, class Err>
void fillErrFriendGetPublicKey(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_FRIEND_GET_PUBLIC_KEY_##toxName, \
      QtTox::Messenger::ErrFriendGetPublicKey::qtName }
    fillError(toxErr, err, {
        ERR(OK,               Ok),
        ERR(FRIEND_NOT_FOUND, FriendNotFound),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillFriendGetLastOnline(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_FRIEND_GET_LAST_ONLINE_##toxName, \
      QtTox::Messenger::ErrFriendGetLastOnline::qtName }
    fillError(toxErr, err, {
        ERR(OK,               Ok),
        ERR(FRIEND_NOT_FOUND, FriendNotFound),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrFriendQuery(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_FRIEND_QUERY_##toxName, QtTox::Messenger::ErrFriendQuery::qtName }
    fillError(toxErr, err, {
        ERR(OK,               Ok),
        ERR(NULL,             Null),
        ERR(FRIEND_NOT_FOUND, FriendNotFound),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrSetTyping(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_SET_TYPING_##toxName, QtTox::Messenger::ErrSetTyping::qtName }
    fillError(toxErr, err, {
        ERR(OK,               Ok),
        ERR(FRIEND_NOT_FOUND, FriendNotFound),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrFriendSendMessage(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_FRIEND_SEND_MESSAGE_##toxName, \
        QtTox::Messenger::ErrFriendSendMessage::qtName }
    fillError(toxErr, err, {
        ERR(OK,                   Ok),
        ERR(NULL,                 Null),
        ERR(FRIEND_NOT_FOUND,     FriendNotFound),
        ERR(FRIEND_NOT_CONNECTED, FriendNotConnected),
        ERR(SENDQ,                Sendq),
        ERR(TOO_LONG,             TooLong),
        ERR(EMPTY,                Empty),
    });
#undef ERR
}

}

namespace QtTox
{

QByteArray Messenger::getFriendPublicKey(uint32_t friendNum, ErrFriendGetPublicKey* err) const
{
    TOX_ERR_FRIEND_GET_PUBLIC_KEY toxErr;
    auto pk = QByteArray{};
    pk.resize(TOX_PUBLIC_KEY_SIZE);
    const auto success = tox_friend_get_public_key(tox, friendNum, data(pk), &toxErr);
    fillErrFriendGetPublicKey(toxErr, err);
    if (!success) {
        return {};
    }

    return pk;
}

uint64_t Messenger::getFriendLastOnline(uint32_t friendNum, ErrFriendGetLastOnline* err) const
{
    TOX_ERR_FRIEND_GET_LAST_ONLINE toxErr;
    const auto time = tox_friend_get_last_online(tox, friendNum, &toxErr);
    fillFriendGetLastOnline(toxErr, err);
    return time;
}

QString Messenger::getFriendName(uint32_t friendNum, ErrFriendQuery* err) const
{
    TOX_ERR_FRIEND_QUERY toxErr;
    const auto size = tox_friend_get_name_size(tox, friendNum, &toxErr);
    fillErrFriendQuery(toxErr, err);
    if (size == SIZE_MAX) {
        return {};
    }

    const auto cName = new uint8_t[size];
    const auto success = tox_friend_get_name(tox, friendNum, cName, &toxErr);
    fillErrFriendQuery(toxErr, err);
    if (!success) {
        return {};
    }

    const auto name = ToxString(cName, size).getQString();
    delete[] cName;
    return name;
}

QString Messenger::getFriendStatusMessage(uint32_t friendNum, ErrFriendQuery* err) const
{
    TOX_ERR_FRIEND_QUERY toxErr;
    const auto size = tox_friend_get_status_message_size(tox, friendNum, &toxErr);
    fillErrFriendQuery(toxErr, err);
    if (size == SIZE_MAX) {
        return {};
    }

    const auto cStatus = new uint8_t[size];
    const auto success = tox_friend_get_status_message(tox, friendNum, cStatus, &toxErr);
    fillErrFriendQuery(toxErr, err);
    if (!success) {
        return {};
    }

    const auto status = ToxString(cStatus, size).getQString();
    delete[] cStatus;
    return status;
}

UserStatus Messenger::getFriendStatus(uint32_t friendNum, ErrFriendQuery* err) const
{
    TOX_ERR_FRIEND_QUERY toxErr;
    const auto toxUserStatus = tox_friend_get_status(tox, friendNum, &toxErr);
    const auto map = QMap<TOX_USER_STATUS, UserStatus> {
        { TOX_USER_STATUS_NONE, UserStatus::None },
        { TOX_USER_STATUS_AWAY, UserStatus::Away },
        { TOX_USER_STATUS_BUSY, UserStatus::Busy },
    };

    fillErrFriendQuery(toxErr, err);
    return map[toxUserStatus];
}

Connection Messenger::getFriendConnectionStatus(uint32_t friendNum, ErrFriendQuery* err) const
{
    TOX_ERR_FRIEND_QUERY toxErr;
    const auto connection = tox_friend_get_connection_status(tox, friendNum, &toxErr);
    const auto map = QMap<TOX_CONNECTION, Connection> {
        { TOX_CONNECTION_NONE, Connection::None },
        { TOX_CONNECTION_TCP,  Connection::TCP },
        { TOX_CONNECTION_UDP,  Connection::UDP },
    };

    fillErrFriendQuery(toxErr, err);
    return map[connection];
}

bool Messenger::getFriendTyping(uint32_t friendNum, ErrFriendQuery* err) const
{
    TOX_ERR_FRIEND_QUERY toxErr;
    const auto typing = tox_friend_get_typing(tox, friendNum, &toxErr);
    fillErrFriendQuery(toxErr, err);
    return typing;
}

bool Messenger::setSelfTyping(uint32_t friendNum, bool typing, ErrSetTyping* err)
{
    TOX_ERR_SET_TYPING toxErr;
    const auto success = tox_self_set_typing(tox, friendNum, typing, &toxErr);
    fillErrSetTyping(toxErr, err);
    return success;
}

uint32_t Messenger::friendSendMessage(uint32_t friendNum, MessageType type, 
        const QString& message, ErrFriendSendMessage* err)
{
    const auto map = QMap<MessageType, TOX_MESSAGE_TYPE> {
        { MessageType::Normal, TOX_MESSAGE_TYPE_NORMAL },
        { MessageType::Action, TOX_MESSAGE_TYPE_ACTION },
    };
    const auto toxType = map[type];
    const auto cMessage = ToxString{message};

    TOX_ERR_FRIEND_SEND_MESSAGE toxErr;
    const auto messageId = tox_friend_send_message(tox, friendNum, toxType,
            cMessage.data(), cMessage.size(), &toxErr);

    fillErrFriendSendMessage(toxErr, err);
    return messageId;
}

}
