package org.apertusvr;

import androidx.annotation.NonNull;
import androidx.annotation.Size;

public final class apeEuler {

    public apeEuler(){
        this.m_yaw = 0.0f;
        this.m_pitch = 0.0f;
        this.m_roll = 0.0f;
        this.m_changed = true;
    }

    public apeEuler(float y, float p, float r){
        this.m_yaw = y;
        this.m_pitch = p;
        this.m_roll = r;
        this.m_changed = true;
    }

    public apeEuler(final apeQuaternion q){
        this.m_yaw = (float)Math.atan2(2.0 * (q.y * q.z + q.w * q.x),q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
        this.m_pitch = (float)Math.asin(-2.0*(q.x*q.z-q.w*q.y));
        this.m_roll = (float)Math.atan2(2.0 * (q.x * q.y + q.w * q.z), q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z);
    }

    float getYaw(){return m_yaw;}
    float getPitch(){return m_pitch;}
    float getRoll(){return m_roll;}

    public apeEuler setYaw(float y){
        m_yaw = y;
        m_changed = true;
        return this;
    }

    public apeEuler setPitch(float p){
        m_pitch = p;
        m_changed = true;
        return this;
    }

    public apeEuler setRoll(float r){
        m_roll = r;
        m_changed = true;
        return this;
    }
    
    public apeQuaternion toQuaternion(){
        if(m_changed){
            double c1 = Math.cos(m_yaw / 2.0f);
            double s1 = Math.sin(m_yaw / 2.0f);
            double c2 = Math.cos(m_pitch / 2.0f);
            double s2 = Math.sin(m_pitch / 2.0f);
            double c3 = Math.cos(m_roll / 2.0f);
            double s3 = Math.sin(m_roll / 2.0f);
            double c1c2 = c1 * c2;
            double s1s2 = s1 * s2;
            double w = c1c2 * c3 - s1s2 * s3;
            double x = c1c2 * s3 + s1s2 * c3;
            double y = s1 * c2 * c3 + c1 * s2 * s3;
            double z = c1 * s2 * c3 - s1 * c2 * s3;
            m_cachedQuaternion = new apeQuaternion((float) w, (float) y, (float) z, (float) x);
            m_changed = false;
        }
        return m_cachedQuaternion;
    }

    @NonNull
    @Override
    public String toString(){return "y: "+(m_yaw*180/Math.PI)+
            ",p: "+(m_pitch*180/Math.PI)+"r: "+(m_roll*180/Math.PI) ;
    }

    public String toJsonString(){
        return "{ \"y\": " + (m_yaw*180/Math.PI)+", \"p\": " + (m_pitch*180/Math.PI)+
        ", \"r\": " + (m_roll*180/Math.PI)+" }";
    }

    public @Size(3) float[] toArray() {
        return new float[] {(float) (m_yaw*180/Math.PI),
                (float) (m_pitch*180/Math.PI),
                (float) (m_roll*180/Math.PI)};
    }

    private float m_yaw;
    private float m_pitch;
    private float m_roll;
    public apeQuaternion m_cachedQuaternion;
    private boolean m_changed;
}
