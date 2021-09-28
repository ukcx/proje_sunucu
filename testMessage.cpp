#include "gtest/gtest.h"
#include "message.h"

TEST(MessageTests, getPriorityTest)
{
	Message msg("t", "c", "s", "b", Medium);
	ASSERT_EQ(msg.getPriority(), Medium);
}

TEST(MessageTests, getPriorityInfoTest)
{
	Message msg("t", "c", "s", "b", Medium);
	ASSERT_EQ(msg.getPriorityInfo(), "Medium");
}

TEST(MessageTests, printMessageTest)
{
	Message msg("t", "c", "s", "b", Medium);

	std::ifstream ifs;
	std::ofstream os;
	os.open("TextFile.txt");
	msg.printMessage(os);
	os.close();

	ifs.open("TextFile.txt");
	string printedMessage, line;
	while (getline(ifs, line))
	{
		printedMessage += line;
		printedMessage += "\n";
	}
	ifs.close();

	ASSERT_EQ(printedMessage, "To: t\nCc: c\nSubject: s\nBody: b\nPriority: Medium\n");
}

TEST(MessageTests, messageToStringTest)
{
	Message msg("t", "c", "s", "b", Medium);
	ASSERT_EQ(msg.messageToString(), "t\nc\ns\nb\nMedium\n");
}

TEST(MessageTests, convertStringToPriorityLevelTest_whenThrowsException)
{
	string pLevel = "random string";
	EXPECT_THROW({
		try
		{
			convertStringToPriorityLevel(pLevel);	//we shouldn't get any exception when the given string is a valid priority level
		}											//but the given string is not a valid priorityLevel in this occasion
		catch (const char* e)
		{
			throw;
		}
		}, const char*);
}

TEST(MessageTests, convertStringToPriorityLevelTest_whenInputIsCorrect)
{
	string pLevel_2 = "low";	//the convertStringToPriorityLevelTest function checks the values case insensitive

	ASSERT_EQ(convertStringToPriorityLevel(pLevel_2), Low);
}

TEST(MessageTests, stringToMessageTest)		//tam test edilemedi
{
	string messageAsString = "t\nc\ns\nb\nMedium\n";
	Message msg("t", "c", "s", "b", Medium);
	ASSERT_EQ(stringToMessage(messageAsString).messageToString(), msg.messageToString());
}