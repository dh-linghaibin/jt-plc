package com.jt28.a6735.jtconfig;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.os.Build;
import android.os.Bundle;
import android.os.Message;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.clj.fastble.BleManager;
import com.clj.fastble.conn.BleCharacterCallback;
import com.clj.fastble.conn.BleGattCallback;
import com.clj.fastble.exception.BleException;
import com.clj.fastble.scan.ListScanCallback;
import com.jt28.a6735.jtconfig.Msg.CmdMsg;
import com.jt28.a6735.jtconfig.Msg.Msg;
import com.jt28.a6735.jtconfig.Msg.downloadmsg;
import com.jt28.a6735.jtconfig.fragment.KeyConfigFragment;
import com.jt28.a6735.jtconfig.fragment.SceneConfigFragment;
import com.jt28.a6735.viewpager.MyFragmentPagerAdapter;
import com.jt28.a6735.viewpager.MyViewPager;
import com.jt28.a6735.viewpager.PageFragment;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;
import java.util.Arrays;

/**
 * Created by a6735 on 2017/7/4.
 */

public class NewActivity extends BaseActivity {
    private MyViewPager viewPager;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.newactivity);
        intiFragment();
        Button qiehuan = (Button) findViewById(R.id.newactivity_switch);
        qiehuan.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(viewPager.getCurrentItem() == 0) {
                    EventBus.getDefault().post(new CmdMsg("GetScene"));
                    viewPager.setCurrentItem(1);
                } else {
                    viewPager.setCurrentItem(0);
                }
            }
        });
        Button blene = (Button) findViewById(R.id.newactivity_blenen);
        blene.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //蓝牙
                bleManager = new BleManager(NewActivity.this);
                bleManager.enableBluetooth();
                scanAndConnect1();
            }
        });
        Button download = (Button) findViewById(R.id.newactivity_download);
        download.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                EventBus.getDefault().post(new CmdMsg("getpro"));
            }
        });
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Msg event) {
        //为dataList赋值，将下面这些数据添加到数据源中
    };

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(downloadmsg event) {
        //为dataList赋值，将下面这些数据添加到数据源中
        byte[] button_ll = new byte[event.getDownload().size()*8];//byte数组;
        int i=  0;
        for(byte[] tmp:event.getDownload()) {
            for(int i_j = 0;i_j < 8;i_j++) {
                button_ll[i] = tmp[i_j];
                i++;
            }
        }
        System.out.println("lhb_接收到2:   " + Arrays.toString(button_ll));

        for(byte[] tmp:event.getDownload()) {

            bleManager.writeDevice(
                    UUID_SERVICE,
                    UUID_WRITE,
                    tmp,
                    new BleCharacterCallback() {
                        @Override
                        public void onSuccess(BluetoothGattCharacteristic characteristic) {
                            Log.i(TAG, "发送成功!");
                        }

                        @Override
                        public void onFailure(BleException exception) {

                        }
                    });
        }
    };

    //Fragment view 初始化
    private void intiFragment() {
        ArrayList<PageFragment> listFragment = new ArrayList<PageFragment>();
        PageFragment oneFragment = new SceneConfigFragment();
        PageFragment twoFragment = new KeyConfigFragment();
//        PageFragment threeFragment = new MainRecAuditNoSub();
//        PageFragment fourFragment = new MainRecAuditOkSub();
        listFragment.add(oneFragment);
        listFragment.add(twoFragment);
//        listFragment.add(threeFragment);
//        listFragment.add(fourFragment);

        MyFragmentPagerAdapter myFragmentPagerAdapter = new MyFragmentPagerAdapter(getSupportFragmentManager(), listFragment);
        viewPager = (MyViewPager) findViewById(R.id.newactivity_viewpager);
        viewPager.setAdapter(myFragmentPagerAdapter);
        viewPager.setOffscreenPageLimit(1);
        viewPager.setCurrentItem(0);
        viewPager.setScrollble(false);
    }


    private static final String TAG = "MainActivity";
    // 下面的所有UUID及指令请根据实际设备替换
    private static final String UUID_SERVICE = "0000ffe5-0000-1000-8000-00805f9b34fb";
    private static final String UUID_SERVICE2 = "0000ffe0-0000-1000-8000-00805f9b34fb";
    private static final String UUID_INDICATE = "0000ffe4-0000-1000-8000-00805f9b34fb";
    private static final String UUID_NOTIFY = "00000000-0000-0000-8000-00805f9b0000";
    private static final String UUID_WRITE = "0000ffe9-0000-1000-8000-00805f9b34fb";
    private static final String UUID_READ = "0000ffe4-0000-1000-8000-00805f9b34fb";
    private byte[] send_data = new byte[11];//byte数组;
    private static final long TIME_OUT = 10000;                                          // 扫描超时时间
    private static final String DEVICE_NAME = "LHB";                         // 符合连接规则的蓝牙设备名
    private static final String[] DEVICE_NAMES = new String[]{};                        // 符合连接规则的蓝牙设备名
    private static final String DEVICE_MAC = "这里写你的设备地址";                        // 符合连接规则的蓝牙设备地址

    private BleManager bleManager;// Ble核心管理类
    private com.clj.fastble.data.ScanResult scanResult;
    /**
     * 判断是否支持ble
     */
    private boolean isSupportBle() {
        return bleManager.isSupportBle();
    }

    /**
     * 手动开启蓝牙
     */
    private void enableBlue() {
        bleManager.enableBluetooth();
    }

    /**
     * 手动关闭蓝牙
     */
    private void disableBlue() {
        bleManager.disableBluetooth();
    }

    /**
     * 刷新缓存操作
     */
    private void refersh() {
        bleManager.refreshDeviceCache();
    }

    /**
     * 关闭操作
     */
    private void close() {
        bleManager.closeBluetoothGatt();
    }

    /**
     * 扫描出周围所有设备
     */
    private void scanDevice() {
        bleManager.scanDevice(new ListScanCallback(TIME_OUT) {
            @Override
            public void onScanning(com.clj.fastble.data.ScanResult result) {
                scanResult = result;
            }

            @Override
            public void onScanComplete(com.clj.fastble.data.ScanResult[] results) {

            }
        });
    }

    /**
     * 当搜索到周围有设备之后，可以选择直接连某一个设备
     */
    private void connectDevice() {
        bleManager.connectDevice(scanResult, true, new BleGattCallback() {
            @Override
            public void onNotFoundDevice() {
                Log.i(TAG, "未发现设备");
            }

            @Override
            public void onFoundDevice(com.clj.fastble.data.ScanResult scanResult) {
                Log.i(TAG, "发现设备: " + scanResult.getDevice().getAddress());
            }

            @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
            @Override
            public void onConnectSuccess(BluetoothGatt gatt, int status) {
                Log.i(TAG, "连接成功");
                gatt.discoverServices();
            }

            @Override
            public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                Log.i(TAG, "发现服务");
                bleManager.getBluetoothState();
            }

            @Override
            public void onConnectFailure(BleException exception) {
                Log.i(TAG, "连接断开：" + exception.toString());
                bleManager.handleException(exception);
            }
        });
    }

    /**
     * 扫描指定广播名的设备，并连接（唯一广播名）
     */
    private boolean s_flag = false;
    private void scanAndConnect1() {
        bleManager.scanNameAndConnect(
                DEVICE_NAME,
                TIME_OUT,
                false,
                new BleGattCallback() {
                    @Override
                    public void onNotFoundDevice() {
                        Log.i(TAG, "未发现设备");
                        text_msg = "未发现设备";
                        Message message=new Message();
                        message.what=1;
                        handler.sendMessage(message);
                    }

                    @Override
                    public void onFoundDevice(com.clj.fastble.data.ScanResult scanResult) {
                        Log.i(TAG, "发现设备: " + scanResult.getDevice().getAddress());
                        text_msg = "发现设备";
                        Message message=new Message();
                        message.what=1;
                        handler.sendMessage(message);
                    }

                    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
                    @Override
                    public void onConnectSuccess(BluetoothGatt gatt, int status) {
                        gatt.discoverServices();
                        Log.i(TAG, "连接成功");
                        text_msg = "连接成功";
                        Message message=new Message();
                        message.what=1;
                        handler.sendMessage(message);
                    }

                    @Override
                    public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                        Log.i(TAG, "发现服务");
                        s_flag = true;
                        text_msg = "发现服务";
                        Message message=new Message();
                        message.what=1;
                        handler.sendMessage(message);
                    }

                    @Override
                    public void onConnectFailure(BleException exception) {
                        Log.i(TAG, "连接中断：" + exception.toString());
                        s_flag = false;
                        text_msg = "连接中断";
                        Message message=new Message();
                        message.what=1;
                        handler.sendMessage(message);
                    }

                });
    }

    /**
     * 扫描指定广播名的设备，并连接（模糊广播名）
     */
    private void scanAndConnect2() {
        bleManager.scanfuzzyNameAndConnect(
                DEVICE_NAME,
                TIME_OUT,
                false,
                new BleGattCallback() {
                    @Override
                    public void onNotFoundDevice() {
                        Log.i(TAG, "未发现设备");
                    }

                    @Override
                    public void onFoundDevice(com.clj.fastble.data.ScanResult scanResult) {
                        Log.i(TAG, "发现设备: " + scanResult.getDevice().getAddress());
                    }

                    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
                    @Override
                    public void onConnectSuccess(BluetoothGatt gatt, int status) {
                        gatt.discoverServices();
                        Log.i(TAG, "连接成功");
                    }

                    @Override
                    public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                        Log.i(TAG, "发现服务");
                    }

                    @Override
                    public void onConnectFailure(BleException exception) {
                        Log.i(TAG, "连接中断：" + exception.toString());
                    }
                });
    }

    /**
     * 扫描指定广播名的设备，并连接（多个广播名）
     */
    private void scanAndConnect3() {
        bleManager.scanNamesAndConnect(
                DEVICE_NAMES,
                TIME_OUT,
                false,
                new BleGattCallback() {
                    @Override
                    public void onNotFoundDevice() {
                        Log.i(TAG, "未发现设备");
                    }

                    @Override
                    public void onFoundDevice(com.clj.fastble.data.ScanResult scanResult) {
                        Log.i(TAG, "发现设备: " + scanResult.getDevice().getAddress());
                    }

                    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
                    @Override
                    public void onConnectSuccess(BluetoothGatt gatt, int status) {
                        gatt.discoverServices();
                        Log.i(TAG, "连接成功");
                    }

                    @Override
                    public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                        Log.i(TAG, "发现服务");
                    }

                    @Override
                    public void onConnectFailure(BleException exception) {
                        Log.i(TAG, "连接中断：" + exception.toString());
                    }
                });
    }

    /**
     * 扫描指定广播名的设备，并连接（模糊、多个广播名）
     */
    private void scanAndConnect4() {
        bleManager.scanfuzzyNamesAndConnect(
                DEVICE_NAMES,
                TIME_OUT,
                false,
                new BleGattCallback() {
                    @Override
                    public void onNotFoundDevice() {
                        Log.i(TAG, "未发现设备");
                    }

                    @Override
                    public void onFoundDevice(com.clj.fastble.data.ScanResult scanResult) {
                        Log.i(TAG, "发现设备: " + scanResult.getDevice().getAddress());
                    }

                    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
                    @Override
                    public void onConnectSuccess(BluetoothGatt gatt, int status) {
                        gatt.discoverServices();
                        Log.i(TAG, "连接成功");
                    }

                    @Override
                    public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                        Log.i(TAG, "发现服务");
                    }

                    @Override
                    public void onConnectFailure(BleException exception) {
                        Log.i(TAG, "连接中断：" + exception.toString());
                    }
                });
    }

    /**
     * 扫描指定物理地址的设备，并连接
     */
    private void scanAndConnect5() {
        bleManager.scanMacAndConnect(
                DEVICE_MAC,
                TIME_OUT,
                false,
                new BleGattCallback() {
                    @Override
                    public void onNotFoundDevice() {
                        Log.i(TAG, "未发现设备");
                    }

                    @Override
                    public void onFoundDevice(com.clj.fastble.data.ScanResult scanResult) {
                        Log.i(TAG, "发现设备: " + scanResult.getDevice().getAddress());
                    }

                    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
                    @Override
                    public void onConnectSuccess(BluetoothGatt gatt, int status) {
                        gatt.discoverServices();
                        Log.i(TAG, "连接成功");
                    }

                    @Override
                    public void onServicesDiscovered(BluetoothGatt gatt, int status) {
                        Log.i(TAG, "发现服务");
                    }

                    @Override
                    public void onConnectFailure(BleException exception) {
                        Log.i(TAG, "连接中断：" + exception.toString());
                    }
                });
    }

    /**
     * 取消搜索
     */
    private void cancelScan() {
        bleManager.cancelScan();
    }

    /**
     * notify
     */
    private void listen_notify() {
        bleManager.notify(
                UUID_SERVICE2,
                UUID_READ,
                new BleCharacterCallback() {
                    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
                    @Override
                    public void onSuccess(BluetoothGattCharacteristic characteristic) {
                        Log.i(TAG, "接受到数据" + characteristic.getStringValue(0));
                        for(int i = 0;i < characteristic.getValue().length;i++) {
                            Log.i(TAG,"" + characteristic.getValue()[i]);
                        }
                        text_msg = "接受到数据" + Arrays.toString(characteristic.getValue());
                        Message message=new Message();
                        message.what=1;
                        handler.sendMessage(message);
                    }

                    @Override
                    public void onFailure(BleException exception) {

                    }
                });
    }

    /**
     * stop notify
     */
    private boolean stop_notify() {
        return bleManager.stopNotify(UUID_SERVICE, UUID_NOTIFY);
    }

    /**
     * indicate
     */
    private void listen_indicate() {
        bleManager.indicate(
                UUID_SERVICE2,
                UUID_READ,
                new BleCharacterCallback() {

                    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
                    @Override
                    public void onSuccess(BluetoothGattCharacteristic characteristic) {
                        Log.i(TAG, "接受到数据" + characteristic.getValue().toString());
                    }

                    @Override
                    public void onFailure(BleException exception) {

                    }
                });
    }

    /**
     * stop indicate
     */
    private boolean stop_indicate() {
        return bleManager.stopIndicate(UUID_SERVICE2, UUID_INDICATE);
    }

    /**
     * write
     */
    private void write() {
        bleManager.writeDevice(
                UUID_SERVICE,
                UUID_WRITE,
                send_data,
                new BleCharacterCallback() {
                    @Override
                    public void onSuccess(BluetoothGattCharacteristic characteristic) {
                        Log.i(TAG, "发送成功!");
                    }

                    @Override
                    public void onFailure(BleException exception) {

                    }
                });
    }

    /**
     * read
     */
    private void read() {
        bleManager.readDevice(
                UUID_SERVICE2,
                UUID_READ,
                new BleCharacterCallback() {
                    @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
                    @Override
                    public void onSuccess(BluetoothGattCharacteristic characteristic) {
                        Log.i(TAG, "接受到数据" + characteristic.getValue().toString());
                    }

                    @Override
                    public void onFailure(BleException exception) {

                    }
                });
    }

    private String text_msg = "";
    //ui线程
    private android.os.Handler handler = new android.os.Handler() {
        @Override
        public void handleMessage(Message msg) {
            super.handleMessage(msg);
            switch (msg.what) {
                case 0:
                    if(s_flag) {
                        write();
                    }
                    break;
                case 1:
                    break;
                default:
                    break;
            }
        }
    };

    @Override
    public void onStop() {
        super.onStop();
        EventBus.getDefault().unregister(this);
    }

    @Override
    public void onStart() {
        super.onStart();
        EventBus.getDefault().register(this);
    }
}
