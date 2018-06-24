#ifndef _QT_TOX_DATA_HELPER_H_
#define _QT_TOX_DATA_HELPER_H_

#include <cstdint>
#include <cstddef>
#include <QByteArray>

inline const uint8_t* data(const QByteArray& arr)
{
    return static_cast<const uint8_t*>(static_cast<const void*>(arr.data()));
}

inline uint8_t* data(QByteArray& arr)
{
    return static_cast<uint8_t*>(static_cast<void*>(arr.data()));
}

inline size_t size(const QByteArray& arr)
{
    return static_cast<size_t>(arr.size());
}

inline QByteArray bytes(const uint8_t * data, size_t len) {
    return QByteArray(static_cast<const char*>(static_cast<const void*>(data)), len);
}


#endif // _QT_TOX_DATA_HELPER_H_
