package bgu.spl.net.srv;

import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicBoolean;

public class BookClubInfo {
    private ConcurrentHashMap<Integer, String> userNames;   //unique id, user name
    private ConcurrentHashMap<String, String> passcodes;    //user name, passcode
    private ConcurrentHashMap<String, AtomicBoolean> connectedUsers;    //username, is connected
    private ConcurrentHashMap<String, ConcurrentLinkedQueue<Integer>> topics;    //topic name, unique id
    private ConcurrentHashMap<String, ConcurrentHashMap<Integer, String>> clientTopicId; //user name, ( non unique id,topic name)
    static private BookClubInfo instance;


    private static class holder{
        private static BookClubInfo instance = new BookClubInfo();
    }

    private BookClubInfo(){
        userNames = new  ConcurrentHashMap<>();
        passcodes = new ConcurrentHashMap<>();
        connectedUsers = new ConcurrentHashMap<>();
        topics = new ConcurrentHashMap<>();
        clientTopicId = new ConcurrentHashMap<>();
    }

    /**
     * Retrieves the single instance of this class.
     */
    public static BookClubInfo getInstance() {
        return holder.instance;
    }

    public ConcurrentHashMap<Integer, String> getUserNames() {
        return userNames;
    }

    public ConcurrentHashMap<String, String> getPasscodes() {
        return passcodes;
    }

    public ConcurrentHashMap<String, AtomicBoolean> getConnectedUsers() {
        return connectedUsers;
    }

    public ConcurrentHashMap<String, ConcurrentHashMap<Integer, String>> getClientTopicId() {
        return clientTopicId;
    }

    public ConcurrentHashMap<String, ConcurrentLinkedQueue<Integer>> getTopics() {
        return topics;
    }
}
