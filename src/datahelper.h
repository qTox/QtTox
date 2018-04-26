#ifndef _QT_TOX_DATA_HELPER_H_
#define _QT_TOX_DATA_HELPER_H_

inline const uint8_t* data(const QByteArray& arr)
{
    return static_cast<const uint8_t*>(static_cast<const void*>(arr.data()));
}

inline uint8_t* data(QByteArray& arr)
{
    return static_cast<uint8_t*>(static_cast<void*>(arr.data()));
}

#endif // _QT_TOX_DATA_HELPER_H_
