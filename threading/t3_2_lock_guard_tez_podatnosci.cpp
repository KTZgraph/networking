/*
Nie dizała idealnie, bo po wyjsciu poza zakres cmutexu czy lock_guard zasób będzie zawsze DOSTĘPNY (unlock)

żeby całkowicie chronić zasoby, mutex musi być zbidowany z zasobem, który chroni
*/

#include <iostream>
#include <thread>
#include <string>
#include <mutex>
using namespace std;

//mutex uzywa do synchronizowanie dostepu do zasobow
mutex mu; //tworzenie obiektu mutexa

void shared_print(string msg, int id){
	lock_guard<mutex> guard(mu); //RAII - resource acquisition is initialization; wzorzez popularny w C++
	//jezeli jednak mutex i lock_guard wyjda po za zakres, to mutex, a w konsekwencji zasoby zawsze BĘDĄ DOSTEPNE (unlock) z lub bez exceptiona
	
	cout << msg << id << endl; //inny problem - cout jest dostpene globalnie, 
	//wiec tak naprawde standardowe wyjscie może być używane przez jakiś wątek wywołujacy bezpośredno cout

	//żeby całkowicie chronić zasoby, mutex musi być zbidowany z zasobem, który chroni

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

