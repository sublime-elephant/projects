import socket

target_host = "192.168.0.199"
target_port = 7778
#create a socket object
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

#connect the client
client.connect((target_host,target_port))
print(f"Successfully connected to {target_host}:{target_port}")
#THIS LINE IS ONLY FOR THE PROXY USE CASE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
client.send(b'192.168.0.199:9004')
attempt_connection_message = client.recv(4096)
print(attempt_connection_message.decode())
sesame = client.recv(4096)
print(sesame.decode())
#THIS LINE IS ONLY FOR THE PROXY USE CASE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
inputter = ''
while inputter != b'oblock':
    inputter = input("> ")
    inputter=inputter.encode()
    client.send(inputter)
    msgack = client.recv(4096)
    if "CalledProcessError" in msgack.decode():
        print(f"\"{inputter.decode()}\" raised an error")
        continue
    print (msgack.decode())
    console = client.recv(4096)
    print (console.decode())
print("Client has received safe-word. Exiting")
client.close()


