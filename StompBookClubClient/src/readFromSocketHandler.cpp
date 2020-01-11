//
// Created by esev on 5.1.2020.
//

#include <vector>
#include "../include/readFromSocketHandler.h"

    std::string getLastLine(std::string str){
        int indexOfLastLine=0;
        while((indexOfLastLine=str.find('\n',indexOfLastLine))!=-1){}
        return str.substr(indexOfLastLine+1,str.length()-1);
}
readFromSocketHandler::readFromSocketHandler(bool *stopp, std::unordered_map<std::string, int> *channelToIdp,
                                             std::mutex *lockp, std::queue<std::string>* messageQueuep, std::mutex *booklockp,
                                             std::string username,
                                             std::unordered_map<std::string, std::unordered_map<std::string, std::string>> *BookToOwnerp,
                                             std::vector<std::pair<std::string, std::string>> *requestedBooksp,
                                             std::mutex *requestedBooksLockp,ConnectionHandler *handlerp) : stop_(stopp), channelToId(channelToIdp), lock(lockp),handler(handlerp), booklock(booklockp),
                                             username(username),BookToOwner(BookToOwnerp),requestedBooks(requestedBooksp), requestedBooksLock(requestedBooksLockp), messageQueue(messageQueuep)
                                             {}
void readFromSocketHandler::run() {
    bool stopThisThread=false;
    std::unordered_map<std::string,std::unordered_map<std::string,std::string>> peopleThatIOweTo;

    while(!stopThisThread){
        std::string incomingMessage;
        incomingMessage=handler->getText(100);
        if(incomingMessage!=""){



        if(incomingMessage.substr(0,6)=="RECEIPT"){
            if(stop_){
                stopThisThread=true;
                std::cout<<"Connection terminated gracefully";
                continue;
            }
        }
        if(incomingMessage.substr(0,4)=="ERROR"){
            int indexOfMessage=incomingMessage.find("message:");
            int endIndexOfMessage=incomingMessage.find('\n',indexOfMessage);
            std::string message=incomingMessage.substr(indexOfMessage+8,endIndexOfMessage-(indexOfMessage+8));
            std::cout<<"Error: "+message;
        }
        if(incomingMessage.substr(0,6)=="MESSAGE"){
            if(incomingMessage.find(" has added the book ")!=-1){
                std::string lastLine=getLastLine(incomingMessage);
                std::cout<<lastLine<<std::endl;
                continue;
            }
            if(incomingMessage.find(" wish to borrow ")!=-1){
                std::string lastLine=getLastLine(incomingMessage);
                std::cout<<lastLine<<std::endl;
                int firstparamstart=incomingMessage.find("destination:");
                int firstparamend=incomingMessage.find("\n",firstparamend);
                std::string firstparam=incomingMessage.substr(firstparamstart+12,firstparamend-1-(firstparamstart+12));
                int secondparamstart=incomingMessage.find(" wish to borrow ",firstparamend);
                int secondparamend=incomingMessage.find('\n',secondparamstart+1);
                std::string secondparam=incomingMessage.substr(secondparamstart+15,secondparamend-1-(secondparamstart+15));
                std::string message="";
                booklock->lock();
                std::unordered_map<std::string,std::unordered_map<std::string,std::string>>::const_iterator it=BookToOwner->find(firstparam);
                if(it!=BookToOwner->end()){
                    std::unordered_map<std::string,std::string>::const_iterator it2=BookToOwner->at(firstparam).find(secondparam);
                    if(it2!=BookToOwner->at(firstparam).end()){
                        message="SEND\ndestination:"+firstparam+"\n"+username+" has "+secondparam+"\n\0";
                        booklock->unlock();
                        lock->lock();
                        handler->sendLine(message);
                        lock->unlock();

                    }
                    else{
                        booklock->unlock();
                    }
                }
                else {
                    booklock->unlock();
                }

            }
            if(incomingMessage.find(" has ")!=-1){
                std::string lastLine=getLastLine(incomingMessage);
                std::cout<<lastLine<<std::endl;
                int firstparamstart=incomingMessage.find(" has ");
                int firstparamend=incomingMessage.find("\n",firstparamend);
                std::string firstparam=incomingMessage.substr(firstparamstart+5,firstparamend-1-(firstparamstart+5));
                std::vector<std::string>* test;
                std::string secondparam=lastLine.substr(0,lastLine.find(" has "-1));
                int thirdparamstart=incomingMessage.find("destination:");
                int thirdparamend=incomingMessage.find("\n",thirdparamend);
                std::string thirdparam=incomingMessage.substr(thirdparamstart+12,thirdparamend-1-(thirdparamstart+12));

                requestedBooksLock->lock();

                for(int i=0;i<requestedBooks->size();++i){

                        if(requestedBooks->at(i).first==firstparam&&requestedBooks->at(i).second==thirdparam){
                            requestedBooks->erase(requestedBooks->begin()+i);
                            i=i-1;
                        }

                }
                requestedBooksLock->unlock();

                std::string message="SEND\ndestination:"+firstparam+"\nTaking "+firstparam+" from "+secondparam+"\n\0";
                lock->lock();
                handler->sendLine(message);
                lock->unlock();

            }
            if(incomingMessage.find("Taking ")!=-1){
                std::string lastLine=getLastLine(incomingMessage);
                std::cout<<lastLine<<std::endl;
                std::string firstparam=lastLine.substr(7,lastLine.find(" from ")-1-7);
                std::string secondparam=lastLine.substr(lastLine.find(" from ")+6,lastLine.find("\n")-1-(lastLine.find(" from ")+6));
                int thirdparamstart=incomingMessage.find("destination:");
                int thirdparamend=incomingMessage.find("\n",thirdparamend);
                std::string thirdparam=incomingMessage.substr(thirdparamstart+12,thirdparamend-1-(thirdparamstart+12));
                if(secondparam==username){
                    booklock->lock();
                    std::unordered_map<std::string,std::unordered_map<std::string,std::string>>::const_iterator it=BookToOwner->find(thirdparam);
                    if(it!=BookToOwner->end()){
                        std::unordered_map<std::string,std::string>::const_iterator it2=BookToOwner->at(thirdparam).find(firstparam);
                        if(it2!=BookToOwner->at(firstparam).end()){
                            std::string previousOwner=BookToOwner->at(thirdparam).at(firstparam);
                            BookToOwner->at(thirdparam).erase(firstparam);

                            booklock->unlock();
                            std::unordered_map<std::string,std::unordered_map<std::string,std::string>>::const_iterator it=peopleThatIOweTo.find(thirdparam);
                            if(it==peopleThatIOweTo.end()) {
                                peopleThatIOweTo.insert(std::pair<std::string,std::unordered_map<std::string,std::string>>(thirdparam,std::unordered_map<std::string,std::string>()));

                            }
                            peopleThatIOweTo.at(thirdparam).insert(std::pair<std::string,std::string>(firstparam,previousOwner));




                            }
                        else{
                            booklock->unlock();
                            std::cout<<"People try to take from me but I didn't have the book"<<std::endl;
                        }
                    }

                    else {
                        booklock->unlock();

                        std::cout<<"People try to take from me but I didn't have the book"<<std::endl;
                    }
                }

            }
            if(incomingMessage.find("Returning ")!=-1){
                std::string lastLine=getLastLine(incomingMessage);
                std::cout<<lastLine<<std::endl;
                std::string firstparam=lastLine.substr(10,lastLine.find(" from ")-1-10);
                std::string secondparam=lastLine.substr(lastLine.find(" from ")+6,lastLine.find("\n")-1-(lastLine.find(" from ")+6));
                int thirdparamstart=incomingMessage.find("destination:");
                int thirdparamend=incomingMessage.find("\n",thirdparamend);
                std::string thirdparam=incomingMessage.substr(thirdparamstart+12,thirdparamend-1-(thirdparamstart+12));
                if(secondparam==username){
                    booklock->lock();
                    std::unordered_map<std::string,std::unordered_map<std::string,std::string>>::const_iterator it=BookToOwner->find(thirdparam);
                    if(it!=BookToOwner->end()){
                        BookToOwner->at(thirdparam).insert(std::pair<std::string,std::string>(firstparam,secondparam));
                        booklock->unlock();
                        peopleThatIOweTo.at(thirdparam).erase(firstparam);

                    }

                    else {
                        booklock->unlock();
                    }
                }

            }
            if(incomingMessage.find("Book status ")!=-1){
                std::string lastLine=getLastLine(incomingMessage);
                std::cout<<lastLine<<std::endl;

            }
        }
        lock->lock();
        while(messageQueue->empty()){
            std::string msg=messageQueue->front();
            messageQueue->pop();
            lock->unlock();
            handler->sendLine(msg);
            lock->lock();
        }
        lock->unlock();


        }
        handler->close();
        delete handler;
        delete messageQueue;
        delete BookToOwner;
        delete channelToId;
        delete lock;
        delete booklock;
        delete requestedBooks;
        delete requestedBooksLock;

    }
}