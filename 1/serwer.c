/**
 * Implementacja wielowątkowe serwera obsługującego wiele klietów
 * protokół TCP
 * gniazda BSD
 * system Unix/Linux
 * testowany na serwerze lts.wmi***
 *
 * Tworzenie obiektu socketa dla serwera
 * Tworzenie obiektu gniazd dla klieta
 * 1. Wątki - obsługa polaczen od wielu klientow
 * 2. Pokazywanie listy dostępnych dla klienta plików
 * 3. Przesylanie za pomocą socketów pliku 
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stddef.h>
#include <netdb.h>
#include <dirent.h>  // do listowania plikow w folderze


#define SERVER_PORT_NUMBER 12346
#define BACKLOG 10 // ilosc dozwolonych połczeńw kolejce przychodzących
#define BUFFER_SIZE 10000
#define SOCKET_DATA_LENGTH 1024 // do obierania i wysylania danych recv() i send()
char BUF[BUFFER_SIZE]; //globalna zmienna na bufor




struct sockaddr_in  create_server_struct(int server_port){
    /* Zwraca obiekt gniazda serwera
    
        struct sockaddr_in { // równoważna sockaddr stworzona przez programistów do łatwijeszej pracy
            short int   sin_family; //rodzina adresów -- odpowiada sa_data z struct sockaddr
            unsigned short int sin_port; // numer portu; kolejnosć sieciowa
            struct in_addr sin_addr; //adres internetowy; sieciowa kolejność
            unsigned char siz_zero[8]; // - zerować memssetem dla zachowania rozmiaru struct sockaddr
        }
    
    @server_address
    @server_port (int): numer portu serwera
    */
    struct sockaddr_in server_struct;

    server_struct.sin_family = AF_INET;
    server_struct.sin_port = htons(server_port); // używam wybranego przez siebie nr portu - ustawiajac na 0 pozwalam funkcji bind() wybrac losowo pot za mnie
    server_struct.sin_addr.s_addr = htonl(INADDR_ANY); // puryście mówią żeby owijać INADDR_ANY w funkcję htonl - Host-to-Network-Long dla pełnej przenośności kodu; INADDR_ANY -  używa mojego adresu IP
    memset(&(server_struct.sin_zero), '\0', 8 ); //wyzerowuje reszte struktury

    return server_struct;
}

int create_socket_TCP(){
    /* buduje gniazdo TCP

    socket(int domain, int type, int protocol) - zwraca deskryptor pliku, bo w Liuksie wszystko jest plikiem
        gdzie: domain - doemna powinna być ustawiona na AF_INET
                type - mówi jądru jaki jest typ gniazda: SOCK_STREAM dla TCP / SOCK_DGRAM dla UDP
                protocol - ustawić na 0, żeby funckja socket() wybrała sama protokół bazują na parametrze type
    
    @return int socket - desktyptor pliku
    */
    return socket(AF_INET, SOCK_STREAM, 0);
}

int bind_server_socket(int sdsocket, struct sockaddr_in myaddr){
    /* binduje gniazdo; UWAGA bind jest tylko z moją strukturą gniazda; nie z klienta łaczącego się zdalnie
    struktura sockaddr jest  i sockaddr_in (in od internet) są równorzędne; 
    ta druga powstała przez programitów aby ułatwić sobie pracę

    bind(int sockfd, struct sockaddr *myaddr, int addrlen);
        gdzie: int sockfd to gniado - deskrypor pliku zwrócony przez socet(); bo w Linuxie wszystko jest plikiem (zwykły int)
               struct sockaddr *myaddr - wskaźnikna strukturę która przechowuje informacje (dokłądniej porcie i adresie IP)  o adresie gniazda dla różnych typów gniazda
               int addrlen - może być ustawiony na rozmiar structury typu sockaddr (jak powyżej): sizeof(struct sockaddr)
    
    bind() w przypadku błędu zwraca -1 i ustawia odpowiednio `errno` na wartośc błędu
           nie jest totalnie potrzbeny gdy łączymy się ze zdalnym serwerem np telnetem  wtedy sywtarczy tylko `connect`

    struct sockaddr {
        unsigned short sa_family; //rodzina adresów, AF_xxx
        char     sa_data[14]; // 14-bajtowy adres protokołowy
    };

    struct sockaddr_in { // równoważna sockaddr stworzona przez programistów do łatwijeszej pracy
        short int   sin_family; //rodzina adresów -- odpowiada sa_data z struct sockaddr
        unsigned short int sin_port; // numer portu; kolejnosć sieciowa
        struct in_addr sin_addr; //adres internetowy; sieciowa kolejność
        unsigned char siz_zero[8]; // - zerować memssetem dla zachowania rozmiaru struct sockaddr
    }

    @note: Używac tylko portów z przedziału [1024; 65535> jeśli nie sa zajęte przez inny program; chyba że jest się superuserem
    */
   int addrlen = sizeof(struct sockaddr_in);

    if(bind(sdsocket, (struct sockaddr* ) &myaddr, addrlen) < 0){
        //bind() w przypadku błedu zwraca -1
        printf("[ERROR] Bind sie nie pwoiodl :<\n");
        return -1;
    }
    printf("Bind sie udał\n");
    return 1;
}

void transfer_file_to_client(){
    /*Przesyła wybrany plik do klienta*/
}

void read_selected_file(){
    /*Odczytuje wybrany przez klienta plik*/
}

char * list_all_files_in_directory(){
    /*
    Zwraca listę plików jakie klient może pobrać
    - ograniczam sie do jednego folderu gdzie uruchamiany 
    jest serwer bo nie chce dac mozliwosci wiekszego przegldana klientowi
    lub zmulenia spowodowanego wyswietlaniem zawartosci calego dysku
    */

   char *filename_list = malloc(sizeof(char) * 1024); //alokowanie zmiennej na całą liste plikow
    int filename_list_length = 0; // zmienna do sprawdzenia czy nie trzeba czasem rozszerzyc filename_list do przechowywania calej listy plikow
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
        // printf("%s\n", dir->d_name);
          if (dir->d_type == DT_REG){ // bez tego wyswietla tez foldery a nie pliki
                printf("%s\n", dir->d_name);

                filename_list_length += sizeof(filename_list)/ sizeof(filename_list[0]);

                if (filename_list_length >= SOCKET_DATA_LENGTH){
                    //powiekszanie zmiennej
                    filename_list = (char*)realloc(filename_list, (filename_list_length + 1024) * sizeof(char)); //rozszerzenie ablicy charów o 1024 

                    if (! filename_list) { //jakby realloc sie nie udał 
                        perror("realloc");
                        exit(-1);
                    }
                }
                strcat(filename_list, dir->d_name);
                char tmp[] = "\n";
                // strcat(filename_list, (char*)'\n');
                strcat(filename_list, tmp); 

            }
        }
        closedir(d);
    }
    return filename_list;
}

int listen_for_client(int *sockfd){
    /*Czeka na połączenia przychodzące - czeka na klienta

    listen(int sockfd, int backlog);
        gdzie: sockfd - deskryptor gniazda serwera zwrócony rzez socket()
                backlog - ilosć dozwolonych połaczęń w kolejsce przychodzacych
    */
   if (listen(*sockfd, BACKLOG) < 1){
       printf("[ERROR] listen sie nie powiodł :<\n");

       return -1;
   }
   printf("Listen sie powiodl\n");
   return 1;
}

int accept_connection(int server_socket, struct sockaddr_in server_struct){
    /* akcpetuje polaczenie na gniezdzie 
    Czeka aż będzie jakieś połaczenie

    recv(sockfd, msg, len, 0);
    */
    int client_socket;
    int struct_len = (sizeof(struct sockaddr_in));
    printf("Proba accept\n\n");

    client_socket = accept(server_socket, (struct sockaddr *)&server_struct, &struct_len );
    
    if (client_socket < 0 ){
        printf("[Error] Accept się nie powiódł\n");
        return -1;
    }

    int len  = 1024;
    printf("Accept sie powidodł\n");
    char *response_msg = malloc( sizeof(char) * len);
    recv(client_socket, response_msg, len, 0 );
    printf("Od klienta: %s\n", response_msg);
    return client_socket;
}

int main(){
    printf("Czekam na połaczenie \n\n\n");
    struct sockaddr_in myaddr, endpoint;
    int server_socket, sdconnection;
    int  received;
    int server_port = SERVER_PORT_NUMBER;

    server_socket = create_socket_TCP();

    myaddr = create_server_struct(server_port);
    if (bind_server_socket(server_socket, myaddr) == -1){
        return -1;
    }

   int a;
   a = listen(server_socket, BACKLOG);
   printf("Listen zwrocil: %d\n", a);

    

    int client_socket = accept_connection(server_socket, myaddr); //zawiesza program aż połączy się klient
    char *filename_list = list_all_files_in_directory();

    printf("filenam list: %s\n\n", filename_list);

    int b = send(client_socket,  
        filename_list, 
        strlen(filename_list), 0);
    printf("Send zwrocil: %d\n\n", b);






    close(server_socket);
    return 0;
}