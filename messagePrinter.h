#ifndef MESSAGE_PRINTER
#define MESSAGE_PRINTER

#include "message.h"
#include <string>
#include <vector>
#include <mysql.h>
using namespace std;

/*
* Interface class. Bir mesaji yazdirmak icin genel fonksiyon seklinde tanimlandi.
* Dosyaya yazdirmak veya veritabanina yazdirmak icin kullanilabilir.
*/
class PrintMessage
{
public:
	virtual void writeMessage(Message msg) = 0;
};



/*
* Dosyalarla ilgili islemler icin class
* verilen path'e dosya adina gore mesaj yazdirabilir
*/
class FileOperations : public PrintMessage
{
private:
	string path;				//yazdirilicak dosyanin bulundugu path

	vector<string> fileNames;	/*
								yazdirilacak dosya isimleri, priorityLevel seviyelerine gore siralanmis halde kullanicidan alinmalidir
								ornek: fileNames[0]'da (eger Low en dusuk degerdeyse) Low oncelik seviyesindeki dosya adi olmalidir
								her bir priorityLevel ogesi icin 1 dosya adi olmalidir
								*/

public:
	FileOperations(string pathToFolder, vector<string> fNames);		//path ve dosya adlariyla nesneyi olustur
	bool changeFileNames(vector<string> fNames);					//dosya adlarini degistir
	void changePath(string pathToFolder);							//path'i degistir
	void writeMessage(Message msg);									//mesaji kendi oncelik seviyesine gore verilen path'e ve uygun dosya adi olan dosyaya yazdir
};



/*
* Veritabani ile ilgili islemler icin class
* Verilen veritabaninda uygun tabloya mesaji yazdirabilir
*/
class DatabaseOperations : public PrintMessage
{
private:
	const char* host;				//baglanti icin gerekli degiskenler
	const char* user;
	const char* password;
	const char* databaseName;
	unsigned int port;
	const char* unix_socket;
	unsigned long clientflag;
	MYSQL  mysql_object;			// Islemler icin kullanilacak mysql nesnesi
	MYSQL* connection;				// Baglanti pointer'i

public:
	DatabaseOperations(const char* host, const char* user, const char* password, const char* databaseName,
		unsigned int port, const char* unix_socket, unsigned long clientflag);

	void writeMessage(Message msg);					//dosya adini bulmak icin "message.h" dosyasindaki priority_map'i kullanir
	bool connectToDatabase();						//veritabanina baglanamazsa false dondurur
};
/*bool doesDatabaseExists(string database_name);
bool createDatabase(string database_name);
bool createTables();*/

#endif 


