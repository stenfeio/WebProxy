#include <iostream>
#include <string.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace std;
using namespace boost::asio;
using boost::asio::ip::tcp;

class tcp_clientCus{
	public:
		tcp_clientCus(io_service& io_service/*, const string server, const int port*/): resolver(io_service), clientSocket(io_service){
		
			cout <<"Just wrote" << endl;	
			connectToServer("www.google.com", 80);
		}
		
		//void setIPPort(string server, int port){
		//	connectToServer(server, port);	
		//}
	private:
		void connectToServer(string serverName, int serverPort){
			try{
				cout << "Trying to talk to server.." << endl;
				tcp::resolver::query query(serverName, "http");
				tcp::resolver::iterator endpoints_iterator = resolver.resolve(query);	

				connect(clientSocket, endpoints_iterator);
			
				boost::system::error_code ignored_error;
				write(clientSocket, buffer(""), ignored_error);

				boost::system::error_code read_error;
				size_t responseLen = clientSocket.read_some(buffer(httpResBuf), read_error);
				cout.write(httpResBuf.data(), responseLen);
			}
			catch(exception& e){
				cerr << e.what() << endl;
			}
		}	
		

		tcp::resolver resolver;
		tcp::socket clientSocket;
		//boost::asio::buffer requestBuffer("");
		boost::array<char, 1024> httpResBuf;
};

class tcp_connection: public boost::enable_shared_from_this<tcp_connection>{
	public:
		typedef boost::shared_ptr<tcp_connection> pointer;
		
		static pointer create(io_service& io_service){
			return pointer(new tcp_connection(io_service));
		}
		
		//returns a reference to the socket being used for this
		//tcp_connection.
		tcp::socket& socket(){
			return socket_;
		}
		
		//TODO to parse and possibly output what's being read
		//from the client. //TODO Connect to server based on 
		//what was parsed.
		void start(){
			boost::array<char, 1024> buf;
			boost::system::error_code error;

			size_t len = socket_.read_some(buffer(buf), error);

		//	if(error == error::eof)
		//		break;
		//	else if (error)
		//		throw boost::system::system_error(error);

			cout.write(buf.data(), len);
			
			std::string s("www.google.com");
			//int p = 80;
			//tcp_clientCus clientCus(io_service);
			//clientCus.setIPPort(s,p);
			
			try{
				//cout << "Trying to talk to server.." << endl;
				//tcp::resolver resolverTool(io_service);
				//tcp::socket remoteSocket(io_service);
				boost::array<char, 1024> resFromRemote;
				
				tcp::resolver::query query("www.google.com", "http");
				tcp::resolver::iterator endpoints_iterator = r_.resolve(query);	
				
				connect(remoteSocket_, endpoints_iterator);
				
				boost::system::error_code ignored_error;
				write(remoteSocket_, buffer(""), ignored_error);
				
				boost::system::error_code read_error;
				size_t responseLen = remoteSocket_.read_some(buffer(resFromRemote), read_error);
				cout.write(resFromRemote.data(), responseLen);
					
			}
			catch(exception& e){
				cerr << e.what() << endl;
			}
			
		}
	
	private:
		//class contructor that assigns the socket_
		tcp_connection(io_service& io_service) : socket_(io_service), r_(io_service), remoteSocket_(io_service) {}
		void handle_write(const boost::system::error_code&, size_t){}
		tcp::socket socket_;
		string m_message;
		tcp::resolver r_;
		tcp::socket remoteSocket_;
};

class tcp_server{
	public:
		tcp_server(io_service& io_service, int port) : acceptor_(io_service, tcp::endpoint(tcp::v4(), port)){
			start_accept();
		}
	
	private:
		void start_accept(){
			tcp_connection::pointer new_connection = tcp_connection::create(acceptor_.get_io_service());
			
			//begin accepting clients asyncronously
			acceptor_.async_accept(new_connection->socket(), boost::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));

		} 
		
		void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error){
			//if we don't get an error with our accepted tcp connection,
			//we call start() to get the job done. TODO modify start() 
			//to parse incoming requests and connect to the right server. 
			if(!error){
				new_connection->start();
			}
			
			//start accepting some more. This is the main part of the
			//async feature.
			start_accept();
		} 
		
		tcp::acceptor acceptor_;
};

int main(int argc, char *argv[]){
	
	if(argc < 3 || argc > 3 || argv[1] != string( "-p")){
		cout << "The correct formating is: ./WebProxy -p <port_num>" << endl;
		return -1;
	}
	
	try{
		io_service io_service;
		tcp_server server(io_service, atoi(argv[2]));
		//tcp_clientCus talal (io_service, "asfd", 5);
	        io_service.run();	

	}
	catch(exception& e){
		cerr << e.what() << endl;
	}
	
	return 0;
}
