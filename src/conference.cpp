#include "conference.h"

#include "toxstring.h"
#include "datahelper.h"
#include "fillerror.h"

#include <QMap>

#include <tox/tox.h>

namespace
{
template<class ToxErr, class Err>
void fillErrPeerQuery(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_CONFERENCE_PEER_QUERY_##toxName, \
      QtTox::Conference::ErrPeerQuery::qtName }
    fillError(toxErr, err, {
        ERR(OK,                   Ok),
        ERR(CONFERENCE_NOT_FOUND, ConferenceNotFound),
        ERR(PEER_NOT_FOUND,       PeerNotFound),
        ERR(NO_CONNECTION,        NoConnection),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrInvite(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_CONFERENCE_INVITE_##toxName, \
      QtTox::Conference::ErrInvite::qtName }
    fillError(toxErr, err, {
        ERR(OK,                   Ok),
        ERR(CONFERENCE_NOT_FOUND, ConferenceNotFound),
        ERR(FAIL_SEND,            FailSend),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrJoin(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_CONFERENCE_JOIN_##toxName, \
      QtTox::Conference::ErrJoin::qtName }
    fillError(toxErr, err, {
        ERR(OK,               Ok),
        ERR(INVALID_LENGTH,   InvalidLength),
        ERR(WRONG_TYPE,       WrongType),
        ERR(FRIEND_NOT_FOUND, FriendNotFound),
        ERR(DUPLICATE,        Duplicate),
        ERR(INIT_FAIL,        InitFail),
        ERR(FAIL_SEND,        FailSend),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrSendMessage(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_CONFERENCE_SEND_MESSAGE_##toxName, \
      QtTox::Conference::ErrSendMessage::qtName }
    fillError(toxErr, err, {
        ERR(OK,                   Ok),
        ERR(CONFERENCE_NOT_FOUND, ConferenceNotFound),
        ERR(TOO_LONG,             TooLong),
        ERR(NO_CONNECTION,        NoConnection),
        ERR(FAIL_SEND,            FailSend),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrTitle(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_CONFERENCE_TITLE_##toxName, \
      QtTox::Conference::ErrTitle::qtName }
    fillError(toxErr, err, {
        ERR(OK,                   Ok),
        ERR(CONFERENCE_NOT_FOUND, ConferenceNotFound),
        ERR(INVALID_LENGTH,       InvalidLength),
        ERR(FAIL_SEND,            FailSend),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrGetType(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_CONFERENCE_GET_TYPE_##toxName, \
      QtTox::Conference::ErrTitle::qtName }
    fillError(toxErr, err, {
        ERR(OK,                   Ok),
        ERR(CONFERENCE_NOT_FOUND, ConferenceNotFound),
    });
#undef ERR
}

}

namespace QtTox
{

Conference::Conference(struct Tox* tox)
    : tox{tox}
{
}

uint32_t Conference::getPeerCount(uint32_t conferenceNum,
        ErrPeerQuery* err) const
{
    TOX_ERR_CONFERENCE_PEER_QUERY toxErr;
    const auto count = tox_conference_peer_count(tox, conferenceNum, &toxErr);
    fillErrPeerQuery(toxErr, err);
    return count;
}

QString Conference::getPeerName(uint32_t conferenceNum, uint32_t peerNum,
        ErrPeerQuery* err) const
{
    TOX_ERR_CONFERENCE_PEER_QUERY toxErr;
    const auto size = tox_conference_peer_get_name_size(tox,
            conferenceNum, peerNum, &toxErr);
    fillErrPeerQuery(toxErr, err);
    if (*err != ErrPeerQuery::Ok) {
        return {};
    }

    auto name = QByteArray();
    name.resize(size);
    const auto success = tox_conference_peer_get_name(tox,
            conferenceNum, peerNum, data(name), &toxErr);
    fillErrPeerQuery(toxErr, err);
    if (!success) {
        return {};
    }

    return name;
}

QByteArray Conference::getPeerPublicKey(uint32_t conferenceNum,
        uint32_t peerNum, ErrPeerQuery* err) const
{
    TOX_ERR_CONFERENCE_PEER_QUERY toxErr;
    auto pk = QByteArray();
    pk.resize(TOX_PUBLIC_KEY_SIZE);
    const auto success = tox_conference_peer_get_public_key(tox,
            conferenceNum, peerNum, data(pk), &toxErr);
    fillErrPeerQuery(toxErr, err);
    if (!success) {
        return {};
    }

    return pk;
}

bool Conference::peerNumberIsOurs(uint32_t conferenceNum, uint32_t peerNum,
        ErrPeerQuery* err) const
{
    TOX_ERR_CONFERENCE_PEER_QUERY toxErr;
    const auto ours = tox_conference_peer_number_is_ours(tox,
            conferenceNum, peerNum, &toxErr);
    fillErrPeerQuery(toxErr, err);
    return ours;
}

bool Conference::invite(uint32_t friendNum, uint32_t conferenceNum,
        ErrInvite* err)
{
    TOX_ERR_CONFERENCE_INVITE toxErr;
    const auto success = tox_conference_invite(tox,
            friendNum, conferenceNum, &toxErr);
    fillErrInvite(toxErr, err);
    return success;
}

uint32_t Conference::join(uint32_t friendNum, const QByteArray& cookie,
        ErrJoin* err)
{
    TOX_ERR_CONFERENCE_JOIN toxErr;
    const auto conferenceNum = tox_conference_join(tox,
            friendNum, data(cookie), cookie.size(), &toxErr);
    fillErrJoin(toxErr, err);
    return conferenceNum;
}

bool Conference::sendMessage(uint32_t conferenceNum,
        MessageType type, const QString& message, ErrSendMessage* err)
{
    TOX_ERR_CONFERENCE_SEND_MESSAGE toxErr;
    const auto map = QMap<MessageType, TOX_MESSAGE_TYPE> {
        { MessageType::Normal, TOX_MESSAGE_TYPE_NORMAL },
        { MessageType::Action, TOX_MESSAGE_TYPE_ACTION },
    };
    const auto toxType = map[type];
    const auto toxMessage = ToxString(message);
    const auto success = tox_conference_send_message(tox, conferenceNum,
            toxType, toxMessage.data(), toxMessage.size(), &toxErr);
    fillErrSendMessage(toxErr, err);
    return success;
}

QString Conference::getTitle(uint32_t conferenceNum, ErrTitle* err) const
{
    TOX_ERR_CONFERENCE_TITLE toxErr;
    const auto size = tox_conference_get_title_size(tox, conferenceNum, &toxErr);
    fillErrTitle(toxErr, err);
    if (*err != ErrTitle::Ok) {
        return {};
    }

    auto cTitle = QByteArray{};
    cTitle.resize(size);
    const auto success = tox_conference_get_title(tox,
            conferenceNum, data(cTitle), &toxErr);
    fillErrTitle(toxErr, err);
    if (!success) {
        return {};
    }

    const auto title = ToxString(cTitle).getQString();
    return title;
}

bool Conference::setTitle(uint32_t conferenceNum, const QString& title,
        ErrTitle* err)
{
    TOX_ERR_CONFERENCE_TITLE toxErr;
    const auto toxTitle = ToxString(title);
    const auto success = tox_conference_set_title(tox,
            conferenceNum, toxTitle.data(), toxTitle.size(), &toxErr);
    fillErrTitle(toxErr, err);
    return success;
}

}
