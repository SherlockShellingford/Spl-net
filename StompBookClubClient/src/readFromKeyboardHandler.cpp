
#include <string>
#include <vector>
#include <cstring>
#include <iostream>
#include "../include/readFromKeyboardHandler.h"
#include "../include/readFromSocketHandler.h"
#include<sys/socket.h>
#include<arpa/inet.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>

using boost::asio::ip::tcp;

void readFromKeyboardHandler::run(ConnectionHandler* handler,std::string username) {
    bool stop=false;
    std::string input="";
    int sock=-1;
    std::atomic<int>* recipentnum=new std::atomic_int(rand()%100 );
    sockaddr_in client;
    int id=1;
    std::mutex *lock=new std::mutex();
    std::mutex *bookslock=new std::mutex();
    
    std::unordered_map<std::string,std::unordered_map<std::string,std::string>>* BookToOwner=new  std::unordered_map<std::string,std::unordered_map<std::string,std::string>>();
    std::vector<std::pair<std::string,std::string>>* requestedBooks=new std::vector<std::pair<std::string,std::string>>() ;
    std::mutex* requestedBooksLock=new std::mutex();
    std::unordered_map<std::string,int>* channelToId=new  std::unordered_map<std::string,int>();
    std::queue<std::string>* messageQueue;
    readFromSocketHandler socketHandler(&stop, channelToId,lock, messageQueue,bookslock,username,BookToOwner,requestedBooks,requestedBooksLock,handler);
    std::thread t(&readFromSocketHandler::run, socketHandler);

    if(t.joinable()){
        t.detach();
    }

    while(!stop){
        std::getline(std::cin, input);
        std::string command=std::strtok(&input[0], " ");


        if(command=="join"){
            int firstparamstart=input.find('{');
            int firstparamend=input.find('}');
            std::string firstparam=input.substr(firstparamstart+1,firstparamend-1-(firstparamstart+1));
            bookslock->lock();
            BookToOwner->insert(std::pair<std::string,std::unordered_map<std::string,std::string>>(firstparam, std::unordered_map<std::string,std::string>()));
            bookslock->unlock();

            std::string message="SUBSCRIBE\ndestination:"+firstparam+"\nid:"+std::to_string(id)+"receipt:"+std::to_string((recipentnum++)->load())+"\n\0";
            lock->lock();
            messageQueue->push(message);
            lock->unlock();
            id=id+1;

        }
        if(command=="exit"){

            int firstparamstart=input.find('{');
            int firstparamend=input.find('}');
            std::string firstparam=input.substr(firstparamstart+1,firstparamend-1-(firstparamstart+1));
            std::string message="UNSUBSCRIBE\nid:"+std::to_string(id)+"receipt:"+std::to_string(channelToId->at(firstparam))+"\n\0";
            lock->lock();
            messageQueue->push(message);
            lock->unlock();

        }
        if(command=="borrow"){
            int firstparamstart=input.find('{');
            int firstparamend=input.find('}');
            std::string firstparam=input.substr(firstparamstart+1,firstparamend-1-(firstparamstart+1));
            int secondparamstart=input.find('{',firstparamend);
            int secondparamend=input.find('}',firstparamend+1);
            std::string secondparam=input.substr(secondparamstart+1,secondparamend-1-(secondparamstart+1));
            requestedBooks->push_back(std::pair<std::string,std::string>(secondparam,firstparam));
            std::string message="SEND\ndestination:"+firstparam+"\n+"+username+" wish to borrow "+secondparam+"\nˆ@";
            lock->lock();
            messageQueue->push(message);
            lock->unlock();
        }
        if(command=="add"){

            int firstparamstart=input.find('{');
            int firstparamend=input.find('}');
            std::string firstparam=input.substr(firstparamstart+1,firstparamend-1-(firstparamstart+1));
            int secondparamstart=input.find('{',firstparamend);
            int secondparamend=input.find('}',firstparamend+1);
            std::string secondparam=input.substr(secondparamstart+1,secondparamend-1-(secondparamstart+1));
            bookslock->lock();
            std::unordered_map<std::string,std::unordered_map<std::string,std::string>>::const_iterator it=BookToOwner->find(firstparam);
            if(it==BookToOwner->end()){
                std::unordered_map<std::string,std::string> toAdd;
                BookToOwner->insert(std::pair<std::string,std::unordered_map<std::string,std::string>>(firstparam,toAdd));

            }
            BookToOwner->at(firstparam).insert(std::pair<std::string,std::string>(secondparam,""));

            bookslock->unlock();
            std::string message="SEND\ndestination: "+firstparam+"\n"+username+" has added the book "+secondparam+"\0";
            lock->lock();
            messageQueue->push(message);
            lock->unlock();


        }
        if(command=="return"){
            int firstparamstart=input.find('{');
            int firstparamend=input.find('}');
            std::string firstparam=input.substr(firstparamstart+1,firstparamend-1-(firstparamstart+1));
            int secondparamstart=input.find('{',firstparamend);
            int secondparamend=input.find('}',firstparamend+1);
            std::string secondparam=input.substr(secondparamstart+1,secondparamend-1-(secondparamstart+1));
            bookslock->lock();
            std::unordered_map<std::string,std::unordered_map<std::string,std::string>>::const_iterator it=BookToOwner->find(firstparam);
            if(it!=BookToOwner->end()){
                std::string lender=BookToOwner->at(firstparam).at(secondparam);
                BookToOwner->at(firstparam).erase(secondparam);
                bookslock->unlock();
                std::string message="SEND\ndestination:"+firstparam+"\nReturning "+secondparam +" to "+lender+"\n\0";
                lock->lock();
                messageQueue->push(message);
                lock->unlock();
            }


        }
        if(command=="status"){
            int firstparamstart=input.find('{');
            int firstparamend=input.find('}');
            std::string firstparam=input.substr(firstparamstart+1,firstparamend-1-(firstparamstart+1));
            std::string message="SEND\ndestination:"+ firstparam+"\nbook statusˆ@";
            std::string message2="SEND\ndestination:"+firstparam+"\n";
            std::vector<std::string> books;
            bookslock->lock();
            books.reserve(BookToOwner->at(firstparam).size());
            for(auto book: BookToOwner->at(firstparam)){
                books.push_back(book.first);
            }
            bookslock->unlock();

            for(int i=0;i<books.size();++i){
                if(i<books.size()-1) {
                    message = message + books[i] + ",";
                }
                else{
                    message = message + books[i];
                }
            }
            message=message+"\n\0";
            lock->lock();
            messageQueue->push(message2);
            lock->unlock();

        }
        if(command=="logout"){

            std::string message="DISCONNECT\nreceipt:"+std::to_string((recipentnum++)->load()) +"\n\0";
            lock->lock();
            messageQueue->push(message);
            lock->unlock();

            stop=true;

        }

    }
}