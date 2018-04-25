#ifndef _TOX_OPTIONS_H_
#define _TOX_OPTIONS_H_

namespace QtTox
{

class Options
{

    enum class ToxProxyType
    {
        None,
        Http,
        Socks5,
    };

    enum class ToxSaveDataType
    {
        None,
        ToxSave,
        SecretKey,
    };

    enum class ToxLogLevel
    {
        Trace,
        Debug,
        Info,
        Warning,
        Error,
    };

};

}

#endif // _TOX_OPTIONS_H_
