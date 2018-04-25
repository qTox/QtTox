#ifndef _QT_TOX_CORE_H_
#define _QT_TOX_CORE_H_

struct Tox;

namespace QtTox
{

class Core
{
public:
    static constexpr uint32_t VersionMajor;
    static constexpr uint32_t VersionMinor;
    static constexpr uint32_t VersionPatch;

    static constexpr bool isAPICompatiable(uint32_t major, uint32_t minor, uint32_t patch);

    static constexpr uint32_t PublicKeySize;
    static constexpr uint32_t SecretKeySize;
    static constexpr uint32_t NospamSize;
    static constexpr uint32_t AddressSize;

    static constexpr uint32_t MaxNameLength;
    static constexpr uint32_t MaxStatusMessageLength;
    static constexpr uint32_t MaxFriendRequestLength;
    static constexpr uint32_t MaxMessageLength;
    static constexpr uint32_t MaxCustomPacketSize;
    static constexpr uint32_t MaxFilenameLength;

    static constexpr uint32_t HashLength;
    static constexpr uint32_t FileIdLength;

    enum class UserStatus
    {
        None,
        Away,
        Busy,
    };

    enum class MesageType
    {
        Normal,
        Action,
    };

    enum class ErrNew
    {
        Ok,
        Null,
        Malloc,
        PortAlloc,
        ProxyBadType,
        ProxyBadHost,
        PorxyBadPort,
        ProxyNotFound,
        LoadEncrypted,
        LoadBadFormat,
    }

    Core(const Options& options, ErrNew* error = nullptr);
    ~Core();

    QByteArray getSavedata();

    enum class ErrBootstrap
    {
        Ok,
        Null,
        BadHost,
        BadPort
    };
    bool bootstrap(const QString& address, uint16_t port, 
            const QByteArray& publicKey, ErrBootstrap* err = nullptr);
    bool addTcpReleay(const QString& address, uint16_t port, 
            ErrBootstrap* err = nullptr);

    enum class Connection
    {
        None,
        Tcp,
        Udp,
    };

    Connection getSelfConnectionStatus() const;
    Q_SIGNAL void selfConnectionStatusChanged(Connection status);

    uint32_t iterationInterval() const;
    void iterate();
    
    QByteArray getSelfAddress() const;

    QByteArray getNospam() const;
    void setNospam(const QByteArray& nospam);

    QByteArray getSelfPublicKey() const;
    QByteArray getSelfSecretKey() const;

    // User-visible client information (nickname/status)

    enum class ErrSetInfo
    {
        Ok,
        Null,
        TooLong
    };

    bool setSelfName(const QString& name, ErrSetInfo* err = nullptr);
    QString getSelfName() const;

    bool setSelfStatusMessage(const QString& statusMessage, ErrSetInfo* err = nullptr)
    QString getSelfStatusMessage() const;

    bool setSelfStatus(UserStatus status);
    UserStatus getSelfStatus() const;

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

    // Friend list queries

    enum class ErrFriendByPublicKey
    {
        Ok,
        Null,
        KeyNotFound,
    };
    uint32_t friendByPublicKey(const QByteArray& publicKey. ErrFriendByPublicKey* err = nullptr) const;

    bool friendExists(uint32_t friendNumer) const;
    QVector<uint32_t> getFriendList() const;

    // Conference management

    enum class ConferenceType
    {
        Text,
        AV,
    };

    Q_SIGNAL void conferenceInvited(uint32_t conferenceNum, ConferenceType type)
    Q_SIGNAL void conferenceMessageReceived(uint32_t conferenceNum, uint32_t peerNum,
            MessageType type, const QString& message);
    Q_SIGNAL void conferenceTitleChanged(uint32_t conferenceNum, uint32_t peerNum, const QString& title);
    Q_SIGNAL void conferencePeerNameChanged(uint32_t conferenceNum, uint32_t peerNum, const QString& name);

    Q_SIGNAL void conferencePeerListChanged(uint32_t conferenceNum);

    enum class ErrConferenceNew
    {
        Ok,
        Init,
    };

    uint32_t conferenceNew(ErrConferenceNew* err = nullptr);

    enum class ErrConferenceDelete
    {
        Ok,
        ConferenceNotFound,
    };

    uint32_t conferenceDelete(uint32_t conferenceNum, ErrConferenceDetele* err = nullptr);

    enum class ErrConferencePeerQuery
    {
        Ok,
        ConferenceNotFound,
        PeerNotFound,
        NoConnection,
    };

    uint32_t getConferencePeerCount(uint32_t conferenceNum, ErrConferencePeerQuery* err = nullptr) const;
    QString getConferencePeerName(uint32_t conferenceNum, uint32_t peerNum, const QString& name) const;
    QByteArray getConferencePeerPublicKey(uint32_t conferenceNum, uint32_t peerNum, ErrConferencePeerQuery* err = nullptr) const;
    bool conferencePeerNumberIsOurs(uint32_t conferenceNum, uint32_t peerNum, ErrConferencePeerQuery* err = nullptr) const;

    enum class ErrConferenceInvite
    {
        Ok,
        ConferenceNotFound,
        FailSend,
    };

    bool conferenceInvite(uint32_t friendNum, uint32_t conferenceNum, ErrConferenceInvite* err = nullptr);

    enum class ErrConferenceJoin
    {
        Ok,
        InvalidLength,
        WrongType,
        FriendNotFound,
        Duplicate,
        InitFail,
        FailSend,
    };

    uint32_t conferenceJoin(uint32_t friendNum, const QByteArray& cookie, ErrConferenceJoin* err = nullptr);

    enum class ErrConferenceSendMessage
    {
        Ok,
        ConferenceNotFound,
        TooLong,
        NoConnection,
        FailSend,
    };

    bool conferenceSendMessage(uint32_t conferenceNum, MessageType type, const Qtring& message,
            ErrConferenceSendMessage* err = nullptr);


    enum class ErrConferenceTitle
    {
        Ok,
        ConferenceNotFound,
        InvalidLength,
        FailSend,
    };

    QString getConferenceTitle(uint32_t conferenceNum, ErrConferenceTitle* err = nullptr) const;
    bool setConferenceTitle(uint32_t conferenceNum, const QString& title, ErrConferenceTitle* err = nullptr);

    QVector<uint32_t> getConferenceChatlist() const;

    enum class ErrConferenceGetType
    {
        Ok,
        ConferenceNotFound,
    };

    ConferenceType getConferenceType(uint32_t conferenceNum, ErrConferenceGetType* err = nullptr);

    // Low-level custom packet sending and receiving

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

    // Low-level network information

    QByteArray getDhtId() const;

    enum cass ErrGetPort
    {
        Ok,
        NotBound,
    };

    uint16_t getSelfUdpPort(ErrGetPort* err = nullptr);
    uint16_t getSelfTcpPort(ErrGetPort* err = nullptr);

signals:
    void Log(LogLevel level, const QString& file, uint32_t line,
            const QString& func, const QString& message);


private:
    struct Tox* tox;
};

}

#endif // _Q_TOX_CORE_H_
