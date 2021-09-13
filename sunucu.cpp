#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#pragma comment(lib, "Ws2_32.lib")              //winsock2'yi linkleyebilmek icin
#include <winsock2.h>
#include <iostream>
#include <string.h>
#include "message.h"
#include "messagePrinter.h"
#include "fileOperations.cpp"
#include "databaseOperations.cpp"
#include <queue>
#include <mutex>
#include <thread>
#include <unordered_map>

using namespace std;

struct messageInfo                  //mesajin kendisi ve yazilacagi yeri beraber tutmak icin gereken yapi
{
    Message mesaj;
    string yazdirilacagi_yer;
    messageInfo(Message& msg, string yer) : mesaj(msg), yazdirilacagi_yer(yer) {}
};

constexpr auto MSG_SIZE = 1000;
constexpr auto LOCATION_INFO_SIZE = 20;                     //mesajin kaydedilecegi yerin string olarak max boyutu
int port = 194;
string path = "C:\\Users\\Administrator\\Desktop\\";        //dosyalarin oldugu path
vector<string> fileNames;
queue <messageInfo> lowPriorityQueue, mediumPriorityQueue, highPriorityQueue;

unordered_map <priorityLevel, queue<messageInfo>> queue_map =
{
    {Low, lowPriorityQueue},    //oncelik seviyelerine gore karsilik gelen queue objelerini O(1) zamanda bulmak icin
    {Medium, mediumPriorityQueue},
    {High, highPriorityQueue}
};

int num_of_acceptors_finished = 0;
bool isThereAnotherMessageBeingSent = true;     //sunucunun mesaj almaya devam edip etmedigini takip etmemiz icin
std::mutex mesaj_mutex, recv_mutex, send_mutex, database_mutex, file_mutex;
std::mutex* priorityQueueMutexes[3];            //her bir queue icin ayri mutexler kullanilacak



void message_acceptor(SOCKET& yeni_soket)
{

    while (true)                 //istemci mesaj gonderdigi surece devam et
    {
        mesaj_mutex.lock();
        if (!isThereAnotherMessageBeingSent)     //daha mesaj gelmiyor
        {
            num_of_acceptors_finished++;        //bu acceptor isini bitirdi
            break;
        }
        mesaj_mutex.unlock();

        char mesaji_yazma_yeri[LOCATION_INFO_SIZE];
        memset(&mesaji_yazma_yeri, 0, LOCATION_INFO_SIZE);

        char alinan_mesaj[MSG_SIZE];
        memset(&alinan_mesaj, 0, MSG_SIZE);                 /*eger memset kullanilmazsa alinan_mesaj'in dolmayan kismi
                                                            sadece rastgele karakterlerle dolacaktir.
                                                            memset alinan_mesaj MSG_SIZE'dan kisaysa bu problemi cozuyor*/

        recv_mutex.lock();
        if ((recv(yeni_soket, mesaji_yazma_yeri, LOCATION_INFO_SIZE, 0) < 0) ||
            (recv(yeni_soket, alinan_mesaj, MSG_SIZE, 0) < 0))     //read, recv ile soket uzerinden mesaji okuyoruz
        {
            cout << "\nMesaj ulasmadi" << endl;
            recv_mutex.unlock();

            mesaj_mutex.lock();
            isThereAnotherMessageBeingSent = false; //eger artik mesaj ulasmiyorsa istemci kapanmistir
            mesaj_mutex.unlock();
        }
        else
        {
            recv_mutex.unlock();
            cout << "\nIstemciden mesaj ulasti" << endl;

            string mesaj_yeri = mesaji_yazma_yeri;
            string alinan_mesaj_string_hali = alinan_mesaj;

            if ((mesaj_yeri != "database") && (mesaj_yeri != "file"))    //mesajin nereye yazdirilacagi dogru verilmemis
            {
                string fail_message = "Mesajin yazdirilacagi yer yanlis verilmis! Mesaj yazdirilamadi!";
                cout << fail_message << endl;

                send_mutex.lock();
                if (send(yeni_soket, fail_message.c_str(), fail_message.length(), 0) < 0)
                    cout << "\nIstemciye mesaj gonderilemedi" << endl;

                else
                    cout << "\nIstemciye mesaj gonderildi" << endl;

                send_mutex.unlock();
            }

            else
            {
                Message mesaj = stringToMessage(alinan_mesaj);     //recv ile alinan string'i message objesine donustur
                messageInfo mesaj_bilgileri(mesaj, mesaj_yeri);    //mesajin kendisi ve yazilan yeri birlikte tutan obje
                int index = mesaj.getPriority();

                (*priorityQueueMutexes[index]).lock();
                queue_map.find(mesaj.getPriority())->second.push(mesaj_bilgileri);     //oncelik seviyesine denk gelen queue'e messageInfo objesini ekle
                (*priorityQueueMutexes[index]).unlock();

            }
        }

    }
    mesaj_mutex.unlock();           //loop'tan cikinca mutex'ler kilitli kaldi onlari ac

}

void fetchMessages(SOCKET& yeni_soket, priorityLevel pLevel)
{

    queue<messageInfo>* queue_for_this_pLevel = &queue_map.find(pLevel)->second;    //islem yapacagimiz queue'i bul
    SingletonFileOperations* fileOperator;          //islem yapacagimiz fileOperations ogesine pointer
    SingletonDatabaseOperations* dbOperator;        //islem yapacagimiz databaseOperations ogesine pointer

    while (true)
    {
        mesaj_mutex.lock();
        (*priorityQueueMutexes[pLevel]).lock();         //verilen oncelik seviyesine denk gelen queue'nin mutex'ini kilitle

        if ((num_of_acceptors_finished >= 10) && queue_for_this_pLevel->empty())
        {
            break;  //10 acceptor thread islerini bitirmis ve queue de bos, artik bu thread islemleri bitirebilir
        }
        mesaj_mutex.unlock();

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

            if (mesaj_bilgileri.yazdirilacagi_yer == "file")
            {
                file_mutex.lock();

                cout << "dosyaya yazdirmaya calisiyorum" << endl;

                fileOperator = SingletonFileOperations::GetInstance();          //singleton objesi ile her seferinde ayni objeyi kullan
                //cout << "SingletonFileOperations adresi: " << fileOperator << endl;

                fileOperator->setFileName(fileNames[pLevel]);
                fileOperator->setPath(path);
                if (fileOperator->writeMessage(mesaj_bilgileri.mesaj))      //mesaji dosyaya yazdirmaya calis
                {
                    success_message += " Ve dosyaya yazdirildi.";
                }
                else
                {
                    success_message += "Ancak dosyaya yazdirilamadi!!";
                }
                file_mutex.unlock();
            }

            if (mesaj_bilgileri.yazdirilacagi_yer == "database")
            {
                database_mutex.lock();

                cout << "database'e yazdirmaya calisiyorum" << endl;
                dbOperator = SingletonDatabaseOperations::GetInstance();        //singleton objesi ile her seferinde ayni objeyi kullan
                //cout << "SingletonDatabaseOperations adresi: " << dbOperator << endl;

                if (!dbOperator->checkConnection())     //database'e daha baglanilmamis
                {
                    dbOperator->connectToDatabase("localhost", "root", "password", "messages", 3306, 0, 0); //yalnizca 1 kere kosturulmasi lazim
                }

                if (dbOperator->checkConnection())
                {
                    cout << "Veritabanina baglanildi" << endl;
                    if (dbOperator->writeMessage(mesaj_bilgileri.mesaj))    //veritabanina mesaji yazdirmaya calis
                    {
                        database_mutex.unlock();
                        success_message += " Ve veritabanina yazdirildi.";
                    }
                    else
                    {
                        database_mutex.unlock();
                        success_message += " Ancak veritabanina yazdirilamadi.";
                    }

                }
                else
                {
                    cout << "Veritabanina baglanilamadi!" << endl;
                    database_mutex.unlock();
                    success_message += " Ancak veritabanina yazdirilamadi.";
                }
            }

            send_mutex.lock();
            if (send(yeni_soket, success_message.c_str(), success_message.length(), 0) < 0) //write, send ile soket uzerine mesaj yaziyoruz
            {
                cout << "\nIstemciye mesaj gonderilemedi" << endl;
            }
            else
            {
                cout << "\nIstemciye mesaj gonderildi" << endl;
            }
            send_mutex.unlock();
        }
    }
    mesaj_mutex.unlock();
    (*priorityQueueMutexes[pLevel]).unlock();       //loop'tan cikinca kilitli kalan mutex'leri ac

}

int main()
{

    try
    {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  //winsock kutuphanesini baslatmak icin gerekli
        {
            throw "Winsock baslatilamadi.";     /*throw komutu yurutulurse ait olunan try scope'undan cikilir ve direkt
                                                olarak try scope'unun disinda scope'a en yakin catch fonksiyonuna gidilir*/
        }


        SOCKET soket = socket(AF_INET, SOCK_STREAM, 0);

        // Veri yapısı ayarları
        sockaddr_in sunucu_bilgileri;      //sockaddr_in
        memset(&sunucu_bilgileri, 0, sizeof(sunucu_bilgileri));  // Veri yapısını sıfırlama
        sunucu_bilgileri.sin_family = AF_INET;
        sunucu_bilgileri.sin_addr.s_addr = htonl(INADDR_ANY);  // veya inet_addr("127.0.0.1")
        sunucu_bilgileri.sin_port = htons(port);

        if (soket == INVALID_SOCKET)
        {
            throw "Soket olusturulamadi";
        }

        // Adres bağlama
        if (bind(soket, (sockaddr*)&sunucu_bilgileri, sizeof(sunucu_bilgileri)) < 0) //soket uzerinden iletisim icin sunucu sokete baglaniyor
        {
            throw "Baglama sirasinda hata olustu.";
        }

        cout << "Sunucu acildi" << endl;

        if (listen(soket, 1) < 0)    //şu anda 1 istemciye izin veriliyor    SOMAXCONN == maximum reasonable value
        {
            throw  "Dinleme sirasinda hata olustu.";
        }

        cout << "Sunucu dinleme asamasinda" << endl;

        int client_len;
        sockaddr client_addr;   //sadece accept fonksiyonunun calısması icin gerekli gecici variable, sonradan kullanilmayacak
        client_len = sizeof(client_addr);

        SOCKET yeni_soket;      /*accept fonksiyonu ile yeni soket acilacak ve mesaj transferi bu soket uzerinden gerceklesecek
                                Eski soket baska istemcileri kabul etmek icin kullanilacaktir*/

        yeni_soket = accept(soket, (sockaddr*)&client_addr, &client_len);

        if (yeni_soket == INVALID_SOCKET)
        {
            throw "Istemci kabul ederken hata olustu.";
        }

        cout << "Sunucu gelen istemcileri kabul asamasinda" << endl;  //buraya gelindiyse hicbir throw komutu calismamistir

        fileNames.push_back("dusuk_oncelikli.txt");
        fileNames.push_back("normal_oncelikli.txt");            //onceliklere gore dosya isimleri
        fileNames.push_back("yuksek_oncelikli.txt");

        mutex low_mutex, medium_mutex, high_mutex;        //onceliklere gore mutexler
        priorityQueueMutexes[0] = &low_mutex;
        priorityQueueMutexes[1] = &medium_mutex;          //bu mutex'leri priorityQueueMutexes ile oncelik seviyesine gore index kullanarak tut
        priorityQueueMutexes[2] = &high_mutex;


        vector<thread> acceptor_thread_pool;

        for (int i = 0; i < 10; i++)                //10 acceptor thread olustur
        {
            acceptor_thread_pool.push_back(thread(message_acceptor, std::ref(yeni_soket)));
        }

        vector<thread> processor_thread_pool;
        for (int i = 0; i < 12; i++)                //12 processor thread olustur      //6 high, 4 medium, 2 low
        {
            if (i < 6)   //yeni_soket'i referansla gonder
            {
                processor_thread_pool.push_back(thread(fetchMessages, std::ref(yeni_soket), High));
            }
            else if (i < 10)
            {
                processor_thread_pool.push_back(thread(fetchMessages, std::ref(yeni_soket), Medium));
            }
            else
            {
                processor_thread_pool.push_back(thread(fetchMessages, std::ref(yeni_soket), Low));
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

        closesocket(yeni_soket);
        closesocket(soket);

    }
    catch (const char* hata_mesaji)
    {
        cout << hata_mesaji << endl;          //socket'te veya kutuphanede hata cikarsa gonderilen hata mesajini yazdir
    }

    return 0;
}
