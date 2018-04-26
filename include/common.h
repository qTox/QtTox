#ifndef _QT_TOX_COMMON_H_
#define _QT_TOX_COMMON_H_

namespace QtTox
{

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

enum class Connection
{
    None,
    Tcp,
    Udp,
};

enum class MesageType
{
    Normal,
    Action,
};

}

#endif // _QT_TOX_COMMON_H_
