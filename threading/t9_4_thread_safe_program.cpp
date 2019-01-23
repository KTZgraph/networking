/*****************************************************
	package_task - thread_safe_program
	
	użycie we współdzielonych danych z mutexem, kolejką, lokerem i condition_variable

Summary - 3 ways to get a future:
	- promise::get_future()
	- packaged_task::get_future()
	- asyns() returns a future



******************************************************
*/

#include "pch.h"
#include <iostream>
#include <future>
#include <deque>
#include <mutex>

using namespace std;

int factorial(int N) {
	int res = 1;
	for (int i = N; i > 1; i--)
		res *= i;

	cout << "Result is: " << res << endl;
	return res;
}
 
deque < packaged_task<int()> > task_q; //jest współdzielona miedzy thread1 a maina thread - we have data race condition - potrzebujemy też mutex
//lets say we have a task queue which is a deck packaged tasks and in the main function 
//I'don't want to execute the task `t` in the same function which is not very helpful, instead after creating
//the task `t` I will push it in the task queue

mutex mu;
condition_variable cond; //rozwiazanie kolejnosci wywolania main, a potem t1, zeby kolejka nie byla pusta, podczas sciagania elemntu z niej

void thread_1() {
	//will create a packaged_task `t`
	packaged_task<int()> t;
	{//zanim dobierzemy się do współdizleonej kolejki trzeba zablokowac mutex - inaczej data race
	//inny problem - t1 może zostać wywowałane zanim main doda obiekt t do kolejki - [rozwiazanie] condition_variables

		//lock_guard<mutex> locker(mu); <- nie można uzywać raz z condition_variable
		unique_lock<mutex> locker(mu);
		
		//variable cond, and before calling thread `front()` function will call cond.wait z obiektem locker i predykatem
		//majac też condition_variable nie możemy dłuzej używać lock_gurad tylko elastycznego unique
		cond.wait(locker, []() {return !task_q.empty(); }); //this predicate verifies that the task queue is not empty
		
		//UWAGA funkcje na wspoldzielonej kolejce `.front()` oraz `.pop_front()` musza byc zabezpieczone tym samym obiektem locker
		//inaczej kod nie jest ezpieczny ze wzgledu na watki - [patrz na mutex lekcje]
		t = move(task_q.front()); //pobiera taska z listy tasków packaged_task
		task_q.pop_front(); //jak element juz zostanie uzyty, to można go wywalić

	}
	t(); //wykonuje wątek


}


int main() {
	/*Now main thread will create the task and push it into task queque ant thread1 will pop of task, and execute it
	
	*/

	thread t1(thread_1);
	packaged_task<int()> t(bind(factorial, 6));

	//pobieranie jeszcze wartości z wątku z funkcji factorial
	//I can use task `t` to create a future 
	future<int> fu = t.get_future();

	{//tutaj tak samo, przed modyfikacją współdzielonej kolejki trzeba blokowac mutex
		//inny problem - t1 może zostać wywowałane zanim main doda obiekt t do kolejki - [rozwiazanie] condition_variables
		lock_guard<mutex> locker(mu);
		task_q.push_back(move(t)); //push the task `t` in the task queue, hoping that somebody will pop of the task, 
		//and execute it in an appropriate time and this somebody will be another thread for example function thread_1
		//ponieważ `t` nie jest już używany w głównej funkcji to mozna obiekt spokjnie przemieścic do kolejki
	}
	cond.notify_one();// po dodaniu elemntu zainteresowane watki sledzace zmienna stanowa zostana wzbudzone
	
	//Teraz program jest bezpieczny ze wzgledu na watki
	// 1. mając zabiezpieczoną liste mutexami
	//2. zadbanie o kolejnosc danych pobieranych/dodawanych z listy

	cout << "Warotsc z factoria z wątku t1: " << fu.get() << endl;
	t1.join();

	
	return 0;
}