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

import org.jetbrains.annotations.NotNull;

public final class apeFileTexture extends apeTexture {

    public enum MapType {
        PF_NONE,
        DIFFUSE,
        SPECULAR,
        NORMAL,
        BUMP,
        PF_INVALID
    }

    public apeFileTexture(String name) {
        super(name, Type.TEXTURE_FILE);
    }

    public void setFileName (String fileName) {
        ApertusJNI.setFileTextureFileName(mName, fileName);
    }

    public String getFileName() {
        return ApertusJNI.getFileTextureFileName(mName);
    }

    public void setMapType (@NotNull MapType mapType) {
        ApertusJNI.setFileTextureMapType(mName, mapType.ordinal());
    }

    public MapType getMapType() {
        return MapType.values()[ApertusJNI.getFileTextureMapType(mName)];
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setFileTextureOwner(mName, ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getFileTextureOwner(mName);
    }
}
