#ifndef MESSAGE_PRINTER
#define MESSAGE_PRINTER

#include "message.h"

/*
* Interface class. Bir mesaji yazdirmak icin genel fonksiyon seklinde tanimlandi. 
* Dosyaya yazdirmak veya veritabanina yazdirmak icin kullanilabilir.
*/
class PrintMessage
{
public:
	virtual bool writeMessage(Message msg) = 0;
};


#endif 
