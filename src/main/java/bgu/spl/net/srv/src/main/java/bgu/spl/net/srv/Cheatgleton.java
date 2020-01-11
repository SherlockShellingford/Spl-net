package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;

public class Cheatgleton {
    private ConcurrentHashMap<String, ConcurrentHashMap<Integer, Pair<Integer, ConnectionHandler>>> channelsMap;
    static private Cheatgleton instance;
    private ConcurrentHashMap<Integer, ConnectionHandler> clientHashMap;

    private static class holder{
        private static Cheatgleton instance = new Cheatgleton();
    }

    private Cheatgleton(){
        channelsMap = new  ConcurrentHashMap<>();
        clientHashMap = new ConcurrentHashMap<>();
    }

    /**
     * Retrieves the single instance of this class.
     */
    public static Cheatgleton getInstance() {
        return holder.instance;
    }

    public void subscribeClient(int uniqueId, int topicId, String topic){
        channelsMap.putIfAbsent(topic,  new ConcurrentHashMap<>());
        channelsMap.get(topic).putIfAbsent(uniqueId, new Pair<>(topicId, clientHashMap.get(uniqueId)));
    }

    public ConcurrentHashMap<String, ConcurrentHashMap<Integer, Pair<Integer, ConnectionHandler>>> getChannelsMap() {
        return channelsMap;
    }

    public ConcurrentHashMap<Integer, ConnectionHandler> getClientHashMap() {
        return clientHashMap;
    }

    public void unsubscribe(String topic, int connectionId){
        channelsMap.get(topic).remove(connectionId);
    }

    public void disconnect(int connectionId){
        for (String topic : channelsMap.keySet()){
            unsubscribe(topic, connectionId);
        }
    }

    public void connect(){

    }
}
