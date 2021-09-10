#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif

#pragma comment(lib, "Ws2_32.lib")              //winsock2'yi linkleyebilmek icin
#include <winsock2.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include "message.h"

using namespace std;

constexpr auto MSG_SIZE = 1000;
int port = 194;

int main()
{  
    WSADATA wsa;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)  //winsock kutuphanesini baslatmak icin gerekli
    {
        printf("Winsock baslatilamadi. Error Code : %d", WSAGetLastError());
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
        cerr << "soket olusturulamadi\n";
        return 1;
    }
    else
    {
        //cout << "soket numarasi: " << soket << endl;
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
                    cerr << "accept'te hata olustu." << endl;
                    return 1;
                }
                else
                {
                    //cout << "soket numarasi: " << yeni_soket << endl;
                    char alinan_mesaj[MSG_SIZE];
                    memset(&alinan_mesaj, 0, MSG_SIZE);
                    cout << "Sunucu gelen istemcileri kabul asamasinda" << endl;

                    if (recv(yeni_soket, alinan_mesaj, MSG_SIZE, 0) < 0) //read, recv ile soket uzerinden mesaji okuyoruz
                    {
                        cout << "\nmesaj ulasmadi" << endl;
                    }
                    else
                    {
                        //cout << "soket numarasi: " << yeni_soket << endl;
                        cout << "\nistemciden mesaj ulasti" << endl;
                        //cout << "alinan mesaj: \n" << alinan_mesaj << endl;
                        Message* mesaj = stringToMessage(alinan_mesaj);
                        ofstream yazilacak_dosya;

                        string path = "C:\\Users\\Administrator\\Desktop\\";
                        if (mesaj->getPriority() == Low) 
                        {
                            string file_name = "dusuk_oncelikli.txt";
                            yazilacak_dosya.open(path + file_name, ios::out | ios::app);
                            if(yazilacak_dosya.is_open())
                            {
                                yazilacak_dosya << "Message" << endl;
                                mesaj->printMessage(yazilacak_dosya);
                                yazilacak_dosya << endl;
                            }
                            else
                            {
                                cerr << "dosya acilamadi!!" << endl;
                            }
                            yazilacak_dosya.close();

                        }
                        else if (mesaj->getPriority() == Medium)
                        {
                            string file_name = "normal_oncelikli.txt";
                            yazilacak_dosya.open(path + file_name, ios::out | ios::app);
                            if (yazilacak_dosya.is_open())
                            {
                                yazilacak_dosya << "Message" << endl;
                                mesaj->printMessage(yazilacak_dosya);
                                yazilacak_dosya << endl;
                            }
                            else
                            {
                                cerr << "dosya acilamadi!!" << endl;
                            }
                            yazilacak_dosya.close();

                        }
                        else
                        {
                            string file_name = "yuksek_oncelikli.txt";
                            yazilacak_dosya.open(path + file_name, ios::out | ios::app);
                            if (yazilacak_dosya.is_open())
                            {
                                yazilacak_dosya << "Message" << endl;
                                mesaj->printMessage(yazilacak_dosya);
                                yazilacak_dosya << endl;
                            }
                            else
                            {
                                cerr << "dosya acilamadi!!" << endl;
                            }
                            yazilacak_dosya.close();

                        }

                        if (send(yeni_soket, mesaj->messageToString().c_str(), mesaj->messageToString().length(), 0) < 0)//write, send ile soket uzerine mesaj yaziyoruz
                        {
                            cout << "\nistemciye mesaj gonderilemedi" << endl;
                        }
                        else 
                        {
                            cout << "\nistemciye mesaj gonderildi" << endl;
                        }
                    }
                }
                closesocket(yeni_soket);
            }
        }

    }
    closesocket(soket);
    return 0;
}