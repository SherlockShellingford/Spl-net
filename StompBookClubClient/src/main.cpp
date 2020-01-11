//
// Created by esev on 5.1.2020.
//


#include <string>
#include <iostream>
#include <cstring>
#include "../include/connectionHandler.h"

int main(int argc, char** argv){
    bool stopinitialloop=false;
    std::string input="";
    while(!stopinitialloop){
        std::getline(std::cin, input);
        std::string command=std::strtok(&input[0], " ");

        if(command=="login"){
            int firstparamstart=input.find('{');
            int firstparamend=input.find('}');
            std::string firstparam=input.substr(firstparamstart+1,firstparamend-1);
            int secondparamstart=input.find('{',firstparamend);
            int secondparamend=input.find('}',firstparamend+1);
            std::string secondparam=input.substr(secondparamstart+1,secondparamend-1);
            int thirdparamstart=input.find('{',secondparamend);
            int thirdparamend=input.find('}',secondparamend+1);
            std::string thirdparam=input.substr(thirdparamstart+1,thirdparamend-1);
            ConnectionHandler handler (firstparam.substr(0,firstparam.find(':')-1),atoi(&firstparam.substr((firstparam.find(':')+1, firstparam.length()-1))[0]));
            if(!handler.connect()){
                std::cout<<"Could not connect to the server";
                continue;
            }
            std::string msg="CONNECT\naccept-version:1.2\nhost:";
            msg.append(firstparam.substr(0,firstparam.find(':')-1));
            msg.append("\n");
            msg=msg+"login:";
            msg=msg+secondparam+"\n";
            msg=msg+"passcode:"+thirdparam+"\n^@";
            handler.sendLine(msg);
            std::string recipent;
            handler.getLine(recipent);
            if(recipent.find("wrong password")!=-1){
                std::cout<<"Wrong password";
                continue;
            }

            if(recipent.find("user already logged")){
                std::cout<<"User already logged in";
                continue;
            }
            std::string username=secondparam;
            stopinitialloop=true;
            std::cout<<"Login successful";



        }
    }


}
