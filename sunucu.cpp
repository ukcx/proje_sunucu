#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#pragma comment(lib, "Ws2_32.lib")              //winsock2'yi linkleyebilmek icin
#include <winsock2.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include "message.h"
#include "messagePrinter.h"
#include "fileOperations.cpp"
#include "databaseOperations.cpp"

using namespace std;

constexpr auto MSG_SIZE = 1000;
int port = 194;
string path = "C:\\Users\\Administrator\\Desktop\\";        //dosyalarin oldugu path


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

        SOCKET yeni_soket;      /*accept fonksiyonu ile yeni soket acilacak ve mesaj transferi bu soket uzerinden gerceklesecektir.
                                Eski soket baska istemcileri kabul etmek icin kullanilacaktir*/

        yeni_soket = accept(soket, (sockaddr*)&client_addr, &client_len);

        if (yeni_soket == INVALID_SOCKET)
        {
            throw "Istemci kabul ederken hata olustu.";
        }

        cout << "Sunucu gelen istemcileri kabul asamasinda" << endl;  //buraya gelindiyse hicbir throw komutu calismamistir

        bool isThereAnotherMessageBeingSent = true;  //sunucunun mesaj almaya devam edip etmedigini takip etmemiz icin
        string whereToPrintMessage = "";

        do
        {
            char alinan_mesaj[MSG_SIZE];
            memset(&alinan_mesaj, 0, MSG_SIZE); /*eger memset kullanilmazsa alinan_mesaj'in dolmayan kismi
                                                sadece rastgele karakterlerle dolacaktir.
                                                memset alinan_mesaj MSG_SIZE'dan kisaysa bu problemi cozuyor*/

            if (recv(yeni_soket, alinan_mesaj, MSG_SIZE, 0) < 0) //read, recv ile soket uzerinden mesaji okuyoruz
            {
                cout << "\nMesaj ulasmadi" << endl;
                isThereAnotherMessageBeingSent = false; //eger artik mesaj ulasmiyorsa istemci kapanmistir
            }
            else
            {
                cout << "\nIstemciden mesaj ulasti" << endl;

                string alinan_mesaj_string_hali = alinan_mesaj;
                if (alinan_mesaj_string_hali == "stop")                     //durdurma mesaji gonderilmis
                {
                    cout << "Ama durdurma mesaji" << endl;
                    isThereAnotherMessageBeingSent = false;             //istemci durma mesaji yollamis, daha mesaj gelmeyecek
                }
                else if (alinan_mesaj_string_hali == "database")            //istemci database'e yazdirmak istiyor
                {
                    whereToPrintMessage = "database";
                }
                else if (alinan_mesaj_string_hali == "file")                //istemci file'a yazdirmak istiyor
                {
                    whereToPrintMessage = "file";
                }
                else if (whereToPrintMessage == "")      //mesajin nereye yazdirilacagi verilmemis
                {
                    string fail_message = "Mesajin yazdirilacagi yer verilmemis! Mesaj yazdirilamadi!";

                    cout << fail_message << endl;

                    if (send(yeni_soket, fail_message.c_str(), fail_message.length(), 0) < 0)
                        cout << "\nIstemciye mesaj gonderilemedi" << endl;

                    else
                        cout << "\nIstemciye mesaj gonderildi" << endl;

                }
                else               //normal mesaj gonderilmis ve mesajin yazdirilacagi yer onceden dogru verilmis
                {
                    Message* mesaj = stringToMessage(alinan_mesaj);

                    string success_message = "Basariyla mesajiniz alindi!";

                    if (whereToPrintMessage == "file")
                    {
                        vector<string> fileNames;
                        fileNames.push_back("dusuk_oncelikli.txt");
                        fileNames.push_back("normal_oncelikli.txt");            //onceliklere gore dosya isimleri
                        fileNames.push_back("yuksek_oncelikli.txt");

                        FileOperations printer(path, fileNames);

                        printer.writeMessage(*mesaj);
                        success_message += " Ve dosyaya yazdirildi.";
                    }

                    if (whereToPrintMessage == "database")
                    {
                        DatabaseOperations dbOperator("localhost", "root", "password", "messages", 3306, 0, 0);

                        if (dbOperator.connectToDatabase())     //veritabanina baglanildi
                        {
                            cout << "Veritabanina baglanildi" << endl;
                            dbOperator.writeMessage(*mesaj);
                            success_message += " Ve veritabanina yazdirildi.";
                        }
                        else
                        {
                            cout << "Veritabanina baglanilamadi!" << endl;
                            success_message += " Ancak veritabanina yazdirilamadi.";
                        }
                    }

                    whereToPrintMessage = "";       //sonraki mesajin yazdirilicagi yeri sifirla

                    if (send(yeni_soket, success_message.c_str(), success_message.length(), 0) < 0) //write, send ile soket uzerine mesaj yaziyoruz
                        cout << "\nIstemciye mesaj gonderilemedi" << endl;

                    else
                        cout << "\nIstemciye mesaj gonderildi" << endl;

                }
            }

        } while (isThereAnotherMessageBeingSent);   //istemci mesaj gonderdigi surece devam et

        closesocket(yeni_soket);
        closesocket(soket);

    }
    catch (const char* hata_mesaji)
    {
        cout << hata_mesaji << endl;          //socket'te veya kutuphanede hata cikarsa gonderilen hata mesajini yazdir
    }

    return 0;
}