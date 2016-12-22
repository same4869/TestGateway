package testgateway.xun.com.testgateway;

/**
 * Created by xunwang on 16/12/19.
 */

public class JSONFormatExcetion extends Exception{
    /**
     *
     */
    private static final long serialVersionUID = 6411745024910244643L;

    public JSONFormatExcetion(){
        super();
    }

    public JSONFormatExcetion(String message){
        super(message);
    }
}
