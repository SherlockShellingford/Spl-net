package bgu.spl.net.srv;

import java.sql.Connection;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

public class ConnectionsImpl<T> implements Connections<T> {
    private ConcurrentHashMap<Integer, NonBlockingConnectionHandler<T>>  clientHashMap; //map for the connection handler for each connected client
    private ConcurrentHashMap<String, ReadOrWriteHandlerList<T>> channelsMap;   //map for the clients of each channel


    @Override
    public boolean send(int connectionId, T msg) {  //send a message to a single client
        NonBlockingConnectionHandler<T> handler= clientHashMap.get(connectionId);
        if(handler==null) {
            System.out.println("Error: Tried to send a message to a non existent user");
            return false;
        }
        handler.send(msg);
        return true;
    }

    @Override
    public void send(String channel, T msg) {   //send a message to all clients of a certain channel
        ReadOrWriteHandlerList<T> channelList=channelsMap.get(channel);
        for(int i=0;i<channelList.size();++i){
            channelList.sendAtIndexi(i,msg);
        }
    }

    @Override
    public void disconnect(int connectionId) {
        Collection<ReadOrWriteHandlerList<T>> t=channelsMap.values();
        for (ReadOrWriteHandlerList<T> tReadOrWriteHandlerList : t) {
            tReadOrWriteHandlerList.disconnectIDi(connectionId);
        }
    }
}
