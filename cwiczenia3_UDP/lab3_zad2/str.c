#include <stdlib.h>
#include <stdio.h>
#include <math.h> // gcc str.c -lm -o str // bo inaczej nie linkuje biblioteki
#include <netinet/in.h>


#define INDEX 123456
#define INDEX_LEN 6

union //musi być unia nie struktura, bo unia przechowuje aktualnie tylko jedna wartosc != strktura
{
    unsigned int integer;
    unsigned char byte[4];
} foo;

int* change_integer_to_int_tab(int integer){
    // zamiana integera na tablice int

    int integer_length = log10(integer) + 1;
    int *number_array = malloc(integer_length * sizeof(int));

    for (int i = 0; i < integer_length; i++) {
        number_array[integer_length-i-1] = integer %10; //żeby nie odwracać na koniec tablicy
        integer = integer / 10;
    }
    return number_array;
}

char* change_int_tab_to_char_tab(int * number_array, int array_len){
    // jeśli tablica była dynamicznie zalokowana, 
    //  to nie można sprawdzić rozmiaru przez sizeof(number_array) / sizeof(number_array[0]);
    // zamiana tablicy int na tablice char
    char * num_char_array = malloc(array_len * sizeof(char));
    for (int i=0; i< array_len; i++)
        num_char_array[i] = (char) number_array[i] + 48;
    
    return num_char_array;
}

unsigned char * create_message(){
    //Zamienia nr indeksu 123456 na formę wymaganą w ciwczeniu 3132333435360001E240 
    //tj liczby jako ascii + hex z tablicy unsigned_char[4] w kolejności sieciowej \+ znak końca linii
    unsigned char* output = malloc((INDEX_LEN*2 + 1) *sizeof( unsigned char)); // + 1 na znak konca linii
    char* ptr = &output[0];
    int* number_array = change_integer_to_int_tab(INDEX);
    char* num_char_array = change_int_tab_to_char_tab(number_array, INDEX_LEN);
    
    for(int i=0; i <INDEX_LEN; i++)
        ptr += sprintf(ptr, "%x", num_char_array[i]);
    
    foo.integer = htonl(INDEX);
    for (int i = 0; i < 4; i++)
        ptr += sprintf(ptr, "%02X", foo.byte[i]);
    
    ptr += sprintf(ptr, "%c",'\n');
    return output;
}
