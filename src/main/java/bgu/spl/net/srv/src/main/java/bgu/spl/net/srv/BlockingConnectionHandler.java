package bgu.spl.net.srv;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.impl.stomp.MessageEncoderDecoderStomp;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.IOException;
import java.net.Socket;

public class BlockingConnectionHandler<T> implements Runnable, ConnectionHandler<String> {

    private final StompMessagingProtocol protocol;
    private final MessageEncoderDecoderStomp encdec;
    private final Socket sock;
    //private BufferedInputStream in;
    //private BufferedOutputStream out;
    private volatile boolean connected = true;
    //private Connections<T> connections;
    //private int id;

    public BlockingConnectionHandler(Socket sock, MessageEncoderDecoderStomp reader, StompMessagingProtocol protocol) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = protocol;
    }

    public BlockingConnectionHandler(Socket sock, MessageEncoderDecoderStomp reader, StompMessagingProtocol protocol, Connections<T> connections, int id) {
        this.sock = sock;
        this.encdec = reader;
        this.protocol = protocol;
        //this.connections = connections;
        //this.id = id;
    }

    @Override
    public void run() {
        BufferedInputStream in;
        try (Socket sock = this.sock) { //just for automatic closing
            int read;

            in = new BufferedInputStream(sock.getInputStream());

            while (!protocol.shouldTerminate() && connected && (read = in.read()) >= 0) {
                String nextMessage = encdec.decodeNextByte((byte) read);
                if (nextMessage != null) {
                    protocol.process(nextMessage);
                }
            }
        } catch (IOException ex) {
            ex.printStackTrace();
        }

    }

    @Override
    public void close() throws IOException {
        connected = false;
        sock.close();
    }

    @Override
    public void send(String msg) {
        BufferedOutputStream out;
        try {
            out = new BufferedOutputStream(sock.getOutputStream());
            if (msg != null) {
                out.write(encdec.encode(msg));
                out.flush();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
}
