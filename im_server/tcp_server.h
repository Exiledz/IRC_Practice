#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <boost/asio.hpp>
#include <string>

#include "tcp_connection.h"

using boost::asio::ip::tcp;

class tcp_server {
 public:
  tcp_server(boost::asio::io_service& io_service);

  void send_all_users(std::string message,
                      int sending_user);

  void remove_user(int user_id);

 private:
  void start_accept();
  void handle_accept(tcp_connection::pointer new_connection,
                     const boost::system::error_code& error);
  
  static int _ID_COUNTER;
  std::map<int, tcp_connection::pointer> user_list_; 
  tcp::acceptor acceptor_;
};

#endif // TCP_SERVER_H
