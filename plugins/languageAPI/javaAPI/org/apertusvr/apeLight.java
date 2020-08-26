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

import androidx.annotation.Size;

public final class apeLight extends apeEntity {

    public enum LightType {
        SPOT,
        DIRECTIONAL,
        POINT,
        INVALID
    }

    public class LightSpotRange {
        public LightSpotRange() {
            innerAngle = new apeDegree();
            outerAngle = new apeDegree();
            falloff = 0.0f;
        }

        public LightSpotRange(apeDegree innerAngle, apeDegree outerAngle, float falloff) {
            this.innerAngle = innerAngle;
            this.outerAngle = outerAngle;
            this.falloff = falloff;
        }

        public LightSpotRange(float innerAngle, float outerAngle, float falloff) {
            this.innerAngle = new apeDegree(innerAngle);
            this.outerAngle = new apeDegree(outerAngle);
            this.falloff = falloff;
        }

        public LightSpotRange(@Size(3) float[] spotRangeAsArray) {
            this.innerAngle = new apeDegree(spotRangeAsArray[0]);
            this.outerAngle = new apeDegree(spotRangeAsArray[1]);
            this.falloff = spotRangeAsArray[2];
        }

        public apeDegree innerAngle;
        public apeDegree outerAngle;
        public float falloff;
    }

    public class LightAttenuation {
        public LightAttenuation() {
            range = 0f;
            constant = 0f;
            linear = 0f;
            quadratic = 0f;
        }

        public LightAttenuation(float range, float constant, float linear, float quadratic) {
            this.range = range;
            this.constant = constant;
            this.linear = linear;
            this.quadratic = quadratic;
        }

        public LightAttenuation(@Size(4) float[] attenuationAsArray) {
            this.range = attenuationAsArray[0];
            this.constant = attenuationAsArray[1];
            this.linear = attenuationAsArray[2];
            this.quadratic = attenuationAsArray[3];
        }

        public float range;
        public float constant;
        public float linear;
        public float quadratic;
    }

    public apeLight(String name) {
        super(name, Type.LIGHT);
    }

    public LightType getLightType() {
        int typeAsInt = ApertusJNI.getLightType(mName);
        return LightType.values()[typeAsInt];
    }

    public apeColor getDiffuseColor() {
        return new apeColor(ApertusJNI.getLightDiffuseColor(mName));
    }

    public apeColor getSpecularColor() {
        return new apeColor(ApertusJNI.getLightSpecularColor(mName));
    }

    public LightSpotRange getLightSpotRange() {
        return new LightSpotRange(ApertusJNI.getLightSpotRange(mName));
    }

    public LightAttenuation getLightAttenuation() {
        return new LightAttenuation(ApertusJNI.getLightAttenuation(mName));
    }

    public apeVector3 getLightDirection() {
        return  new apeVector3(ApertusJNI.getLightDirection(mName));
    }

    public void setLightType(LightType lightType) {
        ApertusJNI.setLightType(mName,lightType.ordinal());
    }

    public void setDiffuseColor(apeColor color) {
        ApertusJNI.setLightDiffuseColor(mName,color.r,color.g,color.b,color.a);
    }

    public void setSpecularColor(apeColor color) {
        ApertusJNI.setLightSpecularColor(mName,color.r,color.g,color.b,color.a);
    }

    public void setLightSpotRange(LightSpotRange spotRange) {
        ApertusJNI.setLightSpotRange(mName,spotRange.innerAngle.degree,spotRange.outerAngle.degree,spotRange.falloff);
    }

    public void setLightAttenuation(LightAttenuation lightAttenuation) {
        ApertusJNI.setLightAttenuation(mName,lightAttenuation.range,lightAttenuation.constant,lightAttenuation.linear, lightAttenuation.quadratic);
    }

    public void setLightDirection(apeVector3 direction) {
        ApertusJNI.setLightDirection(mName, direction.x, direction.y, direction.z);
    }

    public void setParentNode(apeNode parent) {
        ApertusJNI.setLightParentNode(mName, parent.getName());
    }

    public apeNode getParentNode() {
        return new apeNode(ApertusJNI.getLightParentNode(mName));
    }
}
