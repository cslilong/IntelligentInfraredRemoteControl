package hnu.yk;


import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.*;
import android.widget.RadioGroup.OnCheckedChangeListener;
public class SmartControllerActivity extends Activity {

	private RadioGroup 	radiogroup=null;
	private RadioButton	definebutton=null;
	private RadioButton nodefbutton=null;
	private TextView	intruduction=null;
	private Button		go=null;
	private boolean		select=false;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        
        radiogroup = (RadioGroup)findViewById(R.id.welcome_radiogroup);
        definebutton=(RadioButton)findViewById(R.id.already_define);
        nodefbutton =(RadioButton)findViewById(R.id.nodefine);
        intruduction=(TextView)findViewById(R.id.simpleintruduction);
        go			=(Button)findViewById(R.id.go);
        
        radiogroup.setOnCheckedChangeListener(new OnCheckedChangeListener(){

			public void onCheckedChanged(RadioGroup group, int checkedId) {
				// TODO Auto-generated method stub
				if( checkedId==definebutton.getId() ){
					intruduction.setText(R.string.defineIntr);
					select=true;
				}
				else if(checkedId==nodefbutton.getId()){
					intruduction.setText(R.string.nodefIntr);
					select=false;
				}
			}
        	
        });
        
        go.setOnClickListener(new OnClickListener(){

			public void onClick(View v) {
				// TODO Auto-generated method stub
				Intent intent=new Intent();
				if(select==true){
					intent.setClass(SmartControllerActivity.this, ControllerPanel.class);
					SmartControllerActivity.this.startActivity(intent);
					
				}
			}

        });
	
    }
}
