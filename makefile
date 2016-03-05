all:
	g++ ./src/WebProxy.cpp -o ./build/WebProxy -I/home/student/networks/P2/Alothman_Talal-webproxy/lib/boost_1_60_0 -L /home/student/networks/P2/Alothman_Talal-webproxy/lib/boost_1_60_0/stage/lib/ -lboost_system -lboost_thread -lpthread

block:
	g++ ./prac/BlockingWait.cpp -o ./build/BlockingWait -I/home/student/networks/P2/Alothman_Talal-webproxy/lib/boost_1_60_0 -L /home/student/networks/P2/Alothman_Talal-webproxy/lib/boost_1_60_0/stage/lib/ -lboost_system -lboost_thread -lpthread

syncserv:
	g++ ./prac/SyncTcpServer.cpp -o ./build/SyncTcpServer -I/home/student/networks/P2/Alothman_Talal-webproxy/lib/boost_1_60_0 -L /home/student/networks/P2/Alothman_Talal-webproxy/lib/boost_1_60_0/stage/lib/ -lboost_system -lboost_thread -lpthread
