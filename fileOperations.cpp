
#include "messagePrinter.h"
#include <string>

using namespace std;

/*
* Mesaji yazdirmak icin yazdirilacak dosya isimleri ve yazdirilacak dosyanin bulunmasinin
* istendigi path kullanilarak obje olusturulur.
* yazdirilacak dosya isimleri, priorityLevel seviyelerine gore siralanmis halde kullanicidan alinmalidir
	ornek: fileNames[0]'da (eger Low en dusuk degerdeyse) Low oncelik seviyesindeki dosya adi olmalidir
	herbir priorityLevel ogesi icin 1 dosya adi olmalidir
*/
FileOperations::FileOperations(string pathToFolder, vector<string> fNames)
{
	path = pathToFolder;
	if (fNames.size() != priority_map.size())
	{
		throw "Verilen dosya sayisi gereken priority level sayisinda degil!!";
	}
	else
	{
		fileNames = fNames;
	}
}

/*
* Dosya isimlerini sonradan degistirmek icin kullanilabilecek fonksiyon
*/
bool FileOperations::changeFileNames(vector<string> fNames)
{
	if (fNames.size() != priority_map.size())
	{
		return false;
	}
	else
	{
		fileNames = fNames;
		return true;
	}
}

/*
* Dosyanin yazdirilacagi path'i sonradan degistirmek icin kullanilabilecek fonksiyon
*/
void FileOperations::changePath(string pathToFolder)
{
	path = pathToFolder;
}

/*
* Interface class'tan gelen fonksiyon.
* Mesaji oncelik seviyesine gore istenen dosyaya yazdirir.
*/
void FileOperations::writeMessage(Message msg)
{
	string file_name;
	file_name = fileNames[msg.getPriority()];

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
