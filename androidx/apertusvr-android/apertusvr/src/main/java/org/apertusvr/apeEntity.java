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

import org.apertusvr.ApertusJNI;

import java.util.Objects;

public class apeEntity {
    public enum Type {
        LIGHT,
        CAMERA,
        GEOMETRY_FILE,
        GEOMETRY_INDEXEDFACESET,
        GEOMETRY_INDEXEDLINESET,
        GEOMETRY_TEXT,
        GEOMETRY_BOX,
        GEOMETRY_PLANE,
        GEOMETRY_TUBE,
        GEOMETRY_CYLINDER,
        GEOMETRY_SPHERE,
        GEOMETRY_TORUS,
        GEOMETRY_CONE,
        GEOMETRY_RAY,
        GEOMETRY_CLONE,
        MATERIAL_MANUAL,
        MATERIAL_FILE,
        TEXTURE_MANUAL,
        TEXTURE_FILE,
        TEXTURE_UNIT,
        BROWSER,
        WATER,
        SKY,
        POINT_CLOUD,
        INVALID,
        RIGIDBODY
    }

    protected apeEntity(String name, Type type) {
        mName = name;
        mType = type;
    }

    protected apeEntity(long ptr, Type type) {
        mName = ApertusJNI.getNameFromPtr(ptr);
        mType = type;
    }

    public String getName() {
        return mName;
    }

    public Type getType() {
        return mType;
    }

    public static Type getEntityType(String entityName) {
        return Type.values()[ApertusJNI.getEntityType(entityName)];
    }

    public boolean isValid() {
        return ApertusJNI.isEntityValidWithType(mName,mType.ordinal());
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        apeEntity otherEntity = (apeEntity) obj;
        return mName.equals(otherEntity.mName);
    }

    @Override
    public int hashCode() {
        return Objects.hash(mName, mType);
    }

    protected String mName;

    protected Type mType;
}
