/*****************************************************
	Przkeazywanie danych z głównego wątku do dziecka i wyników z dizecka do głownego wątku
******************************************************
*/
#include <iostream>
#include <future>
#include <algorithm>
using namespace std;


int factorial(future<int>& f) {
	int res = 1;

	int N = f.get();//pobieranie wartosci z future
	for (int i = N; i > 1; i--)
		res *= i;

	cout << "Result is: " << res << endl;
	return res;
	//zwrócenie wyniku z child thread to parent thread
}
 
int main() {
	/*pobieranie danych z child thread do main thread
	int x; //wynik z child thread
	future<int> fu = async(launch::async, factorial, 4); 
	x = fu.get(); 
	*/

	/*Na odwrót - przekazywanie danych z parent thread do child thread 
		nie w czasie tworzenia wątku, ale w najbliższej przyszłośc (po jakimś czasie w przyszłosci) - do tego potrzeba promise
	*/
	int x;
	//zwróć uwagę że promise i future są template classes with tye of integer, because value transimitg over is an intiger
	promise<int> p;
	future<int> f = p.get_future();

	//w ten sposób mówimy child thread że wyśle mu się wartość, ale ja nie mam tej wartości jeszcze; więc ją prześle w przyszłości - jest to promise 
	//ten future jest także template of integer, bo wartośc którą dostajemy jest także intiger
	future<int> fu = async(launch::async, factorial, ref(f)); //future jako parametr dla tworzone przez async wątka
	
	//do soething later
	this_thread::sleep_for(chrono::microseconds(20));
	p.set_value(4); //ustawienie wartosci do promisa przekazanego do child thread
	//po ustawieniu child thread dostanie wartosc 

	x = fu.get();
	cout << "Get from child: " << x << endl;
	return 0;
}