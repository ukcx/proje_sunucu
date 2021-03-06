#include "gtest/gtest.h"
#include "fileOperations.h"
#include "fileOperations.cpp"
#include <sstream>

static SingletonFileOperations* fileOperatorTester;

TEST(FileOperationsTests, GetInstance_isItNullptrTest)
{
	//SingletonFileOperations* fileOperatorTester;
	fileOperatorTester = SingletonFileOperations::GetInstance();

	ASSERT_FALSE(fileOperatorTester == nullptr) << "\nobject can not be created! Check the first visit to the GetInstance() function.\n";;
}

TEST(FileOperationsTests, GetInstance_doWeGetTheSameObjectEveryTimeTest)
{
	//SingletonFileOperations* fileOperatorTester;
	//fileOperatorTester = SingletonFileOperations::GetInstance();
	SingletonFileOperations* secondFileOperator;
	secondFileOperator = SingletonFileOperations::GetInstance();

	ASSERT_TRUE(fileOperatorTester == secondFileOperator) << "\nsingleton is not working! we get different objects at different calls to GetInstance()\n";;
}
//path ve filename icin test yazamadim

TEST(FileOperationsTests, WriteMessageTest_whenPathAndFileNameIsSet)
{
	//SingletonFileOperations* fileOperatorTester;
	//fileOperatorTester = SingletonFileOperations::GetInstance();
	fileOperatorTester->setPath("D:\\Users\\ugurkagan\\Desktop\\");
	fileOperatorTester->setFileName("textFile.txt");

	Message msg("t", "c", "s", "b", Low);

	std::ofstream ofs;
	ofs.open("D:\\Users\\ugurkagan\\Desktop\\textFile.txt", fstream::out, fstream::trunc);
	ofs.close();

	bool isMessageWritten = fileOperatorTester->writeMessage(msg);

	std::ifstream ifs;
	ifs.open("D:\\Users\\ugurkagan\\Desktop\\textFile.txt");
	string printedMessage, line;
	while (getline(ifs, line))
	{
		printedMessage += line;
		printedMessage += "\n";
	}
	ifs.close();

	ASSERT_TRUE(isMessageWritten &&  (printedMessage == "Message\nTo: t\nCc: c\nSubject: s\nBody: b\nPriority: Low\n\n"))
	<< "\ncan not write message even though path and file name is given. Check if path exists and check if file name has a valid extension.\n";
}

/*
TEST(FileOperationsTests, WriteMessageTest_whenNoFileNameAndPathIsSet)
{
	//SingletonFileOperations* fileOperatorTester;
	//fileOperatorTester = SingletonFileOperations::GetInstance();

	Message msg("t", "c", "s", "b", Low);

	ASSERT_FALSE(fileOperatorTester->writeMessage(msg)) << "\npath and fileName is somehow not initialized as empty strings.\n";
}

TEST(FileOperationsTests, WriteMessageTest_whenNoFileNameIsSet)
{
	//SingletonFileOperations* fileOperator;
	//fileOperator = SingletonFileOperations::GetInstance();
	fileOperatorTester->setPath("C:\\Users\\Administrator\\Desktop\\");

	Message msg("t", "c", "s", "b", Low);
	ASSERT_FALSE(fileOperatorTester->writeMessage(msg)) << "\nwe are able to write to the file without knowing the file name.\n";
}

TEST(FileOperationsTests, WriteMessageTest_whenNoPathIsSet)
{
	//SingletonFileOperations* fileOperatorTester;
	//fileOperatorTester = SingletonFileOperations::GetInstance();
	fileOperatorTester->setFileName("textFile.txt");

	Message msg("t", "c", "s", "b", Low);
	ASSERT_TRUE(fileOperatorTester->writeMessage(msg)) << "\ncan not write message inside current directory\n";

	//we expect to just print the file inside current path
}
*/