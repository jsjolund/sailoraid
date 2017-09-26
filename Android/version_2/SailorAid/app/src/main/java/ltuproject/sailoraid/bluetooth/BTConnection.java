package ltuproject.sailoraid.bluetooth;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.util.Log;
import android.widget.TextView;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

import ltuproject.sailoraid.R;

/**
 * Created by Henrik on 2017-09-12.
 */

public class BTConnection extends Thread {
    private final BluetoothSocket mmSocket;
    private final BluetoothDevice mmDevice;
    private static final String TAG = "BTConnect";
    private BluetoothAdapter btAdapter;
    public BTConnection(BluetoothDevice device, BluetoothAdapter btAdapter) {
        // Use a temporary object that is later assigned to mmSocket
        // because mmSocket is final.
        this.btAdapter = btAdapter;
        BluetoothSocket tmp = null;
        mmDevice = device;
        //ParcelUuid[] supportedUuids = device.getUuids();

        try {
            // Get a BluetoothSocket to connect with the given BluetoothDevice.
            // MY_UUID is the app's UUID string, also used in the server code.
            tmp = device.createInsecureRfcommSocketToServiceRecord(UUID.fromString("19B10001-E8F2-537E-4F6C-D104768A1214"));
            //tmp = device.createRfcommSocketToServiceRecord(UUID.randomUUID());
        } catch (IOException e) {
            Log.e(TAG, "Socket's create() method failed", e);
        }
        mmSocket = tmp;
        run();
    }

    public void run() {
        // Cancel discovery because it otherwise slows down the connection.
        if(btAdapter.isDiscovering()){
            btAdapter.cancelDiscovery();
        }
        try {
            // Connect to the remote device through the socket. This call blocks
            // until it succeeds or throws an exception.
            mmSocket.connect();
        } catch (IOException connectException) {
            // Unable to connect; close the socket and return.
            try {
                mmSocket.close();
            } catch (IOException closeException) {
                Log.e(TAG, "Could not close the client socket", closeException);
            }
            return;
        }

        // The connection attempt succeeded. Perform work associated with
        // the connection in a separate thread.
        //manageMyConnectedSocket(mmSocket);

    }
    // Closes the client socket and causes the thread to finish.
    public void cancel() {
        try {
            mmSocket.close();
        } catch (IOException e) {
            Log.e(TAG, "Could not close the client socket", e);
        }
    }
}
