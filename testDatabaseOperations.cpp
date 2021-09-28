#include "gtest/gtest.h"
#include "databaseOperations.h"
#include "databaseOperations.cpp"

static SingletonDatabaseOperations * dbOperatorTester;

TEST(DatabaseOperationsTests, GetInstance_isItNullptrTest)
{
	//SingletonDatabaseOperations* dbOperatorTester;
	dbOperatorTester = SingletonDatabaseOperations::GetInstance();

	ASSERT_FALSE(dbOperatorTester == nullptr) << "\nobject can not be created! Check the first visit to the GetInstance() function.\n";
}

TEST(DatabaseOperationsTests, GetInstance_doWeGetTheSameObjectEveryTimeTest)
{
	//SingletonDatabaseOperations* dbOperatorTester;
	//dbOperatorTester = SingletonDatabaseOperations::GetInstance();
	SingletonDatabaseOperations* secondDbTester;
	secondDbTester = SingletonDatabaseOperations::GetInstance();

	ASSERT_TRUE(dbOperatorTester == secondDbTester) << "\nsingleton is not working! we get different objects at different calls to GetInstance()\n";
}

TEST(DatabaseOperationsTests, CheckConnectionTest)
{
	//SingletonDatabaseOperations* dbOperatorTester;
	//dbOperatorTester = SingletonDatabaseOperations::GetInstance();

	ASSERT_FALSE(dbOperatorTester->checkConnection()) << "\nthe connection is not initialized as NULL\n";
}

TEST(DatabaseOperationsTests, connectToDatabaseTest)
{
	//SingletonDatabaseOperations* dbOperatorTester;
	//dbOperatorTester = SingletonDatabaseOperations::GetInstance();

	ASSERT_TRUE(dbOperatorTester->connectToDatabase("localhost", "root", "password", "messages", 3306, 0, 0)) << "\ncan not connect to the database. Probably because some information for connection is given wrong.\n";
}

TEST(DatabaseOperationsTests, CheckConnection_afterCallToconnectToDatabaseTest)
{
	//SingletonDatabaseOperations* dbOperatorTester;
	//dbOperatorTester = SingletonDatabaseOperations::GetInstance();

	ASSERT_TRUE(dbOperatorTester->checkConnection()) << "\nCAN NOT CONNECT TO THE DATABASE\n";
}

TEST(DatabaseOperationsTests, writeMessageAfterConnectionTest)
{
	//SingletonDatabaseOperations* dbOperatorTester;
	//dbOperatorTester = SingletonDatabaseOperations::GetInstance();
	//dbOperatorTester->connectToDatabase("localhost", "root", "password", "messages", 3306, 0, 0);
	
	Message msg("t", "c", "s", "b", High);
	bool isMessageWritten = dbOperatorTester->writeMessage(msg);

	MYSQL* mysql_object = nullptr; 
	mysql_object = mysql_init(mysql_object);
	
	MYSQL* connection = nullptr;
	connection = mysql_real_connect(mysql_object, "localhost", "root", "password", "messages", 3306, 0, 0);
	
	string tableName = msg.getPriorityInfo() + "_priority_messages";		//veritabanindaki tablo adlari

	string query = "SELECT * FROM messages." + tableName
		+ " WHERE message_to = " + "\"t\"" + " AND "
		+ "message_cc = " + "\"c\"" + " AND "
		+ "message_subject = " + "\"s\"" + " AND "
		+ "message_body = " + "\"b\"" + " AND "
		+ "message_priority = " + "\"High\";";

	MYSQL_RES* result = nullptr;
	if(mysql_query(connection, query.c_str()) == 0)
	{
		result = mysql_store_result(connection);
	}

	ASSERT_TRUE(isMessageWritten && (result != nullptr)	)
		<< "\ncan not write message in the database. Check connection if connection is made correctly, check writeMessage() function.\n";
}

/*
TEST(DatabaseOperationsTests, writeMessageBeforeConnectionTest)
{
	//SingletonDatabaseOperations* dbOperatorTester;
	//dbOperatorTester = SingletonDatabaseOperations::GetInstance();

	Message msg("t", "c", "s", "b", High);
	ASSERT_FALSE(dbOperatorTester->writeMessage(msg)) << "\nthe connection is not initialized as NULL, we are able to send message before connecting to the database\n";
}
*/