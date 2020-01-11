package bgu.spl.net.srv;

import javax.crypto.Cipher;
import java.sql.Connection;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

public class ConnectionsImpl<T> implements Connections<T> {
    private ConcurrentHashMap<Integer, ConnectionHandler<T>> clientHashMap; //map for the connection handler for each connected client
    private ConcurrentHashMap<String, ConcurrentHashMap<Integer, ConnectionHandler<T>>> channelsMap;   //map for the clients of each channel

    public ConnectionsImpl(ConcurrentHashMap<Integer, ConnectionHandler<T>> clients, ConcurrentHashMap<String, ConcurrentHashMap<Integer, ConnectionHandler<T>>> channelsMap) {
        clientHashMap = clients;
        this.channelsMap = channelsMap;
    }

    @Override
    public boolean send(int connectionId, T msg) {  //send a message to a single client
        ConnectionHandler<T> handler = clientHashMap.get(connectionId);
        if (handler == null) {
            System.out.println("Error: Tried to send a message to a non existent user");
            return false;
        }
        handler.send(msg);
        return true;
    }

    @Override
    public void send(String channel, T msg) {   //send a message to all clients of a certain channel
//        ReadOrWriteHandlerList<T> channelList=channelsMap.get(channel);
//        for(int i=0;i<channelList.size();++i){
//            channelList.sendAtIndexi(i,msg);
//        }
        for (int i : BookClubInfo.getInstance().getTopics().get(channel)) {
            send(i, msg);
        }
    }

    @Override
    public void disconnect(int connectionId) {
        Cheatgleton.getInstance().disconnect(connectionId);
//        Collection<ConcurrentHashMap<Integer, ConnectionHandler<T>>> t = channelsMap.values();
//        for (ConcurrentHashMap<Integer, ConnectionHandler<T>> tReadOrWriteHandlerList : t) {
//            BookClubInfo.getInstance().getClientTopicId().get(BookClubInfo.getInstance().getUserNames().get(connectionId));
//        }
    }
}
