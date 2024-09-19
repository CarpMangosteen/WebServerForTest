TestServer:
	g++ utils.cpp Client.cpp -o Client && \
	g++ utils.cpp Epoll.cpp InetAddress.cpp Socket.cpp Server.cpp -o Server