package org.apertusvr;

public final class apeSceneManager {

    private apeSceneManager() {}


//    public static Map<String,apeNode> getNodes() {
//    }

    public static apeNode getNode(String name) {
        if(ApertusJNI.isNodeValid(name))
            return new apeNode(name);
        else
            return null;
    }


    public static apeEntity getEntity(String name) {
        if(ApertusJNI.isEntityValid(name))
        {
            apeEntity.Type entityType = apeEntity.Type.values()[ApertusJNI.getEntityType(name)];
            return new apeEntity(name,entityType);
        }

        return null;
    }

}
