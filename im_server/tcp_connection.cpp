#include "tcp_server.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <boost/bind.hpp>
#include <ctime>
#include <iostream>
#include <map>

using boost::asio::ip::tcp;

// static
tcp_connection::pointer tcp_connection::create(tcp_server* server,
                      int user_id,
                      boost::asio::io_service& io_service) {
  return tcp_connection::pointer(new tcp_connection(server, 
                                                    user_id, 
                                                    io_service));
}

tcp_connection::~tcp_connection() {
  std::cout << "Destroyed connection for " << user_id_ << std::endl;
}

tcp::socket& tcp_connection::socket() {
  return socket_;
}

std::string tcp_connection::user_name() {
  return user_name_;
}

void tcp_connection::begin() {
  //wait for username
  socket_.async_read_some(boost::asio::buffer(buf_), 
  boost::bind(&tcp_connection::handle_username, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::handle_username(const boost::system::error_code& error,
                                     std::size_t bytes_transferred) {
  if(error == boost::asio::error::eof) {
    finish();
    return;
  }
  user_name_ = std::string(buf_.data()).substr(0, bytes_transferred);
  std::string notification = user_name_ + " has connected to the server.";
  server_->send_all_users(notification, -1);
  start_read();
}

void tcp_connection::write(std::string message, std::size_t num_bytes) {
    boost::asio::async_write(socket_, boost::asio::buffer(message, num_bytes),
        boost::bind(&tcp_connection::handle_write, shared_from_this()));
}

tcp_connection::tcp_connection(tcp_server* server,
                               int user_id,
                               boost::asio::io_service& io_service)
  : server_(server), 
    user_id_(user_id),
    socket_(io_service) {
  std::cout << "creating a new tcp_connection object" << std::endl;
}

void tcp_connection::handle_write() {}

void tcp_connection::start_read() {
  socket_.async_read_some(boost::asio::buffer(buf_), 
  boost::bind(&tcp_connection::handle_read, shared_from_this(),
              boost::asio::placeholders::error,
              boost::asio::placeholders::bytes_transferred));
}

void tcp_connection::handle_read(const boost::system::error_code& error,
                                 std::size_t bytes_transferred) {
  if(error == boost::asio::error::eof) {
    finish();
    return;
  }
  boost::posix_time::ptime now = boost::posix_time::second_clock::local_time();
  std::string message = to_simple_string(now) + " " + user_name_ + ": " +
                        std::string(buf_.data()).substr(0, bytes_transferred);
  server_->send_all_users(message, user_id_);
  start_read();
}

void tcp_connection::finish() {
  server_->remove_user(user_id_);
}

