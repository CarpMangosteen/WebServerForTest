TestServer:
	g++ utils.cpp Client.cpp -o Client && \
	g++ utils.cpp Server.cpp -o Server