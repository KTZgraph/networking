/*****************************************************
	Moving Promise and Futue
******************************************************
*/

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
	/*Promise i future obikety nie mogą być kopiwoane mogą być jedynie move() 
	dkładnie tak samo jako thread i unique_lock */

	int x;
	promise<int> p;
	//promise<int> p2 = p; //Nieskompiluje się nawet, bo promise moze być tylko przesunięty
	//promise<int> p2 = move(p); można tylko przesuwać
	//i identycznie dla future obiektów
	future<int> f = p.get_future(); //teraz trzeba wywolac p2, bo p jest już puste

	future<int> fu = async(launch::async, factorial, ref(f)); //future jako parametr dla tworzone przez async wątka
	
	this_thread::sleep_for(chrono::microseconds(20));

	p.set_exception(make_exception_ptr(runtime_error("Nie można przekazac obiektu promise do child thread :<")));



	return 0;
}