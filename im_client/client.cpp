#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/version.hpp>
#include <iostream>
using boost::asio::ip::tcp;

class IRC_User {
 public:
  IRC_User(boost::asio::io_service& io_service,
       std::string ip_address,
       std::string user_name) : socket_(io_service) {
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(ip_address, "57486");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    boost::asio::connect(socket_, endpoint_iterator);
    socket_.send(boost::asio::buffer(user_name));

    start_read();
  }
  void start_read() {
    socket_.async_read_some(boost::asio::buffer(buf_), 
    boost::bind(&IRC_User::finish_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
  }

  void send(std::string message) {
    socket_.send(boost::asio::buffer(message));
  }

 private:
  void finish_read(const boost::system::error_code& error, 
                   std::size_t bytes_transferred) {
    if(!error) {
      std::cout.write(buf_.data(), bytes_transferred);
      std::cout << std::endl;
      start_read();
    }
  }
  tcp::socket socket_;
  boost::array<char, 128> buf_;
};

void GetInput(boost::asio::io_service* io_service, IRC_User* user) {
  std::string input;
  while(true) {
    std::getline(std::cin, input);
    io_service->post(boost::bind(&IRC_User::send, user, input));
  }
}

int main(int argc, char* argv[]) {
  try {
    boost::asio::io_service io_service;
    std::string ip_address, user_name;
    std::cout << "Enter IP address of server: ";
    std::getline(std::cin, ip_address);
    if(ip_address=="")ip_address="75.142.43.130";

    std::cout << "Enter desired username: ";
    std::getline(std::cin, user_name)
    
    IRC_User user(io_service, ip_address, user_name);
    boost::thread t(boost::bind(&GetInput, &io_service, &user));
    io_service.run();
    t.join();
  }
  catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
