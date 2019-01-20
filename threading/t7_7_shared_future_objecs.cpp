/*******************************************************************
	Shared_future objects, ideal for broadcast communication model
*******************************************************************
*/

#include <iostream>
#include <future>
#include <algorithm>
using namespace std;


int factorial(shared_future<int> f) { //tutaj juz jako parametr obiekt typu shared_future, 
		//który nalezy przekazywać przez wartosć, a nie referencję
	int res = 1;

	int N = f.get();//pobieranie wartosci z future

	for (int i = N; i > 1; i--)
		res *= i;

	cout << "Result is: " << res << endl;
	//ale wyswietlanie wielu wynikow z wielu wątków na raz jest i tak brzydkie, bo nie zastosowaliśmy mutexa dla chronienia standardowego wyjścia
	/* Przykąłd wyniku w konsoli po uruchomieniu
		Result is: 24Result is:                                                                                                 
		24Result is:                                                                                                            
		24 
	*/
	return res;
}
 
int main() {
	/*sytuacja gdize zakłaadamy, że factorial musi się wykonac wiele, 
	wiele razy zamiast po prostu włączać jeden wątek chce żeby więcej wątków wykonywało tę funkcje
	*/
	int x;
	promise<int> p;
	future<int> f = p.get_future(); //teraz trzeba wywolac p2, bo p jest już puste
	shared_future<int> sf = f.share(); //tworzenie obiektu shared_future() i w odróżnieniu od zwykłej funkcji get_future() 
		//zwracajacej zwykły obiekt future można obikety z shared_future() kopiować
		//jest bardzo poręczny w broadcast communication model  

	//uruchomienie wielu wątków
	future<int> fu = async(launch::async, factorial, sf); //future jako parametr dla tworzone przez async wątka
	future<int> fu2 = async(launch::async, factorial, sf); //future jako parametr dla tworzone przez async wątka
	future<int> fu3 = async(launch::async, factorial, sf); //future jako parametr dla tworzone przez async wątka
	//... można nawet 10 wątków
	//problem polega na tym, że można przekazać tylko jeden obiket future, bo funkcja obiektFuture.get() może sie wykonać TYLKO RAZ
	// więc np. 10 wątków wyowała obiektFuture.get() 10 razy - zły pomysł :<
	//[Rozwiazanie] - stworzenie 10 obiketów promise , a potem 10 future, tak, ze każdy wątek dostanie sówj własny obiekt future
		//ale to jest clumsy- niezgrabny kod
		// na szczęscie standardowa biblioteka ma lepsze rozwiazanie którym jest shared_future
		//te obiekty można kopiowac i zamiast zwykłych obiketów z get_future() 
		// można jes przekazywac do każdego wątku przez wartosć, co jest bardzo poręczne

	p.set_value(4); //teraz każdy wątek dostanie wartośc 4 jako parametr
	




	return 0;
}