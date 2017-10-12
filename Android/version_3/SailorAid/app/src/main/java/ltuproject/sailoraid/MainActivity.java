package ltuproject.sailoraid;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanResult;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.media.Image;
import android.os.Handler;
import android.os.IBinder;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.support.v7.widget.LinearLayoutCompat;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.TextView;

import org.w3c.dom.Text;

import java.util.List;

import ltuproject.sailoraid.bluetooth.BTHandler;
import ltuproject.sailoraid.bluetooth.BTLEConnection;

import static ltuproject.sailoraid.bluetooth.BTLEConnection.STATE_CONNECTED;
import static ltuproject.sailoraid.bluetooth.BTLEConnection.STATE_DISCONNECTED;

public class MainActivity extends AppCompatActivity {
    private final static String TAG = MainActivity.class.getSimpleName();

    private static final int REQUEST_ENABLE_BT = 1;
    private static final int REQUEST_COARSE_LOCATION = 999;

    private AlertDialog.Builder popDialog;
    private AlertDialog alertpop;
    private ArrayAdapter<String> BTArrayAdapter;
    private BTHandler myBTHandler;
    private BluetoothDevice chosenDevice;
    private boolean hasPermission;
    private BluetoothGatt mBluetoothGatt;
    private BTLEConnection mBluetoothLeService;
    private IntentFilter filter;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        this.requestWindowFeature(Window.FEATURE_NO_TITLE);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);

        Button feedbackviewbtn = (Button)findViewById(R.id.feedbackviewbtn);
        Button bluetoothbtn = (Button)findViewById(R.id.btconbtn);
        Button bluetoothdiscbtn = (Button)findViewById(R.id.btdisconbtn);
        Button historybtn = (Button) findViewById(R.id.historyviewbtn);

        // Button to access the feedback view
        assert feedbackviewbtn != null;
        feedbackviewbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // feedback button function
                //mBluetoothLeService.disconnect();
                startActivity(new Intent(MainActivity.this,FeedbackActivity.class));
            }
        });

        assert bluetoothdiscbtn != null;
        bluetoothdiscbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // stat view button function
                if (mBluetoothLeService != null){
                    mBluetoothLeService.disconnect();
                }
            }
        });

        assert bluetoothbtn != null;
        bluetoothbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // stat view button function
                initBTConn();
            }
        });

        assert historybtn != null;
        historybtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // stat view button function
                startActivity(new Intent(MainActivity.this,HistoryActivity.class));
            }
        });
    }

    // Code to manage Service lifecycle.
    private final ServiceConnection mServiceConnection = new ServiceConnection() {

        @Override
        public void onServiceConnected(ComponentName componentName, IBinder service) {
            mBluetoothLeService = ((BTLEConnection.LocalBinder) service).getService();
            if (!mBluetoothLeService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }
            mBluetoothLeService.setmBluetoothDevice(chosenDevice);
            Intent gattServiceIntent = new Intent(getApplicationContext(), BTLEConnection.class);
            startService(gattServiceIntent);
        }

        @Override
        public void onServiceDisconnected(ComponentName componentName) {
            mBluetoothLeService = null;
        }
    };

    public void initBTConn(){
        myBTHandler = new BTHandler(this);
        if(!myBTHandler.getBtAdapter().isEnabled()) {
            turnOn();
        }
        checkLocationPermission();
        if(hasPermission){
            //cleanPop();
            setPopDialog();
            BTArrayAdapter.clear();
            alertpop = popDialog.show();
            myBTHandler.clearLeList();
            myBTHandler.scanLeDevice(mLeScanCallback, true);
        }
    }

    @Override
    public void onDestroy(){
        super.onDestroy();
        Intent gattServiceIntent = new Intent(getApplicationContext(), BTLEConnection.class);
        this.unregisterReceiver(mGattUpdateReceiver);
        stopService(gattServiceIntent);
    }

    @Override
    public void onResume(){
        super.onResume();
        filter = new IntentFilter(BTLEConnection.ACTION_GATT_CONNECTED);
        filter.addAction(BTLEConnection.ACTION_GATT_DISCONNECTED);
        this.registerReceiver(mGattUpdateReceiver, filter);
        if (mBluetoothLeService != null){
            setConnectionButtons(mBluetoothLeService.getConnectionStatus());
        }
    }

    @Override
    public void onPause(){
        super.onPause();
        this.unregisterReceiver(mGattUpdateReceiver);
    }
    /*
    Popup when searching for bluetooth devices
     */
    public void setPopDialog() {

        BTArrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1);
        LayoutInflater inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        View Viewlayout = inflater.inflate(R.layout.bt_list, (ViewGroup) findViewById(R.id.bt_list));
        ListView myListView = (ListView) Viewlayout.findViewById(R.id.BTList);
        myListView.setAdapter(BTArrayAdapter);
        myListView.setOnItemClickListener(new AdapterView.OnItemClickListener()
        {
            public void onItemClick(AdapterView<?> parent, View view,int position, long id) {
                //Highlight chosen item on the popup list
                view.setBackgroundColor(Color.LTGRAY);
                String device = parent.getAdapter().getItem(position).toString();
                String lines[] = device.split("[\\r\\n]+");
                String deviceName = lines[0];
                String deviceAddress = lines[1];
                chosenDevice = myBTHandler.getDevice(deviceName, deviceAddress);
            }
        });
        popDialog = new AlertDialog.Builder(this);
        popDialog.setView(Viewlayout);
        popDialog.setTitle("Devices");
        popDialog.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int id) {
                        dialog.dismiss();
                    }
                }
        );
        popDialog.setPositiveButton("Connect",
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                        if (chosenDevice != null){
                            Intent gattServiceIntent = new Intent(getApplicationContext(), BTLEConnection.class);
                            bindService(gattServiceIntent, mServiceConnection, BIND_AUTO_CREATE);
                        }
                    }
                });
        popDialog.create();
    }

    /*
   Updates the adapter that is on the Listview displayed in the popup window with lists of found devices
    */
    public void updateDevicesShown() {
        BTArrayAdapter.clear();
        for (BluetoothDevice device : myBTHandler.getLeDeviceList()){
            BTArrayAdapter.add(device.getName()+ "\n" + device.getAddress());
        }

    }

    public void cleanPop(){
        if (popDialog != null){
            popDialog = null;
        }
        if (alertpop != null) {
            alertpop.dismiss();
            alertpop = null;
        }
    }

    public void turnOn(){
        Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        this.startActivityForResult(turnOn, REQUEST_ENABLE_BT);
        while(!myBTHandler.getBtAdapter().isEnabled()){
        }
    }

    public void turnOff(){
        myBTHandler.getBtAdapter().disable();
    }

    private final BroadcastReceiver mGattUpdateReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            final String action = intent.getAction();
            if (mBluetoothLeService.ACTION_GATT_CONNECTED.equals(action)) {
                setConnectionButtons(mBluetoothLeService.getConnectionStatus());
            }else if (mBluetoothLeService.ACTION_GATT_DISCONNECTED.equals(action)) {
                setConnectionButtons(mBluetoothLeService.getConnectionStatus());
                Intent gattServiceIntent = new Intent(getApplicationContext(), BTLEConnection.class);
                stopService(gattServiceIntent);
                unbindService(mServiceConnection);
                //MenuItem item = (MenuItem) findViewById(R.id.boat_connection);
                //item.setIcon(getDrawable(R.drawable.pico_disconnected));
            }
        }
    };

    private void setConnectionButtons(int connected){
        TextView tv = (TextView) findViewById(R.id.mainConText);
        Button btnCon = (Button) findViewById(R.id.btconbtn);
        Button btnDis = (Button) findViewById(R.id.btdisconbtn);
        //LinearLayout ivCon = (LinearLayout) findViewById(R.id.main_connection_holder);
        if (connected == STATE_CONNECTED){
            btnCon.setVisibility(View.GONE);
            btnDis.setVisibility(View.VISIBLE);
            tv.setText(R.string.connected);
            //ivCon.setBackground(getDrawable(R.drawable.main_pico_connected));

        } else{
            btnCon.setVisibility(View.VISIBLE);
            btnDis.setVisibility(View.GONE);
            tv.setText(R.string.disconnected);
            //ivCon.setBackground(getDrawable(R.drawable.main_pico_disconnected));
        }

    }
    /*
    Check permissions during runtime for bluetooth discovery
     */
    protected void checkLocationPermission() {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.ACCESS_COARSE_LOCATION},
                    REQUEST_COARSE_LOCATION);
        }
        else{
            hasPermission = true;
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case REQUEST_COARSE_LOCATION: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    hasPermission = true;
                } else {
                    hasPermission = false;
                }
                break;
            }
        }
    }

    /*
    Get results from Bluetooth LE device scan and adds to list on BTHandler and updates adapter so the item will be displayed on the popup window
     */
    private ScanCallback mLeScanCallback = new ScanCallback() {
        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            super.onScanResult(callbackType, result);
            BluetoothDevice bd = result.getDevice();
            if(!myBTHandler.deviceExists(myBTHandler.getLeDeviceList(), bd)){
                myBTHandler.addLeDeviceList(bd);
                BTArrayAdapter.add(bd.getName()+ "\n" + bd.getAddress());
                BTArrayAdapter.notifyDataSetChanged();
            }
            else{
                BTArrayAdapter.notifyDataSetChanged();
            }
        }

        @Override
        public void onBatchScanResults(List<ScanResult> results) {
            super.onBatchScanResults(results);
        }

        @Override
        public void onScanFailed(int errorCode) {
            super.onScanFailed(errorCode);
        }
    };
}
