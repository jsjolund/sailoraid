package ltuproject.sailoraid;

import android.content.Context;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.Window;
import android.view.WindowManager;

import com.google.android.gms.maps.model.LatLng;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.LineGraphSeries;

import java.util.ArrayList;
import java.util.List;

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
        HistoryActivity.getInclineData(pressureList);
        List<String[]> sogList = new ArrayList<String[]>();
        HistoryActivity.getInclineData(sogList);

        populateGraphs(inclineList, pressureList, sogList);
    }

    public void populateGraphs(List<String[]> inclineList, List<String[]> pressureList, List<String[]> sogList){
        GraphView graph = (GraphView) findViewById(R.id.graph);

        LineGraphSeries<DataPoint> series = new LineGraphSeries<>();
        for (String[] data : inclineList){
            DataPoint dp = new DataPoint(Float.parseFloat(data[1]), Float.parseFloat(data[2]));
            series.appendData(dp, true, inclineList.size());
        }
        graph.addSeries(series);
    }
}