package testgateway.xun.com.testgateway;

/**
 * Created by xunwang on 16/12/19.
 */

public class LoginRespModel extends BaseModel {
    private static final long serialVersionUID = -358226570683159418L;
    private int userId;

    public LoginRespModel() {
    }

    public int getUserId() {
        return this.userId;
    }

    public void setUserId(int var1) {
        this.userId = var1;
    }
}
