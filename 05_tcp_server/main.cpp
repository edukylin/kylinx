#include "server.h"

int main(int argc, char* argv[])
{
    TCPServer server;
    // 服务端初始监听；
    server.init();
    server.bind();
    server.listen(20);
    // 服务器消息循环；
    while (server.isValid())
    {
        server.onMessage();
    }
    // 服务器关闭连接；
    server.close();
    return 0;
}