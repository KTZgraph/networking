/*****************************************************
	unique_lock vs lock_guard
******************************************************
*/

#include "pch.h"
#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <mutex>
using namespace std;

class LogFile {
private:
	mutex _mu; // to synchronize the access of `_f` fstream
	ofstream _f;

public:
	LogFile() {
		_f.open("log.txt");
	} // Need destructor to close file
	void shared_print(string id, int value) {
		//lock_guard<mutex> locker(_mu); //lock_guard - jeden ze sposobów na blokowanie mutexu
		//inny sposób to wywoływanie _mu.lock() i _mu.unlock() funckji, co jest NIEzalecane !

		//3. Sposób blokowanie mutexu - unique_lock
		// unique_lock<mutex> locker(_mu); //podobny do lock_guard ale wprowadza większą elastycznosć, tutaj bez parametru defer_lock
		//unique_lock provides a more flexible way to implement finer-grained lock 
		//with unique_lock you can even construct the locker without actually locking the mutex to do that trzeba dodac dodatkowy parametr defer_lock
		unique_lock<mutex> locker(_mu, defer_lock); //now the locker is the owner of the mutex, but mutex is not locked yet

		// ... można robić coś inengo, co nie wymaga dostępu do `_f` fstream - nie wymaga blokowania mutexa

		locker.lock(); // Przed uzyciem chronionych dostępów trzbea zablokowac mutex
		_f << "From " << id << ": " << value << endl; //tylko to printowanie ma być synchornizowae za pomocą mutexa

		locker.unlock(); //mozna odblokowac mutex, jesli kolejne kroki nie wymagaja jego zablokowania
		//np po wykoniu operacji na _f trzeba wykonac jeszcze wiele różnych instukcji; ale zakładamy, że kolejne operacje nie wymagją tego, zeby mutex był zablokowany
	
		//locker.lock(); // po odblokowaniu można znowu zablokować mutex; 
			//you can lock and unlock arbitrary number of times in the code which you cannot do with lock_guard

		/*Uwaga klasy wrappery dla mutexa - lock_guard i unique_lock NIGNY NIE MOGĄ BYĆ kopiowane !; ale unique_lock can be move()
			When you move a unique_lock you are transfering ownership of a mutex from one unique_lock to another unique_lock
		*/

		unique_lock<mutex> locker2 = move(locker); //to move ownership  of the mutex from locker to locker2 and lock_guard can't never be moved
		/*Skoro unique_locker jest taki fajny elastyczny to dlaczego w ogóle uzywac lock_guard?
			- flexibility of unique_lock is not free - a unique_lock is a little moe heavy weighted than lock_guard;
				so if a performance is what we really concern about and you don't really need the extra flexibility of the unique_lock
				you might just use the lock_guard
		*/

	}


};


void function_1(LogFile& log) {
	for (int i = 0; i > -100; i--)
		log.shared_print("From t1: ", i);
}


int main() {
	LogFile log;
	thread t1(function_1, ref(log)); //przekazywanie przez referencje obiektu klasy

	for (int i = 0; i < 100; i++)
		log.shared_print2("From main: ", i);


	t1.join(); //wait for t1 to finish
	return 0;
}