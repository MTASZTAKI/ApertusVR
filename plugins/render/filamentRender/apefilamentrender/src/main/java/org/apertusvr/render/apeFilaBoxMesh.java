package org.apertusvr.render;
import org.apertusvr.apeVector2;
import org.apertusvr.apeVector3;

final class apeFilaBoxMesh extends apeFilaMesh {

    protected apeVector3 dimensions;

    apeFilaBoxMesh() {
        super();
        dimensions = new apeVector3(1f,1f,1f);
    }

    apeFilaBoxMesh(apeVector3 dimensions) {
        super();
        this.dimensions = dimensions;
    }
}
