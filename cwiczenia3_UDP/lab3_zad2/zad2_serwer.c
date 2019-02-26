/*[ZAD2] SERWER UDP
Przygotować program, który wyśle, korzystając z protokołu UDP (IPv4) na adres <adres_ip>, port (zostanie podany na tablicy) z lokalnego komputera pakiet zawierający numer indeksu przesłany jako ciąg znaków ASCII bez spacji, zakończony znakiem przejścia do nowej linii i ten sam numer, zapisany jako czterobajtowa liczba bez znaku, zapisana w kolejności sieciowej. Ewentualną odpowiedź należy zapisać jako potwierdzenie. Przykład dla numeru 123456, podane wartości szesnastkowe kolejnych bajtów (podpowiedź: do zapisu w postaci kodów ASCII można wykorzystać funcję sprintf()):
31 32 33 34 35 36 0A 00 01 E2 40
Serwer będzie uruchomiony w czasie następnych zajęć.
*/
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h> 


char buf[512];

int main(int argc, char **argv)
{
    struct sockaddr_in myaddr, endpoint;
    int sdsocket, r;
    socklen_t addrlen;
    unsigned int port;
    
    printf("Na ktorym porcie mam sluchac? : ");
    scanf("%u", &port);

    if ((sdsocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("socket() nie powiodl sie\n");
        return 1;
    }
        
    myaddr.sin_family = AF_INET;
    myaddr.sin_port = htons(port);
    myaddr.sin_addr.s_addr = INADDR_ANY;

    if (
        bind(sdsocket,
             (struct sockaddr*) &myaddr,
             sizeof(struct sockaddr_in)) < 0) {
        printf("bind() nie powiodl sie\n");
        return 1;
    }

    addrlen = sizeof(struct sockaddr_in);
    while (1) { /* nieskonczona petla */
        memset(buf, 0, 512);
        r = recvfrom(sdsocket,
                     buf,
                     512,
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);
        printf("Wiadomosc od %s: %s\n",
               inet_ntoa(endpoint.sin_addr),
               buf);
    }    
    return 0;
}