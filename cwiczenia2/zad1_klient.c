/*
-----------------------------------------------------
| zad1 Program B - Klient używajacy gethostbyname() |
-----------------------------------------------------

Zmodyfikowac kod programu B z przykładu 5 tak, 
aby użytkownik mógł podać nazwę domenową komputera odbiorcy zamiast jesgo adresu IP
(oczywiście nadal musi podawać numer portu)

Rozwiązanie:
    adres_hosta.sin_addr.s_addr =  *(long *)(he->h_addr_list[0]); //[!] Tu jest magia zamieniania adresu a s_addr



Struktura hostent zdefiniowana w <netdb.h> następująco:

              struct hostent {
                      char    *h_name;        //oficjalna nazwa komputera 
                      char    **h_aliases;     //lista aliasów 
                      int     h_addrtype;      //typ adresu komputera 
                      int     h_length;        //długość adresu 
                      char    **h_addr_list;   //lista adresów 
              }
              #define h_addr  h_addr_list[0]   //dla zachowania zgodności  
                                               //z wcześniejszymi wersjami 
*/

#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h> //gethostbyname

struct hostent *domain_name_to_IP(char user_domain_name[512])
{
    /*
   Funkcja zamieniająca adres domenowy hosta na adres IP
   @param user_domain_name char[512] adres hosta podany przesz użytkownika
   @return struct hostent *he struktura zawierająca dane hosta
*/

    struct hostent *he;

    he = gethostbyname((const char *)user_domain_name);
    if (he == NULL)
    {
        printf("Nie ma hosta o takiej nazwie\n");
        printf("lub blad serwera DNS.\n");
        return NULL;
    }
    else
    {
        printf("Domenowo\n");
        return he; //struktura na hosta
    }
}

int main(void)
{
    char abcd[512];
    struct hostent *he;
    char wiadomosc[1024];
    struct sockaddr_in adres_hosta;
    unsigned int port;
    int gniazdo;

    //pobieranie adresu od użytkownika
    printf("Podaj adres odbiorcy w foramcie IP albo nazwa domeny: \n");
    scanf("%s", abcd);

    //pobieranie nr portu od użytkownika
    printf("Podaj numer portu odbiorcy: \n");
    scanf("%u", &port); //wskaznik

    printf("Podany adres IP: %s \n", abcd);
    printf("Podany nr portu: %d \n\n", port);

    he = domain_name_to_IP(abcd);
    if (he == NULL){
        return 1; //blad - nie ma takiego adresu
    }

    gniazdo = socket(PF_INET, SOCK_STREAM, 0); //TCP
    adres_hosta.sin_family = AF_INET;
    adres_hosta.sin_port = htons(port);
    adres_hosta.sin_addr.s_addr =  *(long *)(he->h_addr_list[0]); //[!] Tu jest magia zamieniania adresu a s_addr

    if (connect(gniazdo, (struct sockaddr *)&adres_hosta, sizeof(adres_hosta)) < 0){
        printf("Nawiazanie połączenia połączenia nie powiodło się.\n");
        return 1;
    }

    printf("Połączenie nawiazane.\nPodaj wiadomość: \n");
    fflush(stdout);
    fgetc(stdin);
    fgets(wiadomosc, 1024, stdin);
    send(gniazdo, wiadomosc, strlen(wiadomosc), 0);
    printf("Wiadomosc wyslana.\n");
    close(gniazdo);

    return 0;
}