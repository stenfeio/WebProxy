#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace std;
using namespace boost::asio;
using boost::asio::ip::tcp;

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
		}
	
	private:
		//class contructor that assigns the socket_
		tcp_connection(io_service& io_service) : socket_(io_service){}
		void handle_write(const boost::system::error_code&, size_t){}
		
		tcp::socket socket_;
		string m_message;
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
		io_service.run();	

	}
	catch(exception& e){
		cerr << e.what() << endl;
	}
	
	return 0;
}
