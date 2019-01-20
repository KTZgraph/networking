/***********************************************************************
	Broken promise -  no sending value from main_thread to  child
************************************************************************
*/

#include "pch.h"
#include <iostream>
#include <future>
#include <algorithm>
using namespace std;


int factorial(future<int>& f) {
	int res = 1;

	int N = f.get();//pobieranie wartosci z future
	//tu f.get() rzuci exception gdy nie podamy promise wartości
	// future_errc::broken_promise; a w moim wypadku poprostu program nie odpowiadał i wyglądał jakby cały czas mielił

	for (int i = N; i > 1; i--)
		res *= i;

	cout << "Result is: " << res << endl;
	return res;
	//zwrócenie wyniku z child thread to parent thread
}
 
int main() {
	/*Sytuacja gdy zapominamy przekazac dziecku wartość - so I have broken mypromise
		- to sprawi że 	int N = f.get();//pobieranie wartosci z future dostanie exception

	*/
	int x;
	promise<int> p;
	future<int> f = p.get_future();

	future<int> fu = async(launch::async, factorial, ref(f)); //future jako parametr dla tworzone przez async wątka
	
	//do soething later
	this_thread::sleep_for(chrono::microseconds(20));
	//p.set_value(4); //ustawienie wartosci do promisa przekazanego do child thread

	//ale jeśli jestem w sytuacji gdy naprawdę wiem, że nie mogę wysłac obiektu, to przesyłam wyjątek z obiektem exception_pointer
	p.set_exception(make_exception_ptr(runtime_error("Nie można przekazac obiektu promise do child thread :<")));
	//i teraz kiedy child wywoła get_function int N = f.get(); to dostanie ten wyjątek of runtime_error
	//so instead of sending over an exception of the infamous broken promise I can come up with a fancy excuse for broking my promise
	//program sie normalnie uruchamia tylko nic nie wyswietla, bo wątek nic nie zrobił


	return 0;
}