#include "server.h"

int main(int argc, char* argv[])
{
    TCPServer server;
    // ����˳�ʼ������
    server.init();
    server.bind();
    server.listen(20);
    // ��������Ϣѭ����
    while (server.isValid())
    {
        server.onMessage();
    }
    // �������ر����ӣ�
    server.close();
    return 0;
}