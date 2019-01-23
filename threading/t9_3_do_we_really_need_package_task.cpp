/*****************************************************
	package_task - czy  naprawdę potrzebujemy tego packaged_task?
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

/*
	packaged_task<int()> t(bind(factorial, 6));
	//do we really need packaged_task?
	//it seems we can do all these things by just using the function object
	t();
*/

	auto t = bind(factorial, 6);
	/*so it seems like the function object can serve our purpose well we don't need a packaged_task 
	the main advantage of a packaged_task is that it can link a callable object to a future and that is very 
	important in a threading environment*/

	
	return 0;
}