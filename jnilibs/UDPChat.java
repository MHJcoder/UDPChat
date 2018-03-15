// package cn.edu.scnu.libhttpserver.httpserver;

/**
 * Created by mhj on 17-11-23.
 */

public class UDPChat {
    static{
        System.loadLibrary("ANUDPChat");
    }
    public static native void joinInGroup(String host,int port,sockInfo info);
    public static native int sendmessage(int fd,int port ,String message);
    public static native String recvmessage(int fd);
    public static native void quitGroup(int sfd,int rfd,String host);
}
