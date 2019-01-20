/*****************************************************
	Condition Variable
******************************************************
*/

#include "pch.h"
#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <mutex>
#include <deque>
using namespace std;

deque<int> q; //globalna zmienna Double-ended queue
mutex mu; //globalny mutex

void function_1() {//producent danych
	//oba watki zanim dobiora sie do danych blokuja mutex, to jest ok bo `q` kolejka jest współdzieloną pamiecią
	//jeśli dostep do kolejki nie byłby blokowany  przez mutex, to byśmy mieli Data Race (wyścig o dane)
	int count = 10;
	while (count > 0) {
		//w pętli while dodaje elemnt, a potem zasypia na sekundę, a potem znowu wraca do pętli
		unique_lock<mutex> locker(mu);
		q.push_front(count);
		locker.unlock();
		this_thread::sleep_for(chrono::seconds(1));
		count--;
	}
}


void function_2() {//konsument danych
	//oba watki zanim dobiora sie do danych blokuja mutex, to jest ok bo `q` kolejka jest współdzieloną pamiecią
	//jeśli dostep do kolejki nie byłby blokowany  przez mutex, to byśmy mieli Data Race (wyścig o dane)
	//w tym wątku jest problem taki, ze t2 aktywnie czeka, cały czas sprawdza czy `q` jest puste
	//jeśli `q` jest puste to odblokowuje utex i przechodzi do następnej pętli - it will keep looping
	//busy waiting is very ineficient

	int data = 0;
	while (data != 1) {
		unique_lock<mutex> locker(mu);
		if (!q.empty()) { //watek 2 sprawdza na bieżaco czy kolejka jest pusta
			data = q.back(); //jesli kolejka nie jest pusta pobiera ostatni element
			q.pop_back();
			locker.unlock();
			cout << "t2 got a value from t1: " << data << endl; //wypisuje wartość
		}
		else {
			locker.unlock();

			//jednym ze sposobem na problem ciągłego wchodzenia w pętle - busy waiiting jest czasowe uśpienie wątku
			this_thread::sleep_for(chrono::seconds(1)); //zmniejsza ilość ciągłego sprawdzania - busy waiting
			//to znacząco zmniejszy liczbę wykonywanych petli
			//problem polega na ustaleniu odpowiedniego czasu wywołania sleep ?
			//jeśli czas jest zbyt krótki to wątek dalej bedzie cały czas aktywnie czekać
			// jesli czas za to będzie za długi, to wątek moze nie odebrac danych na czas
			//[ROZWIAZANIE] wyznaczenie takiego czasu jest bardzo trudne -z rozwiazaniem przychodzą condition variables - z biblioteki standardowej

		}
	}
}


int main() {
	thread t1(function_1);
	thread t2(function_2);


	t1.join(); //wait for t1 to finish
	t2.join();
	return 0;
}