#include <iostream>
#include <thread>
#include <string>
using namespace std;

class FctorReferenceStringParameter {
public:
	void operator() (string& msg) { //w cpp kazda klasa ktora ma zdefiniowany `operator ()` moze byc wyowylana jako funkcja
		//funcktor a fajne to, że moze przechowywac stany, czego zwykla funkcja nie moze
		//operator bezargumentów
		cout << "from Thread t1: " << msg << endl;
		msg = "AAAAAAAAAAAAAAAAAAAAAAAAAAAA"; //zmiana wartości zmiennej 

	}

};


int main() {

	string s = "Pierwotny";
	thread t1( (FctorReferenceStringParameter()), ref(s) ); //prawiwe przekazanie przez referecje, bez tego
            // thread t1( (FctorReferenceStringParameter()), s ); - paramaetr z thread zostanie przekazany przez kopie, bo thread zawsze tak robi i wartoc nie ulegnie zmianie
	t1.join();
	cout << "[MAIN]: " << s << endl; //wysietli juz zmieniona wartosc 



	return 0;
}

