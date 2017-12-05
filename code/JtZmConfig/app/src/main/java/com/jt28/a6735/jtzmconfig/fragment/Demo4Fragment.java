package com.jt28.a6735.jtzmconfig.fragment;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.text.format.Time;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Switch;

import com.jt28.a6735.jtzmconfig.R;
import com.jt28.a6735.jtzmconfig.msg.downloadmsg;
import com.jt28.a6735.viewpager.PageFragment;

import org.greenrobot.eventbus.EventBus;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by a6735 on 2017/7/28.
 */

public class Demo4Fragment extends PageFragment {
    private String TAG = "jt128";
    private List<byte[]> download = new ArrayList<>();
    private Switch coil1;
    private Switch coil2;
    private Switch coil3;
    private Switch coil4;
    private Switch coil5;
    private Switch coil6;
    private Switch coil7;
    private Switch coil8;
    private EditText drvice_address;
    private Button upTime,setsbip,setlyip;
    private EditText sb_ip1,sb_ip2,sb_ip3,sb_ip4;
    private EditText ly_ip1,ly_ip2,ly_ip3,ly_ip4;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_demo4, container, false);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        drvice_address = (EditText) view.findViewById(R.id.demo4_address);
        coil1 = (Switch) view.findViewById(R.id.demo4_coil1);
        coil2 = (Switch) view.findViewById(R.id.demo4_coil2);
        coil3 = (Switch) view.findViewById(R.id.demo4_coil3);
        coil4 = (Switch) view.findViewById(R.id.demo4_coil4);
        coil5 = (Switch) view.findViewById(R.id.demo4_coil5);
        coil6 = (Switch) view.findViewById(R.id.demo4_coil6);
        coil7 = (Switch) view.findViewById(R.id.demo4_coil7);
        coil8 = (Switch) view.findViewById(R.id.demo4_coil8);
        upTime = (Button) view.findViewById(R.id.demo4_uptime);
        setsbip = (Button) view.findViewById(R.id.demo4_setsbip);
        setlyip = (Button) view.findViewById(R.id.demo4_setlyip);
        sb_ip1 = (EditText) view.findViewById(R.id.demo4_sbip1);
        sb_ip2 = (EditText) view.findViewById(R.id.demo4_sbip2);
        sb_ip3 = (EditText) view.findViewById(R.id.demo4_sbip3);
        sb_ip4 = (EditText) view.findViewById(R.id.demo4_sbip4);
        ly_ip1 = (EditText) view.findViewById(R.id.demo4_lyip1);
        ly_ip2 = (EditText) view.findViewById(R.id.demo4_lyip2);
        ly_ip3 = (EditText) view.findViewById(R.id.demo4_lyip3);
        ly_ip4 = (EditText) view.findViewById(R.id.demo4_lyip4);

        upTime.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //更新时间
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        Time t=new Time();// or Time t=new Time("GMT+8"); 加上Time Zone资料
                        t.setToNow();// 取得系统时间。
                        int year = t.year;
                        int month = t.month;
                        int date = t.monthDay;
                        int hour = t.hour; // 0-23
                        int min = t.minute;
                        int sec = t.second;
                        Log.d(TAG,"时间："+(year-2000));
                        Log.d(TAG,"时间："+month);
                        Log.d(TAG,"时间："+date);
                        Log.d(TAG,"时间："+hour);
                        Log.d(TAG,"时间："+min);
                        Log.d(TAG,"时间："+sec);

                        download.clear();
                        byte[] add_lock = new byte[8];//byte数组;
                        add_lock[7] = (byte) 0xcf;
                        add_lock[6] = (byte) 0x00;
                        add_lock[5] = (byte) 0x00;
                        add_lock[4] = (byte) 0x00;
                        add_lock[3] = (byte) 0x00;
                        add_lock[2] = (byte) 254;//网络地址
                        add_lock[1] = (byte) 0x00;
                        add_lock[0] = (byte) 0x00;
                        download.add(add_lock);
                        byte[] add_counter = new byte[8];//byte数组;
                        add_counter[7] = (byte) sec;
                        add_counter[6] = (byte) min;
                        add_counter[5] = (byte) hour;
                        add_counter[4] = (byte) date;
                        add_counter[3] = (byte) month;
                        add_counter[2] = (byte) (year-2000);
                        add_counter[1] = (byte) 0xb1;
                        add_counter[0] = (byte) 0x00;//保留
                        download.add(add_counter);
                        EventBus.getDefault().post(new downloadmsg("uptime", download));
                    }
                }).run();
            }
        });
        setsbip.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                download.clear();
                if( ( "".equals(ly_ip1.getText().toString().trim()) ) &&
                    ( "".equals(ly_ip2.getText().toString().trim()) ) &&
                    ( "".equals(ly_ip3.getText().toString().trim()) ) &&
                    ( "".equals(ly_ip4.getText().toString().trim()) ) ) {

                } else {
                    int ip1 = Integer.valueOf(ly_ip1.getText().toString());
                    int ip2 = Integer.valueOf(ly_ip2.getText().toString());
                    int ip3 = Integer.valueOf(ly_ip3.getText().toString());
                    int ip4 = Integer.valueOf(ly_ip4.getText().toString());

                    byte[] add_lock = new byte[8];//byte数组;
                    add_lock[7] = (byte) 0xcf;
                    add_lock[6] = (byte) 0x00;
                    add_lock[5] = (byte) 0x00;
                    add_lock[4] = (byte) 0x00;
                    add_lock[3] = (byte) 0x00;
                    add_lock[2] = (byte) 254;//网络地址
                    add_lock[1] = (byte) 0x00;
                    add_lock[0] = (byte) 0x00;
                    download.add(add_lock);
                    byte[] ip = new byte[8];//byte数组;
                    ip[7] = (byte) ip4;
                    ip[6] = (byte) ip3;
                    ip[5] = (byte) ip2;
                    ip[4] = (byte) ip1;
                    ip[3] = (byte) 0x00;
                    ip[2] = (byte) 0x00;
                    ip[1] = (byte) 0xb2;
                    ip[0] = (byte) 0x00;//保留
                    download.add(ip);
                    EventBus.getDefault().post(new downloadmsg("lyip", download));
                }
            }
        });
        setlyip.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                download.clear();

                if( ( "".equals(sb_ip1.getText().toString().trim()) ) &&
                    ( "".equals(sb_ip2.getText().toString().trim()) ) &&
                    ( "".equals(sb_ip3.getText().toString().trim()) ) &&
                    ( "".equals(sb_ip4.getText().toString().trim()) ) ) {

                } else {
                    int ip1 = Integer.valueOf(sb_ip1.getText().toString());
                    int ip2 = Integer.valueOf(sb_ip2.getText().toString());
                    int ip3 = Integer.valueOf(sb_ip3.getText().toString());
                    int ip4 = Integer.valueOf(sb_ip4.getText().toString());

                    byte[] add_lock = new byte[8];//byte数组;
                    add_lock[7] = (byte) 0xcf;
                    add_lock[6] = (byte) 0x00;
                    add_lock[5] = (byte) 0x00;
                    add_lock[4] = (byte) 0x00;
                    add_lock[3] = (byte) 0x00;
                    add_lock[2] = (byte) 254;//网络地址
                    add_lock[1] = (byte) 0x00;
                    add_lock[0] = (byte) 0x00;
                    download.add(add_lock);
                    byte[] ip = new byte[8];//byte数组;
                    ip[7] = (byte) ip4;
                    ip[6] = (byte) ip3;
                    ip[5] = (byte) ip2;
                    ip[4] = (byte) ip1;
                    ip[3] = (byte) 0x00;
                    ip[2] = (byte) 0x00;
                    ip[1] = (byte) 0xb3;
                    ip[0] = (byte) 0x00;//保留
                    download.add(ip);
                    EventBus.getDefault().post(new downloadmsg("sbip", download));
                }
            }
        });

        coil1.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(drvice_address.getText().toString().equals("")) {

                } else {
                    int address = Integer.valueOf(drvice_address.getText().toString().trim());
                    download.clear();
                    byte[] add_lock = new byte[8];//byte数组;
                    add_lock[7] = (byte) 0xcf;
                    add_lock[6] = (byte) 0x00;
                    add_lock[5] = (byte) 0x00;
                    add_lock[4] = (byte) 0x00;
                    add_lock[3] = (byte) 0x00;
                    add_lock[2] = (byte) address;
                    add_lock[1] = (byte) 0x00;
                    add_lock[0] = (byte) 0x00;
                    download.add(add_lock);
                    byte[] add_counter = new byte[8];//byte数组;
                    add_counter[7] = (byte) 0x04;
                    add_counter[6] = (byte) 0x04;
                    add_counter[5] = (byte) 0x04;
                    add_counter[4] = (byte) 0x00;
                    add_counter[3] = (byte) 0x00;
                    add_counter[2] = (byte) 0x01;
                    add_counter[1] = (byte) 0x01;
                    add_counter[0] = (byte) 0x01;
                    if (b) {
                        add_counter[4] = (byte) 0x01;
                    }
                    download.add(add_counter);
                    EventBus.getDefault().post(new downloadmsg("counter", download));
                }
            }
        });

        coil2.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(drvice_address.getText().toString().equals("")) {

                } else {
                    int address = Integer.valueOf(drvice_address.getText().toString().trim());
                    download.clear();
                    byte[] add_lock = new byte[8];//byte数组;
                    add_lock[7] = (byte) 0xcf;
                    add_lock[6] = (byte) 0x00;
                    add_lock[5] = (byte) 0x00;
                    add_lock[4] = (byte) 0x00;
                    add_lock[3] = (byte) 0x00;
                    add_lock[2] = (byte) address;
                    add_lock[1] = (byte) 0x00;
                    add_lock[0] = (byte) 0x00;
                    download.add(add_lock);
                    byte[] add_counter = new byte[8];//byte数组;
                    add_counter[7] = (byte) 0x04;
                    add_counter[6] = (byte) 0x04;
                    add_counter[5] = (byte) 0x00;
                    add_counter[4] = (byte) 0x04;
                    add_counter[3] = (byte) 0x00;
                    add_counter[2] = (byte) 0x01;
                    add_counter[1] = (byte) 0x01;
                    add_counter[0] = (byte) 0x01;
                    if (b) {
                        add_counter[5] = (byte) 0x01;
                    }
                    download.add(add_counter);
                    EventBus.getDefault().post(new downloadmsg("counter", download));
                }
            }
        });

        coil3.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(drvice_address.getText().toString().equals("")) {

                } else {
                    int address = Integer.valueOf(drvice_address.getText().toString().trim());
                    download.clear();
                    byte[] add_lock = new byte[8];//byte数组;
                    add_lock[7] = (byte) 0xcf;
                    add_lock[6] = (byte) 0x00;
                    add_lock[5] = (byte) 0x00;
                    add_lock[4] = (byte) 0x00;
                    add_lock[3] = (byte) 0x00;
                    add_lock[2] = (byte) address;
                    add_lock[1] = (byte) 0x00;
                    add_lock[0] = (byte) 0x00;
                    download.add(add_lock);
                    byte[] add_counter = new byte[8];//byte数组;
                    add_counter[7] = (byte) 0x04;
                    add_counter[6] = (byte) 0x00;
                    add_counter[5] = (byte) 0x04;
                    add_counter[4] = (byte) 0x04;
                    add_counter[3] = (byte) 0x00;
                    add_counter[2] = (byte) 0x01;
                    add_counter[1] = (byte) 0x01;
                    add_counter[0] = (byte) 0x01;
                    if (b) {
                        add_counter[6] = (byte) 0x01;
                    }
                    download.add(add_counter);
                    EventBus.getDefault().post(new downloadmsg("counter", download));
                }
            }
        });

        coil4.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(drvice_address.getText().toString().equals("")) {

                } else {
                    int address = Integer.valueOf(drvice_address.getText().toString().trim());
                    download.clear();
                    byte[] add_lock = new byte[8];//byte数组;
                    add_lock[7] = (byte) 0xcf;
                    add_lock[6] = (byte) 0x00;
                    add_lock[5] = (byte) 0x00;
                    add_lock[4] = (byte) 0x00;
                    add_lock[3] = (byte) 0x00;
                    add_lock[2] = (byte) address;
                    add_lock[1] = (byte) 0x00;
                    add_lock[0] = (byte) 0x00;
                    download.add(add_lock);
                    byte[] add_counter = new byte[8];//byte数组;
                    add_counter[7] = (byte) 0x00;
                    add_counter[6] = (byte) 0x04;
                    add_counter[5] = (byte) 0x04;
                    add_counter[4] = (byte) 0x04;
                    add_counter[3] = (byte) 0x00;
                    add_counter[2] = (byte) 0x01;
                    add_counter[1] = (byte) 0x01;
                    add_counter[0] = (byte) 0x01;
                    if (b) {
                        add_counter[7] = (byte) 0x01;
                    }
                    download.add(add_counter);
                    EventBus.getDefault().post(new downloadmsg("counter", download));
                }
            }
        });

        coil5.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(drvice_address.getText().toString().equals("")) {

                } else {
                    int address = Integer.valueOf(drvice_address.getText().toString().trim());
                    download.clear();
                    byte[] add_lock = new byte[8];//byte数组;
                    add_lock[7] = (byte) 0xcf;
                    add_lock[6] = (byte) 0x00;
                    add_lock[5] = (byte) 0x00;
                    add_lock[4] = (byte) 0x00;
                    add_lock[3] = (byte) 0x00;
                    add_lock[2] = (byte) address;
                    add_lock[1] = (byte) 0x00;
                    add_lock[0] = (byte) 0x00;
                    download.add(add_lock);
                    byte[] add_counter = new byte[8];//byte数组;
                    add_counter[7] = (byte) 0x04;
                    add_counter[6] = (byte) 0x04;
                    add_counter[5] = (byte) 0x04;
                    add_counter[4] = (byte) 0x00;
                    add_counter[3] = (byte) 0x00;
                    add_counter[2] = (byte) 0x01;
                    add_counter[1] = (byte) 0x02;
                    add_counter[0] = (byte) 0x01;
                    if (b) {
                        add_counter[4] = (byte) 0x01;
                    }
                    download.add(add_counter);
                    EventBus.getDefault().post(new downloadmsg("counter", download));
                }
            }
        });

        coil6.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(drvice_address.getText().toString().equals("")) {

                } else {
                    int address = Integer.valueOf(drvice_address.getText().toString().trim());
                    download.clear();
                    byte[] add_lock = new byte[8];//byte数组;
                    add_lock[7] = (byte) 0xcf;
                    add_lock[6] = (byte) 0x00;
                    add_lock[5] = (byte) 0x00;
                    add_lock[4] = (byte) 0x00;
                    add_lock[3] = (byte) 0x00;
                    add_lock[2] = (byte) address;
                    add_lock[1] = (byte) 0x00;
                    add_lock[0] = (byte) 0x00;
                    download.add(add_lock);
                    byte[] add_counter = new byte[8];//byte数组;
                    add_counter[7] = (byte) 0x04;
                    add_counter[6] = (byte) 0x04;
                    add_counter[5] = (byte) 0x00;
                    add_counter[4] = (byte) 0x04;
                    add_counter[3] = (byte) 0x00;
                    add_counter[2] = (byte) 0x01;
                    add_counter[1] = (byte) 0x02;
                    add_counter[0] = (byte) 0x01;
                    if (b) {
                        add_counter[5] = (byte) 0x01;
                    }
                    download.add(add_counter);
                    EventBus.getDefault().post(new downloadmsg("counter", download));
                }
            }
        });

        coil7.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(drvice_address.getText().toString().equals("")) {

                } else {
                    int address = Integer.valueOf(drvice_address.getText().toString().trim());
                    download.clear();
                    byte[] add_lock = new byte[8];//byte数组;
                    add_lock[7] = (byte) 0xcf;
                    add_lock[6] = (byte) 0x00;
                    add_lock[5] = (byte) 0x00;
                    add_lock[4] = (byte) 0x00;
                    add_lock[3] = (byte) 0x00;
                    add_lock[2] = (byte) address;
                    add_lock[1] = (byte) 0x00;
                    add_lock[0] = (byte) 0x00;
                    download.add(add_lock);
                    byte[] add_counter = new byte[8];//byte数组;
                    add_counter[7] = (byte) 0x04;
                    add_counter[6] = (byte) 0x00;
                    add_counter[5] = (byte) 0x04;
                    add_counter[4] = (byte) 0x04;
                    add_counter[3] = (byte) 0x00;
                    add_counter[2] = (byte) 0x01;
                    add_counter[1] = (byte) 0x02;
                    add_counter[0] = (byte) 0x01;
                    if (b) {
                        add_counter[6] = (byte) 0x01;
                    }
                    download.add(add_counter);
                    EventBus.getDefault().post(new downloadmsg("counter", download));
                }
            }
        });

        coil8.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(drvice_address.getText().toString().equals("")) {

                } else {
                    int address = Integer.valueOf(drvice_address.getText().toString().trim());
                    download.clear();
                    byte[] add_lock = new byte[8];//byte数组;
                    add_lock[7] = (byte) 0xcf;
                    add_lock[6] = (byte) 0x00;
                    add_lock[5] = (byte) 0x00;
                    add_lock[4] = (byte) 0x00;
                    add_lock[3] = (byte) 0x00;
                    add_lock[2] = (byte) address;
                    add_lock[1] = (byte) 0x00;
                    add_lock[0] = (byte) 0x00;
                    download.add(add_lock);
                    byte[] add_counter = new byte[8];//byte数组;
                    add_counter[7] = (byte) 0x00;
                    add_counter[6] = (byte) 0x04;
                    add_counter[5] = (byte) 0x04;
                    add_counter[4] = (byte) 0x04;
                    add_counter[3] = (byte) 0x00;
                    add_counter[2] = (byte) 0x01;
                    add_counter[1] = (byte) 0x02;
                    add_counter[0] = (byte) 0x01;
                    if (b) {
                        add_counter[7] = (byte) 0x01;
                    }
                    download.add(add_counter);
                    EventBus.getDefault().post(new downloadmsg("counter", download));
                }
            }
        });
    }

//    @Override
//    public void onStop() {
//        super.onStop();
//        EventBus.getDefault().unregister(this);
//    }
//
//    @Override
//    public void onStart() {
//        super.onStart();
//        EventBus.getDefault().register(this);
//    }

}
