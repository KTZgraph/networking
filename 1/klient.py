#coding=utf-8
import socket 

def kill_server_process(port_number):
    """ubija proces na porcie - przydatne podczas testowania"""
    #lsof -i:12345
    #kill -9 $(lsof -t -i:12345)
    pass


class Client:
    def __init__(self, target_host="127.0.0.1", target_port=12346, recv_len=4096):
        """Tworzy obiket socketa dla klienta"""
        self.recv_len = recv_len
        self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # utworzenie obiektu gniazda
        self.client_socket.connect((target_host, target_port))  # polaczenie z klientem

    def send_message_to_server(self, msg):
        self.client_socket.send(msg)
        response = self.client_socket.recv(4096)
        print(response)
        return response

    def request_for_file_list(self):
        """Prosi serwer o liste dostepnych plikow"""
        self.send_message_to_server("daj liste plikow")

    def get_file_from_server(self, filename):
        """Pobiera i zapisuje plik wybrany przez użytkownika od serwera"""
        self.send_message_to_server("daj " + filename)




def main():
    try:
        print("Klinet")
        client = Client()
        client.send_message_to_server("Ala ma kota")
    except BaseException as e:
        import traceback; traceback.print_exc()
        print("E: ", e)

if __name__ == "__main__":
    main()