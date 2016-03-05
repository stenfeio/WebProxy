#include <iostream>
#include <string.h>
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;
using boost::asio::ip::tcp;

int main(int argc, char *argv[]){
	
	if(argc < 3 || argc > 3 || argv[1] != string( "-p")){
		cout << "The correct formating is: ./WebProxy -p <port_num>" << endl;
		return -1;
	}
	
	try{
		io_service io_service;
		tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), atoi(argv[2])));

		for(;;){
			cout << "Preparing socket..." << endl;
			tcp::socket socket(io_service);
			cout << "Server online and listening..." << endl;
			acceptor.accept(socket);
			cout << "Client accepted..." << endl;
		}

	}
	catch(exception& e){
		cerr << e.what() << endl;
	}
	
	return 0;
}
