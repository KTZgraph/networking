/* time constraint - umozliwienia spania odcinku czasowgo/ lub czekania do punktu czasowego*/
// chrono biblioteka dla czasu
#include "pch.h"
#include <iostream>
#include <future>
#include <mutex>

using namespace std;

int factorial(int N) {
	int res = 1;
	for (int i = N; i > 1; i--)
		res *= i;

	//cout << "Result is: " << res << endl;
	return res;
}

int main(){
	/* thread */
	thread t1(factorial, 6);
	//dodawanie do wątku czasu spania
	this_thread::sleep_for(chrono::milliseconds(3)); // watek bedzie spał przez 3 milisekundy
	//zdefiniowanie time point
	chrono::steady_clock::time_point tp = chrono::steady_clock::now() + chrono::microseconds(4);
	this_thread::sleep_until(tp);


	/* mutex */
	mutex mu;
	lock_guard<mutex> locker(mu);
	unique_lock<mutex> ulocker(mu); // wiele razy moze blokowac i odblokowywac mutex; może przenieść własnosć mutexu z jednego unique_lock do drugiego
	// zasypianie mutexów - majaa podobny mechanizm do tego występujacego w `thread`
	ulocker.try_lock(); // probuje zablokowac mutex, jesli sie nie uda to natychmiast robi `return`
	ulocker.try_lock_for(chrono::nanoseconds(500)); // po 500 nanosekundach `mutex` nie moze byc juz blokowany i funkcja natychmaist wykona `return`	
	ulocker.try_lock_until(tp); // blokowanie mutexu do punktu czasowego

	/* condition Variable */
	// do synchronizowania kolejnosci wywoływania wątków 
	condition_variable cond;
	// conditions varables też mogą czekać, ALE JAKO PIERWSYZ PARAMETR przyjmują `ULOCKER`
	cond.wait_for(ulocker, chrono::microseconds(2));
	cond.wait_until(ulocker, tp);

	/* Fututre and promise*/
	promise<int> p;
	future<int> f = p.get_future(); // `f` musi byc po `p`
	f.get(); // domylsnie get i tak wywoluje .wait() bo czeka na wartosc z watku
	// future tez moze czekać
	f.wait();
	f.wait_for(chrono::milliseconds(2));
	f.wait_until(tp); 

	/* async() */
	future<int> fu = async(factorial, 7);

	/* packaged_task */
	// szablon może być parametryzowany przez sygnaturę funkcji dla tasku, a potem task można wywołac jak normalną funkcję
	packaged_task<int(int)> t(factorial); // tez zwraca future
	future<int> fu2 = t.get_future();
	t(6); //wywolanie taska 
}


