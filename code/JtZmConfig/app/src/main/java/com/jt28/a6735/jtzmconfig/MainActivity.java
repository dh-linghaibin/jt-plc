package com.jt28.a6735.jtzmconfig;

import android.Manifest;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.location.LocationManager;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.provider.Settings;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.dd.CircularProgressButton;
import com.jt28.a6735.ble.BleController;
import com.jt28.a6735.ble.callback.ConnectCallback;
import com.jt28.a6735.ble.callback.OnReceiverCallback;
import com.jt28.a6735.ble.callback.OnWriteCallback;
import com.jt28.a6735.ble.callback.ScanCallback;
import com.jt28.a6735.jtzmconfig.adapter.viewpager.SlideViewPager;
import com.jt28.a6735.jtzmconfig.fragment.Demo1Fragment;
import com.jt28.a6735.jtzmconfig.fragment.Demo2Fragment;
import com.jt28.a6735.jtzmconfig.fragment.Demo3Fragment;
import com.jt28.a6735.jtzmconfig.fragment.Demo4Fragment;
import com.jt28.a6735.jtzmconfig.model.FileLs;
import com.jt28.a6735.jtzmconfig.model.PlcControl;
import com.jt28.a6735.jtzmconfig.model.PlcControlchildn;
import com.jt28.a6735.jtzmconfig.model.PlcTimer;
import com.jt28.a6735.jtzmconfig.model.Scene;
import com.jt28.a6735.jtzmconfig.model.Scenechild;
import com.jt28.a6735.jtzmconfig.msg.Msgcoir_groups;
import com.jt28.a6735.jtzmconfig.msg.Msgscenr_groups;
import com.jt28.a6735.jtzmconfig.msg.Msgtime_groups;
import com.jt28.a6735.jtzmconfig.msg.Notice;
import com.jt28.a6735.jtzmconfig.msg.downloadmsg;
import com.jt28.a6735.share.Share;
import com.jt28.a6735.viewpager.MyFragmentPagerAdapter;
import com.jt28.a6735.viewpager.PageFragment;

import org.angmarch.views.NiceSpinner;
import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.io.File;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;

import pub.devrel.easypermissions.EasyPermissions;

public class MainActivity extends AppCompatActivity implements EasyPermissions.PermissionCallbacks{
    private String TAG = "JT128";

    private List<FileLs> Filelist = new ArrayList<>();
    private CircularProgressButton ble_en;
    private CircularProgressButton ble_key_download;
    private CircularProgressButton ble_timer_download;
    private CircularProgressButton ble_sence_download;
    private TextView show_message;


    // gps是否可用
    public static final boolean isGpsEnable(final Context context) {
        LocationManager locationManager
                = (LocationManager) context.getSystemService(Context.LOCATION_SERVICE);
        boolean gps = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER);
        boolean network = locationManager.isProviderEnabled(LocationManager.NETWORK_PROVIDER);
        if (gps || network) {
            return true;
        }
        return false;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //requestBluetoothPermission();

        //Android6.0需要动态申请权限
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION)
                != PackageManager.PERMISSION_GRANTED) {
            //请求权限
            ActivityCompat.requestPermissions(this,
                    new String[]{Manifest.permission.ACCESS_COARSE_LOCATION,
                            Manifest.permission.ACCESS_FINE_LOCATION},
                    3);
            if (ActivityCompat.shouldShowRequestPermissionRationale(this,
                    Manifest.permission.ACCESS_COARSE_LOCATION)) {
                //判断是否跟用户做一个说明
                // DialogUtils.shortT(getApplicationContext(), "需要蓝牙权限");
            }
        }

        if(isGpsEnable(this)) {
            initBle();
        } else {
            //跳转到gps设置页
            Intent intent = new Intent(Settings.ACTION_LOCATION_SOURCE_SETTINGS);
            startActivityForResult(intent, 0);
        }

        initTool();
        initViewPage();
        initFile();
    }
    private void initTool() {
        final LinearLayout toolgrout = (LinearLayout)findViewById(R.id.main_toolgroup);
        final Button tool = (Button) findViewById(R.id.main_open_tool);
        tool.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(toolgrout.getVisibility() == View.GONE) {
                    toolgrout.setVisibility(View.VISIBLE);
                    tool.setText("隐藏");
                } else {
                    toolgrout.setVisibility(View.GONE);
                    tool.setText("展开");
                }
            }
        });
    }
    private void initViewPage() {
        ArrayList<PageFragment> listFragment = new ArrayList<PageFragment>();
        PageFragment oneFragment = new Demo1Fragment();
        PageFragment twoFragment = new Demo2Fragment();
        PageFragment threeFragment = new Demo3Fragment();
        PageFragment fourFragment = new Demo4Fragment();
        listFragment.add(oneFragment);
        listFragment.add(twoFragment);
        listFragment.add(threeFragment);
        listFragment.add(fourFragment);

        MyFragmentPagerAdapter myFragmentPagerAdapter = new MyFragmentPagerAdapter(getSupportFragmentManager(), listFragment);
        SlideViewPager viewPager = (SlideViewPager) findViewById(R.id.frame_main);
        viewPager.setAdapter(myFragmentPagerAdapter);
        viewPager.setOffscreenPageLimit(4);
        viewPager.setCurrentItem(0);
//        viewPager.setOnPageChangeListener(new ViewPager.OnPageChangeListener() {
//            @Override
//            public void onPageSelected(int arg0) {
//
//            }
//            @Override
//            public void onPageScrolled(int arg0, float arg1, int arg2) {
//
//            }
//            @Override
//            public void onPageScrollStateChanged(int arg0) {
//
//            }
//        });
    }
    private void initFile() {
        initProname();
        show_message = (TextView) findViewById(R.id.main_show_messige);
        show_message.setTextColor(0xff00BCD4);
        show_message.setText("新的工程");
        final EditText file_name = (EditText) findViewById(R.id.main_file_name);
        Button file_save = (Button) findViewById(R.id.main_file_save);
        Button file_del = (Button) findViewById(R.id.main_file_del);
        final NiceSpinner niceSpinner = (NiceSpinner) findViewById(R.id.main_file_spinner);
        niceSpinner.attachDataSource(filelist);
        niceSpinner.setOnItemSelectedListener(new AdapterView.OnItemSelectedListener() {
            @Override
            public void onItemSelected(AdapterView<?> adapterView, View view, final int i, long l) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        if(i > 0) {
                            Log.d(TAG, "选择" + Filelist.get(i - 1).getName());
                            getFile(Filelist.get(i - 1).getUrl());
                            show_message.setTextColor(0xffd06ce4);
                            show_message.setText("工程打开成功");
                            file_name.setVisibility(View.GONE);
                        } else {
                            file_name.setVisibility(View.VISIBLE);
                        }
                    }
                }).run();
            }

            @Override
            public void onNothingSelected(AdapterView<?> adapterView) {
            }
        });
        Log.d(TAG, String.valueOf(filelist.size()));
        file_save.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        //说明保存新文件
                        if(niceSpinner.getSelectedIndex() == 0 ) {
                            if("".equals(file_name.getText().toString().trim())) {
                                Toast.makeText(getApplicationContext(), "工程名不能为空", Toast.LENGTH_LONG).show();
                            } else {
                                Log.d(TAG,"保存"+file_name.getText().toString());
                                saveDate(file_name.getText().toString());
                                //同步工程框
                                initProname();
                                niceSpinner.attachDataSource(filelist);
                                niceSpinner.setSelectedIndex(filelist.size()-1);
                                //显示区域
                                show_message.setTextColor(0xff00BCD4);
                                show_message.setText("工程已保存");
                                file_name.setVisibility(View.GONE);
                            }
                        } else {
                            //覆盖保存
                            saveDate(Filelist.get(niceSpinner.getSelectedIndex() - 1).getName());
                            Log.d(TAG,"保存"+Filelist.get(niceSpinner.getSelectedIndex() - 1).getName());
                            show_message.setTextColor(0xff00BCD4);
                            show_message.setText("工程已保存");
                        }
                    }
                }).run();
            }
        });
        file_del.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        //清除工程
                        if(niceSpinner.getSelectedIndex() == 0 ) {
                            coir_groups.clear();
                            coir_children.clear();
                            scenr_groups.clear();
                            scene_children.clear();
                            time_groups.clear();
                            time_children.clear();
                            EventBus.getDefault().post(new Msgcoir_groups(coir_groups,coir_children));
                            EventBus.getDefault().post(new Msgscenr_groups(scenr_groups,scene_children));
                            EventBus.getDefault().post(new Msgtime_groups(time_groups,time_children));
                            //显示区域
                            show_message.setTextColor(0xff00BCD4);
                            show_message.setText("清除工程成功");
                            file_name.setVisibility(View.VISIBLE);
                        } else {
                            //删除工程
                            File file = new File(Environment.getExternalStorageDirectory().toString()
                                    + File.separator + "AJtZmConfig/"
                                    +Filelist.get(niceSpinner.getSelectedIndex() - 1).getName());
                            if (!file.exists()) {
                                file.mkdirs();
                            }
                            deleteFile(file);
                            //同步工程框
                            initProname();
                            niceSpinner.setSelectedIndex(0);
                            //显示区域
                            show_message.setTextColor(0xff00BCD4);
                            show_message.setText("删除工程成功");

                            coir_groups.clear();
                            coir_children.clear();
                            scenr_groups.clear();
                            scene_children.clear();
                            time_groups.clear();
                            time_children.clear();
                            EventBus.getDefault().post(new Msgcoir_groups(coir_groups,coir_children));
                            EventBus.getDefault().post(new Msgscenr_groups(scenr_groups,scene_children));
                            EventBus.getDefault().post(new Msgtime_groups(time_groups,time_children));
                            file_name.setVisibility(View.VISIBLE);
                        }
                    }
                }).run();
            }
        });
    }

    //蓝牙
    private BleController mBleController;
    //搜索结果列表
    private List<BluetoothDevice> bluetoothDevices = new ArrayList<BluetoothDevice>();

    private void scanDevices() {
        mBleController.scanBle(0, new ScanCallback() {
            @Override
            public void onSuccess() {
                if (bluetoothDevices.size() > 0) {
                    for(BluetoothDevice tmp: bluetoothDevices) {
                        if(tmp.getName().equals("LHB")) {
                            // TODO 第三步：点击条目后,获取地址，根据地址连接设备
                            String address = tmp.getAddress();
                            mBleController.connect(0, address, new ConnectCallback() {
                                @Override
                                public void onConnSuccess() {
                                    Toast.makeText(MainActivity.this, "连接成功", Toast.LENGTH_SHORT).show();
                                    // TODO 在新的界面要获取实例，无需init
                                    //mBleController = BleController.getInstance();
                                    // TODO 接收数据的监听
                                    mBleController.registReciveListener("MainActivity", new OnReceiverCallback() {
                                        @Override
                                        public void onRecive(byte[] value) {

                                        }
                                    });
                                    Message message = new Message();
                                    message.what = 2;
                                    mHandler.sendMessage(message);
                                }
                                @Override
                                public void onConnFailed() {
                                    Toast.makeText(MainActivity.this, "连接超时，请重试", Toast.LENGTH_SHORT).show();
                                    Message message = new Message();
                                    message.what = 3;
                                    mHandler.sendMessage(message);
                                }
                            });
                            return;
                        }
                        Message message = new Message();
                        message.what = 3;
                        mHandler.sendMessage(message);
                    }
                } else {
                    Toast.makeText(MainActivity.this, "未搜索到Ble设备", Toast.LENGTH_SHORT).show();
                    Message message = new Message();
                    message.what = 3;
                    mHandler.sendMessage(message);
                }
            }

            @Override
            public void onScanning(BluetoothDevice device, int rssi, byte[] scanRecord) {
                if (!bluetoothDevices.contains(device)) {
                    bluetoothDevices.add(device);
                }
            }
        });
    }


    private void initBle() {
        ///蓝牙
        // TODO  第一步：初始化
//        mBleController = BleController.getInstance().init(this);

        ble_en = (CircularProgressButton) findViewById(R.id.main_ble_en);
        ble_en.setIndeterminateProgressMode(true);
        ble_en.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (bel_num != 100) {
                    Message message = new Message();
                    message.what = 1;
                    mHandler.sendMessage(message);
                    // TODO  第二步：搜索设备，获取列表后进行展示
                    scanDevices();
//                    bleManager.scanNameAndConnect(
//                            DEVICE_NAME,
//                            TIME_OUT,
//                            false,
//                            new BleGattCallback() {
//                                @Override
//                                public void onNotFoundDevice() {
//                                    Log.i(TAG, "未发现设备");
//                                    Message message = new Message();
//                                    message.what = 3;
//                                    mHandler.sendMessage(message);
//                                }
//
//                                @Override
//                                public void onFoundDevice(com.clj.fastble.data.ScanResult scanResult) {
//                                    Log.i(TAG, "发现设备: " + scanResult.getDevice().getAddress());
//                                    Message message = new Message();
//                                    message.what = 1;
//                                    mHandler.sendMessage(message);
//                                }
//
//                                @RequiresApi(api = Build.VERSION_CODES.JELLY_BEAN_MR2)
//                                @Override
//                                public void onConnectSuccess(BluetoothGatt gatt, int status) {
//                                    gatt.discoverServices();
//                                    Log.i(TAG, "连接成功");
//                                }
//
//                                @Override
//                                public void onServicesDiscovered(BluetoothGatt gatt, int status) {
//                                    Log.i(TAG, "发现服务");
//                                    Message message = new Message();
//                                    message.what = 2;
//                                    mHandler.sendMessage(message);
//                                }
//
//                                @Override
//                                public void onConnectFailure(BleException exception) {
//                                    Log.i(TAG, "连接中断：" + exception.toString());
//                                    Message message = new Message();
//                                    message.what = 3;
//                                    mHandler.sendMessage(message);
//                                }
//                            });
                }
            }
        });
        ble_key_download = (CircularProgressButton) findViewById(R.id.main_key_download);
        ble_key_download.setIndeterminateProgressMode(true);
        ble_key_download.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(ble_key_download_num != 50) {
                    Message message = new Message();
                    message.what = 4;
                    mHandler.sendMessage(message);
                    EventBus.getDefault().post(new Notice("download_but"));
                }
            }
        });
        ble_timer_download = (CircularProgressButton) findViewById(R.id.main_timer_download);
        ble_timer_download.setIndeterminateProgressMode(true);
        ble_timer_download.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(ble_timer_download_num != 50) {
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            Message message = new Message();
                            message.what = 7;
                            mHandler.sendMessage(message);
                            EventBus.getDefault().post(new Notice("download_time"));
                        }
                    }).run();
                }
            }
        });
        ble_sence_download = (CircularProgressButton) findViewById(R.id.main_sence_download);
        ble_sence_download.setIndeterminateProgressMode(true);
        ble_sence_download.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                EventBus.getDefault().post(new Notice("sence_time"));
            }
        });
    }

    // 遍历接收一个文件路径，然后把文件子目录中的所有文件遍历并输出来
    private void getAllFilesDr(File root){
        File files[] = root.listFiles();
        if(files != null){
            for (File f : files){
                if(f.isDirectory()){
                    Log.d(TAG,f.getName());
                    Filelist.add(new FileLs(f.getName(),f.getPath()));
                }
            }
        }
    }
    private void getAllFiles(File root){
        File files[] = root.listFiles();
        if(files != null){
            for (File f : files){
                if(f.isDirectory()){
                    if(f.getName().matches(".*AJtZmConfig.*")) {
                        Filelist.clear();
                        getAllFilesDr(f);
                    }
                }
            }
        }
    }
    public void deleteFile(File file) {
        if (file.exists()) { // 判断文件是否存在
            if (file.isFile()) { // 判断是否是文件
                file.delete(); // delete()方法 你应该知道 是删除的意思;
            } else if (file.isDirectory()) { // 否则如果它是一个目录
                File files[] = file.listFiles(); // 声明目录下所有的文件 files[];
                for (int i = 0; i < files.length; i++) { // 遍历目录下所有的文件
                    this.deleteFile(files[i]); // 把每个文件 用这个方法进行迭代
                }
            }
            file.delete();
        } else {
           // Constants.Logdada("文件不存在！"+"\n");
        }
    }
    private List<String> filelist = new LinkedList<>();
    private void initProname() {
        Filelist.clear();
        //检测SD卡是否存在
        File path;
        if (Environment.getExternalStorageState().equals(
                Environment.MEDIA_MOUNTED)) {
            path = Environment.getExternalStorageDirectory() ;
            getAllFiles(path);
        }else{
            Toast.makeText(this, "没有SD卡", Toast.LENGTH_LONG).show();
        }
        filelist.clear();
        filelist.add("新的工程");
        for (FileLs tmp:Filelist) {
            filelist.add(tmp.getName());
        }
    }
    //程序变量
    private List<PlcControl> coir_groups = new ArrayList<>();
    private List<List<PlcControlchildn>> coir_children = new ArrayList<>();

    private List<Scene> scenr_groups = new ArrayList<>();
    private List<List<Scenechild>> scene_children = new ArrayList<>();

    private List<PlcTimer> time_groups = new ArrayList<>();
    private List<List<Scenechild>> time_children = new ArrayList<>();
    //获取文件
    private void getFile(String url) {
        File file = new File(url);
        if (!file.exists()) {
            file.mkdirs();
        }
        Share.init("CACHE", 10 * 1024, file.toString());
        coir_groups = (List<PlcControl>)Share.getObject("coir_groups");
        coir_children = (List<List<PlcControlchildn>>)Share.getObject("coir_children");

        scenr_groups = (List<Scene>)Share.getObject("scenr_groups");
        scene_children = (List<List<Scenechild>>)Share.getObject("scene_children");

        time_groups = (List<PlcTimer>)Share.getObject("time_groups");
        time_children = (List<List<Scenechild>>)Share.getObject("time_children");

        if(coir_groups == null) {
            coir_groups = new ArrayList<>();
        }
        if(coir_children == null) {
            coir_children = new ArrayList<>();
        }
        if(scenr_groups == null) {
            scenr_groups = new ArrayList<>();
        }
        if(scene_children == null) {
            scene_children = new ArrayList<>();
        }
        if(time_groups == null) {
            time_groups = new ArrayList<>();
        }
        if(time_children == null) {
            time_children = new ArrayList<>();
        }

        EventBus.getDefault().post(new Msgcoir_groups(coir_groups,coir_children));
        EventBus.getDefault().post(new Msgscenr_groups(scenr_groups,scene_children));
        EventBus.getDefault().post(new Msgtime_groups(time_groups,time_children));
    }
    //保存文件
    private void saveDate(String name) {
        File file = new File(Environment.getExternalStorageDirectory().toString() + File.separator + "AJtZmConfig/"+name);
        if (!file.exists()) {
            file.mkdirs();
        }
        Share.init("CACHE", 10 * 1024, file.toString());
        Share.putObject("coir_groups",coir_groups);
        Share.putObject("coir_children",coir_children);

        Share.putObject("scenr_groups",scenr_groups);
        Share.putObject("scene_children",scene_children);

        Share.putObject("time_groups",time_groups);
        Share.putObject("time_children",time_children);
    }


    //蓝牙发送队列
    private Queue<byte[]> queue = new LinkedList<>();

    private void Send(final String flag) {
        if(queue.size() >= 1) {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    mBleController.writeBuffer( queue.poll(), new OnWriteCallback() {
                        @Override
                        public void onSuccess() {
                            Log.d(TAG,"发送成功");
                            try {
                                Thread.sleep(40);
                            } catch (InterruptedException e) {
                                return;
                            }
                            Send(flag);
                        }
                        @Override
                        public void onFailed(int state) {
                            queue.clear();
                            Log.d(TAG,"发送失败");
                            if(flag.equals("download_but")) {
                                Message message = new Message();
                                message.what = 6;
                                mHandler.sendMessage(message);
                            } else if(flag.equals("download_time")) {
                                Message message = new Message();
                                message.what = 9;
                                mHandler.sendMessage(message);
                            }
                        }
                    });
                }
            }).run();
        } else {
            if(flag.equals("download_but")) {
                if (true) {
                    Message message = new Message();
                    message.what = 5;
                    mHandler.sendMessage(message);
                } else {
                    Message message = new Message();
                    message.what = 6;
                    mHandler.sendMessage(message);
                }
            } else if(flag.equals("download_time")) {
                if (true) {
                    Message message = new Message();
                    message.what = 8;
                    mHandler.sendMessage(message);
                } else {
                    Message message = new Message();
                    message.what = 9;
                    mHandler.sendMessage(message);
                }
            } else if(flag.equals("counter")) {
                if (true) {
                    Message message = new Message();
                    message.what = 10;
                    mHandler.sendMessage(message);
                } else {
                    Message message = new Message();
                    message.what = 11;
                    mHandler.sendMessage(message);
                }
            } else if(flag.equals("uptime")) {
                if (true) {
                    Message message = new Message();
                    message.what = 12;
                    mHandler.sendMessage(message);
                } else {
                    Message message = new Message();
                    message.what = 13;
                    mHandler.sendMessage(message);
                }
            }
        }
    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(final downloadmsg event) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                //为dataList赋值，将下面这些数据添加到数据源中
                byte[] button_ll = new byte[event.getDownload().size()*8];//byte数组;
                int i=  0;
                for(byte[] tmp:event.getDownload()) {
                    for(int i_j = 0;i_j < 8;i_j++) {
                        button_ll[i] = tmp[i_j];
                        i++;
                    }
                }
                Log.d(TAG, Arrays.toString(button_ll) );

                queue.clear();

                for(byte[] tmp:event.getDownload()) {
                    queue.add(tmp);
                }
                Send(event.getFlag());
            }
        }).run();
    };
    //接收数据更新
    //场景
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Msgcoir_groups event) {
        coir_groups = event.getCoir_groups();
        coir_children = event.getCoir_children();
        Log.d(TAG,"更新coir_groups" + coir_groups);

        show_message.setTextColor(0xffF44336);
        show_message.setText("工程有修改，请保存");
    }
    //按钮
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Msgscenr_groups event) {
        scenr_groups = event.getScenr_groups();
        scene_children = event.getScene_children();
        Log.d(TAG,"更新scenr_groups" + scenr_groups);

        show_message.setTextColor(0xffF44336);
        show_message.setText("工程有修改，请保存");
    }
    //时间
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Msgtime_groups event) {
        time_groups = event.getTime_groups();
        time_children = event.getTime_children();
        Log.d(TAG,"更新time_groups" + time_groups);

        show_message.setTextColor(0xffF44336);
        show_message.setText("工程有修改，请保存");
    }
    private int bel_num = 0;
    private int ble_key_download_num = 0;
    private int ble_timer_download_num = 0;
    public Handler mHandler=new Handler() {
        public void handleMessage(Message msg) {
            switch(msg.what) {
                case 1:
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            ble_en.setProgress(0);
                            ble_en.setProgress(50);
                            bel_num = 50;
                            Log.i(TAG, "设置 50");
                        }
                    }).run();

                    break;
                case 2:
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            if (ble_en.getProgress() != 100) {
                                ble_en.setProgress(0);
                                ble_en.setProgress(100);
                                bel_num = 100;
                                Log.i(TAG, "设置 100");
                            }
                        }
                    }).run();

                    break;
                case 3:
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            if (ble_en.getProgress() != -1) {
                                ble_en.setProgress(0);
                                ble_en.setProgress(-1);
                                bel_num = -1;
                                Log.i(TAG, "设置 -1");
                            }
                        }
                    }).run();

                    break;
                case 4:
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            if (ble_key_download.getProgress() != 50) {
                                ble_key_download.setProgress(0);
                                ble_key_download.setProgress(50);
                                ble_key_download_num = 50;
                                Log.i(TAG, "设置 50");
                            }
                        }
                    }).run();

                    break;
                case 5:
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            if (ble_key_download.getProgress() != 100) {
                                ble_key_download.setProgress(0);
                                ble_key_download.setProgress(100);
                                ble_key_download_num = 0;
                                Log.i(TAG, "设置 100");
                            }
                        }
                    }).run();

                    break;
                case 6:
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            if (ble_key_download.getProgress() != -1) {
                                ble_key_download.setProgress(0);
                                ble_key_download.setProgress(-1);
                                ble_key_download_num = -1;
                                Log.i(TAG, "设置 -1");
                            }
                        }
                    }).run();

                    break;
                case 7:
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            if (ble_timer_download.getProgress() != 50) {
                                ble_timer_download.setProgress(0);
                                ble_timer_download.setProgress(50);
                                ble_timer_download_num = 50;
                                Log.i(TAG, "设置 50");
                            }
                        }
                    }).run();

                    break;
                case 8:
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            if (ble_timer_download.getProgress() != 100) {
                                ble_timer_download.setProgress(0);
                                ble_timer_download.setProgress(100);
                                ble_timer_download_num = 0;
                                Log.i(TAG, "设置 100");
                            }
                        }
                    }).run();

                    break;
                case 9:
                    new Thread(new Runnable() {
                        @Override
                        public void run() {
                            if (ble_timer_download.getProgress() != -1) {
                                ble_timer_download.setProgress(0);
                                ble_timer_download.setProgress(-1);
                                ble_timer_download_num = -1;
                                Log.i(TAG, "设置 -1");
                            }
                        }
                    }).run();
                    break;
                case 10:
                    show_message.setTextColor(0xff00BCD4);
                    show_message.setText("线圈控制成功");
                    break;
                case 11:
                    show_message.setTextColor(0xffF44336);
                    show_message.setText("线圈控制失败,请连接蓝牙！");
                    break;
                case 12:
                    show_message.setTextColor(0xff00BCD4);
                    show_message.setText("时间更新成功");
                    break;
                case 13:
                    show_message.setTextColor(0xffF44336);
                    show_message.setText("时间更新失败,请连接蓝牙！");
                    break;
                default:
                    break;
            }
            super.handleMessage(msg);
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
        requestBluetoothPermission();
    }

    private void requestBluetoothPermission() {
        String[] perms = {
//                Manifest.permission.CAMERA,
                Manifest.permission.WRITE_EXTERNAL_STORAGE,
                Manifest.permission.READ_EXTERNAL_STORAGE,
                Manifest.permission.BLUETOOTH,
                Manifest.permission.BLUETOOTH_ADMIN,
                Manifest.permission.GET_TASKS,
                Manifest.permission.READ_CALL_LOG,
                Manifest.permission.READ_PHONE_STATE,
                Manifest.permission.READ_CONTACTS,
                Manifest.permission.RECEIVE_SMS,
                Manifest.permission.READ_SMS,
                Manifest.permission.GET_ACCOUNTS,
                Manifest.permission.WRITE_CONTACTS,
                Manifest.permission.ACCESS_COARSE_LOCATION,
                Manifest.permission.ACCESS_FINE_LOCATION
        };
        // Do not have permissions, request them now
        if (EasyPermissions.hasPermissions(this, perms)) {
            // Already have permission, do the thing
            Log.d(TAG,"有权限");
//            bleManager = new BleManager(this);
//            bleManager.enableBluetooth();
            // TODO  第一步：初始化
            mBleController = BleController.getInstance().init(this);
        } else
            try {
                EasyPermissions.requestPermissions(this, getResources().getString(R.string.rationale_local), 25, perms);
            } catch (Exception e) {
                Log.e("requesting exception", "" + e.getCause() + "" + e.getMessage());
            }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        EasyPermissions.onRequestPermissionsResult(requestCode, permissions, grantResults, this);
    }

    @Override
    public void onPermissionsGranted(int requestCode, List<String> perms) {
        if (perms.size() >= 6) {
            Log.d(TAG,"有权限");
//            bleManager = new BleManager(this);
//            bleManager.enableBluetooth();
            // TODO  第一步：初始化
            mBleController = BleController.getInstance().init(this);
        }
    }

    @Override
    public void onPermissionsDenied(int requestCode, List<String> perms) {

    }
}
