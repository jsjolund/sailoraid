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
import java.util.Calendar;
import java.util.Date;
import java.util.EmptyStackException;

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

    public SailLog(Context context){
        fileName = new SimpleDateFormat("yyyyMMdd_HHmmss").format(new Date()) +".txt";
        this.context = context;
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
            outPut = "<" +data +">";
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

    public String readLog(){
        String text = "";
        String line;
            /* Reads from the text file */

        try{
            fis = new FileInputStream(outFile);
            isr = new InputStreamReader(fis);
            br = new BufferedReader(isr);
            while((line = br.readLine()) != null){
                text += line;
            }
            br.close();
            isr.close();
            fis.close();
        }catch (IOException e){
            e.printStackTrace();
        }
        return text;
    }
}
