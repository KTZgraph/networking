#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(void)
{
    unsigned int port;
    char bufor[1024];
    int gniazdo, gniazdo2;
    struct sockaddr_in adr, nadawca;
    socklen_t dl = sizeof(struct sockaddr_in);

    printf("Na ktorym porcie mam sluchac? : ");
    scanf("%u", &port);
    gniazdo = socket(PF_INET, SOCK_STREAM, 0);
    adr.sin_family = AF_INET;
    adr.sin_port = htons(port);
    adr.sin_addr.s_addr = INADDR_ANY;
    if (bind(gniazdo, (struct sockaddr *)&adr,
             sizeof(adr)) < 0)
    {
        printf("Bind nie powiodl sie.\n");
        return 1;
    }
    if (listen(gniazdo, 10) < 0)
    {
        printf("Listen nie powiodl sie.\n");
        return 1;
    }
    printf("Czekam na polaczenie ...\n");
    while ((gniazdo2 = accept(gniazdo,
                              (struct sockaddr *)&nadawca,
                              &dl)) > 0)
    {
        memset(bufor, 0, 1024);
        recv(gniazdo2, bufor, 1024, 0);
        printf("Wiadomosc od %s: %s\n",
               inet_ntoa(nadawca.sin_addr),
               bufor);
        close(gniazdo2);
    }
    close(gniazdo);
    return 0;
}