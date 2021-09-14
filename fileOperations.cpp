
#include "fileOperations.h"
#include <string>

using namespace std;

unique_ptr<SingletonFileOperations> SingletonFileOperations::singleton = nullptr;
//singleton variable static oldugu icin hafizada belli bir adrese atanabilmesi icin 1 kez aciklanmasi gerekli

/*
* Dosya isimlerini sonradan degistirmek icin kullanilabilecek fonksiyon
*/

void SingletonFileOperations::setFileName(string fName)
{
	fileName = fName;
}

/*
* Dosyanin yazdirilacagi path'i sonradan degistirmek icin kullanilabilecek fonksiyon
*/
void SingletonFileOperations::setPath(string pathToFolder)
{
	path = pathToFolder;
}

/*
* Interface class'tan gelen fonksiyon.
* Mesaji oncelik seviyesine gore istenen dosyaya yazdirir.
*/
bool SingletonFileOperations::writeMessage(Message msg)
{

	ofstream yazilacak_dosya;
	yazilacak_dosya.open(path + fileName, ios::out | ios::app);	
			//yazdirilacak string'i dosyanin en sonuna ekle ios::app kullanarak
	
	if (yazilacak_dosya.is_open())
	{
		yazilacak_dosya << "Message" << endl;
		msg.printMessage(yazilacak_dosya);
		yazilacak_dosya << endl;
		yazilacak_dosya.close();
		return true;
	}
	else
	{
		return false;
	}

}
