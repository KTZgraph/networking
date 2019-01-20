/*****************************************************
	Async with parameters
******************************************************
*/

#include "pch.h"
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
	future<int> fu = async(launch::deferred, factorial, 4); // launch::deferred - dodatkowy parametr dla async
	// z launch::deferred async nie utworzy wątku,it will actually defer the execution of this function until later on when the get function is called
	//so when x = fu.get(); is called the factorial function will be executed in the same thread


	future<int> fu = async(launch::async, factorial, 4); // launch::async - dodatkowy parametr dla async
	//jeśli damy parametr  launch::async to wedy async utworzy nowy wątek
	//ta opcja daje pewnosc że nowy wątek się narodzi

	future<int> fu = async(launch::async | launch::deferred, factorial, 4); // Można też oba parametry łączyć w wywołaniu funckji async
	//linia powyzej oznacza, że funckja utworzy nowy wątek or not will be 
	// determined by the implementation and this is actually the default value for this parameter
	//wiec ta linia jest tak naprawdę identyczna z 	future<int> fu = async(factorial, 4);



	x = fu.get(); // future.get() będzie czekac aż wątek się zakończy; a wtedy zwrócić wartoścf zwróconą przez child thread 
	//obiket fu moze wywołać funkcję get() TYLKO RAZ !
	//kolejny raz wywołanie fu.get(); wywali kod, chociaz się skompiluje

	
	return 0;
}