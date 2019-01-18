/*
Tutaj wersja z problemem - jeśli kod miedzy mutex.lock() i mutex.unlock() 
rzuci wyjatek to mutex będzie zablokowany na zawsze :<
*/

#include <iostream>
#include <thread>
#include <string>
#include <mutex>
using namespace std;

//mutex uzywa do synchronizowanie dostepu do zasobow
mutex mu; //tworzenie obiektu mutexa
void shared_print(string msg, int id){
	//dewa watki nie moga uzywac cout w tym samym czasie
	mu.lock(); //blokowanie mueteszu - teraz inny watek nie bedzie mogl wyprintowac wiadomosci
	cout << msg << id << endl; //uwaga jesli tutaj bedzie exception, to wtedy mutex bedzie zablokowany na zawsze :<
	mu.unlock(); //odblokowanie, teraz nny watek moze wypisywac

}

void function_1() {
	for (int i = 0; i > -100; i--)
		shared_print("From t1: ", i);
}


int main() {
	string s = "Pierwotny";
	thread t1(function_1);
	
	for (int i = 0; i < 100; i++)
		shared_print("From main: ", i);


	t1.join(); //wait for t1 to finish
	return 0;
}

