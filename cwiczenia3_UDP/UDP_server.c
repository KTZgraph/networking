/*
Podobnie jak w przypadku Przykładu 5 z Ćwiczeń 1, proszę zmodyfikować kod przykładu tak, 
aby po pierwsze użytkownik mógł podawać nazwy domenowe zamiast adresów IP, 
a po drugie aby możliwe było przesłanie dowolnej liczby komunikatów w jednej sesji 
(jednym uruchomieniu programu klienckiego).
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
    int sdsocket, numbytes;
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
    memset(buf, 0, 512);

    while (1) { /* nieskonczona petla */
        printf("TUTAJ\n");
        puts(buf);
        numbytes  = recvfrom(sdsocket,
                     buf,
                     sizeof(buf) + 1,
                     0,
                     (struct sockaddr*) &endpoint,
                     &addrlen);

        if (numbytes >= 0){
            buf[numbytes] = '\0';

        puts(buf);
        }

        memset(buf, 0, 512);


    }    
    return 0;
}