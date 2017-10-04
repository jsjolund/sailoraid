package ltuproject.sailoraid.datalog;

import android.content.Context;
import android.os.Environment;
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

import static java.lang.Math.abs;
import static java.lang.Math.max;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.DATA_TYPE_COMPASS;
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

public class SailLog {
    private String fileName;
    private File outFile;
    private FileOutputStream fos;
    private OutputStreamWriter  out;
    private BufferedWriter bwriter;
    private String outPut = "";
    private Context context;
    private BufferedReader br;
    private FileInputStream fis;
    private InputStreamReader isr;
    private ArrayList<String[]> posDataList;
    private ArrayList<String[]> imuDataList;
    private ArrayList<String[]> compassDataList;
    private ArrayList<String[]> pressureDataList;
    private ArrayList<String[]> sogDataList;
    private int avgIncline, maxIncline, avgDrift, totalDrift, avgPressure, maxPressure;
    private float avgSOG, topSOG;

    public SailLog(Context context){
        fileName = new SimpleDateFormat("yyyyMMdd_HHmm").format(new Date()) +".txt";
        this.context = context;
    }
    public SailLog(Context context, String fileName){
        this.fileName = fileName;
        this.context = context;
        posDataList = new ArrayList<String[]>();
        imuDataList = new ArrayList<String[]>();
        compassDataList = new ArrayList<String[]>();
        pressureDataList = new ArrayList<String[]>();
        sogDataList = new ArrayList<String[]>();
    }

    public void initLogData(){
        if (Environment.MEDIA_MOUNTED.equals(Environment.getExternalStorageState())) {

            outFile = new File(context.getExternalFilesDir(
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

    public ArrayList<String[]> getPosDataList(){
        return posDataList;
    }
    public ArrayList<String[]> getImuDataList(){
        return imuDataList;
    }
    private void calcIMUData(){
        int total = 0;
        int max = 0;
        for (String[] data : imuDataList){
            int x = Integer.parseInt(data[2]);
            total += x;
            if (abs(x) > abs(max)){
                max = x;
            }
        }
        int avg = total/imuDataList.size();
        this.avgIncline = avg;
        this.maxIncline = max;
    }
    public float getAvgSOG(){
        return avgSOG;
    }

    public float getTopSOG(){
        return topSOG;
    }
    public int getAvgDrift(){
        return avgDrift;
    }
    public int getTotalDrift(){
        return totalDrift;
    }
    public int getMaxIncline(){
        return maxIncline;
    }
    public int getAvgIncline(){
        return avgIncline;
    }
    public int getAvgPressure(){
        return avgPressure;
    }
    public int getMaxPressure(){
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
        avgSOG = total/sogDataList.size();
        topSOG = max;
    }

    private void calcPressureData(){
        int total = 0;
        int max = 0;
        for (String[] data : pressureDataList){
            int x = Integer.parseInt(data[2]);
            total += x;
            if (abs(x) > abs(max)){
                max = x;
            }
        }
        maxPressure = max;
        avgPressure = total/pressureDataList.size();
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
}
