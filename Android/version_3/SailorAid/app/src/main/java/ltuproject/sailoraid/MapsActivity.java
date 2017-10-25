package ltuproject.sailoraid;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.FragmentActivity;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;
import android.widget.Toast;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.CameraPosition;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;

import org.w3c.dom.Text;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import ltuproject.sailoraid.datalog.SailLog;

/**
 * Created by Henrik on 2017-09-06.
 */

public class MapsActivity extends AppCompatActivity implements OnMapReadyCallback {

    private int mInterval = 1000; // 1 seconds by default, can be changed later
    private Handler mHandler;

    private GoogleMap mMap;

    private Polyline travelRoutePolyline;
    private Polyline wayPointPolyline;
    private Marker hereMarker;
    private String receivedIntent;
    private Menu mMenu;
    private boolean isAdd = false;
    private boolean isRemove = false;
    private double totalDistance = 0;
    private double totalWPDistance = 0;
    private final Map<Integer, Marker> mMarkers = new ConcurrentHashMap<Integer, Marker>();
    private List<LatLng> mWaypointRoute = new ArrayList<LatLng>();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.map_activity);
        Bundle extras = getIntent().getExtras();
        Toolbar myToolbar = (Toolbar) findViewById(R.id.map_toolbar);

        if(extras == null) {
            receivedIntent= null;
            setSupportActionBar(myToolbar);
        } else {
            myToolbar.setVisibility(View.INVISIBLE);
            receivedIntent= extras.getString("log");
        }

        // Obtain the SupportMapFragment and get notified when the map is ready to be used.
        SupportMapFragment mapFragment = (SupportMapFragment) getSupportFragmentManager()
                .findFragmentById(R.id.map);
        mapFragment.getMapAsync(this);
    }

    /**
     * Manipulates the map once available.
     * This callback is triggered when the map is ready to be used.
     * This is where we can add markers or lines, add listeners or move the camera. In this case,
     * we just add a marker near Sydney, Australia.
     * If Google Play services is not installed on the device, the user will be prompted to install
     * it inside the SupportMapFragment. This method will only be triggered once the user has
     * installed Google Play services and returned to the app.
     */
    @Override
    public void onMapReady(GoogleMap googleMap) {
        mMap = googleMap;
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION)
                == PackageManager.PERMISSION_GRANTED) {
            mMap.setMyLocationEnabled(true);
        }
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        if (receivedIntent == null){
            mMap.setOnMapClickListener(new GoogleMap.OnMapClickListener() {
                @Override
                public void onMapClick(LatLng point) {
                    if (isAdd == true){
                        markWaypoint(point);
                    } else if(isRemove == true){
                        if (!mMarkers.isEmpty()){
                            removeWaypoint(point);
                        } else {
                            isRemove = false;
                        }

                    } else{
                        setResultToToast("Cannot add waypoint");
                    }
                }
            });
        }
        /*CameraPosition cameraPosition = new CameraPosition.Builder()
                .zoom(14)
                .bearing(0)
                .tilt(5)
                .build();
        mMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));*/
        mHandler = new Handler();
        startRepeatingTask();
    }

    private void setResultToToast(final String string){
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                Toast.makeText(getApplicationContext(), string, Toast.LENGTH_SHORT).show();
            }
        });
    }

    private void markWaypoint(LatLng point){
        //Create MarkerOptions object
        MarkerOptions markerOptions = new MarkerOptions();
        markerOptions.position(point);
        markerOptions.icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_BLUE));
        Marker marker = mMap.addMarker(markerOptions);
        mMarkers.put(mMarkers.size(), marker);
        mWaypointRoute.add(point);
    }

    private void removeWaypoint(LatLng point){
        Set<Integer> keySet = mMarkers.keySet();
        Object[] keys = keySet.toArray();
        Marker nearestMarker = mMarkers.get(keys[0]);
        int index = (int) keys[0];
        double distance = FeedbackActivity.distance_on_geoid(nearestMarker.getPosition().latitude, nearestMarker.getPosition().longitude, point.latitude, point.longitude);;
        for (int i = 1; i<mMarkers.size();i++){
            double compare = FeedbackActivity.distance_on_geoid(mMarkers.get(keys[i]).getPosition().latitude, mMarkers.get(keys[i]).getPosition().longitude, point.latitude, point.longitude);
            if (compare < distance){
                distance = compare;
                nearestMarker = mMarkers.get(keys[i]);
                index = (int) keys[i];
            }
        }
        nearestMarker.remove();
        mMarkers.remove(index);
    }

    private double calcRouteDistance(List<LatLng> routeList){
        double routeDistance = 0;
        if (routeList.size() > 1){
            for (int i= 1; i<routeList.size(); i++){
                double lat1 = routeList.get(i-1).latitude;
                double lat2 = routeList.get(i).latitude;

                double lon1 = routeList.get(i-1).longitude;
                double lon2 = routeList.get(i).longitude;
                routeDistance += FeedbackActivity.distance_on_geoid(lat1, lon1, lat2, lon2);
            }
        }
        return routeDistance / 1000;
    }
    @Override
    public void onDestroy() {
        super.onDestroy();
        stopRepeatingTask();
    }

    Runnable mStatusChecker = new Runnable() {
        @Override
        public void run() {
            try {
                // Update travel route on timer update
                List<LatLng> newPoints = new ArrayList<LatLng>();
                if (receivedIntent != null) {
                    HistoryActivity.getRoute(newPoints);
                } else {
                    FeedbackActivity.getRoute(newPoints);
                }
                PolylineOptions newTravelRoute = new PolylineOptions();
                newTravelRoute.addAll(newPoints);
                if (travelRoutePolyline != null)
                    travelRoutePolyline.remove();
                travelRoutePolyline = mMap.addPolyline(newTravelRoute);

                PolylineOptions newWaypointRoute = new PolylineOptions();
                if (newPoints.size() > 0) {
                    totalDistance = calcRouteDistance(newPoints);
                    //TextView totDistItem = (TextView) findViewById(R.id.text_dist_left);
                    //totDistItem.setText(String.valueOf((int) totalDistance) +" m sailed");
                    if (hereMarker != null)
                        hereMarker.remove();
                    LatLng boat = newPoints.get(newPoints.size() - 1);
                    if(!mMarkers.isEmpty()){
                        if (wayPointPolyline != null){
                            wayPointPolyline.remove();
                        }
                        Set<Integer> keySet = mMarkers.keySet();
                        Object[] keys = keySet.toArray();
                        newWaypointRoute.add(boat, mMarkers.get(keys[0]).getPosition());
                        newWaypointRoute.color(getColor(R.color.green));
                        travelRoutePolyline = mMap.addPolyline(newWaypointRoute);
                        double dist = FeedbackActivity.distance_on_geoid(boat.latitude,boat.longitude, mMarkers.get(keys[0]).getPosition().latitude, mMarkers.get(keys[0]).getPosition().longitude);
                        MenuItem distItem = mMenu.findItem(R.id.dist_to_waypoint);
                        distItem.setTitle(String.valueOf((int)dist) +"m to WP");
                        MenuItem totDistItem = mMenu.findItem(R.id.dist_left);
                        totDistItem.setTitle(String.valueOf((int) (totalDistance/totalWPDistance *100)) +" % sailed");
                    }
                    hereMarker = mMap.addMarker(new MarkerOptions().position(boat).title("You are here"));
                    CameraPosition cameraPosition = new CameraPosition.Builder()
                            .target(boat)
                            .zoom(14)
                            .bearing(0)
                            .tilt(5)
                            .build();
                    mMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
                }
            } finally {
                // 100% guarantee that this always happens, even if
                // your update method throws an exception
                if (receivedIntent == null) {
                    mHandler.postDelayed(mStatusChecker, mInterval);
                }
            }
        }
    };

    void startRepeatingTask() {
        mStatusChecker.run();
    }

    void stopRepeatingTask() {
        mHandler.removeCallbacks(mStatusChecker);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        this.mMenu = menu;
        getMenuInflater().inflate(R.menu.map_menu, mMenu);
        return super.onCreateOptionsMenu(mMenu);
    }
    /*
    Handles choices made on the actionbar menu
     */
    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.create_route:
                isAdd = true;
                Toast.makeText(getApplicationContext(), "Start setting waypoints!", Toast.LENGTH_SHORT).show();
                item.setVisible(false);
                mMenu.findItem(R.id.finnish_route).setVisible(true);
                return true;

            case R.id.finnish_route:
                isAdd = false;
                totalWPDistance = calcRouteDistance(mWaypointRoute);
                Toast.makeText(getApplicationContext(), "Finished routing! \n Total distance: " +totalWPDistance +"Km", Toast.LENGTH_SHORT).show();
                item.setVisible(false);
                mMenu.findItem(R.id.create_route).setVisible(true);
                return true;
            case R.id.remove_waypoint:
                isRemove = true;
                Toast.makeText(getApplicationContext(), "Start removing waypoints!", Toast.LENGTH_SHORT).show();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }
}

