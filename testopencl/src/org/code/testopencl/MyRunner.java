package org.code.testopencl;

public class MyRunner implements Runnable{
	private MainActivity ma;
	MyRunner(MainActivity ma){
		this.ma = ma;
	}
	
	@Override
	public void run() {
		ma.setTextContent(ma.testOCL());
	}
	

}
