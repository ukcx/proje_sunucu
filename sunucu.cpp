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

using namespace std;

constexpr auto MSG_SIZE = 1000;
int port = 194;
string path = "C:\\Users\\Administrator\\Desktop\\";

int main()
{  
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  //winsock kutuphanesini baslatmak icin gerekli
    {
        printf("Winsock baslatilamadi. Hata kodu: %d", WSAGetLastError());
        return 1;
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
        cerr << "Soket olusturulamadi\n";
        return 1;
    }
    else
    {
        // Adres bağlama
        if (bind(soket, (sockaddr*)&sunucu_bilgileri, sizeof(sunucu_bilgileri)) < 0) //soket uzerinden iletisim icin sunucu sokete baglaniyor
        {
            cerr << "Baglama sirasinda hata olustu." << endl;
            return 1;
        }
        else
        {
            cout << "Sunucu acildi" << endl;

            if (listen(soket, 1) < 0)    //şu anda 1 istemciye izin veriliyor    SOMAXCONN == maximum reasonable value
            {
                cerr << "Dinleme sirasinda hata olustu." << endl;
                return 1;
            }

            else
            {
                cout << "Sunucu dinleme asamasinda" << endl;
                int client_len;
                sockaddr client_addr; //sadece accept fonksiyonunun calısması icin gerekli gecici variable, sonradan kullanilmayacak
                client_len = sizeof(client_addr);

                SOCKET yeni_soket;      //accept fonksiyonu ile yeni soket acilacak ve mesaj transferi bu soket uzerinden gerceklesecektir. 
                                        //Eski soket baska istemcileri kabul etmek icin kullanilacaktir
                yeni_soket = accept(soket, (sockaddr*)&client_addr, &client_len);

                if (yeni_soket == INVALID_SOCKET)
                {
                    cerr << "Istemci kabul ederken hata olustu." << endl;
                    return 1;
                }
                else
                {
                    cout << "Sunucu gelen istemcileri kabul asamasinda" << endl;
                    bool isThereAnotherMessageBeingSent = true;  //sunucunun mesaj almaya devam edip etmedigini takip etmemiz icin

                    do
                    {
                        char alinan_mesaj[MSG_SIZE];
                        memset(&alinan_mesaj, 0, MSG_SIZE); //eger memset kullanilmazsa alinan_mesaj sadece rastgele karakterlerle dolacaktir
                                                            //alinan_mesaj MSG_SIZE'dan kisaysa bu problemi cozuyor

                        if (recv(yeni_soket, alinan_mesaj, MSG_SIZE, 0) < 0) //read, recv ile soket uzerinden mesaji okuyoruz
                        {
                            cout << "\nMesaj ulasmadi" << endl;
                            isThereAnotherMessageBeingSent = false; //eger artik mesaj ulasmiyorsa istemci kapanmistir
                        }
                        else
                        {
                            cout << "\nIstemciden mesaj ulasti" << endl;

                            string alinan_mesaj_string_hali = alinan_mesaj;
                            if (alinan_mesaj_string_hali == "stop") //durdurma mesaji gonderilmis
                            {
                                cout << "Ama durdurma mesaji" << endl;
                                isThereAnotherMessageBeingSent = false; //istemci durma mesaji yollamis, daha mesaj gelmeyecek
                            }

                            else               //normal mesaj gonderilmis
                            {
                                Message* mesaj = stringToMessage(alinan_mesaj);
                                
                                PrintMessageToFile printer(path, "dusuk_oncelikli.txt", 
                                    "normal_oncelikli.txt", "yuksek_oncelikli.txt");    //onceliklere gore dosya isimleri

                                printer.printMessage(*mesaj);

                                string success_message = "Basariyla mesajiniz alindi!";

                                if (send(yeni_soket, success_message.c_str(), success_message.length(), 0) < 0)//write, send ile soket uzerine mesaj yaziyoruz
                                {
                                    cout << "\nIstemciye mesaj gonderilemedi" << endl;
                                }
                                else
                                {
                                    cout << "\nIstemciye mesaj gonderildi" << endl;
                                }
                            }
                        }
                    } while (isThereAnotherMessageBeingSent);   //istemci mesaj gonderdigi surece devam et
                }
                closesocket(yeni_soket);
            }
            
        }

    }
    closesocket(soket);
    return 0;
}