package org.apertusvr;

public final class apeSceneManager {

    private apeSceneManager() {}

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

    public static apeNode createNode(String name, boolean replicate, String ownerID) {
        if(ApertusJNI.createSceneManagerNode(name,replicate,ownerID)) {
            return new apeNode(name);
        }

        return null;
    }

    public static apeEntity createEntity(String name, apeEntity.Type type, boolean replicate, String ownerID) {
        if (ApertusJNI.createSceneManagerEntity(name,type.ordinal(),replicate,ownerID)) {
            return new apeEntity(name,type);
        }

        return null;
    }

    public static void deleteNode(String name) {
        ApertusJNI.deleteSceneManagerNode(name);
    }

    public static void deleteEntity(String name) {
        ApertusJNI.deleteSceneManagerEntity(name);
    }

}
