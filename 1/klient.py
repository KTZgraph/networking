#coding=utf-8
import socket 
from pprint import pprint
from time import sleep
SOCKET_DATA_LENGTH = 1024

def kill_server_process(port_number):
    """ubija proces na porcie - przydatne podczas testowania"""
    #lsof -i:12345
    #kill -9 $(lsof -t -i:12345)
    # sudo kill $(sudo lsof -t -i:8000) 
    pass


class Client:
    def __init__(self, target_host="127.0.0.1", target_port=12346, recv_len=SOCKET_DATA_LENGTH):
        """Tworzy obiket socketa dla klienta"""
        self.recv_len = recv_len
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # utworzenie obiektu gniazda
        self.client_socket.connect((target_host, target_port))  # polaczenie z klientem

    def get_message_from_server(self):
        """Pobiera wiadomosc od serwera i ja wysiwetla, tak długo jak są jakieś dane"""
        
        response = self.client_socket.recv(SOCKET_DATA_LENGTH)
        print(response)


    def send_message_to_server(self, msg):
        self.client_socket.send(msg)
        self.get_message_from_server()

    def request_for_file_list(self):
        """Prosi serwer o liste dostepnych plikow"""
        self.send_message_to_server("[1]")


    def get_file_from_server(self, filename):
        """Pobiera i zapisuje plik wybrany przez użytkownika od serwera"""
        self.send_message_to_server("[2]<{filename}>".format(filename=filename))
        self.get_message_from_server()

    
    def end_connection(self):
        print("Zamykanie obiektu klienta")
        self.send_message_to_server("[3]")
        sleep(0.1)

        try:
            self.client_socket.close()
        except BaseException:
            pass


def menu():
    """Wyświetla klientowi menu z dostpenymi opcjami jakie moze żadać od serwera"""

    print( """
    [1] Wyświetl listę plików z serwera\n
    [2] Pobierz plik; wymagana <nazwa_pliku>\n
    [3] Zakończ komunikację\n
    """)


def setup():
    """ Pobiera od usera z inputu dane do stworzenia socketu klienta - tworzy obiekt klienta"""
    server_address = str(input("Podaj adres serwera: "))
    server_port = int(input("Podaj port serwera: "))

    try:
        client = Client(target_host=server_address, target_port=int(server_port))
        client.get_message_from_server() # serwer wita
        return client
    except BaseException as exception:
        print("Nie można utworzyć połączenia z serwerem: {0}:{1}.\n\tBłąd{2}".format(
            server_address, server_port, exception
        ))
        exit(-1)


def main():
    """
    Głoówny program i pętla klienta, zakańcza się gdy ten wybierze opcje `3`
    """
    client = setup()  #tworzenie obiektu klietna oraz połączenia
    option = None
    
    while option != 3: #pętla dopóki klient nie wybierze opcji 3
        if option not in ['1','2','3']:
            menu() 
    
        if option == '1':
            print(1)
            client.request_for_file_list()
            option = None # żeby dalej zostać w pętli

        if option == '2':
            print(2)
            filename = str(input("Podaj nazwe pliku: "))
            client.get_file_from_server(filename)
            option = None # żeby dalej zostać w pętli
        
        if option == '3':
            print(3)
            client.end_connection() # usuwa socket i cały obiekt klienta
            exit(1)
        
        print("option: ", option)
        option = str(input("Wybierz opcje: ")) # żeby dalej w pętli pobierac opcje od klienta


if __name__ == "__main__":
    main()
