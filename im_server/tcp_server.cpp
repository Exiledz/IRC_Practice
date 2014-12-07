#include "tcp_server.h"

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <ctime>
#include <iostream>
#include <map>
using boost::asio::ip::tcp;

// static member
int tcp_server::_ID_COUNTER;

tcp_server::tcp_server(boost::asio::io_service& io_service)
  : acceptor_(io_service, tcp::endpoint(tcp::v4(), 57486)) {
  std::cout << "Starting server!" << std::endl;
  start_accept();
}

void tcp_server::send_all_users(std::string message, int sending_user) {

  for(std::map<int, tcp_connection::pointer>::iterator it = user_list_.begin();
      it != user_list_.end();
      it++) {
    if(it->first == sending_user) continue;  
    it->second->write(message, message.length());
  }
}

void tcp_server::remove_user(int user_id) {
  std::string message = user_list_[user_id]->user_name() + 
                        " has disconnected from the server.";
  user_list_.erase(user_id);
  send_all_users(message, -1);
}

void tcp_server::start_accept() {
  std::cout << "Start accept!" << std::endl;
  tcp_connection::pointer new_connection =
    tcp_connection::create(this,++_ID_COUNTER, acceptor_.get_io_service());

  acceptor_.async_accept(new_connection->socket(),
      boost::bind(&tcp_server::handle_accept, this, new_connection,
        boost::asio::placeholders::error));
}

void tcp_server::handle_accept(tcp_connection::pointer new_connection,
                   const boost::system::error_code& error) {
  std::cout << "handle accept!" << std::endl;
  if (!error) {
    user_list_[_ID_COUNTER] = new_connection;
    new_connection->begin();
  }

  start_accept();
}
