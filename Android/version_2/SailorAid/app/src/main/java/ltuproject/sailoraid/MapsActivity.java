package ltuproject.sailoraid;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.FragmentActivity;
import android.support.v4.content.ContextCompat;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.OnMapReadyCallback;
import com.google.android.gms.maps.SupportMapFragment;
import com.google.android.gms.maps.model.CameraPosition;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by Henrik on 2017-09-06.
 */

public class MapsActivity extends FragmentActivity implements OnMapReadyCallback {

    private int mInterval = 1000; // 1 seconds by default, can be changed later
    private Handler mHandler;


    private GoogleMap mMap;

    private Polyline travelRoutePolyline;
    private Marker hereMarker;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.map_activity);

        Intent i = getIntent();

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
        mHandler = new Handler();
        startRepeatingTask();
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
                FeedbackActivity.getRoute(newPoints);
                PolylineOptions newTravelRoute = new PolylineOptions();
                newTravelRoute.addAll(newPoints);
                if (travelRoutePolyline != null)
                    travelRoutePolyline.remove();
                travelRoutePolyline = mMap.addPolyline(newTravelRoute);

                if (newPoints.size() > 0) {
                    if (hereMarker != null)
                        hereMarker.remove();
                    LatLng boat = newPoints.get(newPoints.size() - 1);
                    hereMarker = mMap.addMarker(new MarkerOptions().position(boat).title("You are here"));
                    CameraPosition cameraPosition = new CameraPosition.Builder()
                            .target(boat)
                            .zoom(16)
                            .bearing(0)
                            .tilt(5)
                            .build();
                    mMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));
                }
            } finally {
                // 100% guarantee that this always happens, even if
                // your update method throws an exception
                mHandler.postDelayed(mStatusChecker, mInterval);
            }
        }
    };

    void startRepeatingTask() {
        mStatusChecker.run();
    }

    void stopRepeatingTask() {
        mHandler.removeCallbacks(mStatusChecker);
    }
}

