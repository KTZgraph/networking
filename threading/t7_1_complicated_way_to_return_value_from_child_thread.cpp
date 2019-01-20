/************************************************************************************************
*	Return from thread child to thread parent in compicated way - mutex and condition_variable  *
**************************************************************************************************
*/

#include "pch.h"
#include <future>
#include <iostream>
#include <thread>
#include <mutex>
using namespace std;

mutex mu;
condition_variable cond;

void factorial(int N, int& x) {
	int res = 1;
	for (int i = N; i > 1; i--)
		res *= 1;

	cout << "Result is: " << res << endl;
	x = res;
	//zwrócenie wyniku z child thread to parent thread
}
 
int main() {
	int x; //wynik z child thread
	thread t1(factorial, 4, ref(x)); //przekazywanie x przez referencję,
	//ale to za mało, bo x jest teraz wspódzieloną zmienną miedzy child thread a main thread - dlatego potrzeba jakiegoś mutexa
	//dodatkowo trzeba się upeniwć, że zmienną x najpierw zmienił child thread a dopiero potem main thread ją wypisuje - potrzeba condition_variable
	//a teraz kod się stal bardizej skomplikowany - trzbea zablokować i odblokować mutex; trzeba wywołać condition.notify() i condition.wait()
	// a co ważniejsze mamy teraz dwie zmienne globalne,o które trzeba dbać - struktura kodu trochę sie zabałaganiła

	t1.join(); //wait for t1 to finish
	return 0;
}