/*****************************************************
	package_task
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

	//packaged_task is a template class that is parameterised with the function signature of this task `(factorial)`,
	//so `factorial` is a function that takes an integer, and return integer so this is also a function that takes 
	//integer and return integer `<int(int)>` and when the task is executed it also needs to take an integer parameter `t(6); `
	packaged_task<int(int)> t(factorial);
	// task `t` with the factorial function is a task being packaged up to a package and that this package 
	// can be passed along to diferent places such as a diferent function or a different object or a different 
	// thread so after this many things could happen

	//... po `packaged_task<int(int)> t(factorial);` może się wiele zdarzyć

	//at a particular time of point this task is executed and
	t(6); //and can be executed in the different context other than the place where it is created, so this is a 
	//packaged task mean it's a package of tasks than can be transported to differen place in the program and being executed over there

	// I cannon conventionally get the returned value from T `int x = t(6);` NIE ZADZIALA, becaus `t` always return void
	// warotśc z obiektu packaged_task pobiera się przez `packaged_task_object.get_future().get()`
	int x = t.get_future().get(); //zwróci zwracana wartosć z factorial funckji
	cout << "X: " << x;
	return 0;
}