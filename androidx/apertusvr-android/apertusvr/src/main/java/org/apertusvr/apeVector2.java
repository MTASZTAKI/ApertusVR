package org.apertusvr;

import androidx.annotation.Size;

public final class apeVector2 {

    public apeVector2() {
        x = y = 0;
    }

    public apeVector2(float x, float y) {
        this.x = x;
        this.y = y;
    }

    public apeVector2(@Size(2) float[] arr) {
        x = arr[0];
        y = arr[1];
    }

    public apeVector2(final apeVector2 v) {
        this.x = v.x;
        this.y = v.y;
    }

    public float dotProduct(apeVector2 v2) {
        return x * v2.x + y * v2.y;
    }

    public float length() {
        return (float) Math.sqrt(x * x + y * y);
    }

    public float squaredLength() {
        return x * x + y * y;
    }

    public float distance(apeVector2 v2) {
        float dx = x - v2.x;
        float dy = y - v2.y;
        return (float) Math.sqrt(dx * dx + dy * dy);
    }

    public float getX() {
        return x;
    }

    public float getY() {
        return y;
    }

    public float x, y;
}
