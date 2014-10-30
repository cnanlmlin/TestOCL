package org.code.testopencl;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

//import android.os.Bundle;

public class MainActivity extends Activity{
	static{
		System.loadLibrary("helloOCL");
	}
	public native String testOCL();
	
	public void setTextContent(String s){
		TextView textv = (TextView)findViewById(R.id.resultView);
		textv.setText(s);
	}
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    	Thread t = new Thread(new MyRunner(this));
    	t.start();
      
    }
}
