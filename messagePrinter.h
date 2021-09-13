#ifndef MESSAGE_PRINTER
#define MESSAGE_PRINTER

#include "message.h"
#include <string>
#include <mysql.h>
#include <memory>
using namespace std;

/*
* Interface class. Bir mesaji yazdirmak icin genel fonksiyon seklinde tanimlandi.
* Dosyaya yazdirmak veya veritabanina yazdirmak icin kullanilabilir.
*/
class PrintMessage
{
public:
	virtual bool writeMessage(Message msg) = 0;
};


/*
* Dosyalarla ilgili islemler icin class
* verilen path'e ve dosya adina gore mesaj yazdirabilir
*/
class SingletonFileOperations : public PrintMessage
{
public:
	void setFileName(string fName);				//dosya adlarini degistir
	void setPath(string pathToFolder);			//path'i degistir
	bool writeMessage(Message msg);				//mesaji kendi oncelik seviyesine gore verilen path'e ve uygun dosya adi olan dosyaya yazdir


	static SingletonFileOperations* GetInstance()
	{
		// Allocate with `new` in case Singleton is not trivially destructible.
		static unique_ptr<SingletonFileOperations> instance(new SingletonFileOperations());
		//smart pointer sayesinde new ile olusturulan obje program sona erdiginde silinecek

		if (singleton == nullptr) {
			singleton = std::move(instance);
		}

		return singleton.get();
	}

private:
	string path;				//yazdirilicak dosyanin bulundugu path
	string fileName;

	static unique_ptr<SingletonFileOperations> singleton;

	SingletonFileOperations() : path(""), fileName("") {};

	// Delete copy/move so extra instances can't be created/moved.
	SingletonFileOperations(const SingletonFileOperations&) = delete;
	SingletonFileOperations& operator=(const SingletonFileOperations&) = delete;
	SingletonFileOperations(SingletonFileOperations&&) = delete;
	SingletonFileOperations& operator=(SingletonFileOperations&&) = delete;

};


/*
* Veritabani ile ilgili islemler icin class
* Verilen veritabaninda uygun tabloya mesaji yazdirabilir
*/

class SingletonDatabaseOperations : public PrintMessage
{
public:
	bool writeMessage(Message msg);					//dosya adini bulmak icin "message.h" dosyasindaki priority_map'i kullanir
	bool checkConnection();
	bool connectToDatabase(const char* host, const char* user, const char* password,
		const char* databaseName, unsigned int port, const char* unix_socket,
		unsigned long clientflag);							//veritabanina baglanamazsa false dondurur


	static SingletonDatabaseOperations* GetInstance()
	{
		// Allocate with `new` in case Singleton is not trivially destructible.
		static unique_ptr<SingletonDatabaseOperations> instance(new SingletonDatabaseOperations());
		//smart pointer sayesinde new ile olusturulan obje program sona erdiginde silinecek

		if (singleton == nullptr)
			singleton = std::move(instance);

		return singleton.get();
	}

private:
	MYSQL* mysql_object;			// Islemler icin kullanilacak mysql nesnesi
	MYSQL* connection;				// Baglanti pointer'i

	static unique_ptr<SingletonDatabaseOperations> singleton;

	SingletonDatabaseOperations() : mysql_object(mysql_init(mysql_object)), connection(nullptr) {}

	// Delete copy/move so extra instances can't be created/moved.
	SingletonDatabaseOperations(const SingletonDatabaseOperations&) = delete;
	SingletonDatabaseOperations& operator=(const SingletonDatabaseOperations&) = delete;
	SingletonDatabaseOperations(SingletonDatabaseOperations&&) = delete;
	SingletonDatabaseOperations& operator=(SingletonDatabaseOperations&&) = delete;
};

#endif 

