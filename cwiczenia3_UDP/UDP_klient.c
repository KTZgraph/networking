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

struct hostent* domain_name_to_IP(char user_domainname[512]){
    /*Funckja zamieniajaca adres domenowy na hosta na adres IP - zwraca strukturę z danymi 
    @param user_domainname char[512] adres hosta podany przez użytkownika
    @param struct hostent *he struktura zawierajaca dane hosta
    */

   struct hostent *he;

   he = gethostbyname((const char*) user_domainname); //cała tablica jako argument
   if (he == NULL){
       printf("Nie ma hosta o takiej nazwie \n");
       printf("lub błąd serwera DNS\n");
       return NULL;
   }
   printf("Domenowo działa\n");
   return he; // struktura hostent
}

int main(int argc, char **argv)
{
    struct sockaddr_in adr;
    int gniazdo, r;
    unsigned int port;
    char abcd[512];
    struct hostent *he;


    printf("Podaj adres IP odbiorcy: ");
    scanf("%s", abcd);
    printf("Podaj numer portu odbiorcy: ");
    scanf("%u", &port);

    he = domain_name_to_IP(abcd);
    if (he == NULL){
        return  -1; //błąd - brak hosta o takiej nazwie
    }


    gniazdo = socket(AF_INET, SOCK_DGRAM, 0);
    adr.sin_family = AF_INET;
    adr.sin_port = htons(port);
    
    //adr.sin_addr.s_addr = inet_addr(abcd);
    adr.sin_addr.s_addr = *(long*)(he->h_addr_list[0]); //tutaj rzutowanie

    while (1){
        printf("Podaj wiadomosc: ");
        scanf("%s", buf); // fgetc(stdin); [!] UWAGA [!] stdin jest upośledzony - 
        //w kolejnych pętlach zżrea pierwszy znak (działa ładnie tylko przy pierwszej iteracji)
        
        fflush(stdout);

            
        r = sendto(gniazdo,
                buf,
                sizeof(buf),
                0,
                (struct sockaddr*) &adr,
                sizeof(adr));
        
        printf("BUF1: %s\n", buf);
        // buf[r] = 0;
        if (r != 512) printf("sendto() nie powiodl sie\n");
        else printf("Wiadomosc wyslana.\n");
        // memset (buf, 0, sizeof(buf)); //zerowanie buffera
        bzero(buf,512); //zerowanie bufora alternatywa

    }
    close(gniazdo);
    return 0;
}