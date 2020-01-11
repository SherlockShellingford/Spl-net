#include "../include/connectionHandler.h"

using boost::asio::ip::tcp;

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
#include <iostream>
#include <mutex>
#include <thread>
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>
ConnectionHandler::ConnectionHandler(string host, short port): host_(host), port_(port), io_service_(), socket_(io_service_){
    socket_.open(boost::asio::ip::tcp::v4());
    socket_.non_blocking(true);

}
void setTimeoutTrue(bool timeout){
    timeout=true;
}
void setReadTrue(bool timeout){
    timeout=true;
}

ConnectionHandler::~ConnectionHandler() {
    close();
}

bool ConnectionHandler::connect() {
    std::cout << "Starting connect to "
              << host_ << ":" << port_ << std::endl;
    try {
        tcp::endpoint endpoint(boost::asio::ip::address::from_string(host_), port_); // the server endpoint
        boost::system::error_code error;
        socket_.connect(endpoint, error);
        if (error)
            throw boost::system::system_error(error);
    }
    catch (std::exception& e) {
        std::cerr << "Connection failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }

    return true;
}

std::string ConnectionHandler::getText( int timeout) {

    boost::asio::deadline_timer timer(socket_.get_io_service());
    timer.expires_from_now(boost::posix_time::microseconds(100));

    boost::system::error_code errorCode=boost::asio::error::would_block;
    boost::asio::basic_streambuf<std::string> streambuf;
    boost::asio::async_read_until(socket_,streambuf,'\0',boost::lambda::var(errorCode)=_1);
    do(socket_.get_io_service().reset()); while (errorCode==boost::asio::error::would_block);
    if(errorCode){
        return "";
    }
    std::string ret;
    std::istream istream(&streambuf);
    std::getline(istream,ret,'\0');

    return ret;

}

bool ConnectionHandler::sendBytes(const char bytes[], int bytesToWrite) {
    int tmp = 0;
    boost::system::error_code error;
    try {
        while (!error && bytesToWrite > tmp ) {
            tmp += socket_.write_some(boost::asio::buffer(bytes + tmp, bytesToWrite - tmp), error);
        }
        if(error)
            throw boost::system::system_error(error);
    } catch (std::exception& e) {
        std::cerr << "recv failed (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}

bool ConnectionHandler::getLine(std::string& line) {
    return getFrameAscii(line, "^@");
}

bool ConnectionHandler::sendLine(std::string& line) {
    return sendFrameAscii(line, "^@");
}


bool ConnectionHandler::getFrameAscii(std::string& frame, std::string delimiter) {
    char ch;
    // Stop when we encounter the null character.
    // Notice that the null character is not appended to the frame string.
    try {
        do{
            if(!getBytes(&ch, 1))
            {
                return false;
            }
            if(ch!='\0')
                frame.append(1, ch);
        }while (delimiter != std::to_string(ch));
    } catch (std::exception& e) {
        std::cerr << "recv failed2 (Error: " << e.what() << ')' << std::endl;
        return false;
    }
    return true;
}


bool ConnectionHandler::sendFrameAscii(const std::string& frame, std::string delimiter) {
    bool result=sendBytes(frame.c_str(),frame.length());
    if(!result) return false;
    return sendBytes(&delimiter[0],2);
}

// Close down the connection properly.
void ConnectionHandler::close() {
    try{
        socket_.close();
    } catch (...) {
        std::cout << "closing failed: connection already closed" << std::endl;
    }
}