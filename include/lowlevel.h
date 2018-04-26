#ifndef _QT_TOX_LOW_LEVEL_H_
#define _QT_TOX_LOW_LEVEL_H_

#include <QObject>

struct Tox;

namespace QtTox
{
class LowLevel : public QObject
{
    Q_OBJECT

public:
    enum class ErrFriendCustomPacket
    {
        Ok,
        Null,
        FriendNotFound,
        FriendNotConnected,
        Invalid,
        Empty,
        TooLong,
        Sendq
    };

    bool friendSendLossyPacket(uint32_t friendNum, const QByteArray& data, ErrFriendCustomPacket* err = nullptr);
    bool friendSendLossLessPacket(uint32_t friendNum, const QByteArray& data, ErrFriendCustomPacket* err = nullptr);
    Q_SIGNAL void friendSendLossyPacketReceived(uint32_t friendNum, const QByteArray& data);
    Q_SIGNAL void friendSendLossLessPacketReceived(uint32_t friendNum, const QByteArray& data);

    QByteArray getDhtId() const;

    enum cass ErrGetPort
    {
        Ok,
        NotBound,
    };

    uint16_t getSelfUdpPort(ErrGetPort* err = nullptr);
    uint16_t getSelfTcpPort(ErrGetPort* err = nullptr);

private:
    struct Tox* tox;
};

#endif // _QT_TOX_LOW_LEVEL_H_
