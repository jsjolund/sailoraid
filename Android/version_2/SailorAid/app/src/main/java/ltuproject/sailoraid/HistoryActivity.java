package ltuproject.sailoraid;

import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

import java.io.File;
import java.util.ArrayList;

import ltuproject.sailoraid.datalog.SailLog;

/**
 * Created by Henrik on 2017-10-04.
 */

public class HistoryActivity extends AppCompatActivity {

    ArrayAdapter<String> adapter;
    private AlertDialog.Builder popDialog;
    private String fileName;
    private Menu mMenu;
    private Toolbar myToolbar;
    private Button readLogbtn;
    private TextView maxIncHolder, avgIncHolder;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.history_activity);

        readLogbtn = (Button)findViewById(R.id.readLogBtn);
        maxIncHolder = (TextView) findViewById(R.id.maxIncHolder);
        avgIncHolder = (TextView) findViewById(R.id.avgIncHolder);




        assert readLogbtn != null;
        readLogbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // feedback button function
                showLogsPopup();
            }
        });

    }


    private void showLogsPopup(){
        ArrayList<String> files = getAllLogs(getExternalFilesDir(
                Environment.DIRECTORY_DOCUMENTS) +"/SailorAid/Logs");
        adapter = new ArrayAdapter<String>(getApplicationContext(), android.R.layout.simple_list_item_1, files);
        LayoutInflater inflater = (LayoutInflater) getApplicationContext().getSystemService(LAYOUT_INFLATER_SERVICE);
        View Viewlayout = inflater.inflate(R.layout.log_list, (ViewGroup) findViewById(R.id.logListLayout));
        ListView list = (ListView) Viewlayout.findViewById(R.id.logList);
        list.setAdapter(adapter);


        list.setOnItemClickListener(new AdapterView.OnItemClickListener()
        {
            public void onItemClick(AdapterView<?> parent, View view,int position, long id) {
                //Highlight chosen item on the popup list
                view.setBackgroundColor(Color.LTGRAY);
                fileName = parent.getAdapter().getItem(position).toString();

            }
        });
        popDialog = new AlertDialog.Builder(this);
        popDialog.setView(Viewlayout);
        popDialog.setTitle("Logs");

        popDialog.setPositiveButton("Read",
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        String toast = "Read log nr: " +fileName;
                        Toast.makeText(getApplicationContext(), toast, Toast.LENGTH_SHORT).show();
                        myToolbar = (Toolbar) findViewById(R.id.history_toolbar);
                        setSupportActionBar(myToolbar);
                        readLogbtn.setVisibility(View.GONE);
                        TextView maxIncText = (TextView) findViewById(R.id.maxIncText);
                        TextView avgIncText = (TextView) findViewById(R.id.avgIncText);
                        maxIncText.setVisibility(View.VISIBLE);
                        avgIncText.setVisibility(View.VISIBLE);
                        maxIncHolder.setVisibility(View.VISIBLE);
                        avgIncHolder.setVisibility(View.VISIBLE);
                        maxIncHolder.setText("340 degrees yo");
                        avgIncHolder.setText("-23 degrees");
                        ImageView iv = (ImageView) findViewById(R.id.sailorView);
                        iv.setImageDrawable(getDrawable(R.drawable.sailor_sad));
                        TextView sScore = (TextView) findViewById(R.id.sailorScoreText);
                        sScore.setText("Your Sailor Score was: 14! \n Not so happy then.");
                    }
                    });
        popDialog.create();
        popDialog.show();
    }

    private ArrayList<String> getAllLogs(String dir){
        ArrayList<String> result = new ArrayList<String>(); //ArrayList cause you don't know how many files there is
        File folder = new File(dir); //This is just to cast to a File type since you pass it as a String
        File[] filesInFolder = folder.listFiles(); // This returns all the folders and files in your path
        for (File file : filesInFolder) { //For each of the entries do:
            if (!file.isDirectory()) { //check that it's not a dir
                result.add(new String(file.getName())); //push the filename as a string
            }
        }
        return result;
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.history_menu, menu);
        return super.onCreateOptionsMenu(menu);
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.read_log:
                showLogsPopup();
                // User chose the "Settings" item, show the app settings UI...
                return true;

            default:
                // If we got here, the user's action was not recognized.
                // Invoke the superclass to handle it.
                return super.onOptionsItemSelected(item);
        }
    }
}
