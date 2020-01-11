package bgu.spl.net.srv;

import java.util.LinkedList;
import java.util.List;
import java.util.concurrent.locks.ReadWriteLock;
import java.util.concurrent.locks.ReentrantReadWriteLock;

public class ReadOrWriteHandlerList<T> {
    List<Pair<Integer,NonBlockingConnectionHandler<T>>> lst;
    ReadWriteLock readWriteLock = new ReentrantReadWriteLock();

    public ReadOrWriteHandlerList(){
        lst=new LinkedList<>();
    }

    public void sendAtIndexi(int i,T msg){
        readWriteLock.readLock().lock();
        lst.get(i).getSecond().send(msg);
        readWriteLock.readLock().unlock();;

    }
    public void disconnectIDi(int i){
        readWriteLock.writeLock().lock();
        for(int m=0;m<lst.size();++m){
            if(lst.get(m).getFirst()==i){
                lst.remove(m);
                readWriteLock.writeLock().unlock();
                return;
            }
        }
        readWriteLock.writeLock().unlock();
    }
    public int size(){
        return lst.size();
    }
}
