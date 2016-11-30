package com.example.bluetooth.le;

import android.app.Service;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.content.Context;
import android.os.Message;
import android.os.HandlerThread;
import android.os.Handler;
import android.content.Intent;
import android.os.Binder;
import android.os.Looper;
import android.os.IBinder;
import android.util.Log;

import java.util.List;
import java.util.UUID;

/**
 * Service for managing connection and data communication with a GATT server hosted on a
 * given Bluetooth LE device.
 */
public class BluetoothLeService extends Service {
    private final static String TAG = "ble_test";

    private BluetoothManager mBluetoothManager;
    private BluetoothAdapter mBluetoothAdapter;
    private String mBluetoothDeviceAddress;
    private BluetoothGatt mBluetoothGatt;
    private int mConnectionState = STATE_DISCONNECTED;
	private HandlerThread mHandlerThread;
	private Handler       mHandler;
	private long startTime, lastTime;
    private int sendIdx;
    private int isClear;
    private final static int PACKAGE_NUMBER = 5000;

    private static final int STATE_DISCONNECTED = 0;
    private static final int STATE_CONNECTING = 1;
    private static final int STATE_CONNECTED = 2;

    public final static String ACTION_GATT_CONNECTED =
            "com.example.bluetooth.le.ACTION_GATT_CONNECTED";
    public final static String ACTION_GATT_DISCONNECTED =
            "com.example.bluetooth.le.ACTION_GATT_DISCONNECTED";
    public final static String ACTION_GATT_SERVICES_DISCOVERED =
            "com.example.bluetooth.le.ACTION_GATT_SERVICES_DISCOVERED";
    public final static String ACTION_DATA_AVAILABLE =
            "com.example.bluetooth.le.ACTION_DATA_AVAILABLE";
    public final static String EXTRA_DATA =
            "com.example.bluetooth.le.EXTRA_DATA";

    private final BluetoothGattCallback mGattCallback = new BluetoothGattCallback() {
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            String intentAction;
            if(status == 0){
                if (newState == BluetoothProfile.STATE_CONNECTED) {
                    intentAction = ACTION_GATT_CONNECTED;
                    mConnectionState = STATE_CONNECTED;
                    Log.i(TAG, "Connected to GATT server.");
                    gatt.refresh();
                    broadcastUpdate(intentAction);
                    // Attempts to discover services after successful connection.
                    Message msg = mHandler.obtainMessage(0, gatt);
                    mHandler.sendMessageDelayed(msg, 1000);
                } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                    intentAction = ACTION_GATT_DISCONNECTED;
                    mConnectionState = STATE_DISCONNECTED;
                    Log.i(TAG, "Disconnected from GATT server.");
                    broadcastUpdate(intentAction);
                }
            }else{
                Log.i(TAG, "connection error");
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
			Log.w(TAG, "onServicesDiscovered received: " + status);
            if (status == BluetoothGatt.GATT_SUCCESS) {
				Message msg = mHandler.obtainMessage(1, gatt);
				mHandler.sendMessage(msg);
            } else {
            }
        }
        @Override
		public void onCharacteristicWrite (BluetoothGatt gatt,
				                BluetoothGattCharacteristic characteristic, int status){
			Log.w(TAG, "onCharacteristicWrite : " + status);
            //test write with response
            if(sendIdx < PACKAGE_NUMBER){
                Message msg = mHandler.obtainMessage(1, gatt);
                mHandler.sendMessage(msg);
            }
		}

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt,
                                         BluetoothGattCharacteristic characteristic,
                                         int status) {
            if (status == BluetoothGatt.GATT_SUCCESS) {
                broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt,
                                            BluetoothGattCharacteristic characteristic) {
            broadcastUpdate(ACTION_DATA_AVAILABLE, characteristic);
        }
    };

    private void broadcastUpdate(final String action) {
        final Intent intent = new Intent(action);
        sendBroadcast(intent);
    }

    private void broadcastUpdate(final String action,
                                 final BluetoothGattCharacteristic characteristic) {
        final Intent intent = new Intent(action);
        sendBroadcast(intent);
    }

    public class LocalBinder extends Binder {
        BluetoothLeService getService() {
            return BluetoothLeService.this;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    @Override
    public boolean onUnbind(Intent intent) {
        close();
        return super.onUnbind(intent);
    }

    private final IBinder mBinder = new LocalBinder();

    /**
     * Initializes a reference to the local Bluetooth adapter.
     *
     * @return Return true if the initialization is successful.
     */

    private class MyHandler extends Handler {
        private MyHandler(Looper looper) {
            super(looper);
        }
        @Override
        public void handleMessage(Message msg) {
            BluetoothGatt gatt = (BluetoothGatt) msg.obj;
            switch (msg.what) {
            case 0:
				gatt = (BluetoothGatt) msg.obj;
                Log.i(TAG, "Attempting to start service discovery:" +
                    gatt.discoverServices());
                isClear = 0;
                break;
            case 1:
				gatt = (BluetoothGatt) msg.obj;
				BluetoothGattService gattService = gatt.getService(
						UUID.fromString("0000fff0-0000-1000-8000-00805f9b34fb"));
				if(gattService == null){
					Log.w(TAG, "Service not found");
					return;
				}else if(gattService.getCharacteristic(
					UUID.fromString("0000fff1-0000-1000-8000-00805f9b34fb")) == null){
					Log.w(TAG, "Characteristic not found");
					return;
				}
				BluetoothGattCharacteristic charac;
				charac = gattService.getCharacteristic(
						UUID.fromString("0000fff1-0000-1000-8000-00805f9b34fb"));
				byte[] data = new byte[20];
                if(isClear < 3){
                    isClear++;
                    data[0] = 0x36; data[1] = 0x36; data[2] = 0x36; data[3] = 0x3;
                    sendIdx = 0;
                }else{
                    if(sendIdx == 0){//start
                        data[0] = 115; data[1] = 116; data[2] = 97; data[3] = 114; data[4] = 116;
                        startTime = System.currentTimeMillis();
                    }else if(sendIdx == PACKAGE_NUMBER-1){//finish
                        data[0] = 102; data[1] = 105; data[2] = 110;
                        data[3] = 105; data[4] = 115; data[5] = 104;
                    }else{
                        for(int i=5;i<+20;i++){data[i]=(byte)(0x30+((i+sendIdx)%10));}

                        data[0] = (byte)(0x30 +((sendIdx/1000) % 10));
                        data[1] = (byte)(0x30 +((sendIdx/100) % 10));
                        data[2] = (byte)(0x30 +((sendIdx/10) % 10));
                        data[3] = (byte)(0x30 +((sendIdx/1) % 10));
                        data[4] = (byte)0x20;
                    }
                    sendIdx++;
                    lastTime = System.currentTimeMillis();
                    Log.w(TAG, "send_idx:"+ sendIdx + "  total: "+sendIdx*20+" Bytes."
                            + " T:"+((lastTime-startTime)/1000)+" s."
                            + " ave:"+(int)(sendIdx*20/((float)(lastTime-startTime)/1000))+" B/s");
                }
				charac.setValue(data);
				charac.setWriteType (BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE);
                boolean res = gatt.writeCharacteristic(charac);
                if(!res){
                    Log.w(TAG, "writeCharacteristic failed, send_idx:"+ sendIdx);
                }
                if(sendIdx >= PACKAGE_NUMBER){
                    Log.w(TAG, "send done");
                }else{
                    //test write without response (unsafe)
                    //Message new_msg = mHandler.obtainMessage(1, gatt);
                    //mHandler.sendMessageDelayed(new_msg, 6);
                }
                break;
            case 886:
                mHandler.removeMessages(0);
                mHandler.removeMessages(1);
                //getLooper().quit();
            default:
                break;
            }
        }
    }

    public boolean initialize() {
        // For API level 18 and above, get a reference to BluetoothAdapter through
        // BluetoothManager.
        if (mBluetoothManager == null) {
            mBluetoothManager = (BluetoothManager) getSystemService(Context.BLUETOOTH_SERVICE);
            if (mBluetoothManager == null) {
                Log.e(TAG, "Unable to initialize BluetoothManager.");
                return false;
            }
        }

        mBluetoothAdapter = mBluetoothManager.getAdapter();
        if (mBluetoothAdapter == null) {
            Log.e(TAG, "Unable to obtain a BluetoothAdapter.");
            return false;
        }

        if(mHandlerThread == null){
            mHandlerThread = new HandlerThread("handler");
            mHandlerThread.start();
        }
		mHandler = new MyHandler(mHandlerThread.getLooper());
        return true;
    }
    public boolean connect(final String address) {
        if (mBluetoothAdapter == null || address == null) {
            Log.w(TAG, "BluetoothAdapter not initialized or unspecified address.");
            return false;
        }
        final BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
        if (device == null) {
            Log.w(TAG, "Device not found.  Unable to connect.");
            return false;
        }
        mBluetoothGatt = device.connectGatt(this, false, mGattCallback);
        mBluetoothDeviceAddress = address;
        mConnectionState = STATE_CONNECTING;
        return true;
    }
    public void disconnect() {
        if (mBluetoothAdapter == null || mBluetoothGatt == null) {
            Log.w(TAG, "BluetoothAdapter not initialized");
            return;
        }
        mBluetoothGatt.disconnect();
        mHandler.sendEmptyMessage(886);
    }
    public void close() {
        if (mBluetoothGatt == null) {
            return;
        }
        mBluetoothGatt.close();
        mBluetoothGatt = null;
    }
}
