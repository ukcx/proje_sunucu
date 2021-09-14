#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H

#include "writeMessage.h"
#include "message.h"
#include <string>
#include <mysql.h>
#include <memory>
using namespace std;


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

#endif
