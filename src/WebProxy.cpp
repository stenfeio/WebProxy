#include <iostream>
#include <string.h>
#include <cctype>
#include <locale>
#include <boost/locale.hpp>
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
			boost::array<char, 512> reqBuf;
			boost::system::error_code error;

			size_t len = socket_.read_some( buffer(reqBuf), error);
			string domain;
			//cout << len << endl;
		//	if(error == error::eof)
		//		break;
		//	else if (error)
		//		throw boost::system::system_error(error);

			//cout.write(reqBuf.data(), len);
			//TODO parse reqBuf to get address
			//boost::array<char, len> theRequest;	
			parseRequest(reqBuf, len, domain);
			//cout << domain << endl;
			//std::string s("www.google.com");
			
			try{
				boost::array<char, 1024> resFromRemote;
				
				tcp::resolver::query query(domain, "http");
				tcp::resolver::iterator endpoints_iterator = r_.resolve(query);	
				
				connect(remoteSocket_, endpoints_iterator);
				
				boost::system::error_code ignored_error;
				write(remoteSocket_, buffer(reqBuf), ignored_error);
				
				boost::system::error_code read_error;
				size_t responseLen = remoteSocket_.read_some(buffer(resFromRemote), read_error);
				//cout.write(resFromRemote.data(), responseLen);
				write(socket_, buffer(resFromRemote, responseLen), error);
					
			}
			catch(exception& e){
				cerr << e.what() << endl;
			}
			
		}
	
	private:
		//class contructor that assigns the socket_
		tcp_connection(io_service& io_service) : socket_(io_service), r_(io_service), remoteSocket_(io_service) {}
		void handle_write(const boost::system::error_code&, size_t){}
		
		void parseRequest(boost::array<char,512> &request, size_t size, string &domain){
			bool errorFlag(false);

			string requestString(request.begin(), size);
			istringstream parser(requestString);
			string requestType;
			//string domain;
			parser >> requestType;
			
			for(int i = 0; i < 4; i++){
				parser >> domain;
			}
	
			for(int i = 0; i < requestType.length(); i++){
				if(!isupper(requestType.at(i))){
					//cout << "checking request " << endl;
					errorFlag = true;
					break;
				}	
			}
			
			/*		
			const boost::locale::generator gen;
			const std::locale loc = gen.generate(std::locale(), "");
			std::locale::global(loc);
			std::cout.imbue(loc);
			*/
			
			std::locale loc;
			if(errorFlag){
				//TODO capitalize request
				string upperTemp(requestType);
				for(int i = 0; i < requestType.length(); i++){
					upperTemp.at(i) = toupper(requestType.at(i), loc);
				}//cout << "Replacing " << requestType << " with " << toupper(requestType, loc) << endl;
				bool temp = tcp_connection::replace(requestString, requestType, upperTemp);
				
				//cout << requestString << " " << requestType << " " << toupper(requestType,loc) << endl;
				//string x("get get get"), y("get"), z("GET");
				//bool temp = tcp_connection::replace(x, y, z);

				cout << "checking request " << endl;
			}
			
			for(int i = 0; i < requestType.length(); i++){
				request[i] = requestString.at(i);
			}
			cout << requestString << endl;
			
			//cout << s << endl;
		}
		
		bool replace(std::string& str, const std::string& from, const std::string& to) {
			size_t start_pos = str.find(from);
    			if(start_pos == std::string::npos)
        			return false;
    			cout << "Inside replace ... " << endl;
    			str.replace(start_pos, from.length(), to);
    			cout << "Inside replace ... " << endl;
    			return true;
		}		
		
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
