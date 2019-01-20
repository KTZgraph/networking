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
condition_variable cond; //zmienna typu condition_variable (zmienna stanowa) do oszacowania czasu jaki wątek ma spać

void function_1() {//producent danych
	//oba watki zanim dobiora sie do danych blokuja mutex, to jest ok bo `q` kolejka jest współdzieloną pamiecią
	//jeśli dostep do kolejki nie byłby blokowany  przez mutex, to byśmy mieli Data Race (wyścig o dane)
	int count = 10;
	while (count > 0) {
		//w pętli while dodaje elemnt, a potem zasypia na sekundę, a potem znowu wraca do pętli
		unique_lock<mutex> locker(mu);
		q.push_front(count);
		locker.unlock();
		cond.notify_one(); //Notify one waiting thread, if there is one.
		//obudzi jeden ze śpiących wątków, jeśli istnieje taki, który czeka na ten stan (condition)
		//wątek t1 dodaje dnae do globalnej kolejki, a potem powiadamia wątek2 żeby się obudził

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
		cond.wait(locker, []() {return !q.empty(); }); //this will put thread 2 in to sleep until being notidy by thread1
		//condition variables can enforce that the thread2 will go ahead and fetch the data only  
		// after thread one has pushed the data into the que. In others words it 
		// can enforce certaint part of the two threads to be executed in predefined ordee
		// Dlaczego cond.wait(locker) jako parametr przyjmuje lockera?
			// trzeba pamięc że w tym momencie mutex jest zablokowany przez wątek t2 and 
			//a thread nigdy nie powinien iść spać kiedy obługuje mutex; bo skoro używasz sleep'a to po co w ogóle używasz mutexa?
			// nie chcesz zablokowac wszsytkich kiedy śpisz
			// więc przed  funkcją wait wywołanie sleep'a w wątku2 nalezy dblokowac mutexa, a dopiero po odblokowaniu iść spać
		//thread t2 jest budzony przez powiadomienie z wątku1 a wtedy zostanie zablokowany mutex na nowo, a potem dopiero zacznie dobiera się do danych z `q`
		//a po pobraniu danych będzię odblokowywanie wąku locker.unlock()
		//ponieważ blokujemy i odblokowujemy mutex wiele razy musimy użyć unique_lock<mutex> locker<mu);
		//for condition variable we cannot use lock_guard

		//wszystko tutaj jest OK tylko wtedy gdy pętla while wykonuje się po powiadomieniu z thread1
		//ale to nie jest zadanie - thread t2 może się sam z sieibe obudzić - to jest nazywane SPURIOS WAKE
		//jeśli jest to obudzenie SPURIOUS WAKE we don't want the hread to continue running - chcemy z powrotem powrócić do spania wątku
			// więc funkcja cond.wait(locker) może pobrać jeszcze jeden parametr ,
			//which is a predicate that determinses whether the condition is reallu met for the thread to 
			//contnue running and in this case we'll use a lambda function - jeżeli `q` nie jest puste 
				// - jeśli wątek2 się obudzi i zobaczy, że `q` jest puste wątek wróci do spania
				// - jeśli wątek2 się obudzi i zobaczy, że `q` NIE jest puste to pobierze dane z `data`

		//Nalezy dodać, ze może być więcej niż jeden wątek, który czeka na ten sam stan condition, jeżeli to jest zadaniem (więcej wątków czekajacyh na ten sam warunek)
		// jesli wywołam cond.notify_one() to obudzę tylko jeden z wątków czekajacych na to samo powiadomienie
			//jeśli chce się obudzić wszsytkie takie wątki to nalezy wyołać cond.notify_all() - to obuszi wszsytkie watki

		//po obudszeniu wątek will pop of the data process the data ang to the next loop, and waiting for the next data to availble

		//condition variables są dobre do synchonizowania wykonania sekwencji wątków 
		
		//majac zmeinne typu condition_variable mamy pewnosć że wątki działą w poprawionej koeljnosci for certain poriotn of their code in 
		data = q.back(); //jesli kolejka nie jest pusta pobiera ostatni element
		q.pop_back();
		locker.unlock();
		cout << "t2 got a value from t1: " << data << endl; //wypisuje wartość
	}

}


int main() {
	thread t1(function_1);
	thread t2(function_2);


	t1.join(); //wait for t1 to finish
	t2.join();
	return 0;
}