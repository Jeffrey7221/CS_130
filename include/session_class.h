// session_class header

using boost::asio::ip::tcp;

#ifndef SESSION_CLASS_H
#define SESSION_CLASS_H    

class session {
  public:
    session(boost::asio::io_service& io_service)
      : socket_(io_service) { };
    tcp::socket& socket();
    void start();

  private:
    void handle_read(const boost::system::error_code& error,
      size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
};

#endif