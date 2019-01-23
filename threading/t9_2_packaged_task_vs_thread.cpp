/*****************************************************
	package_task porównanie z thread
******************************************************
*/

#include <iostream>
#include <future>
#include <algorithm>
using namespace std;

int factorial(int N) {
	int res = 1;
	for (int i = N; i > 1; i--)
		res *= i;

	cout << "Result is: " << res << endl;
	return res;
}
 

int main() {

	thread t1(factorial, 6); //tworzenie obiektu thread, parametr do funkcji `factorial`
	//podaje się jako drugi parametr dla tej funckji w konstuktorze wątka, i ten parametr będzie traktowany jako parametr funkcji `factorial`
	//ale nie da sie tego zrobić dla packaged_task
	
	/*packaged_task<int(int)> t(factorial);//nie można przekazać dodatkowych parametrów jak w konstruktorze thread, ale można użyć funkcji bind
	t(6); //and can be executed in the different context other than the place where it is created*/

	packaged_task<int()> t(bind(factorial, 6)); /*żeby połączyć/zbindowac funkcję factorial z jej parametrem and create a function object 
	// and this function object is the passed to the constructor of the package task to create a package task 
	// note that this new constructed function object `(bind(factorial, 6)` cannot take parameter anymore, becaues the parameter 
	// is already bundled with the factorial function so this template argument also needs to remve integer parameter
	*/

	//po zbindowaniu factorial w tworzeniu `packaged_task<int(int)> t(bind(factorial, 6))` nie mozna wywołać `t` z parametrami
	t(); 
	//so a packaged_task is created differently from the way a thread is created 


	

	return 0;
}