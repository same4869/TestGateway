package testgateway.xun.com.testgateway;

import android.util.Log;

import org.json.JSONObject;

import static testgateway.xun.com.testgateway.MediaControlManager.TSConncetStatus.UNCONNECTED;
import static testgateway.xun.com.testgateway.MediaControlManager.TSConncetStatus.UNKOWN;

/**
 * Created by xunwang on 16/12/19.
 */

public class MediaControlManager {
    static {
        System.loadLibrary("prof");//livelog的so库，jni/logger
        System.loadLibrary("media_control");//中控服务器的so库，jni/media_control
    }

    private static TSConncetStatus curTSConncetStatus = UNKOWN;

    public enum TSConncetStatus {
        UNCONNECTED, CONNECTING, CONNECTED, UNKOWN
    }

    public TSConncetStatus getCurTSConncetStatus(){
        return curTSConncetStatus;
    }

    private static MediaControlManager instance;

    private MediaControlManager() {
    }

    public static MediaControlManager getInstance() {
        if (instance == null) {
            synchronized (MediaControlManager.class) {
                if (instance == null) {
                    instance = new MediaControlManager();
                }
            }
        }
        return instance;
    }

    /**
     * 此接口为java层回调接口，在其外部类中，会有以对应名字生成的方法提供给中控sdk的cpp层进行调用，交叉编译，详见jni/media_control/media_jni.cpp
     * 此接口会被LiveManager实现
     *
     * @author silvercc
     * @note 请勿主动调用该接口实现类方法！！！
     */
    public interface MediaControlNotifyListener {
        /**
         * 当网络环境极差，多次重连仍然失败的时候，会调用这个方法
         *
         * @param result {@link BaseModel}
         */
        public void onDisconnect(BaseModel result);

        /**
         * 开始重连回调，重连操作由中控自动判断网络状况进行重连
         */
        public void onStartReconnect();

        /**
         * 重连成功回调
         */
        public void onReconnectSuccess();

        public void OnRecvMsg(JSONObject result);

    }

    /**
     * 该回调接口的实现子类将会作为参数传递给cpp中的函数进行调用
     *
     * @author silvercc
     */
    public static interface ResponseCallback {
        public void onResponse(JSONObject resonse);
    }

    public abstract static class MediaRespCallback<T> implements MediaControlManager.ResponseCallback {
        private Class<T> cls;

        public MediaRespCallback(Class<T> var1) {
            this.cls = var1;
        }

        public final void onResponse(JSONObject var1) {
            T var2 = null;
            if (var1 != null) {
                try {
                    var2 = JSONToBeanHandler.fromJsonString(var1.toString(), this.cls);
                } catch (JSONFormatExcetion var4) {
                    var4.printStackTrace();
                }
            }

            this.respCallBack(var2);
        }

        public abstract void respCallBack(T var1);
    }

    private MediaControlNotifyListener mNotifyListener;

    public MediaControlManager(MediaControlNotifyListener notifyListener) {
        mNotifyListener = notifyListener;
        start();
    }

    public void init(MediaControlNotifyListener notifyListener){
        mNotifyListener = notifyListener;
        start();
    }

    public void setConncetStatus(TSConncetStatus status) {
        this.curTSConncetStatus = status;
    }

    /**
     * 启动中控SDK，使用SDK时首先要调用此函数
     */
    private void start() {
        fun_start();
    }

    /**
     * java调用
     * 登录中控服务器
     *
     * @param host
     * @param port
     * @param userName
     * @param password
     * @param clientVer
     * @param callback
     */
    public void login(final String host, final int port, final String userName, final String password, final String
            clientVer, final int operId, final ResponseCallback callback) {

        WenbaThreadPool.poolExecute(new Runnable() {

            @Override
            public void run() {
                fun_login(host, port, userName, password, clientVer, operId, callback);
            }
        });

    }

    public void sendMsgToUser(final String message, final int receiverId, final int receiverOperId, final
    int operId, final ResponseCallback callback) {

        WenbaThreadPool.poolExecute(new Runnable() {

            @Override
            public void run() {
                fun_sendMsgToUser(message, receiverId, receiverOperId, operId, callback);
            }
        });

    }

    public void logout(final int userId, final int operId, final ResponseCallback callback) {
        WenbaThreadPool.poolExecute(new Runnable() {

            @Override
            public void run() {
                fun_logout(userId, operId, callback);
            }
        });

    }

    /**
     * cpp层调用
     *
     * @param result
     */
    public void OnRecvMsg(JSONObject result) {
        Log.d("kkkkkkkk", "cpp -> java OnRecvMsg －－> result:" + result.toString());
    }

    public void OnStartReconnect() {
        curTSConncetStatus = TSConncetStatus.CONNECTING;
        Log.d("kkkkkkkk", "cpp -> java onStartReconnect" + " curTSConncetStatus --> " + curTSConncetStatus);
    }

    public void OnReconnectSuccess() {
        curTSConncetStatus = TSConncetStatus.CONNECTED;
        Log.d("kkkkkkkk", "cpp -> java OnReconnectSuccess + curTSConncetStatus --> " + curTSConncetStatus);
    }

    public void OnDisconnect(JSONObject result) {
        curTSConncetStatus = UNCONNECTED;
        Log.d("kkkkkkkk", "cpp -> java OnDisconnect result --》 " + result + " curTSConncetStatus --> " + curTSConncetStatus);
    }

    /**
     * start
     */
    native private boolean fun_start();

    native private void fun_login(final String host, final int port, final String userName, final String password,
                                  final String clientVer, final int operId, final ResponseCallback callback);

    native private void fun_stop();

    native private void fun_destory();

    native private void fun_logout(int userId, int operId, final ResponseCallback callback);

    native private void fun_sendMsgToUser(final String message, final int receiverId, final int receiverOperId, final
    int operId, final ResponseCallback callback);

    native private void fun_SendMsg(final String message, final int operId, final ResponseCallback callback);

}
