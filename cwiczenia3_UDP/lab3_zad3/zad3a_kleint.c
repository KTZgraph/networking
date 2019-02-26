/*
[KLIENT UDP] [Zadanie 3a)] 
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
#include <sys/time.h>

#define BUFFER_SIZE 30000 //[Zadanie 3a)]  Przesyłanie bloków 30000-bajtowych
#define ATTEMPTS 100

char buf[BUFFER_SIZE];

/*
argv[1] - nazwa hosta
argv[2] - numer portu
*/
int main(int argc, char **argv){
    // budowanie UDP
    struct sockaddr_in socket_struct_for_server; //struktura do komunikowania się z serwerem
    int gniazdo;
    int received_data_len;  // długość odebranych danych
    unsigned int server_port; //numer portu na którym serwer słucha
    char server_address_abcd_form[512]; 
    struct timeval time_start, time_end; // do pomiaru czasu
    int i = 0; // licznik do petli


    printf("Podaj adres IP odbiorcy: ");
    scanf("%s", server_address_abcd_form);
    printf("Podaj numer portu odbiorcy: ");
    scanf("%u", &server_port);
        //. Tworzenie struktury gniazda UDP dla serwera
        socket_struct_for_server.sin_family = AF_INET; // to jest zawsze
        socket_struct_for_server.sin_port = htons(server_port); // zamiana porstu na format sieciowy
        socket_struct_for_server.sin_addr.s_addr = inet_addr(server_address_abcd_form);



    printf("LKLIENT GNIAZDO: %d", gniazdo);


    gettimeofday(&time_start, NULL); // pomiar czasu [START

    while ( i < ATTEMPTS){
        // // Tworzenie gniazda UDP; SOCK_DGRAM - protokół UDP; 
        //tworzę w pętli bo zamykam też je w pętli
        gniazdo = socket(AF_INET, SOCK_DGRAM, 0);
        // memset(buf, 0, BUFFER_SIZE);

        // 100 razy tworzy połącznie, wysyła dane, zamyka połączenie 
        received_data_len = sendto(gniazdo,
                buf,
                BUFFER_SIZE,
                0,
                (struct sockaddr*) &socket_struct_for_server,
                sizeof(socket_struct_for_server));
        // 
        if (received_data_len != BUFFER_SIZE){
            printf("sendto() nie powiodl sie\n");
            printf("received_data_len: %d\n", received_data_len);
        }
        else 
        printf("Wiadomosc wyslana.\n");
        
        i++;
        // każdorazowe zamykanie gniazda
        close(gniazdo); //zamykanie gniazda za kazdym razem

    }


    gettimeofday(&time_end, NULL);  // pomiar czasu [KONIEC]

    // manipulowanie czasem, zeby dostać średnią z operacji
    printf("czas: %.6f s\n",
        (((double) (time_end.tv_sec - time_start.tv_sec) * 1000000) +
        ((double) (time_end.tv_usec - time_start.tv_usec)))
        / (1000000.0 * ATTEMPTS));

    return 0;
}
