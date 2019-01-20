/*****************************************************
	Async
******************************************************
*/

#include <iostream>
#include <future>
#include <algorithm>
using namespace std;


int factorial(int N) {
	int res = 1;
	for (int i = N; i > 1; i--)
		res *= 1;

	cout << "Result is: " << res << endl;
	return res;
	//zwrócenie wyniku z child thread to parent thread
}
 
int main() {
	int x; //wynik z child thread
	//thread to klasa, a async() to funkcja
	//Funckja async() moze ale nie musi utworzyć kolejnego wątku , a to może być kontrolowane przez inny parametr


	//ogólnie future reprezentuje obiket, którego wartosć możesz odebrac wprzyszłości
	future<int> fu = async(factorial, 4); //async zwraca bardzo ważną rzecz - future 
	//future to kanał gdzi emogę pobrac wynik z child thread 
	x = fu.get(); // future.get() będzie czekac aż wątek się zakończy; a wtedy zwrócić wartoścf zwróconą przez child thread 
	//obiket fu moze wywołać funkcję get() TYLKO RAZ !
	//kolejny raz wywołanie fu.get(); wywali kod, chociaz się skompiluje

	
	return 0;
}