import argparse
import socket
import shlex
import subprocess
import sys
import textwrap
import threading

class NetCat:
    def __init__(self, args, buffer=None):
        self.args = args
        self.buffer = buffer
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    def run(self):
        print ("got to run")
        if self.args.listen:
            self.listen()
        else:
            self.send()
    def send(self):
        print ("got to send")
        self.socket.connect((self.args.target, self.args.port))
        #if self.buffer:
        #    self.socket.send(self.buffer)
        try:
            while True:
                recv_len = 1
                response = ''
                while recv_len:
                    data = self.socket.recv(4096)
                    recv_len = len(data)
                    response += data.decode()
                    if recv_len < 4096:
                        break
                if response:
                    print(response)
                    buffer = input('> ')
                    #buffer += '\n'
                    self.socket.send(buffer.encode())
        except KeyboardInterrupt:
            print('User terminated.')
            self.socket.close()
            sys.exit()
    def listen(self):
        print ("got to listen")
        self.socket.bind((self.args.target, self.args.port))
        self.socket.listen(5)
        while True:
            client_socket, client_address = self.socket.accept()
            client_thread = threading.Thread(target=self.handle, args=(client_socket,))
            client_thread.start()
    def handle(self, client_socket):
        print ("got to handle")
        if self.args.execute:
            output = execute(self.args.execute)
            client_socket.send(output.encode())
        elif self.args.upload:
            print("got to upload")
            file_buffer = b''
            while True:
                data = client_socket.recv(4096)
                if data:
                    file_buffer += data
                else:
                    break
            with open(self.args.upload, 'wb') as f:
                f.write(file_buffer)
            message = "f'Saved file {self.args.upload}'client_socket.send(message.encode())"
        elif self.args.command:
            print("got to command")
            cmd_buffer = b''
            while True:
                try:
                    client_socket.send(b'BHP: #> ')
                    while '\n' not in cmd_buffer.decode():
                        cmd_buffer += client_socket.recv(1024)
                        response = execute(cmd_buffer.decode())
                        if response:
                            print(response)
                            client_socket.send(response.encode())
                            cmd_buffer = b''
                    if '\n' in cmd_buffer.decode():
                        print ("found \n")
                except Exception as e:
                    print(f'server killed {e}')
                    self.socket.close()
                    sys.exit()


def execute(cmd):
    cmd = cmd.strip()
    if not cmd:
        return
    output = subprocess.check_output(shlex.split(cmd),stderr=subprocess.STDOUT, shell=True)
    return output.decode()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='BHP Net Tool', formatter_class=argparse.RawDescriptionHelpFormatter, epilog=textwrap.dedent('''
    Examples:
    netcat.py -t 192.168.1.108 -p 5555 -l -c #command shell
    netcat.py -t 192.168.1.108 -p 5555 -l -u=mytest.txt #upload to file
    netcat.py -t 192.168.1.108 -p 5555 -l -e=\"cat /etc/passwd\" # execute command
    echo 'ABC' | ./netcat.py -t SERVER -p PORT #echo text to SERVER at PORT)
    netcat.py -t HOST -p PORT #connect to HOST'''))
    parser.add_argument('-c', '--command', action='store_true', help='command shell')
    parser.add_argument('-e', '--execute', help='execute specified command')
    parser.add_argument('-l', '--listen', action='store_true', help='listen')
    parser.add_argument('-p', '--port', type=int, default=5555, help='specified port')
    parser.add_argument('-t', '--target', default='192.168.0.66', help='specified IP')
    parser.add_argument('-u', '--upload', help='upload file')
    args = parser.parse_args()
    if args.listen:
        buffer = ''
    else:
        buffer = ''

nc = NetCat(args, buffer.encode())
nc.run()

