all: server_b03705018.cpp
all: client_b03705018.cpp
	g++ -o server.out server_b03705018.cpp -I/opt/local/include -lcrypto -lssl
	g++ -o client.out client_b03705018.cpp -I/opt/local/include -lcrypto -lssl
 
