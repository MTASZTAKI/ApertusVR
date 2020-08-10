package org.apertusvr;

public interface apeBuilder<T> {
    T build(String name, apeEntity.Type type);
}
