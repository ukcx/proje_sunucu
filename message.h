#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <iostream>
#include <fstream>
using namespace std;

/*constexpr char LOW[4] = "Low";
constexpr char MEDIUM[7] = "Medium";
constexpr char HIGH[5] = "High";*/

enum priorityLevel { Low, Medium, High };

class Message
{
public:
    Message(string to, string cc, string subject, string body, priorityLevel priority);
    void printMessage();
    void printMessage(ofstream&);
    string messageToString();
    priorityLevel getPriority();

private:
    string to;
    string cc;
    string subject;
    string body;
    priorityLevel priority;

    string getPriorityInfo();
};

//MEMBER FONKSIYONLARI
/*
*Default Message constructor
*/
Message::Message(string t, string c, string subj, string bdy, priorityLevel prio)
{
    to = t;
    cc = c;
    subject = subj;
    body = bdy;
    priority = prio;
}

/*
* Private fonskiyon, enum tipte olan priority'i string'e cevir
*/
string Message::getPriorityInfo()
{
    try
    {
        if (this->priority == 0)
            return "Low";
        if (this->priority == 1)
            return "Medium";
        if (this->priority == 2)
            return "High";
        else
            throw "Bu gecerli bir oncelik seviyesi degil!!";
    }
    catch (const char* error_message)
    {
        cout << error_message << endl;
    }

}
/*
*Mesaji duzgun sekilde yazdirir.
*/
void Message::printMessage()
{
    cout << "To: " << this->to << endl;
    cout << "Cc: " << this->cc << endl;
    cout << "Subject: " << this->subject << endl;
    cout << "Body: " << this->body << endl;
    cout << "Priority: " << getPriorityInfo() << endl;
}

/*
*Mesaji duzgun sekilde verilen dosyaya yazdirir.
*/
void Message::printMessage(ofstream& file_descriptor)
{
    file_descriptor << "To: " << this->to << endl;
    file_descriptor << "Cc: " << this->cc << endl;
    file_descriptor << "Subject: " << this->subject << endl;
    file_descriptor << "Body: " << this->body << endl;
    file_descriptor << "Priority: " << getPriorityInfo() << endl;
}

/*
* Mesaji icindeki bilgileri arasina ve sonuna \n ekleyerek string'e donusturur.
*/
string Message::messageToString()
{
    return to + "\n" + cc + "\n" + subject + "\n" + body + "\n" + getPriorityInfo() + "\n";
}

/*
* Mesajin oncelik seviyesini geri dondurur.
*/
priorityLevel Message::getPriority()
{
    return priority;
}

//FREE FONSIYONLAR
/*
*Bu fonksiyon bir string objesi alip onu priorityLevel objesine donusturur.
priorityLevel sinifi -- enum priorityLevel { Low, Medium, High } -- seklinde tanimlanmistir.
Eger verilen string tanimlanan herhangi bir priorityLevel seviyesi degilse hata mesaji dondurulur.
Buyuk ve kucuk harf kullanimi tolere edilmistir.
*/
priorityLevel convertStringToPriorityLevel(string str)   //parse 
{
    if (str.length() > 1)
    {
        string temp = "";
        temp += toupper(str[0]);
        for (int i = 1; i < str.length(); i++)
        {
            temp += tolower(str[i]);
        }
        if (temp == "Low")
            return Low;
        else if (temp == "Medium")
            return Medium;
        else if (temp == "High")
            return High;
        else
            throw "Bu gecerli bir oncelik seviyesi degil!!";
    }
    else
        throw "Bu gecerli bir oncelik seviyesi degil!!";
}

/*
*Bu fonksiyon bir string objesi alip onu Message objesine donusturur.
Verilen string "to + \n + cc + \n + subject + \n + body + \n + priority + \n"
seklinde olusturulmus olmalidir.
messageToString() member fonksiyonunun tersidir.
*/
Message* stringToMessage(string msg)
{
    string info[5] = { "", "", "", "", "" };

    int i = 0;
    for (int j = 0; (j < msg.length()) && (i < 5); j++)
    {
        if (msg[j] == '\n')
            i++;
        else
        {
            info[i] += msg[j];
        }
    }

    Message* message = new Message(info[0], info[1], info[2], info[3], convertStringToPriorityLevel(info[4]));

    return message;
}
#endif