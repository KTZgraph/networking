//klient UDP

/* 
Przygotować program, który wyśle, korzystając z protokołu UDP (IPv4) na adres 150.254.77.129, port (zostanie podany na tablicy)
 z lokalnego komputera pakiet zawierający numer indeksu przesłany jako ciąg znaków ASCII bez spacji,
  zakończony znakiem przejścia do nowej linii i ten sam numer, zapisany jako czterobajtowa liczba bez znaku, 
  zapisana w kolejności sieciowej. Ewentualną odpowiedź należy zapisać jako potwierdzenie.
   Przykład dla numeru 123456, podane wartości szesnastkowe kolejnych bajtów 
(podpowiedź: do zapisu w postaci kodów ASCII można wykorzystać funcję sprintf()):
31 32 33 34 35 36 0A 00 01 E2 40
Serwer będzie uruchomiony w czasie następnych zajęć. 
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
#include <sys/time.h>

#define BUFFER_SIZE 10000
#define ATTEMPTS 100

char buf[BUFFER_SIZE];



void index(){
    /*Funkcja zamieniajaca podany ciag znaków na :
        ciąg znaków ASCII bez spacji + \n
        + numer indeksy zapisany w koljeności sieciowej

    Args:
        index (string): numer indeksu
    
    Returns:
        (string) index jako 
    
    Note:
        atoi() - wbudowana funckja zamieniająca stringi na liczby

    */
   char * index = "153131";
   int index_liczba = atoi(index);
   printf("Index liczba %d", index_liczba);



}



/*
argv[1] - nazwa hosta
argv[2] - numer portu
*/
int main(int argc, char **argv)
{
    struct sockaddr_in endpoint; //socket do serwera
    int sdsocket, addrlen, i, numbytes;
    struct hostent *host_endpoint;
    struct timeval time_b, time_e;

    if (argc<3) { //1: hosst, 2:port serwera
        printf("podaj nazwe hosta i numer portu jako parametry\n");
        return 1;
    }

    host_endpoint = gethostbyname(argv[1]); //host-enpoint he
    if (host_endpoint == NULL) {
        //zamiana 
        printf("Nieznany host: %s\n",argv[1]);
        return 0;
    }

    // ----------------------Bindowanie -----------------------------
    if(( sdsocket = socket( AF_INET, SOCK_DGRAM, 0 ) ) == - 1 ) {
        perror( "socket" );
        exit( 1 );
    }


    endpoint.sin_family = AF_INET; // host byte order
    endpoint.sin_port = htons(atoi(argv[2])); // short, network byte order
    endpoint.sin_addr = *(( struct in_addr * ) host_endpoint->h_addr );
    memset( &( endpoint.sin_zero ), '\0', 8 ); // wyzeruj resztę struktury
   
    if(( numbytes = sendto( sdsocket, argv[ 2 ], strlen( argv[ 2 ] ), 0,
    ( struct sockaddr * ) & endpoint, sizeof( struct sockaddr ) ) ) == - 1 ) {
        perror( "sendto" );
        exit( 1 );
    }
   
    printf( "sent %d bytes to %s\n", numbytes,
    inet_ntoa( endpoint.sin_addr ) );
   
    close( sdsocket );
   
    return 0;


}