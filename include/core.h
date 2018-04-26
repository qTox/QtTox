#ifndef _QT_TOX_CORE_H_
#define _QT_TOX_CORE_H_

#include <QObject>

struct Tox;

namespace QtTox
{

class ChatList;
class Conference;
class Files;
class LowLevel;
class Messenger;
class Options;
class Self;

class Core : public QObject
{
    Q_OBJECT

public:

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

    uint32_t iterationInterval() const;
    void iterate();
    
signals:
    void Log(LogLevel level, const QString& file, uint32_t line,
            const QString& func, const QString& message);


private:
    struct Tox* tox;
    ChatList* chatList;
    Conference* conference;
    Files* files;
    LowLevel* lowLevel;
    Messenger* messenger;
    Options* options;
    Self* self;
};

}

#endif // _Q_TOX_CORE_H_
