package hnu.yk;

import java.util.List;


import com.mokoid.server.LedService;

import android.app.Activity;
import android.os.Bundle;
//import android.widget.TextView;

import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;

public class ControllerPanel extends Activity implements OnClickListener {
	
	private Button openButton = null;
	private Button volupButton = null;
	private Button[] numButton = null;
	private Button voldownButton = null;
	
	private Button chupButton = null;
	private Button chdownButton = null;
	private Button backButton = null;
	private int[] code = null;
	
	LedService ls = null;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		// Call an API on the library.
		//LedService ls = new LedService();
		//ls.setOff(2);   

		//TextView tv = new TextView(this);
		//tv.setText(" LED  is off.");
		//setContentView(tv);

		ls = new LedService();

		setContentView(R.layout.controllerpanel);

		code = new int[] {0x10, 0xf0, 0x72, 0x62, 0x30, 0x60, 0xa0, 0xf2, 0xb2, 0x32};
		
		numButton = new Button[10];

		openButton = (Button)findViewById(R.id.button_open);
		volupButton = (Button)findViewById(R.id.vol_up);
		voldownButton = (Button)findViewById(R.id.vol_down);
		chupButton = (Button)findViewById(R.id.ch_up);
		chdownButton = (Button)findViewById(R.id.ch_down);
		
		numButton[0] = (Button)findViewById(R.id.n_0);
		numButton[1] = (Button)findViewById(R.id.n_1);
		numButton[2] = (Button)findViewById(R.id.n_2);
		numButton[3] = (Button)findViewById(R.id.n_3);
		numButton[4] = (Button)findViewById(R.id.n_4);
		numButton[5] = (Button)findViewById(R.id.n_5);
		numButton[6] = (Button)findViewById(R.id.n_6);
		numButton[7] = (Button)findViewById(R.id.n_7);
		numButton[8] = (Button)findViewById(R.id.n_8);
		numButton[9] = (Button)findViewById(R.id.n_9);
		
		backButton = (Button)findViewById(R.id.back);
		
		openButton.setOnClickListener(this);
		backButton.setOnClickListener(this);
		volupButton.setOnClickListener(this);
		voldownButton.setOnClickListener(this);
		chupButton.setOnClickListener(this);
		chdownButton.setOnClickListener(this);
		for(int i = 0;i<10;++i) numButton[i].setOnClickListener(this);

	}
    
    public void onClick(View v){
    	if(v.equals(openButton)) {
    		//Log.e("HELLO", "open clicked!");
    		ls.setOn(0x1c);
    	} else if(v.equals(backButton)) {
    		//Log.e("HELLO", "close clicked!");
    		finish();
    	}else if(v.equals(chupButton)) {
    		//Log.e("HELLO", "close clicked!");
    		ls.setOn(0x55);
    	}else if(v.equals(chdownButton)) {
    		//Log.e("HELLO", "close clicked!");
    		ls.setOn(0x52);
    	}else if(v.equals(volupButton)) {
    		//Log.e("HELLO", "close clicked!");
    		ls.setOn(0x48);
    	}else if(v.equals(voldownButton)) {
    		//Log.e("HELLO", "close clicked!");
    		ls.setOn(0x1f);
    	}
    	for(int i = 0;i<10;++i) if(v.equals(numButton[i])) {
    		//Log.e("HELLO", "close clicked!");
    		ls.setOn(code[i]);
    	}
    }

	
}

