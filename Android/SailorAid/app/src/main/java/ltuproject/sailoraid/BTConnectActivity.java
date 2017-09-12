package ltuproject.sailoraid;

import android.Manifest;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.provider.SyncStateContract;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewAnimationUtils;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Set;

public class BTConnectActivity extends AppCompatActivity {

    private static final int REQUEST_ENABLE_BT = 1;
    private static final int REQUEST_COARSE_LOCATION = 999;
    private BluetoothAdapter btAdapter;
    private Button searchbtn, pairedbtn;
    private Set<BluetoothDevice> pairedDevices;
    private ArrayList<BluetoothDevice> newDeviceList;
    private BluetoothDevice newDevice;
    private ListView myListView;
    private ArrayAdapter<String> BTArrayAdapter;
    private IntentFilter filter;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.connect_activity);

        // Register for broadcasts when a device is discovered.
        filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
        this.registerReceiver(mReceiver, filter);
        btAdapter = BluetoothAdapter.getDefaultAdapter();

        pairedbtn = (Button)findViewById(R.id.paireddevicebtn);
        searchbtn = (Button)findViewById(R.id.searchdevicebtn);

        newDeviceList = new ArrayList<>();

        assert pairedbtn != null;
        pairedbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // stat view button function
                if(!btAdapter.isEnabled()) {
                    turnOn();
                }
                showBTDialog();
            }
        });

        assert searchbtn != null;
        searchbtn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                // stat view button function
                if(!btAdapter.isEnabled()) {
                    turnOn();
                }
                //newDeviceList.clear();
                checkLocationPermission();
            }
        });
    }

    public void showBTDialog() {

        final AlertDialog.Builder popDialog = new AlertDialog.Builder(this);
        final LayoutInflater inflater = (LayoutInflater) this.getSystemService(LAYOUT_INFLATER_SERVICE);
        final View Viewlayout = inflater.inflate(R.layout.bt_list, (ViewGroup) findViewById(R.id.bt_list));

        popDialog.setTitle("Bluetooth Devices");
        popDialog.setView(Viewlayout);

        // create the arrayAdapter that contains the BTDevices, and set it to a ListView
        myListView = (ListView) Viewlayout.findViewById(R.id.BTList);
        BTArrayAdapter = new ArrayAdapter<String>(this, android.R.layout.simple_list_item_1);

        myListView.setAdapter(BTArrayAdapter);

        pairedDevices = btAdapter.getBondedDevices();
        // put it's one to the adapter
        for(BluetoothDevice device : pairedDevices)
            BTArrayAdapter.add(device.getName()+ "\n" + device.getAddress());

        if(!newDeviceList.isEmpty()){
            for (BluetoothDevice device : newDeviceList){
                String name = device.getName();
                BTArrayAdapter.add(device.getName()+ "\n" + device.getAddress());
            }
            ((ArrayAdapter) myListView.getAdapter()).notifyDataSetChanged();
        }
        // Button OK
        popDialog.setPositiveButton("Pair",
                new DialogInterface.OnClickListener() {
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();
                    }
                });
        // Create popup and show
        popDialog.create();
        popDialog.show();

    }

    protected void checkLocationPermission() {
        if (ActivityCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {

            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.ACCESS_COARSE_LOCATION},
                    REQUEST_COARSE_LOCATION);
        }
        else{
            btAdapter.startDiscovery();
        }
    }
    @Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case REQUEST_COARSE_LOCATION: {
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    btAdapter.startDiscovery(); // --->
                } else {
                    //TODO re-request
                }
                break;
            }
        }
    }
    // Create a BroadcastReceiver for ACTION_FOUND.
    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {

                String action = intent.getAction();

                // When discovery finds a device
                if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                    // Get the BluetoothDevice object from the Intent
                    BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    // If it's already paired, skip it, because it's been listed already
                    if (device.getBondState() != BluetoothDevice.BOND_BONDED)
                    {
                        boolean exists = false;
                        for (int i=0;i<newDeviceList.size();i++){
                            if (newDeviceList.get(i).equals(device)){
                                exists = true;
                                break;
                            }
                        }
                        if(!exists || newDeviceList.isEmpty()){
                            String name = device.getName();
                            newDeviceList.add(device);
                        }
                        showBTDialog();
                    }
                }
                else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)){
                    btAdapter.cancelDiscovery();
                }
            }
    };

    @Override
    protected void onResume() {
        super.onResume();
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_STARTED);
        this.registerReceiver(mReceiver, filter);
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        this.unregisterReceiver(mReceiver);
    }

    @Override
    protected  void onPause() {
        super.onPause();
        this.unregisterReceiver(mReceiver);
    }

    public void turnOn(){
        Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
        this.startActivityForResult(turnOn, REQUEST_ENABLE_BT);
    }


    public void turnOff(){
        btAdapter.disable();
    }
}