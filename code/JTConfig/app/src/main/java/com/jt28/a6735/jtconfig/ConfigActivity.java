package com.jt28.a6735.jtconfig;

import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCharacteristic;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.os.Message;
import android.support.annotation.RequiresApi;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.widget.Button;
import android.widget.Spinner;

import com.clj.fastble.BleManager;
import com.clj.fastble.conn.BleCharacterCallback;
import com.clj.fastble.conn.BleGattCallback;
import com.clj.fastble.exception.BleException;
import com.clj.fastble.scan.ListScanCallback;
import com.jt28.a6735.jtconfig.Msg.Msg;
import com.jt28.a6735.jtconfig.adapter.SceneListViewAdapter;
import com.jt28.a6735.jtconfig.bean.GroupInfo;
import com.jt28.a6735.jtconfig.bean.ProductInfo;
import com.jt28.a6735.jtconfig.bean.SceneInfo;
import com.jt28.a6735.jtconfig.widget.SceneableListView;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by a6735 on 2017/6/30.
 */

public class ConfigActivity extends BaseActivity implements SceneListViewAdapter.CheckInterface, View.OnClickListener, SceneListViewAdapter.ModifyCountInterface{
    private Context context;
    private SceneableListView exListView;
    private SceneListViewAdapter scenelist;
    private List<SceneInfo> groups = new ArrayList<>();// 组元素数据列表
    private Map<String, List<SceneInfo>> children = new HashMap<>();// 子元素数据列表

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_config);
        context = this;
        for (int i = 0; i < 4; i++) {
            groups.add(new SceneInfo(i + "","按钮"+(i+1),false));
            List<SceneInfo> products = new ArrayList<>();
            children.put(groups.get(i).getId(), products);// 将组元素的一个唯一值，这里取Id，作为子元素List的Key
        }
        exListView = (SceneableListView) findViewById(R.id.ConfigListView);
        scenelist = new SceneListViewAdapter(groups, children, this);
        scenelist.setCheckInterface(this);// 关键步骤1,设置复选框接口
        scenelist.setModifyCountInterface(this);// 关键步骤2,设置数量增减接口
        exListView.setAdapter(scenelist);

        for (int i = 0; i < scenelist.getGroupCount(); i++) {
            exListView.expandGroup(i);// 关键步骤3,初始化时，将ExpandableListView以展开的方式呈现
        }

        InitAddArrow();

        Button ble = (Button) findViewById(R.id.config_ble);
        ble.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //蓝牙
                bleManager = new BleManager(context);
                bleManager.enableBluetooth();
                scanAndConnect1();
            }
        });

        Button ble_download = (Button) findViewById(R.id.config_download);
        ble_download.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
//                for (int i = 0; i < groups.size(); i++) {
//                    System.out.println( "lhb" + groups.get(i).getId());
//                    System.out.println( "lhb" + groups.get(i).getName());
//                }
                for(SceneInfo Val : groups) {
                    System.out.println( "lhb" + Val.getId());
                    System.out.println( "lhb" + Val.getName());
                }
                for (String key : children.keySet()) {
                    System.out.println("lhb_s" + key);
                    for(SceneInfo Val : children.get(key)) {
                        System.out.println( "lhb_s" + Val.getId());
                        System.out.println( "lhb_s" + Val.getName());
                    }
                }
            }
        });

        List<GroupInfo> groups = new ArrayList<>();// 组元素数据列表
        Map<String, List<ProductInfo>> children = new HashMap<>();// 子元素数据列表
        EventBus.getDefault().post(new Msg(groups,children));
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Msg event) {
        /* Do something */
        System.out.println("lhb_c" + event.GetGroups());
    };

    private android.app.AlertDialog.Builder builder = null;
    private View view_custom;
    private android.app.AlertDialog alert = null;
    private void InitAddArrow() {
        //初始化Builder
        builder = new android.app.AlertDialog.Builder(context);
        //加载自定义的那个View,同时设置下'
        LayoutInflater inflater = this.getLayoutInflater();
        view_custom = inflater.inflate(R.layout.item_add_config, null, false);
        builder.setView(view_custom);
        builder.setCancelable(false);
        alert = builder.create();
        final Spinner changj = (Spinner)view_custom.findViewById(R.id.item_add_config_spr);
        view_custom.findViewById(R.id.item_add_config_close).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                alert.dismiss();
            }
        });
        view_custom.findViewById(R.id.item_add_config_ok).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                children.get(groups.get(grour).getId()).add(new SceneInfo(1+"",changj.getSelectedItem().toString(),false));
                for (int i = 0; i < scenelist.getGroupCount(); i++) {
                    exListView.expandGroup(i);// 关键步骤3,初始化时，将ExpandableListView以展开的方式呈现
                }
                scenelist.notifyDataSetChanged();
                alert.dismiss();
            }
        });
    }

    @Override
    public void onClick(View view) {

    }

    @Override
    public void checkGroup(int groupPosition, boolean isChecked) {

    }

    @Override
    public void checkChild(int groupPosition, int childPosition, boolean isChecked) {

    }

    //组添加
    private int grour = 0;
    @Override
    public void checkButAdd(int groupPosition) {
        grour = groupPosition;
        alert.show();
    }

    @Override
    public void doIncrease(int groupPosition, int childPosition, View showCountView, boolean isChecked) {

    }

    @Override
    public void doDecrease(int groupPosition, int childPosition, View showCountView, boolean isChecked) {

    }

    @Override
    public void doChange(int groupPosition, int childPosition, View showCountView) {

    }

    @Override
    public void onStart() {
        super.onStart();
        EventBus.getDefault().register(this);
    }

    @Override
    public void onStop() {
        super.onStop();
        EventBus.getDefault().unregister(this);
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
}
