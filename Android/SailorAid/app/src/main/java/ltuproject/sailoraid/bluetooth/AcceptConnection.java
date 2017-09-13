package ltuproject.sailoraid.bluetooth;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothServerSocket;
import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.util.Log;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.UUID;

/**
 * Created by Henrik on 2017-09-12.
 */

public class AcceptConnection extends Thread {
    private final BluetoothServerSocket mmServerSocket;
    private static final String TAG = "BTConnect";
    private String NAME = "Harbor";
    private OutputStream outputStream;
    public InputStream inStream;
    private byte[] readBuffer;

    public AcceptConnection(BluetoothAdapter btAdapter) {
        // Use a temporary object that is later assigned to mmServerSocket
        // because mmServerSocket is final.
        BluetoothServerSocket tmp = null;
        try {
            // MY_UUID is the app's UUID string, also used by the client code.
            tmp = btAdapter.listenUsingRfcommWithServiceRecord(NAME, UUID.fromString("00001101-0000-1000-8000-00805F9B34FB"));
        } catch (IOException e) {
            Log.e(TAG, "Socket's listen() method failed", e);
        }
        mmServerSocket = tmp;
    }

    public void run() {
        BluetoothSocket socket = null;
        // Keep listening until exception occurs or a socket is returned.
        while (true) {
            try {
                socket = mmServerSocket.accept();
                outputStream = socket.getOutputStream();
                inStream = socket.getInputStream();
                beginListenForData();
            } catch (IOException e) {
                Log.e(TAG, "Socket's accept() method failed", e);
                break;
            }

            if (socket != null) {
                // A connection was accepted. Perform work associated with
                // the connection in a separate thread.
                try {
                    mmServerSocket.close();
                } catch (IOException e) {
                    Log.e(TAG, "Socket's close() method failed", e);
                    break;
                }

                break;
            }
        }
    }

    // Closes the connect socket and causes the thread to finish.
    public void cancel() {
        try {
            mmServerSocket.close();
        } catch (IOException e) {
            Log.e(TAG, "Could not close the connect socket", e);
        }
    }

    void beginListenForData() {
        final Handler handler = new Handler();
        final byte delimiter = 10; //This is the ASCII code for a newline character

        int readBufferPosition = 0;
        readBuffer = new byte[1024];
        final int message_size = 62;
        Thread workerThread = new Thread(new Runnable() {
            public void run() {
                while (!Thread.currentThread().isInterrupted()) {
                    try {
                        int bytesAvailable = inStream.available();
                        if (bytesAvailable >= message_size) {
                            inStream.read(readBuffer, 0, message_size);
                            final byte[] encodedBytes = new byte[message_size];
                            System.arraycopy(readBuffer, 0, encodedBytes, 0, message_size);
                            final String data = new String(encodedBytes, "US-ASCII");
                            int readBufferPosition = 0;

                            handler.post(new Runnable() {
                                public void run() {

                                }
                            });
                        }
                    } catch (IOException e) {
                        Log.e(TAG, "Could not close the connect socket", e);
                    }
                }
            }
        });

        workerThread.start();
    }
}