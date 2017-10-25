package ltuproject.sailoraid.datalog;

import android.app.Service;
import android.bluetooth.BluetoothGattService;
import android.content.Context;
import android.content.Intent;
import android.os.Binder;
import android.os.Environment;
import android.os.IBinder;
import android.os.PowerManager;
import android.support.annotation.Nullable;
import android.util.Log;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.EmptyStackException;
import java.util.zip.Inflater;

import ltuproject.sailoraid.bluetooth.BTLEConnection;

import static java.lang.Math.abs;
import static java.lang.Math.max;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_COMPASS;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_DRIFT;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_FREE_FALL;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_HUMIDITY;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_INCLINE;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_POSITION;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_PRESSURE;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_SOG;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_TEMPERATURE;
/**
 * Created by Henrik on 2017-10-03.
 */

public class SailLog extends Service {
    private String fileName;
    private File outFile;
    private FileOutputStream fos;
    private OutputStreamWriter  out;
    private BufferedWriter bwriter;
    private String outPut = "";
    private BufferedReader br;
    private FileInputStream fis;
    private InputStreamReader isr;
    private ArrayList<String[]> posDataList;
    private ArrayList<String[]> imuDataList;
    private ArrayList<String[]> compassDataList;
    private ArrayList<String[]> pressureDataList;
    private ArrayList<String[]> sogDataList;
    private ArrayList<String[]> tempDataList;
    private ArrayList<String[]> humDataList;
    private ArrayList<String[]> driftDataList;
    private float avgIncline, maxIncline;
    private float avgDrift, totalDrift;
    private float avgSOG, topSOG, avgPressure, maxPressure;
    private boolean isLoggin;
    private boolean isInit = false;
    PowerManager.WakeLock wl;

    public SailLog(){

    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        Log.d("PrinterService", "Onstart Command");
        fileName = new SimpleDateFormat("yyyyMMdd_HHmm").format(new Date()) +".txt";
        isLoggin = true;
        initLogData();
        PowerManager pm = (PowerManager) getApplicationContext().getSystemService(Context.POWER_SERVICE);
        wl = pm.newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "My Tag");
        wl.acquire();
        return START_STICKY;
    }

    public void initReadData(String fileName){
        this.fileName = fileName;
        posDataList = new ArrayList<String[]>();
        imuDataList = new ArrayList<String[]>();
        compassDataList = new ArrayList<String[]>();
        pressureDataList = new ArrayList<String[]>();
        sogDataList = new ArrayList<String[]>();
        tempDataList = new ArrayList<String[]>();
        humDataList = new ArrayList<String[]>();
        driftDataList = new ArrayList<String[]>();
    }

    public void initLogData(){
        if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())) {

            outFile = new File(getApplication().getExternalFilesDir(
                    Environment.DIRECTORY_DOCUMENTS) +"/SailorAid/Logs", fileName);

            if (!outFile.exists()) {
                try {
                    outFile.getParentFile().mkdirs();
                    //File does not exists
                    outFile.createNewFile();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }
        }
        isInit = true;
    }

    public void writeToLog(String data){
        try {
	/*Create and open necessary stream for writing information */

            fos = new FileOutputStream(outFile, true);
            out = new OutputStreamWriter(fos);
            bwriter = new BufferedWriter(out);
        }
        catch (FileNotFoundException e) {
            e.printStackTrace();
        }
        try {
            outPut = data +"\n";
            bwriter.append(outPut);
        }
        catch (IOException e) {
            e.printStackTrace();
        }
        stopLogData();
    }

    public void stopLogData(){
        try {
            bwriter.close();
            out.close();
            fos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void finalizeLog(){
        wl.release();
        isLoggin = false;
    }

    public void readLog(){
        String data;
            /* Reads from the text file */
        try{
            fis = new FileInputStream(outFile);
            isr = new InputStreamReader(fis);
            br = new BufferedReader(isr);
            while((data = br.readLine()) != null){
                String[] splitLines = data.split(":");
                String type = splitLines[0];
                if(type.equals(DATA_TYPE_INCLINE)){
                    imuDataList.add(splitLines);
                } else if (type.equals(DATA_TYPE_PRESSURE)){
                    pressureDataList.add(splitLines);
                } else if (type.equals(DATA_TYPE_COMPASS)){
                    compassDataList.add(splitLines);
                } else if (type.equals(DATA_TYPE_POSITION)){
                    posDataList.add(splitLines);
                } else if (type.equals(DATA_TYPE_SOG)){
                    sogDataList.add(splitLines);
                } else if (type.equals(DATA_TYPE_TEMPERATURE)){
                    tempDataList.add(splitLines);
                } else if (type.equals(DATA_TYPE_HUMIDITY)){
                    humDataList.add(splitLines);
                } else if (type.equals(DATA_TYPE_DRIFT)){
                    driftDataList.add(splitLines);
                }
            }
            br.close();
            isr.close();
            fis.close();
        }catch (IOException e){
            e.printStackTrace();
        }
        calcIMUData();
        calcPressureData();
        calcSOGData();
    }

    public boolean deleteLog(String fileName){
        outFile = new File(getApplication().getExternalFilesDir(
                Environment.DIRECTORY_DOCUMENTS) +"/SailorAid/Logs", fileName);
        return outFile.delete();
    }

    public ArrayList<String[]> getPosDataList(){
        return posDataList;
    }
    public ArrayList<String[]> getImuDataList(){
        return imuDataList;
    }
    public ArrayList<String[]> getDriftDataList(){
        return driftDataList;
    }
    private void calcIMUData(){
        float total = 0;
        float max = 0;
        for (String[] data : imuDataList){
            float x = Float.parseFloat(data[2]);
            total += x;
            if (abs(x) > abs(max)){
                max = x;
            }
        }
        if (imuDataList.isEmpty()){
            this.avgIncline = 0;
        } else{
            this.avgIncline = total/imuDataList.size();
        }
        this.maxIncline = max;
    }
    public float getAvgSOG(){
        return avgSOG;
    }

    public float getTopSOG(){
        return topSOG;
    }
    public float getAvgDrift(){
        float total = 0;
        float max = 0;
        for (String[] data : driftDataList){
            float x = Float.parseFloat(data[2]);
            total += x;
            if (abs(x) > abs(max)){
                max = x;
            }
        }
        if (driftDataList.isEmpty()){
            this.avgDrift = 0;
        } else{
            this.avgDrift = total/driftDataList.size();
        }
        this.totalDrift = total;
        return avgDrift;
    }
    public float getTotalDrift(){
        return totalDrift;
    }
    public float getMaxIncline(){
        return maxIncline;
    }
    public float getAvgIncline(){
        return avgIncline;
    }
    public float getAvgPressure(){
        return avgPressure;
    }
    public float getMaxPressure(){
        return maxPressure;
    }

    private void calcSOGData(){
        float total = 0;
        float max = 0;
        for (String[] data : sogDataList){
            float x = Float.parseFloat(data[2]);
            total += x;
            if (abs(x) > abs(max)){
                max = x;
            }
        }
        if (sogDataList.isEmpty()){
            avgSOG = 0;
        } else{
            avgSOG = total/sogDataList.size();
        }
        topSOG = max;
    }

    private void calcPressureData(){
        float total = 0;
        float max = 0;
        for (String[] data : pressureDataList){
            float x = Float.parseFloat(data[2]);
            total += x;
            if (abs(x) > abs(max)){
                max = x;
            }
        }
        if (pressureDataList.isEmpty()){
            avgPressure = 0;
        } else{
            avgPressure = total/pressureDataList.size();
        }
        maxPressure = max;
    }

    public ArrayList<String[]> getCompassDataList(){
        return compassDataList;
    }
    public ArrayList<String[]> getPressureDataList(){
        return pressureDataList;
    }
    public ArrayList<String[]> getSogDataList(){
        return sogDataList;
    }
    public ArrayList<String[]> getTempDataList(){
        return tempDataList;
    }
    public ArrayList<String[]> getHumDataList(){
        return humDataList;
    }

    public boolean isLogging(){
        return isLoggin;
    }

    public boolean isInit(){
        return isInit;
    }

    public class LocalBinder extends Binder {
        public SailLog getService() {
            return SailLog.this;
        }
    }

    private final IBinder mBinder = new SailLog.LocalBinder();

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

}
