package org.apertusvr;

public class apeSceneNetwork {

    public enum ParticipantType {
        HOST,
        GUEST,
        NONE,
        INVALID
    }

    private apeSceneNetwork() {}

    public static ParticipantType getParticipantType() {
        return ParticipantType.values()[ApertusJNI.getSceneNetworkParticipantType()];
    }

    public static boolean isRoomRunning(String roomName) {
        return ApertusJNI.isSceneNetworkRoomRunning(roomName);
    }

    public static void connectToRoom(String roomName) {
        ApertusJNI.connectSceneNetworkToRoom(roomName);
    }

    public static String getCurrentRoomName() {
        return ApertusJNI.getSceneNetworkCurrentRoomName();
    }

}
