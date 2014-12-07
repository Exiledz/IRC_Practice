#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>

class tcp_server;

using boost::asio::ip::tcp;

class tcp_connection
  : public boost::enable_shared_from_this<tcp_connection> {
 public:
  typedef boost::shared_ptr<tcp_connection> pointer;
  ~tcp_connection();

  static pointer create(tcp_server* server,
                        int user_id,
                        boost::asio::io_service& io_service);

  tcp::socket& socket();
  void write(std::string message, std::size_t num_bytes);
  std::string user_name();
  void begin();

 private:
  tcp_connection(tcp_server* server,
                 int user_id,
                 boost::asio::io_service& io_service);

  void handle_write();
  void handle_username(const boost::system::error_code& error, 
                       std::size_t bytes_transferred);

  void start_read();
  void handle_read(const boost::system::error_code& error,
                   std::size_t bytes_transferred);

  void finish();

  tcp_server* server_;
  std::string user_name_;
  int user_id_;
  tcp::socket socket_;
  boost::array<char, 128> buf_;
};

#endif // TCP_CONNECTION_H
