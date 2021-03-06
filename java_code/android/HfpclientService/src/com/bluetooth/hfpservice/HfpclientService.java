package com.bluetooth.hfpclient;

import java.util.List;
import java.util.UUID;
import java.util.ArrayList;

import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.IBluetoothManager;
import android.bluetooth.BluetoothHeadsetClient;
import android.bluetooth.BluetoothHeadsetClientCall;
import android.bluetooth.BluetoothAssignedNumbers;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.os.Bundle;
import android.os.HandlerThread;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.content.Intent;
import android.content.IntentFilter;
import android.app.Service;
import android.os.IBinder;
import android.os.Looper;

public class HfpclientService extends Service {
    private static final String TAG = "hfp_service";
	private BluetoothDevice mBluetoothDevice = null;
	private BluetoothHeadsetClient mBluetoothHeadsetClient = null;
	private BluetoothAdapter mAdapter;
    private HandlerThread mHandlerThread;
    private Handler mHandler;

	private static final int HANDLE_MSG_CALL_CHANGED = 1;
	private static final int HANDLE_MSG_CALL_CHUP    = 2;

	@Override
	public IBinder onBind(Intent intent) {
		return null;
	}
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {
		return super.onStartCommand(intent, flags, startId);
	}
    @Override
    public void onCreate() {
		init();
        super.onCreate();
	}
    @Override
    public void onDestroy() {
        super.onDestroy();
	}

	private final BluetoothProfile.ServiceListener mBluetoothServiceListener =
		new BluetoothProfile.ServiceListener() {
			public void onServiceConnected(int profile, BluetoothProfile proxy) {
				if (profile == BluetoothProfile.HEADSET_CLIENT) {
					mBluetoothHeadsetClient = (BluetoothHeadsetClient) proxy;
					Log.d(TAG, "onServiceConnected to profile: " + profile);
				}
			}
			public void onServiceDisconnected(int profile) {
				if (profile == BluetoothProfile.HEADSET_CLIENT) {
					Log.d(TAG, "onServiceDisonnected form profile: " + profile);
					mBluetoothHeadsetClient = null;
				}
			}
		};

	final BroadcastReceiver mReceiver = new BroadcastReceiver() {
		@Override
			public void onReceive(Context context, Intent intent) {
				int ps, cs;
				String action = intent.getAction();
				if(action.equals(
							BluetoothHeadsetClient.ACTION_CONNECTION_STATE_CHANGED)){
					Log.v(TAG, "=== ACTION_CONNECTION_STATE_CHANGED ===");
					ps = intent.getIntExtra(BluetoothProfile.EXTRA_PREVIOUS_STATE, -1);
					cs = intent.getIntExtra(BluetoothProfile.EXTRA_STATE, -1);
					Log.v(TAG, "connected(2) connecting(1) disconnected(0)");
					Log.v(TAG, "status: " + ps + "->" + cs);
				}else if(action.equals(
							BluetoothHeadsetClient.ACTION_AUDIO_STATE_CHANGED)){
					Log.v(TAG, "=== ACTION_AUDIO_STATE_CHANGED ===");
					Log.v(TAG, "connected(2) connecting(1) disconnected(0)");
					ps = intent.getIntExtra(BluetoothProfile.EXTRA_PREVIOUS_STATE, -1);
					cs = intent.getIntExtra(BluetoothProfile.EXTRA_STATE, -1);
					Log.v(TAG, "status: " + ps + "->" + cs);
				}else if(action.equals(
							BluetoothHeadsetClient.ACTION_CALL_CHANGED)){
					Log.v(TAG, "=== ACTION_CALL_CHANGED ===");
					BluetoothHeadsetClientCall call = (BluetoothHeadsetClientCall)
						intent.getParcelableExtra(BluetoothHeadsetClient.EXTRA_CALL);
					Message msg = mHandler.obtainMessage(HANDLE_MSG_CALL_CHANGED, call);
					mHandler.sendMessage(msg);
					Log.v(TAG,
							" ACTIVE(0)  " + " HELD(1)    "
							+ " DIALING(2) " + " ALERTING(3)");
					Log.v(TAG,
							" INCOMING(4)" + " WAITING(5) "
							+ " H_B_HOLD(6)" + " TERMINATED(7)");

					Log.v(TAG, "id="+call.getId()+" st="+call.getState()
							+" num="+call.getNumber()
							+" isMulti="+call.isMultiParty()
							+" isOut="+call.isOutgoing());
				}else if(action.equals(
							BluetoothHeadsetClient.ACTION_AG_EVENT)){
					int tmp;
					Log.v(TAG, "=== ACTION_AG_EVENT ===");
					tmp = intent.getIntExtra(
							BluetoothHeadsetClient.EXTRA_NETWORK_STATUS,-255);
					if(tmp != -255){
						Log.v(TAG, "NETWORK_STATUS="+tmp);
					}
					tmp = intent.getIntExtra(
							BluetoothHeadsetClient.EXTRA_NETWORK_SIGNAL_STRENGTH,-255);
					if(tmp != -255){
						Log.v(TAG, "NETWORK_SIGNAL="+tmp);
					}
					tmp = intent.getIntExtra(
							BluetoothHeadsetClient.EXTRA_NETWORK_ROAMING,-255);
					if(tmp != -255){
						Log.v(TAG, "NETWORK_ROAMING="+tmp);
					}
					tmp = intent.getIntExtra(
							BluetoothHeadsetClient.EXTRA_BATTERY_LEVEL,-255);
					if(tmp != -255){
						Log.v(TAG, "BATTERY_LEVEL="+tmp);
					}
					String name = intent.getStringExtra(
							BluetoothHeadsetClient.EXTRA_OPERATOR_NAME);
					if(name != null){
						Log.v(TAG, "OPERATOR_NAME="+name);
					}
					tmp = intent.getIntExtra(
							BluetoothHeadsetClient.EXTRA_VOICE_RECOGNITION,-255);
					if(tmp != -255){
						Log.v(TAG, "VOICE_RECOGNITION="+tmp);
					}
					tmp = intent.getIntExtra(
							BluetoothHeadsetClient.EXTRA_IN_BAND_RING,-255);
					if(tmp != -255){
						Log.v(TAG, "IN_BAND_RING="+tmp);
					}
				}else{
					Log.v(TAG, "Unhandled BroadcastReceiver act = " + action);
				}
			}// end onReceive.
	};

    private class MyHandler extends Handler {
        private MyHandler(Looper looper) {
            super(looper);
        }
        @Override
        public void handleMessage(Message msg) {
			BluetoothHeadsetClientCall call;
			List<BluetoothDevice> devices;
            switch (msg.what) {
				case HANDLE_MSG_CALL_CHANGED :
					call = (BluetoothHeadsetClientCall)msg.obj;
					if(call != null){
						if(call.getState() == 4){
							//mHandler.sendEmptyMessage(HANDLE_MSG_CALL_CHANGED);
							mHandler.sendEmptyMessageDelayed(HANDLE_MSG_CALL_CHUP, 5000);
							if(mBluetoothHeadsetClient != null){
								devices = mBluetoothHeadsetClient.getConnectedDevices();
								Log.v(TAG, "device num:" + devices.size());
								for (BluetoothDevice dev : devices) {
									Log.v(TAG, "device:" + dev.getAddress());
								}
							}
						}
					}
					break;
				case HANDLE_MSG_CALL_CHUP :
					mHandler.removeMessages(HANDLE_MSG_CALL_CHUP);
					if(mBluetoothHeadsetClient != null){
						devices = mBluetoothHeadsetClient.getConnectedDevices();
						if(devices != null){
							for (BluetoothDevice dev : devices) {
								Log.v(TAG, "chup device:" + dev.getAddress());
								mBluetoothHeadsetClient.rejectCall(dev);
							}
						}
					}
					break;
                default:
                    break;
            }
        }
    }

    void init() {
		Log.v(TAG, "init hfp_status");
		mAdapter = BluetoothAdapter.getDefaultAdapter();
		mAdapter.getProfileProxy(this,
				mBluetoothServiceListener, BluetoothProfile.HEADSET_CLIENT);

		mHandlerThread = new HandlerThread("ancs_gatt_client");
		mHandlerThread.start();
		mHandler = new MyHandler(mHandlerThread.getLooper());

        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothHeadsetClient.ACTION_CONNECTION_STATE_CHANGED);
        filter.addAction(BluetoothHeadsetClient.ACTION_AUDIO_STATE_CHANGED);
        filter.addAction(BluetoothHeadsetClient.ACTION_CALL_CHANGED);
        filter.addAction(BluetoothHeadsetClient.ACTION_RESULT);
        filter.addAction(BluetoothHeadsetClient.ACTION_AG_EVENT);
        registerReceiver(mReceiver, filter);

		mAdapter.setScanMode(mAdapter.SCAN_MODE_CONNECTABLE_DISCOVERABLE, 0);
	}
}
