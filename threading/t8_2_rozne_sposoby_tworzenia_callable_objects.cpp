/*****************************************************
	Różne sposoby użycia callable objects
******************************************************
*/

#include "pch.h"
#include <iostream>
#include <future>
#include <algorithm>
using namespace std;


class A {
public:
	void f(int x, char c) {}
	long g(double x) { return 0; }
	int operator()(int N) { return 0; } //klasa A jest także funktorem

};
 
void foo(int x) {}

int main() {
	A a;

	//callback object nie musi być funktore, może być zwykłą funkcją, może być także funkcją lambda 
	thread t1(a, 6); //creates by copy of a and the invoke it as a functor in a different thread 
	thread t2(ref(a), 6); //mam juz a, i na podstawie tego samego a, this will simply launch a as a functor in different thread 
	
	thread t3(A(), 6); //stworzenie a w locie, this will create tep A and then temporary A is moved into the thread object and then create a thread
	
	thread t4([](int x) {return x * x; }, 6); //funckja lambda która jako parametr przyjmuje inta 
	thread t5(foo, 7); //tworzenie wątku z funkcji
	
	thread t6(&A::f, a, 8, 'W'); //create thread with the member function; this is again making copy_of_a.f(8, 'W') 
	//and then invoke a with f function w parametreami 8 i 'W' in a diferent thread
	thread t7(&A::f, &a, 8, 'W'); //jeśli nie chcemy robić kopii z a, to mozna przekazać adres obiektu &a; to wywoła po prosu a.f(8,'W') z parametrami

	thread t8(move(a), 6); //jeśli nie chcę przekazywac a jako kopii, i ie chce jej przekazywac przez referencję, to mogę użyć move() funkcji, żeby przenieś a z obecnego wątku do wątku dziecka
	//tylko trzeba być bardzo uważnym bo a już się nie nadaje do uzycia - w tym wątku głównym/rodzicu jest po prostu pusta po przeniesieniu

	/*Te callable objects mozna używac między innymi w  funkcjach:
		1. thread() //zwraca obiekt
		2. bind()
		3. async()
		4. call_once();
	*/

	return 0;
}