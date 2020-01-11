//
// Created by esev on 5.1.2020.
//

#ifndef BOOKCLIENT_READFROMSOCKETHANDLER_H
#define BOOKCLIENT_READFROMSOCKETHANDLER_H



#include <unordered_map>
#include <mutex>
#include <queue>
#include "connectionHandler.h"

class readFromSocketHandler {
private:
    bool* stop_;
    std::unordered_map<std::string,int>* channelToId;
    std::mutex* lock;
    std::mutex* booklock;
    std::queue<std::string>* messageQueue;
    std::unordered_map<std::string,std::unordered_map<std::string,std::string>>* BookToOwner;
    std::vector<std::pair<std::string,std::string>>* requestedBooks;
    std::mutex* requestedBooksLock;
    ConnectionHandler* handler;
    std::string username;
public:

    readFromSocketHandler(bool* stopp, std::unordered_map<std::string,int>* channelToIdp, std::mutex* lockp, std::queue<std::string>* messageQueuep, std::mutex* booklockp,std::string username,std::unordered_map<std::string,std::unordered_map<std::string,std::string>>* BookToOwnerp,std::vector<std::pair<std::string,std::string>>* requestedBooksp,std::mutex* requestedBooksLockp, ConnectionHandler* handlerp );
    void run();
};


#endif //BOOKCLIENT_READFROMSOCKETHANDLER_H
