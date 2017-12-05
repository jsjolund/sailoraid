package ltuproject.sailoraid.location;

import com.google.android.gms.maps.model.LatLng;

import static java.lang.Math.acos;
import static java.lang.Math.cos;
import static java.lang.Math.sin;

/**
 * Created by Henrik on 2017-12-04.
 */

public class Locator {

    /*
Calculate distance in meters based on two points in longitude latitude
 */
    public static double distance_on_geoid(double lat1, double lon1, double lat2, double lon2) {

        // Convert degrees to radians
        lat1 = lat1 * Math.PI / 180.0;
        lon1 = lon1 * Math.PI / 180.0;

        lat2 = lat2 * Math.PI / 180.0;
        lon2 = lon2 * Math.PI / 180.0;

        // radius of earth in metres
        double r = 6378100;

        // P
        double rho1 = r * cos(lat1);
        double z1 = r * sin(lat1);
        double x1 = rho1 * cos(lon1);
        double y1 = rho1 * sin(lon1);

        // Q
        double rho2 = r * cos(lat2);
        double z2 = r * sin(lat2);
        double x2 = rho2 * cos(lon2);
        double y2 = rho2 * sin(lon2);

        // Dot product
        double dot = (x1 * x2 + y1 * y2 + z1 * z2);
        double cos_theta = dot / (r * r);
        if (cos_theta > 1){
            cos_theta = 1;
        }
        double theta = acos(cos_theta);

        // Distance in Metres
        return r * theta;
    }

    /*
    Estimate where next position should be using bearing and distance traveled between last two points.
     */
    public static LatLng calcNextEstimatePos(LatLng pos, double distance, double radialBearing){
        int R = 6378100; // Earth Radius in m

        /*double lat2 = Math.asin(Math.sin(Math.PI / 180 * pos.latitude) * Math.cos(distance / R) + Math.cos(Math.PI / 180 * pos.latitude) * Math.sin(distance / R) * Math.cos(Math.PI / 180 * radialBearing));
        double lon2 = Math.PI / 180 * pos.longitude + Math.atan2(Math.sin( Math.PI / 180 * radialBearing) * Math.sin(distance / R) * Math.cos( Math.PI / 180 * pos.longitude ), Math.cos(distance / R) - Math.sin( Math.PI / 180 * pos.longitude) * Math.sin(lat2));

        return new LatLng(180 / Math.PI * lat2 , 180 / Math.PI * lon2);
        */
        //Rhumb
        double rBearing = radialBearing * Math.PI /180;

        double lat1 = pos.latitude * Math.PI / 180.0;
        double lon1 = pos.longitude * Math.PI / 180.0;

        double angDistance = distance/R;
        double deltaLat = angDistance * Math.cos(rBearing);
        double lat2 = lat1 + deltaLat;

        double projLatDiff = Math.log(Math.tan(lat2/2+Math.PI/4)/Math.tan(lat1/2+Math.PI/4));
        double q = Math.abs(projLatDiff) > 10e-12 ? deltaLat / projLatDiff : Math.cos(lat1); // E-W course becomes ill-conditioned with 0/0

        double deltaLon = angDistance*Math.sin(rBearing)/q;
        double lon2 = lon1 + deltaLon;

        lat2 = lat2 * 180.0 / Math.PI;
        lon2 = lon2  * 180.0 / Math.PI;
        return new LatLng(lat2,lon2);
    }

}
