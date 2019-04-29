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
 * 3. Przesylanie za pomocą socketów wybranego przez klienta pliku 
 * 
 * 
 *     gcc serwer.c -lpthread -o S

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
#include<pthread.h> // do watkow; uwaga na komende klompilacji! [gcc serwer.c -lpthread -o S]
#include <fcntl.h> // for open
#include <unistd.h> // for close


#define SERVER_PORT_NUMBER 12345
#define BACKLOG 10 // ilosc dozwolonych połczeńw kolejce przychodzących
#define BUFFER_SIZE 10000
#define SOCKET_DATA_LENGTH 1024 // do obierania i wysylania danych recv() i send()

char BUF[BUFFER_SIZE]; //globalna zmienna na bufor
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER; //semafor do blokowania


/********************* Deklaracje ****************************/
int get_message_from_client(int client_socket);
void * server_handler(void * client_socket);
int accept_connection(int server_socket, struct sockaddr_in server_struct);
void end_client_connection(int client_socket);

/*************************** Definicje /***************************/

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

void bind_server_socket(int sdsocket, struct sockaddr_in myaddr){
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

            exit(-1);
    }
    printf("Bind sie udał\n");
}

void start_listen(int server_socket){
    /*Serwer rzpoczyna nasłuchiwanie

    listen(int sockfd, int backlog);
        gdzie: sockfd - deskryptor gniazda serwera zwrócony rzez socket()
                backlog - ilosć dozwolonych połaczęń w kolejsce przychodzacych
    listen() zwraca -1 w przypadku błedu i ustawia odpowiednio zmienna errno
    */

    if (listen(server_socket, BACKLOG) == -1){
       printf("[Error] Listen sie nie powiodl\n");
       exit(-1);
   }
   printf("Listen sie powiodl\n");
}

int transfer_file_to_client(int client_socket, char* filename ){
    /*Przesyła wybrany plik do klienta*/
    
    char file_data[SOCKET_DATA_LENGTH];

    FILE *fs = fopen(filename, "r");
    if(fs == NULL)
    {
        printf("[ERROR] Brak pliku '%s'. :< \n", filename);
        return -2;
    }

    bzero(file_data, SOCKET_DATA_LENGTH); 

    int fs_block_sz; 

    while((fs_block_sz = fread(file_data, sizeof(char), SOCKET_DATA_LENGTH, fs))>0)
    {
        if(send(client_socket, file_data, fs_block_sz, 0) < 0)
        {
            printf("[ERROR] Send nie wysłał pliku '%s' :<.\n", filename);
            return -1;
        }
        bzero(file_data, SOCKET_DATA_LENGTH);
    }
    printf("Udało sie wysłać plik '%s' do kleinta <3 \n", filename);
    return 1;

}

char* list_all_files_in_directory(){
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
                // printf("%s\n", dir->d_name);

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


int accept_connection(int server_socket, struct sockaddr_in server_struct){
    /* akcpetuje polaczenie na gniezdzie 
    Czeka aż będzie jakieś połaczenie

    zwraca socket dla clienta
    */

    int client_socket;
    int struct_len = (sizeof(struct sockaddr_in));
    printf("Proba accept\n\n");

	
    client_socket = accept(server_socket, (struct sockaddr *)&server_struct, &struct_len);
        //dopoki klienci sie lacza tworz nowe watki dla klientow przychodzacych

    if (client_socket < 0 ){ // nieudane accept()
        printf("[Error] Accept się nie powiódł\n");
        return -1; // nie wylaczam progeamu gdy np jeden z wielu klientow sie nie polaczyl
    }

    return client_socket;
}

int send_message_to_client(int client_socket, char * msg){
    /*Wysyła wiadomosc do klienta; czsami wiadomosc jest dluzsza 
    @param int client_socket - gniazdo/deskryptor klienta
    @param char *msg wiadomosc do klienta(string)
    */
    int send_length = send(client_socket,  
        msg, 
        strlen(msg), 0);
    
    if (send_length == 0){
        printf("[Error] nie udało sie wysłac wiadomosci do klienta\n");
        return -1;
    }

    return 1;
}

char* find_filename_in_message(char* client_message){
    /*znajduje nazwe pliku pomiedzy znakami <nazwa_pliku.rozszerzenie>
    i ją zwraca - do komunikacji z klientem by przeslac mu wybrany przez siebie plik
    zakłdam że znaki < - informujacy o początku nazwy pliku
    i znak `>` informujacy że ońcyz się nazwa pliku ystępuje tylko raz; 
    gdyby zakonczenia wystepowały wielokrotnie należy użyć `str.find_last_of(STOPDELIMITER);`
    */
    client_message = "[3]<a.py>";
    int message_len = strlen(client_message);
    int start = -1; //poczatkowy indeks
    int stop = -1; //koncowy indeks

    int i =0;
    while(i < message_len){
        if(client_message[i] == '<'){
            start = i+1;
        }
        if(client_message[i] == '>'){
            stop = i-1;
        }
        i++;
    }

    char* filename = malloc(sizeof(char)*(stop-start+1)); //na razie alokuje na jeden znak
    int j = start;
    for(j; j<= stop; j++ ){
        filename[j-start] = client_message[j];
    }
    filename[stop] = '\0';

    return filename;
}

int get_message_from_client(int client_socket){ //Cała komunikacja i sterowanie z kliente,
    /* Odbiera wiadomosc od klienta; sprawdza tresc -
    Klient wysyła pojedyncze wiadomosci lub `[2]<nazwa_pliku.rozszerzenie>` 
    dlatego nie zwiększam bufora response_msg

    jeśli klient wysłał widomosc o tresci [3] zamyka połączenie z tym klientem
    
    jesli sa tam slowa kluczowe opcji to je wywoluje
    @param int client_socket - gniazdo/deskryptor klienta
    */

    char *response_msg = malloc( sizeof(char) * SOCKET_DATA_LENGTH);

    while(1) {
        // spawdza występowanie słowa np `[1]`, `[2]`, `[3]` w odpowiedzi klienta
        printf("[Klient]: %s\n", response_msg);
        recv(client_socket, response_msg, SOCKET_DATA_LENGTH, 0 );

        if ( strstr(response_msg, "[1]")){
            printf("TUTAJ [1]");
            // jeśli klietn wybrał pierszą opcję to wysyła klientowi liste plikow
            char* filename_list = list_all_files_in_directory();
            send_message_to_client(client_socket, filename_list);
        }

        if ( strstr(response_msg, "[2]")){
            send_message_to_client(client_socket, "Przesylanie pliku");
            // sprawdzanie nazwy pliku
            char* filename = find_filename_in_message(response_msg);
            transfer_file_to_client( client_socket, filename );
        }

        if ( strstr(response_msg, "[3]")){
            end_client_connection(client_socket);
            return 1;
        }

    };

    end_client_connection(client_socket);
    return 1;
}

void end_client_connection(int client_socket){
    /*Zamyka polaczenie z klientem*/
    printf("Zakoczenie polaczenia z klientem\n");
    send_message_to_client(client_socket, "[Serwer] Połaczenie zamkniete przez klienta");
    close(client_socket);

}


void* server_handler(void * thread_socket){ //Najrrudniejsza funkcja [!]
    /* Handler dla watku klienta
    w watkach przekazywanie warosic jest trudniejsze i dziwniejsze

    @ param: thread_socket - socket klienta do watku zwrocony przez accept() z accept_connection()
    */
   
    // pthread_mutex_lock(&lock); //niepotrzebny nie wspoldziele danych miedzy klientami

    static char message_from_client[SOCKET_DATA_LENGTH]; // iadomosc od klienta
    int read_size, write_size;
    int client_socket = *(int*) thread_socket; // client_socket zostanie zamkniety tutaj
    printf("Funkcja Server handler\n");


    //wyslanie takiego mojego ACK Z serwera DO klienta
    send_message_to_client(client_socket, "Serwer wita :)\n");

    if (get_message_from_client(client_socket)){
        printf("Klient sam sie rozaczyl\n");
        pthread_exit(NULL);
        return 0;
    }


    if (client_socket){
            end_client_connection(client_socket); // zamykanie po zakonczeniue komunikacji jak sie nie uda  w get_message_from_client
    }
    pthread_exit(NULL);
    return 0;
}

int main(){
    printf("Czekam na połaczenie \n\n\n");
    struct sockaddr_in myaddr, endpoint;
    int server_socket, client_socket;
    int server_port = SERVER_PORT_NUMBER; // na sztywno ustawiono dla szybszego testowania/ bez inputu

    server_socket = create_socket_TCP(); //tworzenie gnaizda dla serwera
    myaddr = create_server_struct(server_port); //struktura przechowujaca dane serwera

    bind_server_socket(server_socket, myaddr); //1. Bindowanie
    start_listen(server_socket); //2. Rozpoczenie nasluchu

    // Akceptacja przychodzace polaczenie klienta
    pthread_t clients_thread_list[20];
    int i = 0; //licznik watkow; i jakos mi bardziej pasuje jak Licznik
    while(1){
        //Pętla główna dla serwera do akceptowania klientow i 
        client_socket = accept_connection(server_socket, myaddr); //zawiesza program aż połączy się klient

        // mu
        if( pthread_create(&clients_thread_list[i], NULL, server_handler, &client_socket) != 0 )
           printf("Failed to create thread\n");
        if( i >= BACKLOG) //gdzie BACKLOG to parametr z funkcji listen() 
        //mowiąca o maksymalnej liczbie połaczeń w kolejnce; po cichu z reguły ustawiona na 5 lub 10
        {
          i = 0; //licznik watkow
          while(i < BACKLOG){
            // musi byc joinowanie w watku glownym, bo inaczej main nie czeka na zakonczenie dzialania watku klietna
            pthread_join(clients_thread_list[i++],NULL); //joinowanie watkow w glownym watku main
          }
          i = 0; //licznik watkow
        }
    }

    close(server_socket); //zamykanie serwera
    return 0;
}
