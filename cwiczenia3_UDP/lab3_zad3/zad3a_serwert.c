/*
[SERWER UDP] [Zadanie 3a)] 
Przesyłanie bloków 30000-bajtowych za pomocą protokołu UDP z pomiarem czasu. 
Program powinien działać wg schematu pomiar czasu, socket, 100 razy sendto i 
recvfrom, close, pomiar czasu, podanie czasu podzielonego przez 100.

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

/* rozmiar bufora */
#define BUFFER_SIZE 30000 // [Zadanie 3a)]  Przesyłanie bloków 30000-bajtowych

/* liczba powtorzen */
#define ATTEMPTS 100

char buf[BUFFER_SIZE]; //globalnnby buffer na dane


int main(int argc, char **argv)
{
    struct sockaddr_in myaddr; // struktura gniazda dla serwera
    struct sockaddr_in endpoint; // struktura dla klienta - dane na zewnatrz
    int sdsocket;
    socklen_t addrlen; // długość struktury typu struct sockaddr_in
    int received; //do dlugosci otrzymanej wiadomosci
    int server_port ; // = atoi(argv[1]);

    printf("Na ktorym porcie mam sluchac? : ");
    scanf("%u", &server_port);

    // utworzenie obiektu gniazda UDP
    sdsocket = socket(AF_INET, SOCK_DGRAM, 0); // [Zadanie 3a)] TCP - SOCK_STREAM; UDP SOCK_DGRAM

    if (sdsocket < 0) { //spr czy utworzenie obiektu gniazda UDP się udało się utworzyć
        printf("socket() nie powiodl sie\n");
        return 1;
    }

    printf("NR server_port: %d\n", server_port);

    // struktura z moimi danymi hosta - dla serwera tutaj; przy TCP i UDP robi się identycznie
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(server_port); //argv[1] - pierwszy  argument po nazwie programu to port
    myaddr.sin_addr.s_addr = INADDR_ANY;


    // W  UDP też trzeba  bindować obiekty socketów
    //bindowanie włąsnego gniazda
    if ( // http://www.linux.pl/man/index.php?command=bind
        bind(sdsocket,  //  bind - przywiązanie nazwy do gniazda
            (struct sockaddr*) &myaddr, //rzutowanie struktury typu struct sockaddr_in 
             sizeof(struct sockaddr_in)) < 0) { //addrlen = sizeof(struct sockaddr_in); - długość struktury dla naszego  serwera
        printf("bind() nie powiodl sie\n");
        return 1;
    }

    // UDP nie ma listen - nie czeka na połaczenie i  go nie nawiązuje
    // if (listen(sdsocket, 10) < 0) {
    //     printf("listen() nie powiodl sie\n");
    //     return 1;
    // }

    addrlen = sizeof(struct sockaddr_in);
    while (1) { /* nieskonczona petla */
        memset(buf, 0, BUFFER_SIZE);
        received = recvfrom(sdsocket,
                     buf,
                     BUFFER_SIZE,
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);
        printf("Wiadomosc od %s: %s\n",
               inet_ntoa(endpoint.sin_addr),
               buf);
    

    }  

    //brak close() serwer nie zamyka gniazda - wygoda, żeby kilka razy odpalić samewgo kleinta do pomiaru czasu
    return 0;
}