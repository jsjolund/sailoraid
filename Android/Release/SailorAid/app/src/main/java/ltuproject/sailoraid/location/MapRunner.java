package ltuproject.sailoraid.location;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.MenuItem;

import com.google.android.gms.maps.CameraUpdateFactory;
import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.model.BitmapDescriptorFactory;
import com.google.android.gms.maps.model.CameraPosition;
import com.google.android.gms.maps.model.LatLng;
import com.google.android.gms.maps.model.Marker;
import com.google.android.gms.maps.model.MarkerOptions;
import com.google.android.gms.maps.model.Polyline;
import com.google.android.gms.maps.model.PolylineOptions;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

import ltuproject.sailoraid.FeedbackActivity;
import ltuproject.sailoraid.MainActivity;
import ltuproject.sailoraid.MapsActivity;
import ltuproject.sailoraid.R;

/**
 * Created by Henrik on 2017-12-05.
 */

public class MapRunner implements Runnable {
    private List<Polyline> mWPLineList = new ArrayList<>();
    private Polyline travelRoutePolyline;
    private Marker hereMarker;
    private Polyline wayPointPolyline;
    private LatLng boat;
    private final Map<Integer, Marker> mMarkers = new ConcurrentHashMap<Integer, Marker>();
    private ArrayList<LatLng> wpRoute = new ArrayList<LatLng>();
    private Context contx;
    private Handler mHandler;
    private int mInterval = 1000;
    GoogleMap mMap;
    public MapRunner(Context contx, GoogleMap mMap, ArrayList<LatLng> wpRoute){
        this.mMap = mMap;
        this.contx = contx;
        mHandler = new Handler();
        this.wpRoute = wpRoute;
        if (!wpRoute.isEmpty()){
            for (LatLng point:wpRoute){
                MarkerOptions markerOptions = new MarkerOptions();
                markerOptions.position(point);
                markerOptions.icon(BitmapDescriptorFactory.defaultMarker(BitmapDescriptorFactory.HUE_BLUE));
                Marker marker = mMap.addMarker(markerOptions);
                mMarkers.put(mMarkers.size(), marker);
            }
            calcNewWPRoute();
        }
    }

    @Override
    public void run() {
        try {
            List<LatLng> newPoints = new ArrayList<LatLng>();
            FeedbackActivity.getRoute(newPoints);
            // Update travel route on timer update
            PolylineOptions newTravelRoute = new PolylineOptions();
            newTravelRoute.addAll(newPoints);
            if (travelRoutePolyline != null) {
                travelRoutePolyline.remove();
            }
            travelRoutePolyline = mMap.addPolyline(newTravelRoute);
           if (newPoints.size() > 0) {
                if (hereMarker != null)
                    hereMarker.remove();
                boat = newPoints.get(newPoints.size() - 1);
                if(!mMarkers.isEmpty()){
                    if (wayPointPolyline != null){
                        wayPointPolyline.remove();
                    }
                    Set<Integer> keySet = mMarkers.keySet();
                    Object[] keys = keySet.toArray();
                    int index = 0;
                    for (int i = 0; i< keySet.size(); i++){
                        if (mMarkers.get(keys[i]).isVisible()){
                            index = i;
                            break;
                        }
                    }
                    PolylineOptions newWaypointRoute = new PolylineOptions();
                    newWaypointRoute.add(boat, mMarkers.get(keys[index]).getPosition());
                    newWaypointRoute.color(contx.getColor(R.color.green));
                    wayPointPolyline = mMap.addPolyline(newWaypointRoute);
                    double dist = Locator.distance_on_geoid(boat.latitude,boat.longitude, mMarkers.get(keys[index]).getPosition().latitude, mMarkers.get(keys[index]).getPosition().longitude);

                    //Check if close to wp and remove to redraw
                    if (dist < 10){
                        Marker closeMarker = mMarkers.get(keys[index]);
                        removeMarker(closeMarker);
                    }
                } else{
                    //  MenuItem totDistItem = mMenu.findItem(R.id.dist_left);
                    //  totDistItem.setVisible(false);
                    if (wayPointPolyline != null){
                        wayPointPolyline.remove();
                    }
                }
                hereMarker = mMap.addMarker(new MarkerOptions().position(boat).title("You are here"));
                CameraPosition cameraPosition = new CameraPosition.Builder()
                        .target(boat)
                        .zoom(15)
                        .bearing(0)
                        .tilt(5)
                        .build();
                mMap.animateCamera(CameraUpdateFactory.newCameraPosition(cameraPosition));

            }
        } finally {
            // 100% guarantee that this always happens, even if
            // your update method throws an exception
            mHandler.postDelayed(this, mInterval);
        }
    }
    private void calcNewWPRoute(){

        if (!mWPLineList.isEmpty()){
            for (Polyline line : mWPLineList) {
                line.remove();
            }
            mWPLineList.clear();
        }
        PolylineOptions newWPRoute = new PolylineOptions();
        newWPRoute.color(contx.getColor(R.color.red));
        newWPRoute.addAll(wpRoute);
        mWPLineList.add(mMap.addPolyline(newWPRoute));
        Intent i = new Intent(contx, MainActivity.class);
        Bundle args = new Bundle();
        args.putSerializable("ARRAYLIST", wpRoute);
        i.putExtra("bundle", args);
        ((Activity) contx).setResult(Activity.RESULT_OK, i);
    }
    private void removeMarker(Marker marker){
        marker.setVisible(false);
        Set<Integer> keySet = mMarkers.keySet();
        Object[] keys = keySet.toArray();
        for (int i = 0; i< keys.length; i++){
            if (marker.equals(mMarkers.get(keys[i]))){
                mMarkers.remove(keys[i]);
            }
        }
        wpRoute.remove(marker.getPosition());
        calcNewWPRoute();
        marker.remove();
    }
    public ArrayList<LatLng> getWpRoute(){
        return wpRoute;
    }
    public void stopMap(){
        mHandler.removeCallbacks(this);
    }
    public void updateWpRoute(ArrayList<LatLng> wpRoute){
        this.wpRoute = wpRoute;
        calcNewWPRoute();
    }
}
