/* Wersja z zakleszczeniem
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
		lock_guard<mutex> locker(m_mutex); //locker to nie szablon, przyjmuje obiket jako parametr
		lock_guard<mutex> locker2(m_mutex2); //drugi mutex
		cout << "From " << id << ": " << value << endl;
	}

	void shared_print2(string id, int value) {
		lock_guard<mutex> locker2(m_mutex2); //drugi mutex
		lock_guard<mutex> locker(m_mutex); //locker to nie szablon, przyjmuje obiket jako parametr
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