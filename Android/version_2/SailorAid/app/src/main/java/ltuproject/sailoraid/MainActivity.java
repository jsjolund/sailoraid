package ltuproject.sailoraid;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);

        Button feedbackviewbtn = (Button)findViewById(R.id.feedbackviewbtn);
        Button bluetoothbtn = (Button)findViewById(R.id.btconbtn);
        Button historybtn = (Button) findViewById(R.id.historyviewbtn);

        // Button to access the feedback view
        assert feedbackviewbtn != null;
        feedbackviewbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // feedback button function
                startActivity(new Intent(MainActivity.this,FeedbackActivity.class));
            }
        });

        assert bluetoothbtn != null;
        bluetoothbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // stat view button function
                startActivity(new Intent(MainActivity.this,BTConnectActivity.class));
            }
        });

        assert historybtn != null;
        historybtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // stat view button function
                startActivity(new Intent(MainActivity.this,HistoryActivity.class));
            }
        });
    }
}
