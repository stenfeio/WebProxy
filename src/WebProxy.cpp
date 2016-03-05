#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
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
	
		tcp::socket& socket(){
			return socket_;
		}
		
		//TODO
		void start(){
			
		}
	
	private:
		//class contructor
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
			//acceptor_.async_accept(new_connection->socket(), boost::bind(&tcp_server::handle_accept, this, new_connection, placeholders::error));
			acceptor_.async_accept(new_connection->socket(), boost::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));
		} 
		
		void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error){
			if(!error){
				new_connection->start();
			}
			
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
