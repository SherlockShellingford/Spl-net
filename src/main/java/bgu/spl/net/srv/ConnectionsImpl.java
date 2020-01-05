package bgu.spl.net.srv;

import java.sql.Connection;
import java.util.Collection;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

public class ConnectionsImpl<T> implements Connections<T> {
    ConcurrentHashMap<Integer, NonBlockingConnectionHandler<T>>  clientHashMap;
    ConcurrentHashMap<String, ReadOrWriteHandlerList<T>> channelsMap;


    @Override
    public boolean send(int connectionId, T msg) {
        NonBlockingConnectionHandler<T> handler= clientHashMap.get(connectionId);
        if(handler==null) {
            System.out.println("Error: Tried to send a message to a non existent user");
            return false;
        }
        else{
            handler.send(msg);
            return true;
        }
    }

    @Override
    public void send(String channel, T msg) {
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
