package testgateway.xun.com.testgateway;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import org.json.JSONObject;

public class RequestMainActivity extends AppCompatActivity implements View.OnClickListener {
    private Button startLogin, sendMsg, startLoginS;
    private MediaControlManager mediaControlManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_request_main);

        mediaControlManager = new MediaControlManager(new MediaControlManager
                .MediaControlNotifyListener() {


            @Override
            public void onDisconnect(BaseModel result) {

            }

            @Override
            public void onStartReconnect() {

            }

            @Override
            public void onReconnectSuccess() {

            }

            @Override
            public void OnRecvMsg(JSONObject result) {
                Log.d("kkkkkkkk", "OnRecvMsg --> " + result.toString());
            }
        });

        startLogin = (Button) findViewById(R.id.start_login);
        startLogin.setOnClickListener(this);

        sendMsg = (Button) findViewById(R.id.send_msg);
        sendMsg.setOnClickListener(this);

        startLoginS = (Button) findViewById(R.id.start_login_s);
        startLoginS.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.start_login:
                mediaControlManager.login("ax.wpss.cn", 1090, "11000000001", "e10adc3949ba59abbe56e057f20f883e", "1.0" +
                        ".1", 21, new MediaControlManager.MediaRespCallback<LoginRespModel>(LoginRespModel.class) {

                    @Override
                    public void respCallBack(LoginRespModel var1) {
                        Log.d("kkkkkkkk", "var1.getErrorMsg() --> " + var1.getErrorMsg() + " var1.getUserId() --> " +
                                var1.getUserId() + " var1.isSuccess() --> " + var1.isSuccess() + " var1.getCode() -->" +
                                " " + var1.getCode());
                    }
                });
                break;
            case R.id.send_msg:
                mediaControlManager.sendMsgToUser("h哈哈哈发送到发送到!!", 1419, 20, 21, new MediaControlManager
                        .MediaRespCallback<BaseModel>(BaseModel.class) {


                    @Override
                    public void respCallBack(BaseModel var1) {
                        Log.d("kkkkkkkk", "sendMessage 回调");
                    }
                });
                break;
            case R.id.start_login_s:
                mediaControlManager.login("ax.wpss.cn", 1090, "10000000002", "e10adc3949ba59abbe56e057f20f883e", "1.0" +
                        ".1", 20, new MediaControlManager.MediaRespCallback<LoginRespModel>(LoginRespModel.class) {

                    @Override
                    public void respCallBack(LoginRespModel var1) {
                        Log.d("kkkkkkkk", "var1.getErrorMsg() --> " + var1.getErrorMsg() + " var1.getUserId() --> " +
                                var1.getUserId() + " var1.isSuccess() --> " + var1.isSuccess() + " var1.getCode() -->" +
                                " " + var1.getCode());
                    }
                });
                break;
            default:
                break;
        }

    }
}
