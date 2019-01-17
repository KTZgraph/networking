#include <iostream>
#include <thread>

using namespace std;

void function_1() {
	cout << "Beaty is only skin-deep" << endl;
}

int main() {
	thread t1(function_1); //stworzenie watku
	//można tylko raz użyć .join() albo .detach()
    t1.detach(); //watek zyje wlasnym zyciem --daemon; jednak nic nie wyprintuje, 
                    //bo włascicielem standardowego wyjsaic jest watek głowny, który kończy się szybciej niż czas tworzeni watku
                    //gdyby bylo t1.join, to watek głowny czeka na zakończenie tego peoceu

    
    //zasada jest taka, że jeśli drugi wątek ma dostęp/używa zasobób pierwsze wątku, 
    //to pierwszy - tutaj główny wątek musi żyć tak długo, dopóki dziecko wątek używa tych zasobów
    
	if (t1.joinable()) {
		cout << "Jest joinable" << endl;
		t1.join();
	}

	return 0;
}