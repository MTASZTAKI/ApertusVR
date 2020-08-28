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

public final class apeFileMaterial extends apeMaterial {
    public apeFileMaterial(String name) {
        super(name, Type.MATERIAL_FILE);
    }

    public void setFileName (String fileName) {
        ApertusJNI.setFileMaterialFileName(mName, fileName);
    }

    public String getFileName () {
        return ApertusJNI.getFileMaterialfFileName(mName);
    }

    public void setAsSkyBox() {
        ApertusJNI.setFileMaterialAsSkyBox(mName);
    }

    public void setTexture(apeTexture texture) {
        ApertusJNI.setFileMaterialTexture(mName, texture.getName());
    }

    public apeTexture getTexture() {
        String textureName = ApertusJNI.getFileMaterialTexture(mName);
        apeEntity.Type textureType = apeEntity.Type.values()[ApertusJNI.getEntityType(textureName)];

        return new apeTexture(textureName, textureType);
    }

    public void setOwner(String ownerID) {
        ApertusJNI.setFileMaterialOwner(mName, ownerID);
    }

    public String getOwner() {
        return ApertusJNI.getFileMaterialOwner(mName);
    }
}
