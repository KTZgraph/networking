/*
Przygotować program, który wyśle, korzystając z protokołu UDP (IPv4) na adres 150.254.77.129, port (zostanie podany na tablicy) z lokalnego komputera pakiet zawierający numer indeksu przesłany jako ciąg znaków ASCII bez spacji, zakończony znakiem przejścia do nowej linii i ten sam numer, zapisany jako czterobajtowa liczba bez znaku, zapisana w kolejności sieciowej. Ewentualną odpowiedź należy zapisać jako potwierdzenie. Przykład dla numeru 123456, podane wartości szesnastkowe kolejnych bajtów 
(podpowiedź: do zapisu w postaci kodów ASCII można wykorzystać funcję sprintf()):
31 32 33 34 35 36 0A 00 01 E2 40
Serwer będzie uruchomiony w czasie następnych zajęć. 
*/

//Serwer UDP
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>


// Serwer
/* rozmiar bufora */
#define BUFFER_SIZE 10000

/* liczba powtorzen */
#define ATTEMPTS 100

char buf[BUFFER_SIZE];


/*
argv[1] - numer portu
*/
int main(int argc, char **argv)
{
    struct sockaddr_in myaddr; // moj_adres
    struct sockaddr_in  endpoint; // adres z ktorym sie komunikuje
    int sdsocket, sdconnection, addrlen, received;
    int bytes;
    int server_port;

    if (argc < 2) {
        printf("podaj numer portu jako parametr\n");
        return 1;
    }

    // atoi( "60" )= 60 liczba; zamienia str na int ale z awartosci stringa, a nie jak chary po bajtach
    server_port = atoi(argv[1]); //port na którym nasłuchiwać będze tutaj mój serwer

    // Tworzenie obiektu gniazda dla UDP
    //SOCK_DGRAM - połączenia datagramowe
    //AF_INET - 
    if(( sdsocket = socket( AF_INET, SOCK_DGRAM, 0 ) ) == - 1 ) {
        perror( "socket" );
        exit( 1 );
    }

/*
INADDR_ANY jest używany, gdy nie musisz wiązać gniazda z określonym adresem IP. 
Kiedy użyjesz tej wartości jako adresu podczas wywoływania funkcji bind (), 
gniazdo akceptuje połączenia ze wszystkimi adresami IP urządzenia.

AF_INET IPv4 Internet protocols ip(7)
When INADDR_ANY is specified in the bind call, the socket will be bound to all local interfaces.
*/

    //Tworzenie struktury odpowiadajacjęj za połączenie z mojej strony
    myaddr.sin_family = AF_INET; // host byte order
    myaddr.sin_port = htons( server_port );//htons - hotst-to-network-short: z formatu lokalnego liczba -> format sieciowy liczby <netinet/in.h>
    myaddr.sin_addr.s_addr = INADDR_ANY; // uzupełnij moim adresem IP
    memset( &( myaddr.sin_zero ), '\0', 8 ); // wyzeruj resztę struktury

/*
memset <string.h>  Wypełnia kolejne bajty w pamięci ustaloną wartością.

Przykład:
    buffer - adres początkowy
    c - wpisywana wartość (dla napisu - numer znaku)
    num - ile bajtów zapisać

    void * memset ( void * buffer, int c, size_t num );

Wartość zwracana

    buffer


użycie:

#include <stdio.h>
#include <string.h>

int main()
{
    char napis[] = "jakis napis";   // kopiujemy napis do tablicy znakow
    memset(napis, '*', 5);
    puts(napis);
    return 0;
}

Wynik działania:

 ***** napis

*/

// -----------------BINDOWANIE adresu io z portem ------------------
    if( bind( sdsocket,( struct sockaddr * ) & myaddr,
    sizeof( struct sockaddr ) ) == - 1 ) {
        perror( "bind" );
        exit( 1 );
    }


// obsluga polaczenie przez obiekty socketow
    addrlen = sizeof( struct sockaddr );
    if(( bytes = recvfrom( sdsocket, buf, BUFFER_SIZE - 1, 0,
    ( struct sockaddr * ) & endpoint, & addrlen ) ) == - 1 ) {
        perror( "recvfrom" );
        exit( 1 );
    }


// wypisanie otrzymanych danych z obcego hosta
    printf( "got packet from %s\n", inet_ntoa( endpoint.sin_addr ) );
    printf( "packet is %d bytes long\n", bytes );
    buf[ bytes ] = '\0';
    printf( "packet contains \"%s\"\n", buf );
   
   //zamykanie obiektu gniazda UDP
    close( sdsocket );
   
    return 0;
}