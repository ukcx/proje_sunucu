#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS         //inet_addr kullanmak icin
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif


#ifndef SERVER_OPERATIONS_H
#define SERVER_OPERATIONS_H

#pragma comment(lib, "Ws2_32.lib")              //winsock2'yi linkleyebilmek icin
#include <winsock2.h>
#include <string.h>
#include "message.h"

constexpr auto MAX_MSG_SIZE = 1000;

class ServerOperations
{
public:
	ServerOperations();
	~ServerOperations();
    bool initializeSocket(string address, int port);
    bool bindServer();
    bool listenSocket(int maxNumOfClients);
    SOCKET acceptClient();
    bool receiveMessage(SOCKET socket, string& msg);
    bool sendMessage(SOCKET socket, string msg);

private:
    SOCKET createSocket();

    SOCKET server_socket;
    string server_ip_address;
    sockaddr_in server_information;
    int port;
};

ServerOperations::ServerOperations()
{}

ServerOperations::~ServerOperations()
{
    closesocket(server_socket);
}

bool ServerOperations::initializeSocket(string address, int p)
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return false;

    server_ip_address = address;
    port = p;
    
    server_socket = createSocket();
    
    if (server_socket == INVALID_SOCKET)
        return false;

    return true;
}

SOCKET ServerOperations::createSocket()
{
    memset(&server_information, 0, sizeof(server_information));
    server_information.sin_family = AF_INET;                                  //IPv4
    server_information.sin_addr.s_addr = inet_addr(server_ip_address.c_str());   //sunucu IP adresi
    server_information.sin_port = htons(port);

    return socket(AF_INET, SOCK_STREAM, 0);     //SOCK_STREAM TCP/IP protokolune uygun olan sabit
}

bool ServerOperations::listenSocket(int maxNumOfClients)
{
    if ((maxNumOfClients > SOMAXCONN) || (listen(server_socket, maxNumOfClients) < 0) )   // SOMAXCONN = maximum reasonable value
    {
        return false;
    }
    else {
        return true;
    }
}

bool ServerOperations::bindServer()
{
    if (bind(server_socket, (sockaddr*)&server_information, sizeof(server_information)) < 0) //soket uzerinden iletisim icin sunucu sokete baglaniyor
    {
        return false;
    }
    else {
        return true;
    }
}

SOCKET ServerOperations::acceptClient()
{
    int client_len;
    sockaddr client_addr;   //sadece accept fonksiyonunun calismasi icin gerekli gecici variable, sonradan kullanilmayacak
    client_len = sizeof(client_addr);

    return accept(server_socket, (sockaddr*)&client_addr, &client_len);
}

bool ServerOperations::sendMessage(SOCKET socket, string msg)
{
    if (send(socket, msg.c_str(), msg.length(), 0) < 0)
    {
        return false;
    }
    else
        return true;
}

bool ServerOperations::receiveMessage(SOCKET socket, string &msg)
{
    char received_message[MAX_MSG_SIZE];
    memset(&received_message, 0, MAX_MSG_SIZE);                 //eger memset kullanilmazsa alinan_mesaj'in dolmayan kismi
                                                            //sadece rastgele karakterlerle dolacaktir.
                                                            //memset alinan_mesaj MSG_SIZE'dan kisaysa bu problemi cozuyor
    if (recv(socket, received_message, MAX_MSG_SIZE, 0) < 0)
    {
        return false;
    }
    else
    {
        msg = received_message;
        return true;
    }
}
#endif

