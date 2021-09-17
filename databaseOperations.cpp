
#include "databaseOperations.h"

using namespace std;

unique_ptr<SingletonDatabaseOperations> SingletonDatabaseOperations::singleton = nullptr;
//singleton variable static oldugu icin hafizada belli bir adrese atanabilmesi icin 1 kez aciklanmasi gerekli

/*
* Veritabanina baglan, eger baglanti basariliysa true dondur, aksi takdirde false dondur
*/
bool SingletonDatabaseOperations::connectToDatabase(const char* host, const char* user, const char* password,
	const char* databaseName, unsigned int port, const char* unix_socket, unsigned long clientflag)
{
	connection = nullptr;

	connection = mysql_real_connect(mysql_object, host, user, password, databaseName, port, unix_socket, clientflag);
	
	return checkConnection();
}

/*
* Veritabanina baglantiyi kontrol et, baglanti varsa true, yoksa false dondur
*/
bool SingletonDatabaseOperations::checkConnection()
{
	if (connection == NULL)		// eger baglanti saglanmadiysa
	{
		return false;
	}
	else						// Baglanti basarili ise
	{
		return true;
	}
}

/*
* Interface class'tan gelen fonksiyon.
* Mesaji oncelik seviyesine gore veritabanindaki gerekli tabloya yazdirir.
*/
bool SingletonDatabaseOperations::writeMessage(Message msg)
{
	if(connection == nullptr)
	{
		return false;
	}

	string messageAsString = msg.messageToString();

	string mesajdaki_bilgiler[5];
	string mesajin_kalan_kismi = messageAsString;	//mesajin bir bolumu cikarilinca kalan kismi tutar
	int len = messageAsString.length();
	for (int i = 0; i < 5; i++)				//mesajin icerisindeki bilgileri parca parca mesajdaki_bilgiler'e aktar
	{
		mesajdaki_bilgiler[i] = mesajin_kalan_kismi.substr( 0, mesajin_kalan_kismi.find('\n'));
		mesajin_kalan_kismi = mesajin_kalan_kismi.substr(mesajin_kalan_kismi.find('\n') + 1 , len);
	}

	string tableName = msg.getPriorityInfo() + "_priority_messages";		//veritabanindaki tablo adlari

	string query = "INSERT INTO messages." + tableName
		+ "(message_to, message_cc, message_subject, message_body, message_priority) VALUES(" 
		+ "\"" + mesajdaki_bilgiler[0] + "\",\"" + mesajdaki_bilgiler[1] + "\",\"" + mesajdaki_bilgiler[2] 
		+ "\",\"" + mesajdaki_bilgiler[3] + "\",\"" + mesajdaki_bilgiler[4] + "\")";


	if (mysql_query(connection, query.c_str()) == 0)		//verilen sorguyu yurut. Eger dogruysa 0 dondurur.
	{
		cout << "Mesaj veritabanina yazdirildi." << endl;
		return true;
	}
	else
	{
		cout << "Mesaji veritabanina yazdirirken sorun olustu!" << endl;
		return false;
	}
}
