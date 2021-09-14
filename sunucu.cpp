#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#pragma comment(lib, "Ws2_32.lib")              //winsock2'yi linkleyebilmek icin
#include <winsock2.h>
#include <iostream>
#include <string.h>
#include "message.h"
#include "fileOperations.h"
#include "databaseOperations.h"
#include "fileOperations.cpp"
#include "databaseOperations.cpp"
#include "server_operations.h"
#include <queue>
#include <mutex>
#include <thread>
#include <unordered_map>

using namespace std;

struct messageInfo                  //mesajin kendisi ve yazilacagi yeri beraber tutmak icin gereken yapi
{
    Message mesaj;                  
    string yazdirilacagi_yer;
    SOCKET mesajin_oldugu_socket;
    messageInfo(Message msg, string yer, SOCKET soket) : mesaj(msg), yazdirilacagi_yer(yer), 
                    mesajin_oldugu_socket(soket){}
};

string server_address = "127.0.0.1";     //"localhost";
int port = 194;
string path = "C:\\Users\\Administrator\\Desktop\\";        //dosyalarin oldugu path
vector<string> fileNames;
queue <messageInfo> lowPriorityQueue, mediumPriorityQueue, highPriorityQueue;
std::mutex accept_mutex, recv_mutex, send_mutex, database_mutex, file_mutex;
std::mutex* priorityQueueMutexes[3];            //her bir queue icin ayri mutexler kullanilacak


unordered_map <priorityLevel, queue<messageInfo>> queue_map =
{
    {Low, lowPriorityQueue},    //oncelik seviyelerine gore karsilik gelen queue objelerini O(1) zamanda bulmak icin
    {Medium, mediumPriorityQueue},
    {High, highPriorityQueue}
};


void message_acceptor(ServerOperations &server)
{

    while (true)                 //istemci mesaj gonderdigi surece devam et
    {
        SOCKET newSocket;      /*accept fonksiyonu ile yeni soket acilacak ve mesaj transferi bu soket uzerinden gerceklesecek
                                Eski soket baska istemcileri kabul etmek icin kullanilacaktir*/
               
        accept_mutex.lock();
        newSocket = server.acceptClient();
        accept_mutex.unlock();
                                                    
        string mesaj_yazdirma_yeri;
        string alinacak_mesaj;

        recv_mutex.lock();
        server.receiveMessage(newSocket, mesaj_yazdirma_yeri);
        server.receiveMessage(newSocket, alinacak_mesaj);        //read, recv ile soket uzerinden mesaji okuyoruz
        recv_mutex.unlock();

        cout << "\nIstemciden mesaj ulasti" << endl;        

        if((mesaj_yazdirma_yeri != "Database") && (mesaj_yazdirma_yeri != "File"))    //mesajin nereye yazdirilacagi dogru verilmemis
        {
            string fail_message = "Mesajin yazdirilacagi yer yanlis verilmis! Mesaj yazdirilamadi!";
            cout << fail_message << endl;

            send_mutex.lock();
            if (server.sendMessage(newSocket, fail_message))
                cout << "\nIstemciye mesaj gonderilemedi" << endl;

            else
                cout << "\nIstemciye mesaj gonderildi" << endl;
                
            send_mutex.unlock();
        }
        else
        {
            Message mesaj = stringToMessage(alinacak_mesaj);     //recv ile alinan string'i message objesine donustur
            messageInfo mesaj_bilgileri(mesaj, mesaj_yazdirma_yeri, newSocket);    //mesajin kendisi ve yazilan yeri birlikte tutan obje
            int index = mesaj.getPriority();


            (*priorityQueueMutexes[index]).lock();
            queue_map.find(mesaj.getPriority())->second.push(mesaj_bilgileri);     //oncelik seviyesine denk gelen queue'e messageInfo objesini ekle
            (*priorityQueueMutexes[index]).unlock();

        }
    }
}

void fetchMessages(priorityLevel pLevel, ServerOperations& server)
{
    
    queue<messageInfo>* queue_for_this_pLevel = &queue_map.find(pLevel)->second;    //islem yapacagimiz queue'i bul
    SingletonFileOperations* fileOperator;          //islem yapacagimiz fileOperations ogesine pointer
    SingletonDatabaseOperations* dbOperator;        //islem yapacagimiz databaseOperations ogesine pointer
    
    while (true)
    {
        (*priorityQueueMutexes[pLevel]).lock();         //verilen oncelik seviyesine denk gelen queue'nin mutex'ini kilitle

        if (queue_for_this_pLevel->empty())
        {
            (*priorityQueueMutexes[pLevel]).unlock();
        }
        else
        {
            messageInfo mesaj_bilgileri = queue_for_this_pLevel->front();       //queue'nun en onunden ilk mesaji getir
            queue_for_this_pLevel->pop();                                       //bu mesaji queue'den cikar
            (*priorityQueueMutexes[pLevel]).unlock();
            
            string success_message = "Basariyla mesajiniz alindi!";

            if (mesaj_bilgileri.yazdirilacagi_yer == "File")
            {
                file_mutex.lock();

                cout << "dosyaya yazdirmaya calisiyorum" << endl;

                fileOperator = SingletonFileOperations::GetInstance();          //singleton objesi ile her seferinde ayni objeyi kullan

                fileOperator->setFileName(fileNames[pLevel]);
                fileOperator->setPath(path);
                if (fileOperator->writeMessage(mesaj_bilgileri.mesaj))      //mesaji dosyaya yazdirmaya calis
                {
                    success_message += " Ve dosyaya yazdirildi.";
                }
                else{
                    success_message += "Ancak dosyaya yazdirilamadi!!";
                }
                file_mutex.unlock();
            }

            if (mesaj_bilgileri.yazdirilacagi_yer == "Database")
            {
                database_mutex.lock();

                cout << "database'e yazdirmaya calisiyorum" << endl;
                dbOperator = SingletonDatabaseOperations::GetInstance();        //singleton objesi ile her seferinde ayni objeyi kullan

                if (!dbOperator->checkConnection())     //database'e daha baglanilmamis
                {
                    dbOperator->connectToDatabase("localhost", "root", "password", "messages", 3306, 0, 0); //yalnizca 1 kere kosturulmasi lazim
                }

                if (dbOperator->checkConnection())
                {
                    cout << "Veritabanina baglanildi" << endl;
                    if (dbOperator->writeMessage(mesaj_bilgileri.mesaj))    //veritabanina mesaji yazdirmaya calis
                    {
                        success_message += " Ve veritabanina yazdirildi.";
                    }
                    else{      
                        success_message += " Ancak veritabanina yazdirilamadi.";
                    }
                    
                }
                else
                {
                    cout << "Veritabanina baglanilamadi!" << endl;
                    success_message += " Ancak veritabanina yazdirilamadi.";
                }
                database_mutex.unlock();
            }
            
            send_mutex.lock();
            if (server.sendMessage(mesaj_bilgileri.mesajin_oldugu_socket, success_message)) //write, send ile soket uzerine mesaj yaziyoruz
            {
                cout << "\nIstemciye mesaj gonderilemedi" << endl;
            }
            else
            {
                cout << "\nIstemciye mesaj gonderildi" << endl;
            }
            closesocket(mesaj_bilgileri.mesajin_oldugu_socket);
            send_mutex.unlock();
        }
    }                           
    (*priorityQueueMutexes[pLevel]).unlock();       //loop'tan cikinca kilitli kalan mutex'leri ac

}


int main()
{  
    try {

        ServerOperations server;
        if (!server.initializeSocket(server_address, port)){
            throw "Soket olusturulamadi";
        }

        if (!server.bindServer()){
            throw "Baglama sirasinda hata olustu.";
        }

        cout << "Sunucu acildi" << endl;

        if (!server.listenSocket(SOMAXCONN))    {             // SOMAXCONN = maximum reasonable value
            throw  "Dinleme sirasinda hata olustu.";
        }

        cout << "Sunucu dinleme asamasinda" << endl;

        fileNames.push_back("dusuk_oncelikli.txt");
        fileNames.push_back("normal_oncelikli.txt");            //onceliklere gore dosya isimleri
        fileNames.push_back("yuksek_oncelikli.txt");

        mutex low_mutex, medium_mutex, high_mutex;        //onceliklere gore mutexler
        priorityQueueMutexes[0] = &low_mutex;
        priorityQueueMutexes[1] = &medium_mutex;          //bu mutex'leri priorityQueueMutexes ile oncelik seviyesine gore index kullanarak tut
        priorityQueueMutexes[2] = &high_mutex;


        cout << "Sunucu gelen istemcileri kabul asamasinda" << endl;  //buraya gelindiyse hicbir throw komutu calismamistir

        
        vector<thread> acceptor_thread_pool;
        for (int i = 0; i < 10; i++)                //10 acceptor thread olustur
        {
            acceptor_thread_pool.push_back(thread(message_acceptor, std::ref(server)));
        }

        vector<thread> processor_thread_pool;
        for (int i = 0; i < 12; i++)                //12 processor thread olustur      //6 high, 4 medium, 2 low
        {
            if (i < 6)  
            {
                processor_thread_pool.push_back(thread(fetchMessages, High, std::ref(server)));
            }
            else if (i < 10)
            {
                processor_thread_pool.push_back(thread(fetchMessages, Medium, std::ref(server)));
            }
            else
            {
                processor_thread_pool.push_back(thread(fetchMessages, Low, std::ref(server)));
            }
        }


        for (int i = 0; i < 10; i++)                //butun threadler join
        {
            acceptor_thread_pool[i].join();
        }
        for (int i = 0; i < 12; i++)                //butun threadler join
        {
            processor_thread_pool[i].join();
        }

    }
    catch(const char * hata_mesaji)
    {
        cout << hata_mesaji << endl;
    }
    return 0;
}
