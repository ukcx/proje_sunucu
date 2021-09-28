#include "gtest/gtest.h"
#include "server_operations.h"

static ServerOperations serverTester;
static SOCKET acceptedClientSocket;

TEST(ServerOperationsTest, InitializeSocketTest)
{
	//ServerOperations serverTester;

	ASSERT_TRUE(serverTester.initializeSocket("127.0.0.1", 194)) 
		<< "\nThe socket can not be initialized, probably winspock2 library is not working\n";
	//we should be able to initialize our socket
}

TEST(ServerOperationsTest, bindServerTest_withCorrectInitialization)
{
	//ServerOperations serverTester;
	serverTester.initializeSocket("127.0.0.1", 194);
	ASSERT_TRUE(serverTester.bindServer()) 
		<< "\nwe can not bind this server in localhost. Probably because the port is currently in use by some other program\n";
}

TEST(ServerOperationsTest, listenSocketTest_withCorrectInitializationAndBindServer)
{
	//ServerOperations serverTester;
	//serverTester.initializeSocket("127.0.0.1", 194);
	//serverTester.bindServer();

	ASSERT_TRUE(serverTester.listenSocket(SOMAXCONN)) //we shouldn't be able to listen to the socket since we didn't initialize the socket & we didn't bind the server 
		<< "\nlistenSocket() is not working\n";	
}

TEST(ServerOperationsTest, acceptClientTest)
{
	//ServerOperations serverTester;
	//serverTester.initializeSocket("127.0.0.1", 194);
	//serverTester.bindServer();
	//serverTester.listenSocket(SOMAXCONN);
	
	acceptedClientSocket = serverTester.acceptClient();
	ASSERT_FALSE(acceptedClientSocket == INVALID_SOCKET) 
		<< "\naccept client is not working! If initialization, bind server and listenSocket works correctly, this should not happen.\n";
}

TEST(ServerOperationsTest, receiveMessageTest)
{
	//ServerOperations serverTester;
	//serverTester.initializeSocket("127.0.0.1", 194);
	//serverTester.bindServer();
	//serverTester.listenSocket(SOMAXCONN);
	//acceptedClientSocket = serverTester.acceptClient();
	
	string messageToBeReceived = "";
	ASSERT_TRUE(serverTester.receiveMessage(acceptedClientSocket, std::ref(messageToBeReceived)))
		<< "\nMessage can not be recieved from the client. receiveMessage did not work\n";
}

TEST(ServerOperationsTest, sendMessageTest)
{
	//ServerOperations serverTester;
	//serverTester.initializeSocket("127.0.0.1", 194);
	//serverTester.bindServer();
	//serverTester.listenSocket(SOMAXCONN);
	//acceptedClientSocket = serverTester.acceptClient();
	
	string messageToSend = "";
	ASSERT_TRUE(serverTester.sendMessage(acceptedClientSocket, messageToSend)) 
		<< "\nCan not send any feedback to the client. sendMessage did not work\n";
}

/*
TEST(ServerOperationsTest, InitializeSocket_withInvalidIpAddressTest)
{
	//ServerOperations serverTester;

	ASSERT_TRUE(serverTester.initializeSocket("random string", 194));	//this won't give error since socket is technically still created, but we can not do anything with this socket
}
TEST(ServerOperationsTest, bindServerTest)
{
	ServerOperations server;

	ASSERT_FALSE(server.bindServer())
		<< "\nwe are binded to a server when socket is not initialized\n";
	//we shouldn't be able to bind this server since we didn't initialize it
}
TEST(ServerOperationsTest, bindServerTest_withIncorrectInitialization)
{
	//ServerOperations serverTester;
	serverTester.initializeSocket("random string", 194);
	ASSERT_FALSE(serverTester.bindServer())
		<< "\nwe are binded to a server when server ip address is invalid\n";
}


*/