/*****************************************************
Różne sposoby tworzenia wątku przez użycie callable objects 
******************************************************
*/

#include "pch.h"
#include <iostream>
#include <future>
#include <algorithm>
using namespace std;


class A {
public:
	void f(int x, char c) {}
	long g(double x) { return 0; }
	int operator()(int N) { return 0; } //klasa A jest także funktorem

};
 
int main() {
	A a;

	//pierwsza opcja uruchomienia child thread - używanie thread objcet
	thread t1(a, 6);

	//druga opcja tworzenia wątku przez funkcję async
	async(launch::async, a, 6);
	//function have similar signature, they take a callable object followed by a variable number of arguments 

	//trzeacia opcja with similar function signature
	bind(a, 6);
	//also take a callable object followed by arguments and 

	//4 sposób
	call_once(once_flag, a, 6);
	//which takes a once flag and then callable object with  arguments so as you see this kind of usage of using 
	//a collable object followed by aa variable number of arguments is quite popular in standard library

	return 0;
}