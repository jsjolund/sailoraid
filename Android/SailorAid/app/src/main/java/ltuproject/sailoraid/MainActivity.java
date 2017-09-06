package ltuproject.sailoraid;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button feedbackviewbtn = (Button)findViewById(R.id.feedbackviewbtn);

        // Button to access the feedback view
        assert feedbackviewbtn != null;
        feedbackviewbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // feedback button function
                startActivity(new Intent(MainActivity.this,FeedbackActivity.class));
            }
        });
    }
}
