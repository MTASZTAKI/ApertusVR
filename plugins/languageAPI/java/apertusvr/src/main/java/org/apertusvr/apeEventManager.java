/*MIT License

Copyright (c) 2018 MTA SZTAKI

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.*/

package org.apertusvr;

import java.util.LinkedList;
import java.util.Objects;
import java.util.TreeMap;

public final class apeEventManager {

    static {
        mEventMap = new TreeMap<apeEvent.Group, LinkedList<apeEventCallback>>();
    }

    private apeEventManager() {
    }

    public static void connectEvent(apeEvent.Group group, apeEventCallback callback) {
        if(mEventMap.get(group) == null) {
            mEventMap.put(group, new LinkedList<apeEventCallback>());
        }

        Objects.requireNonNull(mEventMap.get(group)).add(callback);
    }

    public static void disconnectEvent(apeEvent.Group group, apeEventCallback callback) {
        if(mEventMap.get(group) != null) {
            Objects.requireNonNull(mEventMap.get(group)).remove(callback);
        }
    }


    public static void fireEvent(int typeAsInt, int groupAsInt, String subjectName) {
        apeEvent.Type type = apeEvent.Type.values()[typeAsInt];
        apeEvent.Group group = apeEvent.Group.values()[groupAsInt];

        apeEvent event = new apeEvent(subjectName,type,group);

        if(mEventMap.get(group) != null) {
            for (apeEventCallback eventCallback : Objects.requireNonNull(mEventMap.get(group))) {
                eventCallback.onEvent(event);
            }
        }
    }

    private static TreeMap<apeEvent.Group, LinkedList<apeEventCallback>> mEventMap;
}
