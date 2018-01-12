package ltuproject.sailoraid;

import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ProgressBar;

import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.helper.DateAsXAxisLabelFormatter;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;


public class GraphActivity extends AppCompatActivity {
    List<String[]> inclineList, pressureList, sogList, compassList, humList, tempList, driftList, waveList, rangeList;
    ProgressBar progress;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.graph_activity);
        Toolbar myToolbar = findViewById(R.id.graph_toolbar);
        setSupportActionBar(myToolbar);
        progress = this.findViewById(R.id.progressBar);

        inclineList = new ArrayList<>();
        HistoryActivity.getInclineData(inclineList);
        pressureList = new ArrayList<>();
        HistoryActivity.getPressureData(pressureList);
        sogList = new ArrayList<>();
        HistoryActivity.getSOGData(sogList);
        compassList = new ArrayList<>();
        HistoryActivity.getCompassData(compassList);
        humList = new ArrayList<>();
        HistoryActivity.getHumData(humList);
        tempList = new ArrayList<>();
        HistoryActivity.getTempData(tempList);
        driftList = new ArrayList<>();
        HistoryActivity.getDriftData(driftList);
        waveList = new ArrayList<>();
        HistoryActivity.getWavesData(waveList);
        rangeList = new ArrayList<>();
        HistoryActivity.getRangeData(rangeList);
    }

    public void populateChosenGraph(List<String[]> dataList, final int view, final String title, final String vLabel, int millis, final int horLabels){

        final LineGraphSeries<DataPoint> dataSeries = new LineGraphSeries<>();
        for (String[] data : dataList){
            Date date = dateFromString(data[1]);
            DataPoint dp = new DataPoint(date, Float.parseFloat(data[2]));
            dataSeries.appendData(dp, true, dataList.size());
        }
        SimpleDateFormat dt = new SimpleDateFormat("hh:mm:ss.S");
        if (millis == 0){
            dt = new SimpleDateFormat("hh:mm:ss");
        } else if (millis == 2){
            dt = new SimpleDateFormat("hh:mm:ss.SS");
        }
        final SimpleDateFormat dtfin = dt;
        new Handler(Looper.getMainLooper()).post(new Runnable() {

            @Override
            public void run() {
                final GraphView graph = findViewById(view);
                graph.setVisibility(View.VISIBLE);

                graph.addSeries(dataSeries);
                graph.setTitle(title);
                graph.getViewport().setScalable(true);
                graph.getViewport().setScalableY(true);
                graph.getGridLabelRenderer().setLabelFormatter(new DateAsXAxisLabelFormatter(getApplicationContext(), dtfin));
                graph.getGridLabelRenderer().setHorizontalAxisTitle("Time");
                graph.getGridLabelRenderer().setVerticalAxisTitle(vLabel);
                graph.getGridLabelRenderer().setNumHorizontalLabels(horLabels);
//stuff that updates ui

            }
        });

    }

    public void removeChosenGraph(int view){
        GraphView graph = findViewById(view);
        graph.setVisibility(View.GONE);
        graph.removeAllSeries();
    }

    /*
    Converts the string from log to Date format for graph time representation
     */
    private Date dateFromString(String data){
        String hour = data.substring(0,2);
        String min = data.substring(2,4);
        String sec = data.substring(4,6);
        String mSec;
        if ( data.length() > 8){
            mSec = data.substring(6,data.length());
        }else{
            mSec = data.substring(6,8);
        }

        String time = hour +":" +min +":" +sec +"." +mSec;
        SimpleDateFormat dt = new SimpleDateFormat("hh:mm:ss.SSS");
        Date date = new Date();
        try {
            date = dt.parse(time);
        }catch (ParseException e) {
            e.printStackTrace();
        }
        return date;
    }

    /*
 Handles choices made on the actionbar menu
  */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.heel_enable:
                if (!item.isChecked()){
                    item.setChecked(true);
                    new ProgressTask().execute(inclineList, R.id.graph_incline,"Heel", "Degrees", 1, 3);
                    //populateChosenGraph(inclineList, R.id.graph_incline,"Heel", "Degrees", 1, 3);
                } else{
                    item.setChecked(false);
                    removeChosenGraph(R.id.graph_incline);
                }
                return true;

            case R.id.sog_enable:
                if (!item.isChecked()){
                    item.setChecked(true);
                    new ProgressTask().execute(sogList, R.id.graph_sog,"SOG", "Knots", 0, 3);
                } else{
                    item.setChecked(false);
                    removeChosenGraph(R.id.graph_sog);
                }
                return true;
            case R.id.drift_enable:
                if (!item.isChecked()){
                    item.setChecked(true);
                    new ProgressTask().execute(driftList, R.id.graph_drift,"Drift", "Meters", 0, 3);
                } else{
                    item.setChecked(false);
                    removeChosenGraph(R.id.graph_drift);
                }
                return true;

            case R.id.pressure_enable:
                if (!item.isChecked()){
                    item.setChecked(true);
                    new ProgressTask().execute(pressureList, R.id.graph_pressure,"Pressure", "%", 1, 3);
                } else{
                    item.setChecked(false);
                    removeChosenGraph(R.id.graph_pressure);
                }
                return true;
            case R.id.wave_enable:
                if (!item.isChecked()){
                    item.setChecked(true);
                    new ProgressTask().execute(waveList, R.id.graph_wave,"Wave", "Hz", 0, 3);
                } else{
                    item.setChecked(false);
                    removeChosenGraph(R.id.graph_wave);
                }
                return true;
            case R.id.temp_enable:
                if (!item.isChecked()){
                    item.setChecked(true);
                    new ProgressTask().execute(tempList, R.id.graph_temp,"Temperature", "\u00B0C", 0, 3);
                } else{
                    item.setChecked(false);
                    removeChosenGraph(R.id.graph_temp);
                }
                return true;
            case R.id.hum_enable:
                if (!item.isChecked()){
                    item.setChecked(true);
                    new ProgressTask().execute(humList, R.id.graph_hum,"Humidity", "%", 0, 3);
                } else{
                    item.setChecked(false);
                    removeChosenGraph(R.id.graph_hum);
                }
                return true;
            case R.id.range_enable:
                if (!item.isChecked()){
                    item.setChecked(true);
                    new ProgressTask().execute(rangeList, R.id.graph_range,"Range", "cm", 0, 3);
                } else{
                    item.setChecked(false);
                    removeChosenGraph(R.id.graph_range);
                }
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.graph_menu, menu);
        return super.onCreateOptionsMenu(menu);
    }

    private class ProgressTask extends AsyncTask<Object,Void,Void> {
        @Override
        protected void onPreExecute(){
            progress.setVisibility(View.VISIBLE);
        }

        @Override
        protected Void doInBackground(Object... params) {
            ArrayList<String[]> dataList = (ArrayList<String[]>) params[0];
            int view = (int) params[1];
            String title = (String) params[2];
            String vLabel = (String) params[3];
            int millis = (int) params[4];
            int hLabels = (int) params[5];
            populateChosenGraph(dataList, view,title, vLabel, millis, hLabels);
            return null;
        }

        @Override
        protected void onPostExecute(Void result) {
            progress.setVisibility(View.GONE);
        }
    }
}