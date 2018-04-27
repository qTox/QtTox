#include "messenger.h"

#include "datahelper.h"
#include "fillerror.h"
#include "services.h"
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

void onFriendName(struct Tox* tox, uint32_t friendNum,
        const uint8_t* cName, size_t length, void* payload)
{
    auto service = static_cast<QtTox::Services*>(payload);
    const auto name = ToxString(cName, length).getQString();
    emit service->messenger->friendNameChanged(friendNum, name);
}

void onFriendStatusMessage(struct Tox* tox, uint32_t friendNum,
        const uint8_t* cMessage, size_t length, void* payload)
{
    auto service = static_cast<QtTox::Services*>(payload);
    const auto message = ToxString(cMessage, length).getQString();
    emit service->messenger->friendStatusMessageChanged(friendNum, message);
}

void onFriendStatus(struct Tox* tox, uint32_t friendNum,
        TOX_USER_STATUS toxStatus, void* payload)
{
    auto service = static_cast<QtTox::Services*>(payload);
    const auto map = QMap<TOX_USER_STATUS, QtTox::UserStatus> {
        { TOX_USER_STATUS_NONE, QtTox::UserStatus::None },
        { TOX_USER_STATUS_BUSY, QtTox::UserStatus::Busy },
        { TOX_USER_STATUS_AWAY, QtTox::UserStatus::Away },
    };
    const auto status = map[toxStatus];
    emit service->messenger->friendStatusChanged(friendNum, status);
}

void onFriendConnectionStatus(struct Tox* tox, uint32_t friendNum,
        TOX_CONNECTION toxStatus, void* payload)
{
    auto service = static_cast<QtTox::Services*>(payload);
    const auto map = QMap<TOX_CONNECTION, QtTox::Connection> {
        { TOX_CONNECTION_NONE, QtTox::Connection::None },
        { TOX_CONNECTION_TCP,  QtTox::Connection::TCP },
        { TOX_CONNECTION_UDP,  QtTox::Connection::UDP },
    };
    const auto status = map[toxStatus];
    emit service->messenger->friendConnectionStatusChanged(friendNum, status);
}

void onFriendTyping(struct Tox* tox, uint32_t friendNum,
        bool typing, void* payload)
{
    auto service = static_cast<QtTox::Services*>(payload);
    emit service->messenger->friendTypingChanged(friendNum, typing);
}

void onFriendReadReceipt(struct Tox* tox, uint32_t friendNum,
        uint32_t messageId, void* payload)
{
    auto service = static_cast<QtTox::Services*>(payload);
    emit service->messenger->friendReceiptReaded(friendNum, messageId);
}

void onFriendMessage(struct Tox* tox, uint32_t friendNum,
        TOX_MESSAGE_TYPE toxType, const uint8_t* cMessage, size_t length,
        void* payload)
{
    auto service = static_cast<QtTox::Services*>(payload);
    const auto map = QMap<TOX_MESSAGE_TYPE, QtTox::MessageType> {
        { TOX_MESSAGE_TYPE_NORMAL, QtTox::MessageType::Normal },
        { TOX_MESSAGE_TYPE_ACTION, QtTox::MessageType::Action },
    };
    const auto type = map[toxType];
    const auto message = ToxString(cMessage, length).getQString();
    emit service->messenger->friendMessage(friendNum, type, message);
}
}

namespace QtTox
{

Messenger::Messenger(struct Tox* tox)
{
    tox_callback_friend_name(tox, onFriendName);
    tox_callback_friend_status_message(tox, onFriendStatusMessage);
    tox_callback_friend_status(tox, onFriendStatus);
    tox_callback_friend_connection_status(tox, onFriendConnectionStatus);
    tox_callback_friend_typing(tox, onFriendTyping);
    tox_callback_friend_read_receipt(tox, onFriendReadReceipt);
    tox_callback_friend_message(tox, onFriendMessage);
}

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
