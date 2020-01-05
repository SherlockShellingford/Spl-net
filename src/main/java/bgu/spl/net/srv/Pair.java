package bgu.spl.net.srv;

/**
 * Pair class- a class that holds 2 objects of different types: K and T
 * @param <T>
 * @param <K>
 */
public class Pair<T,K> {
    private T first;
    private K second;

    public Pair(T first, K second){
        this.first=first;
        this.second=second;
    }
    //returns first object
    public T getFirst() {
        return first;
    }
    //sets first object
    public void setFirst(T first) {
        this.first = first;
    }
    //returns second object
    public K getSecond() {
        return second;
    }
    //sets second object
    public void setSecond(K second) {
        this.second = second;
    }
}
