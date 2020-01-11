//
// Created by esev on 6.1.2020.
//
#include <boost/asio.hpp>
#include "connectionHandler.h"
#ifndef BOOKCLIENT_READFROMKEYBOARDHANDLER_H
#define BOOKCLIENT_READFROMKEYBOARDHANDLER_H


class readFromKeyboardHandler {
ConnectionHandler* handler;
std::string username;
public:
    void run(ConnectionHandler* handler,std::string username);

};
#endif //BOOKCLIENT_READFROMKEYBOARDHANDLER_H
