package org.apertusvr.render;

import com.google.android.filament.Entity;
import com.google.android.filament.TransformManager;

import org.apertusvr.ApertusJNI;

public class apeFilaMeshClone {

    public apeFilaMeshClone() {
        renderable = Entity.NULL;
        sourceMeshName = ApertusJNI.NA_STR;
        parentTransform = null;
    }

    public void setParentTransform(apeFilaTransform transform, TransformManager tcm) {
        tcm.setParent(tcm.getInstance(renderable), transform.transform);
    }

    @Entity int renderable;
    String sourceMeshName;
    apeFilaTransform parentTransform;
}
