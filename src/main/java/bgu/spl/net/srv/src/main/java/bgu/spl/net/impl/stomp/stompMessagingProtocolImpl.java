package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.*;

import java.net.NoRouteToHostException;
import java.net.ResponseCache;
import java.net.http.HttpResponse;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

public class stompMessagingProtocolImpl implements StompMessagingProtocol {
    private int connectionId;
    private BookClubInfo bookClubInfo;
    private boolean terminate;
    private AtomicInteger messageId;
    private Connections connections;
    private ConcurrentHashMap<Integer, ConnectionHandler<String>> clientHashMap;
    private ConcurrentHashMap<String, ReadOrWriteHandlerList<String>> channelsMap;

    public stompMessagingProtocolImpl(ConcurrentHashMap<Integer, ConnectionHandler<String>> clientHashMap, ConcurrentHashMap<String, ReadOrWriteHandlerList<String>> channelsMap){
        this.clientHashMap = clientHashMap;
        this.channelsMap = channelsMap;
    }

    @Override
    public void start(int connectionId, Connections<String> connections) {
        this.connectionId = connectionId;
        this.bookClubInfo = BookClubInfo.getInstance();
        this.connections = connections;
        terminate = false;
        messageId = new AtomicInteger(0);
    }

    @Override
    public void process(String message) {
        checkType(message);
    }

    private void checkType(String message) {
        String[] s = message.split("\n");
        switch (s[0]) { //todo: check for illegal input
            case "CONNECT":
                connectMsg(s);
                break;
            case "SUBSCRIBE":
                subscribeMsg(s);
                break;
            case "UNSUBSCRIBE":
                unsubscribeMsg(s);
                break;
            case "SEND":
                String dest = s[1].substring(s[1].indexOf(":") + 1);
                int sub = 0;
                for (int i : bookClubInfo.getClientTopicId().get(bookClubInfo.getUserNames().get(connectionId)).keySet()){
                    if (bookClubInfo.getClientTopicId().get(bookClubInfo.getUserNames().get(connectionId)).get(i).equals(dest)){
                        sub = i;
                    }
                }
                sendMsg(s, messageId.incrementAndGet(), dest, sub);
                break;
            case "DISCONNECT":
                disconnectMsg(s);
                break;
            default:
                System.out.println("no such command");
                break;
        }
    }

    private void unsubscribeMsg(String[] s) {
        String id = s[1].substring(s[1].indexOf(":") + 1);
        String receipt = s[2].substring(s[2].indexOf(":") + 1);
        String topic = bookClubInfo.getClientTopicId().get(bookClubInfo.getUserNames().get(connectionId)).get(Integer.parseInt(id));
        bookClubInfo.getClientTopicId().get(bookClubInfo.getUserNames().get(connectionId)).remove(Integer.parseInt(id));
        bookClubInfo.getTopics().get(topic).remove(connectionId);
        Cheatgleton.getInstance().unsubscribe(topic, connectionId);
        String response = "RECEIPT\nid:" + id + "\nreceipt-id:" + receipt + "\n\n\u0000";
        connections.send(connectionId, response);
    }

    private void disconnectMsg(String[] s) {
        String receipt = s[1].substring(s[1].indexOf(":") + 1);
        for (int i : bookClubInfo.getClientTopicId().get(bookClubInfo.getUserNames().get(connectionId)).keySet()){
            bookClubInfo.getTopics().get(bookClubInfo.getClientTopicId().get(bookClubInfo.getUserNames().get(connectionId)).get(i)).remove(connectionId);
            bookClubInfo.getClientTopicId().get(bookClubInfo.getUserNames().get(connectionId)).remove(i);
        }
        connections.disconnect(connectionId);
        String response = "RECEIPT\nreceipt-id:" + receipt + "\n\n\u0000";
        Cheatgleton.getInstance().disconnect(connectionId);
        bookClubInfo.getConnectedUsers().put(bookClubInfo.getUserNames().get(connectionId), new AtomicBoolean(false));
        connections.send(connectionId, response);
        terminate = true;
    }

    private void sendMsg(String[] s, int mId, String dest, int sub) {
        String body = s[3];
        String messageId = String.format("%05d", mId);
        String response = "MESSAGE\nsubscription:" + sub + "\nMessage-id:" + messageId + "\ndestination:" + dest + "\n\n" + body;
        connections.send(dest, response);
    }

    private void subscribeMsg(String[] s) {
        String topic = s[1].substring(s[1].indexOf(":") + 1);
        String id = s[2].substring(s[2].indexOf(":") + 1);
        String receipt = s[3].substring(s[3].indexOf(":") + 1);
        if(!bookClubInfo.getTopics().containsKey(topic)){
            bookClubInfo.getTopics().put(topic, new ConcurrentLinkedQueue<>());
        }
        bookClubInfo.getTopics().get(topic).add(Integer.parseInt(id));
        bookClubInfo.getClientTopicId().get(bookClubInfo.getUserNames().get(connectionId)).putIfAbsent(Integer.parseInt(id), topic);
        String response = "RECEIPT\nreceipt-id:" + receipt + "\n\n\u0000";
        Cheatgleton.getInstance().subscribeClient(connectionId, Integer.parseInt(id), topic);
        connections.send(connectionId, response);
    }

    private void connectMsg(String[] s) {
        String version;
        String user;
        String passcode;
        String receipt;
        version = s[1].substring(s[1].indexOf(":") + 1);
        user = s[3].substring(s[3].indexOf(":") + 1);
        passcode = s[4].substring(s[4].indexOf(":") + 1);
        if (!bookClubInfo.getUserNames().containsValue(user)){
            bookClubInfo.getUserNames().put(connectionId, user);
            bookClubInfo.getConnectedUsers().put(user, new AtomicBoolean(true));
            String response = "CONNECTED\nversion:" + version + "\n\n\u0000";
            connections.send(connectionId, response);
        }
        if (bookClubInfo.getConnectedUsers().get(user).get()){
            String response = "ERROR\nmessage: user already connected\n\nthe user " + user + " is already connected to the server\n\u0000";
            connections.send(connectionId, response);
        }
        if (!bookClubInfo.getPasscodes().get(user).equals(passcode)){
            String response = "ERROR\nmessage: wrong passcode\n\nthe user " + user + "the user and passcode combination does not exist\n\u0000";
            connections.send(connectionId, response);
        }
        bookClubInfo.getConnectedUsers().put(user, new AtomicBoolean(true));
        String response = "CONNECTED\nversion:" + version + "\n\n\u0000";
        connections.send(connectionId, response);
    }

    @Override
    public boolean shouldTerminate() {
        return terminate;
    }
}
