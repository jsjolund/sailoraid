package ltuproject.sailoraid;

import android.content.Context;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Window;
import android.view.WindowManager;

import com.google.android.gms.maps.model.LatLng;
import com.jjoe64.graphview.DefaultLabelFormatter;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.helper.StaticLabelsFormatter;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

import java.text.NumberFormat;
import java.util.ArrayList;
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

        populateGraphs(inclineList, pressureList, sogList, compassList, humList, tempList);
    }

    public void populateGraphs(List<String[]> inclineList, List<String[]> pressureList, List<String[]> sogList, List<String[]> compassList, List<String[]> humList, List<String[]> tempList){
        GraphView graphI = (GraphView) findViewById(R.id.graph_incline);
        GraphView graphP = (GraphView) findViewById(R.id.graph_pressure);
        GraphView graphS = (GraphView) findViewById(R.id.graph_sog);
        GraphView graphT= (GraphView) findViewById(R.id.graph_temp);
        GraphView graphH = (GraphView) findViewById(R.id.graph_hum);
        GraphView graphC = (GraphView) findViewById(R.id.graph_compass);
        LineGraphSeries<DataPoint> seriesIncline = new LineGraphSeries<>();
        LineGraphSeries<DataPoint> seriesPressure = new LineGraphSeries<>();
        LineGraphSeries<DataPoint> seriesSOG = new LineGraphSeries<>();
        LineGraphSeries<DataPoint> seriesCompass = new LineGraphSeries<>();
        LineGraphSeries<DataPoint> seriesTemp = new LineGraphSeries<>();
        LineGraphSeries<DataPoint> seriesHum = new LineGraphSeries<>();

        for (String[] data : inclineList){
            DataPoint dp = new DataPoint(Float.parseFloat(data[1]), Float.parseFloat(data[2]));
            seriesIncline.appendData(dp, true, inclineList.size());
        }

        for (String[] data : pressureList){
            DataPoint dp = new DataPoint(Float.parseFloat(data[1]), Float.parseFloat(data[2]));
            seriesPressure.appendData(dp, true, pressureList.size());
        }

        for (String[] data : sogList){
            DataPoint dp = new DataPoint(Float.parseFloat(data[1]), Float.parseFloat(data[2]));
            seriesSOG.appendData(dp, true, sogList.size());
        }

        for (String[] data : compassList){
            DataPoint dp = new DataPoint(Float.parseFloat(data[1]), Float.parseFloat(data[2]));
            seriesCompass.appendData(dp, true, compassList.size());
        }

        for (String[] data : tempList){
            DataPoint dp = new DataPoint(Float.parseFloat(data[1]), Float.parseFloat(data[2]));
            seriesTemp.appendData(dp, true, tempList.size());
        }

        for (String[] data : humList){
            DataPoint dp = new DataPoint(Float.parseFloat(data[1]), Float.parseFloat(data[2]));
            seriesHum.appendData(dp, true, humList.size());
        }

        graphP.addSeries(seriesIncline);
        graphP.setTitle("Pressure");
        graphP.getViewport().setScalable(true);
        graphP.getViewport().setScalableY(true);


        graphI.addSeries(seriesPressure);
        graphI.setTitle("Heel");
        graphI.getViewport().setScalable(true);
        graphI.getViewport().setScalableY(true);
        NumberFormat nf = NumberFormat.getInstance();
        nf.setMinimumFractionDigits(1);
        nf.setMaximumFractionDigits(2);
        nf.setMinimumIntegerDigits(1);
        nf.setMaximumIntegerDigits(3);
        graphI.getGridLabelRenderer().setLabelFormatter(new DefaultLabelFormatter(nf, nf));

        graphS.addSeries(seriesSOG);
        graphS.setTitle("Speed over ground");
        graphS.getViewport().setScalable(true);
        graphS.getViewport().setScalableY(true);

        graphT.addSeries(seriesTemp);
        graphT.setTitle("Temperature");
        graphT.getViewport().setScalable(true);
        graphT.getViewport().setScalableY(true);

        graphH.addSeries(seriesHum);
        graphH.setTitle("Humidity");
        graphH.getViewport().setScalable(true);
        graphH.getViewport().setScalableY(true);

        graphC.addSeries(seriesCompass);
        graphC.setTitle("Bearing");
        graphC.getViewport().setScalable(true);
        graphC.getViewport().setScalableY(true);
        StaticLabelsFormatter staticLabelsFormatter = new StaticLabelsFormatter(graphC);
        staticLabelsFormatter.setVerticalLabels(new String[] {"South", "West", "North", "East", "South"});
        graphC.getGridLabelRenderer().setLabelFormatter(staticLabelsFormatter);
    }
}