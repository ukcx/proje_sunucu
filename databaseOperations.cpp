
#include "messagePrinter.h"

using namespace std;

/*
* Default constructor
* Veritabanina baglanmak icin gerekli bilgileri kullanarak veritabani ogesini olustur
*/
DatabaseOperations::DatabaseOperations(const char* h, const char* u, const char* pword, const char* name,
	unsigned int p, const char* socket, unsigned long flag)
{
	host = h;
	user = u;
	password = pword;
	databaseName = name;
	port = p;
	unix_socket = socket;
	clientflag = flag;

	mysql_init(&mysql_object);
	connection = NULL;
}

/*
* Veritabanina baglan, eger baglanti basariliysa true dondur, aksi takdirde false dondur
*/
bool DatabaseOperations::connectToDatabase()
{
	connection = mysql_real_connect(&mysql_object, host, user, password, databaseName, port, unix_socket, clientflag);
	if (connection == NULL) // eger baglanti saglanmadiysa
	{
		return false;
	}
	else					// Baglanti basarili ise
	{
		return true;
	}
}

/*
*
*/
void DatabaseOperations::writeMessage(Message msg)
{
	string messageAsString = msg.messageToString();

	string mesajdaki_bilgiler[5];
	string mesajin_kalan_kismi = messageAsString;	//mesajin bir bolumu cikarilinca kalan kismi tutar
	int len = messageAsString.length();
	for (int i = 0; i < 5; i++)				//mesajin icerisindeki bilgileri parca parca mesajdaki_bilgiler'e aktar
	{
		mesajdaki_bilgiler[i] = mesajin_kalan_kismi.substr(0, mesajin_kalan_kismi.find('\n'));
		mesajin_kalan_kismi = mesajin_kalan_kismi.substr(mesajin_kalan_kismi.find('\n') + 1, len);
	}

	string tableName = msg.getPriorityInfo() + "_priority_messages";		//veritabanindaki tablo adlari
	string dbName = databaseName;

	string query = "INSERT INTO " + dbName + "." + tableName
		+ "(message_to, message_cc, message_subject, message_body, message_priority) VALUES("
		+ "\"" + mesajdaki_bilgiler[0] + "\",\"" + mesajdaki_bilgiler[1] + "\",\"" + mesajdaki_bilgiler[2]
		+ "\",\"" + mesajdaki_bilgiler[3] + "\",\"" + mesajdaki_bilgiler[4] + "\")";


	if (mysql_query(connection, query.c_str()) == 0)		//verilen sorguyu yurut. Eger dogruysa 0 dondurur.
	{
		cout << "Mesaj veritabanina yazdirildi." << endl;
	}
	else
	{
		cout << "Mesaji veritabanina yazdirirken sorun olustu!" << endl;
	}
}
