#include "files.h"

#include "datahelper.h"
#include "fillerror.h"
#include "services.h"
#include "toxstring.h"

#include <QMap>

#include <tox/tox.h>

namespace
{

const auto fileControlMap1 = QMap<TOX_FILE_CONTROL, QtTox::Files::FileControl> {
    {TOX_FILE_CONTROL::TOX_FILE_CONTROL_RESUME, QtTox::Files::FileControl::Resume},
    {TOX_FILE_CONTROL::TOX_FILE_CONTROL_PAUSE, QtTox::Files::FileControl::Pause},
    {TOX_FILE_CONTROL::TOX_FILE_CONTROL_CANCEL, QtTox::Files::FileControl::Cancel}
};

const auto fileControlMap2 = QMap<QtTox::Files::FileControl, TOX_FILE_CONTROL> {
    {QtTox::Files::FileControl::Resume, TOX_FILE_CONTROL::TOX_FILE_CONTROL_RESUME},
    {QtTox::Files::FileControl::Pause, TOX_FILE_CONTROL::TOX_FILE_CONTROL_PAUSE},
    {QtTox::Files::FileControl::Cancel, TOX_FILE_CONTROL::TOX_FILE_CONTROL_CANCEL}
};

const auto fileKindMap1 = QMap<TOX_FILE_KIND, QtTox::Files::FileKind> {
    {TOX_FILE_KIND::TOX_FILE_KIND_DATA, QtTox::Files::FileKind::Data},
    {TOX_FILE_KIND::TOX_FILE_KIND_AVATAR, QtTox::Files::FileKind::Avatar}
};

const auto fileKindMap2 = QMap<QtTox::Files::FileKind, TOX_FILE_KIND> {
    {QtTox::Files::FileKind::Data, TOX_FILE_KIND::TOX_FILE_KIND_DATA},
    {QtTox::Files::FileKind::Avatar, TOX_FILE_KIND::TOX_FILE_KIND_AVATAR}
};


template<class ToxErr, class Err>
void fillErrFileControl(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_FILE_CONTROL_##toxName, \
      QtTox::Files::ErrFileControl::qtName }
    fillError(toxErr, err, {
        ERR(OK,                     Ok),
        ERR(FRIEND_NOT_FOUND,       FriendNotFound),
        ERR(FRIEND_NOT_CONNECTED,   FriendNotConnected),
        ERR(NOT_FOUND,              NotFound),
        ERR(NOT_PAUSED,             NotPaused),
        ERR(DENIED,                 Denied),
        ERR(ALREADY_PAUSED,         AlreadyPaused),
        ERR(SENDQ,                  Sendq),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrFileSeek(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_FILE_SEEK_##toxName, \
      QtTox::Files::ErrFileSeek::qtName }
    fillError(toxErr, err, {
        ERR(OK,                     Ok),
        ERR(FRIEND_NOT_FOUND,       FriendNotFound),
        ERR(FRIEND_NOT_CONNECTED,   FriendNotConnected),
        ERR(NOT_FOUND,              NotFound),
        ERR(DENIED,                 Denied),
        ERR(INVALID_POSITION,       InvalidPosition),
        ERR(SENDQ,                  Sendq),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrFileGet(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_FILE_GET_##toxName, \
      QtTox::Files::ErrFileGet::qtName }
    fillError(toxErr, err, {
        ERR(OK,                     Ok),
        ERR(NULL,                   Null),
        ERR(FRIEND_NOT_FOUND,       FriendNotFound),
        ERR(NOT_FOUND,              NotFound),
    });
#undef ERR
}


template<class ToxErr, class Err>
void fillErrFileSend(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_FILE_SEND_##toxName, \
      QtTox::Files::ErrFileSend::qtName }
    fillError(toxErr, err, {
        ERR(OK,                     Ok),
        ERR(NULL,                   Null),
        ERR(FRIEND_NOT_FOUND,       FriendNotFound),
        ERR(FRIEND_NOT_CONNECTED,   FriendNotConnected),
        ERR(NAME_TOO_LONG,          NameTooLong),
        ERR(TOO_MANY,               TooMany),
    });
#undef ERR
}

template<class ToxErr, class Err>
void fillErrFileSendChunk(ToxErr toxErr, Err* err)
{
#define ERR(toxName, qtName) \
    { TOX_ERR_FILE_SEND_CHUNK_##toxName, \
      QtTox::Files::ErrFileSendChunk::qtName }
    fillError(toxErr, err, {
        ERR(OK,                     Ok),
        ERR(NULL,                   Null),
        ERR(FRIEND_NOT_FOUND,       FriendNotFound),
        ERR(FRIEND_NOT_CONNECTED,   FriendNotConnected),
        ERR(NOT_FOUND,              NotFound),
        ERR(NOT_TRANSFERRING,       NotTransferring),
        ERR(INVALID_LENGTH,         InvalidLength),
        ERR(SENDQ,                  Sendq),
        ERR(WRONG_POSITION,         WrongPosition),
    });
#undef ERR
}

void onFileReceiveControl(Tox *tox, uint32_t friendNum, uint32_t fileNum, TOX_FILE_CONTROL control,
                          void *payload)
{
    auto service = static_cast<QtTox::Services*>(payload);
    QtTox::Files::FileControl toxControl = fileControlMap1.value(control);
    emit service->files->fileControlReceived(friendNum, fileNum, toxControl);
}

void onFileChunkRequest(Tox* tox, uint32_t friendNum, uint32_t fileNum,
                        uint64_t position, size_t length, void* payload)
{
    auto service = static_cast<QtTox::Services*>(payload);
    emit service->files->fileChunkRequest(friendNum, fileNum, position, length);
}

void onFileReceive(Tox *tox, uint32_t friendNum, uint32_t fileNum,
                   uint32_t kind, uint64_t file_size, const uint8_t *filename,
                   size_t filename_length, void *payload)
{
    auto service = static_cast<QtTox::Services*>(payload);
    // TODO(sudden6): remove the cast to TOX_FILE_KIND when the API definition is fixed
    QtTox::Files::FileKind toxKind = fileKindMap1.value(static_cast<TOX_FILE_KIND>(kind));
    ToxString toxFilename{filename, filename_length};
    emit service->files->fileReceived(friendNum, fileNum, toxKind, file_size, toxFilename.getQString());
}

void onFileReceiveChunk(Tox *tox, uint32_t friendNum, uint32_t fileNum,
                        uint64_t position, const uint8_t *data, size_t length,
                        void *payload)
{
    auto service = static_cast<QtTox::Services*>(payload);
    QByteArray chunk = bytes(data, length);
    emit service->files->fileChunkReceived(friendNum, fileNum, position, chunk);
}
}

namespace QtTox
{

QByteArray hash(const QByteArray &toHash)
{
    QByteArray hash{TOX_HASH_LENGTH, Qt::Initialization::Uninitialized};
    if(!tox_hash(data(hash), data(toHash), size(toHash))) {
        return {};
    }

    return hash;
}

Files::Files(struct Tox* tox)
{
    tox_callback_file_recv_control(tox, onFileReceiveControl);
    tox_callback_file_chunk_request(tox, onFileChunkRequest);
    tox_callback_file_recv(tox, onFileReceive);
    tox_callback_file_recv_chunk(tox, onFileReceiveChunk);
    this->tox = tox;
}

bool Files::fileControl(uint32_t friendNum, uint32_t fileNum, FileControl control, ErrFileControl *err)
{
    TOX_FILE_CONTROL ctrl = fileControlMap2.value(control);
    TOX_ERR_FILE_CONTROL toxErr;
    bool success = tox_file_control(tox, friendNum, fileNum, ctrl, &toxErr);
    fillErrFileControl(toxErr, err);
    return success;
}

bool Files::fileSeek(uint32_t friendNum, uint32_t fileNum, uint64_t position, Files::ErrFileSeek *err)
{
    TOX_ERR_FILE_SEEK toxErr;
    bool success = tox_file_seek(tox, friendNum, fileNum, position, &toxErr);
    fillErrFileSeek(toxErr, err);
    return success;
}

QByteArray Files::getFileId(uint32_t friendNum, uint32_t fileNum, Files::ErrFileGet *err)
{
    TOX_ERR_FILE_GET toxErr;
    QByteArray fileId(static_cast<int>(tox_file_id_length()),
                      Qt::Initialization::Uninitialized);
    bool success = tox_file_get_file_id(tox, friendNum, fileNum, data(fileId), &toxErr);
    fillErrFileGet(toxErr, err);
    if(!success) {
        return {};
    }

    return fileId;
}

uint32_t Files::fileSend(uint32_t friendNum, FileKind kind, uint64_t file_size,
                         const QByteArray &fileId, const QString &filename,
                         Files::ErrFileSend *err)
{
    TOX_ERR_FILE_SEND toxErr;
    TOX_FILE_KIND toxKind = fileKindMap2.value(kind);
    ToxString toxFilename{filename};
    uint32_t fileNum = tox_file_send(tox, friendNum, toxKind, file_size,
                                     data(fileId), toxFilename.data(),
                                     toxFilename.size(), &toxErr);
    fillErrFileSend(toxErr, err);
    return fileNum;
}

bool Files::fileSendChunk(uint32_t friendNum, uint32_t fileNum, uint32_t position, const QByteArray &fileData, Files::ErrFileSendChunk *err)
{
    TOX_ERR_FILE_SEND_CHUNK toxErr;
    bool success = tox_file_send_chunk(tox, friendNum, fileNum, position, data(fileData), size(fileData), &toxErr);
    fillErrFileSendChunk(toxErr, err);
    return success;
}

}
