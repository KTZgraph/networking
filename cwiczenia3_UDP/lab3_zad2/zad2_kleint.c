/*
[ZAD2] [Klinet UDP] przesyłajacy wiadomosc
Przygotować program, który wyśle, korzystając z protokołu UDP (IPv4) na adres <adres_ip>, port (zostanie podany na tablicy) z lokalnego komputera pakiet zawierający numer indeksu przesłany jako ciąg znaków ASCII bez spacji, zakończony znakiem przejścia do nowej linii i ten sam numer, zapisany jako czterobajtowa liczba bez znaku, zapisana w kolejności sieciowej. Ewentualną odpowiedź należy zapisać jako potwierdzenie. Przykład dla numeru 123456, podane wartości szesnastkowe kolejnych bajtów (podpowiedź: do zapisu w postaci kodów ASCII można wykorzystać funcję sprintf()):
31 32 33 34 35 36 0A 00 01 E2 40
Serwer będzie uruchomiony w czasie następnych zajęć.*/

// gcc zad2_klient.c -lm -o 2K // bo inaczej nie linkuje biblioteki math.h :<

#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include "str.c"


char buf[512];

int main(int argc, char **argv)
{
    struct sockaddr_in adr;
    int gniazdo, r;
    unsigned int port;
    char abcd[512];    

    printf("Podaj adres IP odbiorcy: ");
    scanf("%s", abcd);
    printf("Podaj numer portu odbiorcy: ");
    scanf("%u", &port);
    gniazdo = socket(AF_INET, SOCK_DGRAM, 0);
    adr.sin_family = AF_INET;
    adr.sin_port = htons(port);
    adr.sin_addr.s_addr = inet_addr(abcd);
    // printf("Podaj wiadomosc: ");


    // fflush(stdout); fgetc(stdin);

    // fgets(buf, 512, (char*) create_message());
    strcpy(buf, create_message());
           
    r = sendto(gniazdo,
               buf,
               512,
               0,
               (struct sockaddr*) &adr,
               sizeof(adr));
    if (r != 512) printf("sendto() nie powiodl sie\n");
    else printf("Wiadomosc wyslana.\n");
    close(gniazdo);
    return 0;
}