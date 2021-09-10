#ifndef MESSAGE_PRINTER
#define MESSAGE_PRINTER

#include "message.h"
#include <string>
#include <vector>
using namespace std;

/*
* Interface class. Bir mesaji yazdirmak icin genel fonksiyon seklinde tanimlandi.
* Dosyaya yazdirmak veya veritabanina yazdirmak icin kullanilabilir.
*/
class PrintMessage
{
public:
	virtual void printMessage(Message msg) = 0;
};

class PrintMessageToFile :public PrintMessage
{
private:
	string path;				//yazdirilicak dosyanin bulundugu path
	vector<string> fileNames;	//yazdirilacak dosya isimleri

public:
	PrintMessageToFile(string pathToFolder, string low_priority_fn, string medium_priority_fn, string high_priority_fn);
	void changeFileNames(string low_priority_fn, string medium_priority_fn, string high_priority_fn);
	void changePath(string pathToFolder);
	void printMessage(Message msg);
};

/*
* Mesaji yazdirmak icin yazdirilacak dosya isimleri ve yazdirilacak dosyanin bulunmasinin
* istendigi path kullanilarak obje olusturulur.
*/
PrintMessageToFile::PrintMessageToFile(string pathToFolder, string low_priority_fn, string medium_priority_fn, string high_priority_fn)
{
	fileNames.push_back(low_priority_fn);
	fileNames.push_back(medium_priority_fn);
	fileNames.push_back(high_priority_fn);
	path = pathToFolder;
}
/*
* Dosya isimlerini sonradan degistirmek icin kullanilabilecek fonksiyon
*/
void PrintMessageToFile::changeFileNames(string low_priority_fn, string medium_priority_fn, string high_priority_fn)
{
	fileNames[0] = low_priority_fn;
	fileNames[1] = medium_priority_fn;
	fileNames[2] = high_priority_fn;
}
/*
* Dosyanin yazdirilacagi path'i sonradan degistirmek icin kullanilabilecek fonksiyon
*/
void PrintMessageToFile::changePath(string pathToFolder)
{
	path = pathToFolder;
}
/*
* Interface class'tan gelen fonksiyon.
* Mesaji oncelik seviyesine gore istenen dosyaya yazdirir.
*/
void PrintMessageToFile::printMessage(Message msg)
{
	string file_name;
	if (msg.getPriority() == Low) {
		file_name = fileNames[0];
	}
	else if (msg.getPriority() == Medium) {
		file_name = fileNames[1];
	}
	else {
		file_name = fileNames[2];
	}

	ofstream yazilacak_dosya;
	yazilacak_dosya.open(path + file_name, ios::out | ios::app);
	if (yazilacak_dosya.is_open())
	{
		yazilacak_dosya << "Message" << endl;
		msg.printMessage(yazilacak_dosya);
		yazilacak_dosya << endl;
	}
	else
	{
		cerr << "Dosya acilamadi!!" << endl;
	}
	yazilacak_dosya.close();
}


//class PrintMessageToDatabase

#endif 

