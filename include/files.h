#ifndef _Q_TOX_FILES_H_
#define _Q_TOX_FILES_H_

#include <QObject>

struct Tox;

namespace QtTox
{

QByteArray hash(const QByteArray& data);

class Files : public QObject
{
    Q_OBJECT

public:
    enum class FileKind
    {
        Data,
        Avatar,
    };

    enum class FileControl
    {
        Resume,
        Pause,
        Cancel,
    };

    enum class ErrFileControl
    {
        Ok,
        FriendNotFound,
        FriendNotConnected,
        NotFound,
        NotPaused,
        Denied,
        AlreadyPaused,
        Sendq,
    };

    bool fileControl(uint32_t friendNum, uint32_t fileNum, FileControl control, ErrFileControl* err = nullptr);
    Q_SIGNAL void fileControlReceived(uint32_t friendNum, uint32_t fileNum, FileControl control);

    enum class ErrFileSeek
    {
        Ok,
        FriendNotFound,
        FriendNotConnected,
        NotFound,
        Denied,
        InvalidPosition,
        Sendq,
    };

    bool fileSeek(uint32_t friendNum, uint32_t fileNum, uint64_t position, ErrFileSeek* err = nullptr);

    enum class ErrFileGet
    {
        Ok,
        Null,
        FriendNotFound,
        NotFound,
    };

    QByteArray getFileId(uint32_t friendNum, uint32_t fileNum, ErrFileGet* err = nullptr);

    // File transmission: sending

    enum class ErrFileSend
    {
        Ok,
        Null,
        FriendNotFound,
        FriendNotConnected,
        NameTooLong,
        TooMany,
    };

    uint32_t fileSend(uint32_t friendNum, FileKind kind, uint64_t file_size, const QByteArray& fileId,
            const QString& filename, ErrFileSend* err = nullptr);

    enum class ErrFileSendChunk
    {
        Ok,
        Null,
        FriendNotFound,
        FriendNotConnected,
        NotFound,
        NotTransferring,
        InvalidLength,
        Sendq,
        WrongPosition,
    };

    bool fileSendChunk(uint32_t friendNum, uint32_t fileNum, uint32_t position,
            const QByteArray& data, ErrFileSendChunk* err = nullptr);

    Q_SIGNAL void fileChunkRequest(uint32_t friendNum, uint32_t fileNum, uint64_t position, size_t length);

    // File transmission: receiving
    
    Q_SIGNAL void fileReceived(uint32_t friendNum, uint32_t fileNum, FileKind kind,
            uint64_t fileSize, const QString& filename);
    Q_SIGNAL void fileChunkReceived(uint32_t friendNum, uint32_t fileNum, uint64_t position,
            const QByteArray& data);

    Files(Tox *tox);
private:
    struct Tox* tox;
};

}

#endif // _Q_TOX_FILES_H_
