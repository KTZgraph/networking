/***************************************************
 *             How to avoid deadlock?              *
 ***************************************************
 1.Pomysl czy na serio potrzebujesz dwóch czy więcej mutexów? w tym samym czasie; jeśli nie to używaj tylko jednego mutexu
	-> Prefer locking single mutex at the time: 
		void sharepd_print2(string id, int value){ // NIE robić w ten sposób - brzydko!
			{
				lock_guard<mutex> locker(m_mutex);
				//.. Do something
			}
			{
				lock_guard<mutex> locker2(m_mutex2);
				cout << "From " << id << ": " << value >> endl;
			}

		}
________________________________________________________________

2. Avoiding locking a mutex and the calling a user provided function.
	Try not to lock the mutex and then call some user function, because you will never know what the user provided function will do
		-> it might end up blocking another mutex and so  
			and then you have two mutex being locked or it might even try to lock this same mutex again

________________________________________________________________

3.Use std::lock() to lock more than one mutex.
	Jeśli już naprawde musisz użyć więcej niż dwa obikety mutex to używaj funkcji standazrdowej `lock()`, żeby je zablokować, ponieważ
	standardowa funkcja `lock()` zapewnia algorytmy unikania deadlock'ów (provides some deadlock avoidance algorithm) przy zamykaniu mutexów

________________________________________________________________

4. Lock the mutex in the same order
	czasami użycie standardowej funkcji `lock()` jest niemożliwe więc wtedy nalezy blokowac mutexy we wszystkich wątkach w tej samej kolejności:
		
		void shared_print(string id, int value) {
			lock_guard<mutex> locker(m_mutex); //pierwszy mutex
			lock_guard<mutex> locker2(m_mutex2); //drugi mutex
			cout << "From " << id << ": " << value << endl;
		}

		void shared_print2(string id, int value) {
		//kolejnosc blokowania mutexów jak w innej funkcji tutaj `shared_print` 
			lock_guard<mutex> locker(m_mutex); //pierwszy mutex
			lock_guard<mutex> locker2(m_mutex2); //drugi mutex
			cout << "From " << id << ": " << value << endl;
		}

	Alternatywnie można stowrzyć hierarchiczne mutexy - 
		// when a thread is holding a lower-level mutex it is not allowed to lock a higher-level mutex
______________________________________________________________________________
*/

/* Locking resource shoul happen at an appropriate granularity:

*****************************************
*          Locking Granularity          *
*****************************************
	- Fine-granined lock: protects small amount of data
	- Coarse-grained lock : protexts big amount of data

If yours locks are to fine-grained then your program becomes tricky and you are more exposed to deadlock
If yours logs are to coarse-grained the you're loosing big oportunity of pararell computing because many 
	threads will spend a lot of time for the resources  

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
	mutex m_mutex;
	mutex m_mutex2;

	ofstream f; //resorce after is under total protection of m_mutex
	//dobra zasada - nie dawac zasobów do zewnetrznego świata - jest to gwaranacje, że program z wątkami jest bezieczny

public:
	LogFile() {
		f.open("log_t3_3.txt");
	} // Need destructor to close file
	void shared_print(string id, int value) {
		lock(m_mutex, m_mutex2);

		//parametr adopt_lock mówi locker czy mutex jest już zablokowany
			// its mean to adopt to the ownership of that new mutex
		lock_guard<mutex> locker(m_mutex, adopt_lock); //locker to nie szablon, przyjmuje obiket jako parametr
		//if you go out of scope remember to unlock the mutex 
		lock_guard<mutex> locker2(m_mutex2, adopt_lock); //drugi mutex
		cout << "From " << id << ": " << value << endl;
	}

	void shared_print2(string id, int value) {
		lock_guard<mutex> locker(m_mutex); //locker to nie szablon, przyjmuje obiket jako parametr

		lock_guard<mutex> locker2(m_mutex2); //drugi mutex
		cout << "From " << id << ": " << value << endl;
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


/*Dodatkowe info - odwrócenie kolejności mutexów powinno spowodowac mi deadlocka; 
    ale w visual studio code nie spowodowało :<; dlaczego? tego jeszcze nie wiem, ale moze kiedyś się dowiem
public:
	LogFile() {
		f.open("log_t3_3.txt");
	} 
	void shared_print(string id, int value) {
		lock_guard<mutex> locker(m_mutex); //pierwszy lockde
		lock_guard<mutex> locker2(m_mutex2); //drugi mutex
		cout << "From " << id << ": " << value << endl;
	}

	void shared_print2(string id, int value) {
        lock_guard<mutex> locker2(m_mutex2); //drugi mutex
		lock_guard<mutex> locker(m_mutex); //locker to nie szablon, przyjmuje obiket jako parametr
		cout << "From " << id << ": " << value << endl;
	}

};


*/