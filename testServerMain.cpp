#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS         //inet_addr kullanmak icin
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#pragma comment(lib, "Ws2_32.lib")              //winsock2'yi linkleyebilmek icin
#include <winsock2.h>

#include "testDatabaseOperations.cpp"
#include "testFileOperations.cpp"
#include "testMessage.cpp"
#include "testServerOperations.cpp"
#include <gtest/gtest.h>


int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}