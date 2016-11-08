package com.example.greeting;

import java.util.List;
import java.util.UUID;
import java.util.ArrayList;

import android.app.Activity;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.IBluetoothManager;
import android.bluetooth.BluetoothHeadset;
import android.bluetooth.BluetoothAssignedNumbers;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.content.Intent;
import android.content.IntentFilter;

public class MainActivity extends Activity implements View.OnClickListener {

	private final static String TAG = "hfp_battery";
	private Button mOkButton = null;
	private BluetoothDevice mBluetoothDevice = null;
	private BluetoothHeadset mBluetoothHeadset = null;
	private BluetoothAdapter mAdapter;

	private final BluetoothProfile.ServiceListener mBluetoothServiceListener =
		new BluetoothProfile.ServiceListener() {
			public void onServiceConnected(int profile, BluetoothProfile proxy) {
				if (profile == BluetoothProfile.HEADSET) {
					mBluetoothHeadset = (BluetoothHeadset) proxy;
					Log.d(TAG, "onServiceConnected to profile: " + profile);
				}
			}
			public void onServiceDisconnected(int profile) {
				if (profile == BluetoothProfile.HEADSET) {
					Log.d(TAG, "onServiceDisonnected form profile: " + profile);
					mBluetoothHeadset = null;
				}
			}
		};


	final BroadcastReceiver mReceiver = new BroadcastReceiver() {
		@Override
			public void onReceive(Context context, Intent intent) {
				String action = intent.getAction();
				Log.v(TAG, "BroadcastReceiver act = " + action);
				if(action.equals(BluetoothHeadset.ACTION_VENDOR_SPECIFIC_HEADSET_EVENT)){
					String command = intent.getStringExtra(
							BluetoothHeadset.EXTRA_VENDOR_SPECIFIC_HEADSET_EVENT_CMD);
					int commandType = intent.getIntExtra(
							BluetoothHeadset.EXTRA_VENDOR_SPECIFIC_HEADSET_EVENT_CMD_TYPE, 0);
					BluetoothDevice device = intent.getParcelableExtra(
							BluetoothDevice.EXTRA_DEVICE);
					Object[] args = (Object[]) intent.getExtras().get(
							BluetoothHeadset.EXTRA_VENDOR_SPECIFIC_HEADSET_EVENT_ARGS);
					if(command.equals("+IPHONEACCEV") &&
							commandType  == BluetoothHeadset.AT_CMD_TYPE_SET){
						int len = args.length;
						int battery_level = -1;
						if(len > 0){
							for(int i=1;i<len-1;i+=2){
								if((int)args[i] == 1){
									battery_level = (int)args[i+1];
									break;
								}
							}
						}
						if(battery_level >= 0){
							Log.v(TAG, " device:" + device.getAddress() +
									" battery level : " + (battery_level+1) + "/10");
							mOkButton.setText((battery_level+1) + "/10");
						}else{
							mOkButton.setText("failed");
							Log.v(TAG, "read battery level failed");
						}
					}
				}
			}// end onReceive.
	};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mOkButton = (Button)findViewById(R.id.button1);
		mOkButton.setText("-");
		mOkButton.setOnClickListener(this);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		mAdapter = BluetoothAdapter.getDefaultAdapter();
		mAdapter.getProfileProxy(this,
				mBluetoothServiceListener, BluetoothProfile.HEADSET);

        IntentFilter filter = new IntentFilter();
        filter.addAction(BluetoothHeadset.ACTION_VENDOR_SPECIFIC_HEADSET_EVENT);
		filter.addCategory(BluetoothHeadset.VENDOR_SPECIFIC_HEADSET_EVENT_COMPANY_ID_CATEGORY
				+ "." + Integer.toString(BluetoothAssignedNumbers.APPLE));
        registerReceiver(mReceiver, filter);
    }
    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }
    @Override
    public void onStop() {
    	super.onStop();
    }
    @Override
    public void onClick(View v) {
    	switch (v.getId()){
    	case R.id.button1:
    		Log.v(TAG, "onClick");
    	}

    }

}
