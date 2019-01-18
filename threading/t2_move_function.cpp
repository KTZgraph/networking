#include <iostream>
#include <thread>
#include <string>
using namespace std;

class Funktor {
public:
	void operator() (string msg) { //w cpp kazda klasa ktora ma zdefiniowany `operator ()` moze byc wyowylana jako funkcja
		//funcktor a fajne to, że moze przechowywac stany, czego zwykla funkcja nie moze
		//operator bezargumentów
		cout << "from Thread t1: " << msg << endl;
		msg = "NOWAAAAAAAAAAAAAAAAAAAA warotsc";
		cout << "from Thread t1 after change `msg`: " << msg << endl;

	}

};


int main() {
	string s = "Pierwotny";
	thread t1( (Funktor()), move(s) );
	cout <<"Id wątku t1" << t1.get_id() << endl;
	t1.join();

	cout << "ID głownego watku" << this_thread::get_id() << endl;

	//cout << "[MAIN]: " << s << endl;



	return 0;
}

