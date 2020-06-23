package org.apertusvr;

import java.util.LinkedList;
import java.util.concurrent.Semaphore;

public class apeDoubleQueue<T> {
    private LinkedList<T> mPush;
    private LinkedList<T> mPop;
    private boolean unique;

    public apeDoubleQueue(boolean unique) {
        mPush = new LinkedList<T>();
        mPop = new LinkedList<T>();
        this.unique = unique;
    }

    public void swap() {
        // mutex ?
        LinkedList<T> tmp = new LinkedList<T>(mPush);
        mPush.clear();
        mPush.addAll(mPop);
        mPop.clear();
        mPop.addAll(tmp);
    }

    public boolean empty() {
        return mPush.isEmpty() && mPop.isEmpty();
    }

    public boolean emptyPop() {
        return mPop.isEmpty();
    }

    public boolean emptyPush() {
        return mPush.isEmpty();
    }

    public T front() {
        return mPop.isEmpty() ? null : mPop.getFirst();
    }

    public void push(T elem) {
        // mutex ?

        if(!unique || !mPush.contains(elem)) {
            mPush.addLast(elem);
        }
    }

    public void pop() {
        if(!mPop.isEmpty()) {
            mPop.removeFirst();
        }
    }
}
