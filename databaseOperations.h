#ifndef DATABASEOPERATIONS_H
#define DATABASEOPERATIONS_H

#include "writeMessage.h"
#include "message.h"
#include <string>
#include <mysql.h>
#include <memory>
using namespace std;


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
