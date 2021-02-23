#include "client.h"
#include <thread>

void cmdTrhead(TCPClient* client)
{
    char cmdBuffer[128] = { 0 };
    while (true)
    {
        memset(cmdBuffer, 0, sizeof(cmdBuffer));
        scanf("%s", cmdBuffer);

        if (0 == strcmp(cmdBuffer, "exit"))
        {
            printf("exit.\n");
            client->close();
            break;
        }
        else if (0 == strcmp(cmdBuffer, "login"))
        {
            Login login;
            if (SOCKET_ERROR == client->sendData(&login))
            {
                printf("failed: send login error.\n");
                break;
            }
        }
        else if (0 == strcmp(cmdBuffer, "logout"))
        {
            Logout logout;
            if (SOCKET_ERROR == client->sendData(&logout))
            {
                printf("failed: send logout error.\n");
                break;
            }
        }
        else
        {
            printf("error: unknown command.\n");
        }
    }
}

int main(int argc, const char* argv[])
{
    TCPClient client;
    client.init();
    client.connect("127.0.0.1", 9981);
    // client.connect("172.168.8.128", 9981);

    // UI interactive thread;
    std::thread thread(cmdTrhead, &client);
    thread.detach();

    // Network transport thread;
    while (client.isValid())
    {
        client.onMessage();
    }

    return 0;
}

/* 测试程序；
int main(int argc, const char* argv[])
{
    TCPClient client1;
    client1.init();
    client1.connect("127.0.0.1", 9981);

    TCPClient client2;
    client2.init();
    client2.connect("140.143.224.41", 9981); // 腾讯云服务器；

    // User interactive thread;
    std::thread thread1(cmdTrhead, &client1);
    thread1.detach();

    std::thread thread2(cmdTrhead, &client2);
    thread2.detach();

    // Network transport thread;
    while (client1.isValid() || client2.isValid())
    {
        client1.onMessage();
        client2.onMessage();
    }

    return 0;
}
*/