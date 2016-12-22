package testgateway.xun.com.testgateway;

import java.io.Serializable;

/**
 * Created by xunwang on 16/12/19.
 */

public class BaseModel implements Serializable {

    private static final long serialVersionUID = 7980806036380987323L;
    private int errCode = -1;

    private String errMsg;

    public int getCode() {
        return errCode;
    }

    public void setCode(int errCode) {
        this.errCode = errCode;
    }

    public boolean isSuccess() {
        return (errCode == 0 || errCode == 10) ? true : false;
    }

    public String getErrorMsg() {
        return errMsg;
    }

    public void setErrorMsg(String errorMsg) {
        this.errMsg = errorMsg;
    }
}
