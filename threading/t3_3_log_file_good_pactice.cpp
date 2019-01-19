// ConsoleApplication2.cpp : Ten plik zawiera funkcję „main”. W nim rozpoczyna się i kończy wykonywanie programu.
//

#include "pch.h"
#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <mutex>
using namespace std;

class LogFile {
private:
	std::mutex m_mutex;
	ofstream f; //resorce after is under total protection of m_mutex
	//dobra zasada - nie dawac zasobów do zewnetrznego świata - jest to gwaranacje, że program z wątkami jest bezieczny

public:
	LogFile() {
		f.open("log_t3_3.txt");
	} // Need destructor to close file
	void shared_print(string id, int value) {
		lock_guard<mutex> locker(m_mutex); //locker to nie szablon, przyjmuje obiket jako parametr
		f << "From " << id << ": " << value << endl;
	}
	//[!] Neverreturn f to the outside word
	//[!] nigdy nie robić czegoś jak implementacj funkcji: ofstream& getStream() { return f; } 

	/*[!] Never pass f as an argument to user provided function
	void processf(void fun(ofstream&)) {
		//jeśli się to zrobi, to użytkownik może zoribć poprzez jej wywołanie złe rzeczy zasobowi `f`
		//moze uzyskać dostępy pomijajac mutexa, może skopiować F to globalnej zmiennej , tak, , że każdy może dostać się do zmiennej f swoodnie
		fun(f)
	};
	*/
};

void function_1(LogFile& log) {
	for (int i = 0; i > -100; i--)
		log.shared_print("From t1: ", i);
}


int main() {
	LogFile log;
	thread t1(function_1, ref(log)); //przekazywanie przez referencje obiektu klasy
	
	for (int i = 0; i < 100; i++)
		log.shared_print("From main: ", i);


	t1.join(); //wait for t1 to finish
	return 0;
}

