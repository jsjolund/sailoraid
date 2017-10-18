package ltuproject.sailoraid;

import android.content.ComponentName;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.ServiceConnection;
import android.graphics.Color;
import android.os.Bundle;
import android.os.Environment;
import android.os.IBinder;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.maps.model.LatLng;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import ltuproject.sailoraid.datalog.SailLog;

/**
 * Created by Henrik on 2017-10-04.
 */

public class HistoryActivity extends AppCompatActivity {

    private final static String TAG = HistoryActivity.class.getSimpleName();
    ArrayAdapter<String> adapter;
    private AlertDialog.Builder popDialog;
    private String fileName;
    private Menu mMenu;
    private Toolbar myToolbar;
    private Button readLogBtn, mapLogBtn, graphLogBtn;
    private TextView maxIncHolder, avgIncHolder, maxDriftHolder, totalDriftHolder, avgSOGHolder, topSOGHolder, maxPressureHolder, avgPressureHolder;
    private SailLog mLogService;
    private View lastSelView;

    private ArrayList<String[]> posDataList;
    private ArrayList<String[]> imuDataList;
    private ArrayList<String[]> compassDataList;
    private ArrayList<String[]> pressureDataList;
    private ArrayList<String[]> sogDataList;
    private ArrayList<String[]> tempDataList;
    private ArrayList<String[]> humDataList;
    private ArrayList<String[]> driftDataList;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.history_activity);

        myToolbar = (Toolbar) findViewById(R.id.history_toolbar);
        setSupportActionBar(myToolbar);

        myToolbar.setVisibility(View.GONE);
        readLogBtn = (Button)findViewById(R.id.readLogBtn);
        mapLogBtn = (Button) findViewById(R.id.mapLogBtn);
        graphLogBtn = (Button) findViewById(R.id.graphLogBtn);
        maxIncHolder = (TextView) findViewById(R.id.maxIncHolder);
        avgIncHolder = (TextView) findViewById(R.id.avgIncHolder);
        maxDriftHolder = (TextView) findViewById(R.id.maxDriftHolder);
        totalDriftHolder = (TextView) findViewById(R.id.totalDriftHolder);
        avgSOGHolder = (TextView) findViewById(R.id.avgSOGHolder);
        topSOGHolder = (TextView) findViewById(R.id.topSOGHolder);
        avgPressureHolder = (TextView) findViewById(R.id.avgPressureHolder);
        maxPressureHolder = (TextView) findViewById(R.id.maxPressureHolder);

        assert readLogBtn != null;
        readLogBtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // feedback button function
                showLogsPopup();
            }
        });

        assert graphLogBtn != null;
        graphLogBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // feedback button function
                Intent intent = new Intent(HistoryActivity.this, GraphActivity.class);
                startActivity(intent);
            }
        });
        assert mapLogBtn != null;
        mapLogBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // feedback button function
                Intent intent = new Intent(HistoryActivity.this, MapsActivity.class);
                intent.putExtra("log", "log");
                startActivity(intent);
            }
        });
    }

    // Code to manage Service lifecycle.
    private final ServiceConnection mLogServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mLogService = ((SailLog.LocalBinder) service).getService();
            if (mLogService.isLogging()){
                mLogService.stopLogData();
                mLogService.finalizeLog();
            }

        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mLogService = null;
        }
    };

    @Override
    public void onResume(){
        super.onResume();
        if (mLogService == null){
            Intent logServiceIntent = new Intent(getApplicationContext(), SailLog.class);
            bindService(logServiceIntent, mLogServiceConnection, BIND_AUTO_CREATE);
        }

    }

    @Override
    public void onDestroy(){
        super.onDestroy();
        if (mLogServiceConnection != null){
            unbindService(mLogServiceConnection);
        }
    }
    private void showLogsPopup() {
        ArrayList<String> files = getAllLogs(getExternalFilesDir(
                Environment.DIRECTORY_DOCUMENTS) + "/SailorAid/Logs");
        adapter = new ArrayAdapter<String>(getApplicationContext(), android.R.layout.simple_list_item_1, files);
        LayoutInflater inflater = (LayoutInflater) getApplicationContext().getSystemService(LAYOUT_INFLATER_SERVICE);
        View Viewlayout = inflater.inflate(R.layout.log_list, (ViewGroup) findViewById(R.id.logListLayout));
        ListView list = (ListView) Viewlayout.findViewById(R.id.logList);
        list.setAdapter(adapter);
        list.setChoiceMode(AbsListView.CHOICE_MODE_SINGLE);
        list.setSelector(android.R.color.holo_blue_light);
        list.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
                //Highlight chosen item on the popup list

                fileName = parent.getAdapter().getItem(position).toString();

            }

        });
        popDialog = new AlertDialog.Builder(this);
        popDialog.setView(Viewlayout);
        popDialog.setTitle("Logs");

        popDialog.setNegativeButton("Delete",
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        if (fileName != null){
                            String toast;
                            if ( mLogService.deleteLog(fileName)){
                                toast = "Deleted log nr: " +fileName;
                                fileName = null;
                            } else {
                                toast = "Failed to delete: " +fileName;
                            }
                            Toast.makeText(getApplicationContext(), toast, Toast.LENGTH_SHORT).show();
                        }
                    }
                });

        popDialog.setNeutralButton("Cancel",
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        fileName = null;
                    }
                });
        popDialog.setPositiveButton("Read",
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();

                        if (fileName != null){
                            String toast = "Read log nr: " +fileName;
                            Toast.makeText(getApplicationContext(), toast, Toast.LENGTH_SHORT).show();
                            readLogBtn.setVisibility(View.GONE);
                            mapLogBtn.setVisibility(View.VISIBLE);
                            graphLogBtn.setVisibility(View.VISIBLE);
                            myToolbar.setVisibility(View.VISIBLE);
                            mapLogBtn.setVisibility(View.VISIBLE);
                            showTextViews();
                            mLogService.initReadData(fileName);
                            mLogService.initLogData();
                            getLogDataToArray();
                            ImageView iv = (ImageView) findViewById(R.id.sailorView);
                            iv.setImageDrawable(getDrawable(R.drawable.sailor_sad));
                            TextView sScore = (TextView) findViewById(R.id.sailorScoreText);
                            sScore.setVisibility(View.VISIBLE);
                            sScore.setText("Your Sailor Score was: 14! \n Not so happy then.");
                        } else {
                            Toast.makeText(getApplicationContext(), "Your need to choose a file!", Toast.LENGTH_SHORT).show();
                        }
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

    private void setTravelRoute(ArrayList<String[]> posList){
        logTravelRoute.clear();
        for (String[] loc : posList){{
            logTravelRoute.add(new LatLng(Float.parseFloat(loc[2]), Float.parseFloat(loc[3])));
        }}
    }
    static private List<LatLng> logTravelRoute = new ArrayList<LatLng>();
    public static void getRoute(List<LatLng> output) {
        output.addAll(logTravelRoute);
    }

    private void setInclineData(){
        ArrayList<String[]> inclineList = new ArrayList<String[]>();
        inclineList = mLogService.getImuDataList();
        logInclineData.clear();
        for (String[] loc : inclineList){{
            logInclineData.add(loc);
        }}
    }

    private void setPressureData(){
        ArrayList<String[]> inclineList = mLogService.getPressureDataList();
        logPressureData.clear();
        for (String[] loc : inclineList){{
            logPressureData.add(loc);
        }}
    }

    private void setSOGData(){
        ArrayList<String[]> inclineList = mLogService.getSogDataList();
        logSOGData.clear();
        if (inclineList.size() > 0){
            for (String[] loc : inclineList){{
                logSOGData.add(loc);
            }}
        }
    }
    private void setCompassData(){
        ArrayList<String[]> inclineList = mLogService.getCompassDataList();
        logCompassData.clear();
        if (inclineList.size() > 0){
            for (String[] loc : inclineList){{
                logCompassData.add(loc);
            }}
        }
    }
    static private List<String[]> logInclineData = new ArrayList<String[]>();
    public static void getInclineData(List<String[]> output) {
        output.addAll(logInclineData);
    }
    static private List<String[]> logPressureData = new ArrayList<String[]>();
    public static void getPressureData(List<String[]> output) {
        output.addAll(logPressureData);
    }
    static private List<String[]> logSOGData = new ArrayList<String[]>();
    public static void getSOGData(List<String[]> output) {
        output.addAll(logSOGData);
    }
    static private List<String[]> logCompassData = new ArrayList<String[]>();
    public static void getCompassData(List<String[]> output) {
        output.addAll(logCompassData);
    }
    static private List<String[]> logHumData = new ArrayList<String[]>();
    public static void getHumData(List<String[]> output) {
        output.addAll(logHumData);
    }
    static private List<String[]> logTempData = new ArrayList<String[]>();
    public static void getTempData(List<String[]> output) {
        output.addAll(logTempData);
    }
    static private List<String[]> logDriftData = new ArrayList<String[]>();
    public static void getDriftData(List<String[]> output) {
        output.addAll(logDriftData);
    }

    private void showTextViews(){
        TextView maxIncText = (TextView) findViewById(R.id.maxIncText);
        TextView avgIncText = (TextView) findViewById(R.id.avgIncText);
        TextView maxDriftText = (TextView) findViewById(R.id.maxDriftText);
        TextView totalDriftText = (TextView) findViewById(R.id.totalDriftText);
        TextView avgSOGText = (TextView) findViewById(R.id.avgSOGText);
        TextView topSOGText = (TextView) findViewById(R.id.topSOGText);
        TextView avgPressureText = (TextView) findViewById(R.id.avgPressureText);
        TextView maxPressureText = (TextView) findViewById(R.id.maxPressureText);
        maxIncText.setVisibility(View.VISIBLE);
        avgIncText.setVisibility(View.VISIBLE);
        maxDriftText.setVisibility(View.VISIBLE);
        totalDriftText.setVisibility(View.VISIBLE);
        avgSOGText.setVisibility(View.VISIBLE);
        topSOGText.setVisibility(View.VISIBLE);
        avgPressureText.setVisibility(View.VISIBLE);
        maxPressureText.setVisibility(View.VISIBLE);
        maxIncHolder.setVisibility(View.VISIBLE);
        avgIncHolder.setVisibility(View.VISIBLE);
        maxDriftHolder.setVisibility(View.VISIBLE);
        totalDriftHolder.setVisibility(View.VISIBLE);
        avgSOGHolder.setVisibility(View.VISIBLE);
        topSOGHolder.setVisibility(View.VISIBLE);
        avgPressureHolder.setVisibility(View.VISIBLE);
        maxPressureHolder.setVisibility(View.VISIBLE);
    }

    private void getLogDataToArray(){
        mLogService.readLog();
        posDataList = mLogService.getPosDataList();
        setTravelRoute(posDataList);
        maxIncHolder.setText(String.valueOf(mLogService.getMaxIncline()));
        avgIncHolder.setText(String.valueOf(mLogService.getAvgIncline()));
        avgSOGHolder.setText(String.valueOf(mLogService.getAvgSOG()));
        topSOGHolder.setText(String.valueOf(mLogService.getTopSOG()));
        maxDriftHolder.setText(String.valueOf(mLogService.getAvgDrift()));
        totalDriftHolder.setText(String.valueOf(mLogService.getTotalDrift()));
        avgPressureHolder.setText(String.valueOf(mLogService.getAvgPressure()));
        maxPressureHolder.setText(String.valueOf(mLogService.getMaxPressure()));
        setInclineData();
        setSOGData();
        setPressureData();
        setCompassData();
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
