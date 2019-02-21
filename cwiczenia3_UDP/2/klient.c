/*
gcc klient.c -lm -o C 
trzbeainaczej kompilować, bo 

/tmp/ccnfjYgM.o: In function `splitInput':
klient.c:(.text+0x80): undefined reference to `pow'
collect2: error: ld returned 1 exit status


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
#include <math.h>

#define BUFFER_SIZE 10000
#define ATTEMPTS 100

char buf[BUFFER_SIZE];


union //musi być unia nie struktura, bo unia przechowuje aktualnie tylko jedna wartosc != strktura
{
    unsigned int integer;
    unsigned char byte[4];
} foo;

int splitInput(int arr[], int sizeArr, char num[])
{
    // sizeArr -2 bo dwa ostatnie znaki to '\n'
    int liczba = 0;
    for(int i = 0; i < sizeArr -2; i++){
        // We are subtracting 48 because the numbers in ASCII starts at 48.
        arr[i] = (int)num[i] -48;
        if (arr[i] >= 0 && arr[i] < 10){
            // printf("a[i]: %d\n\n", (arr[i] * (int)pow(10, (sizeArr - (2 + i) ))));
            liczba +=  arr[i] * (int)pow(10, (sizeArr - (2 +i) -1)); // ostia cyfra dzięsiętna to liczba * 10^0
        }
    }

    return liczba;
}

 unsigned char* change_indx_to_hex( char * num_str){

    // unsigned char output[32];
    
    unsigned char *output = malloc (sizeof (unsigned char) * 32); //trzeba jawnie zaalokowac, bo lokalne zmienne są niszczone

    char *ptr = &output[0];

        size_t n = sizeof(num_str) / sizeof(num_str[0]);
        
        int liczba_cyfry[n];
        int liczba;
        liczba = splitInput(liczba_cyfry, n, num_str);

        foo.integer = liczba;
        
        for (int j=0; j < n - 1; j++){
            // printf("%2X ", (int)num_str[j]);
            ptr += sprintf(ptr, "%02X", (int)num_str[j]);
            ptr += sprintf(ptr, "%c", ' ');
        }

        for(int i=0; i<4; i++){
            ptr += sprintf(ptr, "%02X",foo.byte[i]);
            ptr += sprintf(ptr, "%c", ' ');
        }

        ptr += sprintf(ptr, "%c", '\n'); //znak konca linii 
        return output; 

}

/*
argv[1] - nazwa hosta
argv[2] - numer portu
*/
int main(int argc, char **argv)
{
    struct sockaddr_in endpoint;
    int sdsocket, addrlen, i, received;
    struct hostent *he;
    struct timeval time_b, time_e;

    if (argc<3) {
        printf("podaj nazwe hosta i numer portu jako parametry\n");
        return 1;
    }

    he = gethostbyname(argv[1]);
    if (he == NULL) {
        printf("Nieznany host: %s\n",argv[1]);
        return 0;
    }

    char  num_str[] = "123456\n";
    unsigned char *output;
    output = change_indx_to_hex(num_str);
    printf("W main: %s\n",output);


    endpoint.sin_family = AF_INET;
    endpoint.sin_port = htons(atoi(argv[2]));
    endpoint.sin_addr = *(struct in_addr*) he->h_addr;
    addrlen = sizeof(struct sockaddr_in);

    gettimeofday(&time_b, NULL);
    for (i=0; i<1; i++) {

        if ((sdsocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("socket() %d nie powiodl sie\n", i);
            return 1;
        }

        if (connect(sdsocket,(struct sockaddr*) &endpoint, addrlen) < 0) {
            printf("connect() %d nie powiodl sie\n", i);
            return 0;
        }
        // send(sdsocket, buf, BUFFER_SIZE, 0);
        send(sdsocket, output, BUFFER_SIZE, 0); // wysylanie otputu na serwer

        received = 0;
        while (received < BUFFER_SIZE)
        {
            received += recv(sdsocket, 
                             buf+received, 
                             BUFFER_SIZE-received, 
                             0);
        }
        close(sdsocket);
    }

    printf("buf od serwera: %s\n", buf); //bufer ma wartosc z serwera

    gettimeofday(&time_e, NULL);

    printf("czas: %.6f s\n",
        (((double) (time_e.tv_sec - time_b.tv_sec) * 1000000) +
        ((double) (time_e.tv_usec - time_b.tv_usec)))
        / (1000000.0 * ATTEMPTS));
    return 0;
}