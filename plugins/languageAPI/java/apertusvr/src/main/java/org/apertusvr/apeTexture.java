package org.apertusvr;

public class apeTexture extends apeEntity {

    public enum PixelFormat {
        PF_NONE,
        R8G8B8,
        R8G8B8A8,
        A8R8G8B8,
        PF_INVALID
    }

    public enum Usage {
        U_NONE,
        RENDERTARGET,
        DYNAMIC_WRITE_ONLY,
        U_INVALID
    }

    public enum AddressingMode {
        AM_NONE,
        WRAP,
        MIRROR,
        CLAMP,
        BORDER,
        AM_INVALID
    }

    public enum Filtering {
        F_NONE,
        POINT,
        LINEAR,
        ANISOTROPIC,
        F_INVALID
    }

    protected apeTexture(String name, Type type) {
        super(name, type);
    }
}
