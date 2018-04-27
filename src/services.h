#ifndef _QT_TOX_SERVICES_H_
#define _QT_TOX_SERVICES_H_

namespace QtTox
{

class Messenger;
class ChatList;

struct Services
{
    Messenger* messenger;
    ChatList* chatList;
};

}

#endif // _QT_TOX_SERVICES_H_
