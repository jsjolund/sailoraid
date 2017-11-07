package ltuproject.sailoraid;

import android.content.Context;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Window;
import android.view.WindowManager;

import com.google.android.gms.maps.model.LatLng;
import com.jjoe64.graphview.DefaultLabelFormatter;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.GridLabelRenderer;
import com.jjoe64.graphview.helper.DateAsXAxisLabelFormatter;
import com.jjoe64.graphview.helper.StaticLabelsFormatter;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

import java.text.DateFormat;
import java.text.NumberFormat;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;

import static ltuproject.sailoraid.R.drawable.graph;

public class GraphActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.graph_activity);


        List<String[]> inclineList = new ArrayList<String[]>();
        HistoryActivity.getInclineData(inclineList);
        List<String[]> pressureList = new ArrayList<String[]>();
        HistoryActivity.getPressureData(pressureList);
        List<String[]> sogList = new ArrayList<String[]>();
        HistoryActivity.getSOGData(sogList);
        List<String[]> compassList = new ArrayList<String[]>();
        HistoryActivity.getCompassData(compassList);
        List<String[]> humList = new ArrayList<String[]>();
        HistoryActivity.getHumData(humList);
        List<String[]> tempList = new ArrayList<String[]>();
        HistoryActivity.getTempData(tempList);
        List<String[]> driftList = new ArrayList<String[]>();
        HistoryActivity.getDriftData(driftList);

        populateGraphs(inclineList, pressureList, sogList, compassList, humList, tempList, driftList);
    }

    public void populateGraphs(List<String[]> inclineList, List<String[]> pressureList, List<String[]> sogList,
                               List<String[]> compassList, List<String[]> humList, List<String[]> tempList, List<String[]> driftList){
        GraphView graphI = (GraphView) findViewById(R.id.graph_incline);
        GraphView graphP = (GraphView) findViewById(R.id.graph_pressure);
        GraphView graphS = (GraphView) findViewById(R.id.graph_sog);
        GraphView graphT= (GraphView) findViewById(R.id.graph_temp);
        GraphView graphH = (GraphView) findViewById(R.id.graph_hum);
        GraphView graphC = (GraphView) findViewById(R.id.graph_compass);
        GraphView graphD = (GraphView) findViewById(R.id.graph_drift);
        LineGraphSeries<DataPoint> seriesIncline = new LineGraphSeries<>();
        LineGraphSeries<DataPoint> seriesPressure = new LineGraphSeries<>();
        LineGraphSeries<DataPoint> seriesSOG = new LineGraphSeries<>();
        LineGraphSeries<DataPoint> seriesCompass = new LineGraphSeries<>();
        LineGraphSeries<DataPoint> seriesTemp = new LineGraphSeries<>();
        LineGraphSeries<DataPoint> seriesHum = new LineGraphSeries<>();
        LineGraphSeries<DataPoint> seriesDrift = new LineGraphSeries<>();

        for (String[] data : inclineList){
            Date date = dateFromString(data[1]);
            DataPoint dp = new DataPoint(date, Float.parseFloat(data[2]));
            seriesIncline.appendData(dp, true, inclineList.size());

        }

        for (String[] data : pressureList){
            Date date = dateFromString(data[1]);
            DataPoint dp = new DataPoint(date, Float.parseFloat(data[2]));
            seriesPressure.appendData(dp, true, pressureList.size());
        }

        for (String[] data : sogList){
            Date date = dateFromString(data[1]);
            DataPoint dp = new DataPoint(date, Float.parseFloat(data[2]));
            seriesSOG.appendData(dp, true, sogList.size());
        }

        for (String[] data : compassList){
            Date date = dateFromString(data[1]);
            DataPoint dp = new DataPoint(date, Float.parseFloat(data[2]));
            seriesCompass.appendData(dp, true, compassList.size());
        }

        for (String[] data : tempList){
            Date date = dateFromString(data[1]);
            DataPoint dp = new DataPoint(date, Float.parseFloat(data[2]));
            seriesTemp.appendData(dp, true, tempList.size());
        }

        for (String[] data : humList){
            Date date = dateFromString(data[1]);
            DataPoint dp = new DataPoint(date, Float.parseFloat(data[2]));
            seriesHum.appendData(dp, true, humList.size());
        }

        for (String[] data : driftList){
            Date date = dateFromString(data[1]);
            DataPoint dp = new DataPoint(date, Float.parseFloat(data[2]));
            seriesDrift.appendData(dp, true, driftList.size());
        }

        SimpleDateFormat dt = new SimpleDateFormat("hh:mm:ss.SSS");

        graphP.addSeries(seriesPressure);
        graphP.setTitle("Pressure");
        graphP.getViewport().setScalable(true);
        graphP.getViewport().setScalableY(true);
        graphP.getGridLabelRenderer().setLabelFormatter(new DateAsXAxisLabelFormatter(getApplicationContext(), dt));
        graphP.getGridLabelRenderer().setHorizontalAxisTitle("Time");
        graphP.getGridLabelRenderer().setVerticalAxisTitle("Psi");
        graphP.getGridLabelRenderer().setNumHorizontalLabels(4); // only 4 because of the space

        graphI.addSeries(seriesIncline);
        graphI.setTitle("Heel");
        graphI.getViewport().setScalable(true);
        graphI.getViewport().setScalableY(true);
        graphI.getGridLabelRenderer().setLabelFormatter(new DateAsXAxisLabelFormatter(getApplicationContext(), dt));
        graphI.getGridLabelRenderer().setHorizontalAxisTitle("Time");
        graphI.getGridLabelRenderer().setVerticalAxisTitle("Degrees");
        graphI.getGridLabelRenderer().setNumHorizontalLabels(4); // only 4 because of the space


        graphS.addSeries(seriesSOG);
        graphS.setTitle("Speed over ground");
        graphS.getViewport().setScalable(true);
        graphS.getViewport().setScalableY(true);
        graphS.getGridLabelRenderer().setLabelFormatter(new DateAsXAxisLabelFormatter(getApplicationContext(), dt));
        graphS.getGridLabelRenderer().setHorizontalAxisTitle("Time");
        graphS.getGridLabelRenderer().setVerticalAxisTitle("m/s");
        graphS.getGridLabelRenderer().setNumHorizontalLabels(4); // only 4 because of the space

        graphT.addSeries(seriesTemp);
        graphT.setTitle("Temperature");
        graphT.getViewport().setScalable(true);
        graphT.getViewport().setScalableY(true);
        graphT.getGridLabelRenderer().setLabelFormatter(new DateAsXAxisLabelFormatter(getApplicationContext(), dt));
        graphT.getGridLabelRenderer().setHorizontalAxisTitle("Time");
        graphT.getGridLabelRenderer().setVerticalAxisTitle("C");
        graphT.getGridLabelRenderer().setNumHorizontalLabels(4); // only 4 because of the space

        graphH.addSeries(seriesHum);
        graphH.setTitle("Humidity");
        graphH.getViewport().setScalable(true);
        graphH.getViewport().setScalableY(true);
        graphH.getGridLabelRenderer().setLabelFormatter(new DateAsXAxisLabelFormatter(getApplicationContext(), dt));
        graphH.getGridLabelRenderer().setHorizontalAxisTitle("Time");
        graphH.getGridLabelRenderer().setVerticalAxisTitle("%");
        graphH.getGridLabelRenderer().setNumHorizontalLabels(4); // only 4 because of the space

        //Todo better representaiton of bearing
        graphC.addSeries(seriesCompass);
        graphC.setTitle("Bearing");
        graphC.getViewport().setScalable(true);
        graphC.getViewport().setScalableY(true);
        //StaticLabelsFormatter staticLabelsFormatter = new StaticLabelsFormatter(graphC);
        //staticLabelsFormatter.setVerticalLabels(new String[] {"South", "West", "North", "East", "South"});
        //graphC.getGridLabelRenderer().setLabelFormatter(staticLabelsFormatter);
        graphC.getGridLabelRenderer().setLabelFormatter(new DateAsXAxisLabelFormatter(getApplicationContext(), dt));
        graphC.getGridLabelRenderer().setHorizontalAxisTitle("Time");
        graphC.getGridLabelRenderer().setVerticalAxisTitle("Degree");
        graphC.getGridLabelRenderer().setNumHorizontalLabels(4); // only 4 because of the space

        graphD.addSeries(seriesDrift);
        graphD.setTitle("Drift");
        graphD.getViewport().setScalable(true);
        graphD.getViewport().setScalableY(true);
        graphD.getGridLabelRenderer().setLabelFormatter(new DateAsXAxisLabelFormatter(getApplicationContext(), dt));
        graphD.getGridLabelRenderer().setHorizontalAxisTitle("Time");
        graphD.getGridLabelRenderer().setVerticalAxisTitle("m");
        graphD.getGridLabelRenderer().setNumHorizontalLabels(4); // only 4 because of the space
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

}