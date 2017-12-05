package com.jt28.a6735.jtconfig.fragment;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.Spinner;

import com.jt28.a6735.jtconfig.Msg.CmdMsg;
import com.jt28.a6735.jtconfig.Msg.Msg;
import com.jt28.a6735.jtconfig.Msg.downloadmsg;
import com.jt28.a6735.jtconfig.R;
import com.jt28.a6735.jtconfig.adapter.SceneListViewAdapter;
import com.jt28.a6735.jtconfig.bean.GroupInfo;
import com.jt28.a6735.jtconfig.bean.ProductInfo;
import com.jt28.a6735.jtconfig.bean.SceneInfo;
import com.jt28.a6735.jtconfig.bean.lhbplc;
import com.jt28.a6735.jtconfig.widget.SceneableListView;
import com.jt28.a6735.viewpager.PageFragment;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by a6735 on 2017/7/4.
 */

public class KeyConfigFragment extends PageFragment  implements SceneListViewAdapter.CheckInterface, View.OnClickListener, SceneListViewAdapter.ModifyCountInterface{

    private SceneableListView exListView;
    private SceneListViewAdapter scenelist;
    private List<SceneInfo> groups = new ArrayList<>();// 组元素数据列表
    private Map<String, List<SceneInfo>> children = new HashMap<>();// 子元素数据列表

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        for (int i = 0; i < 4; i++) {
            groups.add(new SceneInfo(i + "","按钮"+(i+1),false));
            List<SceneInfo> products = new ArrayList<>();
            children.put(groups.get(i).getId(), products);// 将组元素的一个唯一值，这里取Id，作为子元素List的Key
        }
        exListView = (SceneableListView) getActivity().findViewById(R.id.keyconfigfragment_exListView);
        scenelist = new SceneListViewAdapter(groups, children, getContext());
        scenelist.setCheckInterface(this);// 关键步骤1,设置复选框接口
        scenelist.setModifyCountInterface(this);// 关键步骤2,设置数量增减接口
        exListView.setAdapter(scenelist);

        for (int i = 0; i < scenelist.getGroupCount(); i++) {
            exListView.expandGroup(i);// 关键步骤3,初始化时，将ExpandableListView以展开的方式呈现
        }
        InitAddArrow();
    }

    //定义一个String类型的List数组作为数据源
    private List<String> dataList;
    private Spinner changj;
    //定义一个ArrayAdapter适配器作为spinner的数据适配器
    private ArrayAdapter<String> adapter;

    private android.app.AlertDialog.Builder builder = null;
    private View view_custom;
    private android.app.AlertDialog alert = null;
    private void InitAddArrow() {
        //初始化Builder
        builder = new android.app.AlertDialog.Builder(getContext());
        //加载自定义的那个View,同时设置下'
        LayoutInflater inflater = getActivity().getLayoutInflater();
        view_custom = inflater.inflate(R.layout.item_add_config, null, false);
        builder.setView(view_custom);
        builder.setCancelable(false);
        alert = builder.create();

        changj = (Spinner)view_custom.findViewById(R.id.item_add_config_spr);
        //为dataList赋值，将下面这些数据添加到数据源中
        dataList = new ArrayList<>();
        dataList.add("请定义场景");
        /*为spinner定义适配器，也就是将数据源存入adapter，这里需要三个参数
        1. 第一个是Context（当前上下文），这里就是this
        2. 第二个是spinner的布局样式，这里用android系统提供的一个样式
        3. 第三个就是spinner的数据源，这里就是dataList*/
        adapter = new ArrayAdapter<String>(getContext(),android.R.layout.simple_spinner_item,dataList);
        //为适配器设置下拉列表下拉时的菜单样式。
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        //为spinner绑定我们定义好的数据适配器
        changj.setAdapter(adapter);

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

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.keyconfigfragment, container, false);
        return view;
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


    private List<GroupInfo> l_groups;// 组元素数据列表
    private Map<String, List<ProductInfo>> l_children;// 子元素数据列表

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Msg event) {
        l_groups = new ArrayList<>();
        l_children = new HashMap<>();
        l_groups = event.GetGroups();
        l_children = event.Getchildren();

        //为dataList赋值，将下面这些数据添加到数据源中
        dataList = new ArrayList<>();
        /* Do something */
        for (String key : event.Getchildren().keySet()) {
            System.out.println("lhb_bKey = " + key);
            for(ProductInfo tmp:event.Getchildren().get(key)){
                System.out.println("lhb_bKey" + tmp.getId() + tmp.getName() + tmp.getDesc());
            }
        }

        for(GroupInfo tmp:event.GetGroups()){
            System.out.println("lhb_b" + tmp.getId() + tmp.getName());
            dataList.add(tmp.getName());
        }
        /*为spinner定义适配器，也就是将数据源存入adapter，这里需要三个参数
        1. 第一个是Context（当前上下文），这里就是this
        2. 第二个是spinner的布局样式，这里用android系统提供的一个样式
        3. 第三个就是spinner的数据源，这里就是dataList*/
        adapter = new ArrayAdapter<String>(getContext(),android.R.layout.simple_spinner_item,dataList);
        //为适配器设置下拉列表下拉时的菜单样式。
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        //为spinner绑定我们定义好的数据适配器
        changj.setAdapter(adapter);
    };

    List<byte[]> download_lhb = new ArrayList<>();

    List<String> lhb = new ArrayList<>();
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(CmdMsg event) {
        /* Do something */
        System.out.println("lhb_cmd" + event.getCmd());
        if(event.getCmd().equals("getpro")) {
            //每个都需要重新初始化命令
            lhb = new ArrayList<>();
            download_lhb = new ArrayList<>();
            byte[] lock_1 = new byte[8];//byte数组;
            lock_1[7] = (byte)0x5a;
            lock_1[6] = (byte)0xa5;
            lock_1[5] = (byte)0x5a;
            lock_1[4] = (byte)0xa5;
            lock_1[3] = (byte)0xaa;
            lock_1[2] = (byte)0xa5;
            lock_1[1] = (byte)0x5a;
            lock_1[0] = (byte)0xa5;
            download_lhb.add(lock_1);

            byte[] lock_2 = new byte[8];//byte数组;
            lock_2[7] = (byte)0x55;
            lock_2[6] = (byte)0x45;
            lock_2[5] = (byte)0x35;
            lock_2[4] = (byte)0x25;
            lock_2[3] = (byte)0x15;
            lock_2[2] = (byte)0x55;
            lock_2[1] = (byte)0x54;
            lock_2[0] = (byte)0x53;
            download_lhb.add(lock_2);


            byte[] button = new byte[1];//byte数组;
            //分组内容
            for (String key : children.keySet()) {
                System.out.println("lhb_pro = " + key);
                String ccc = "";
                List<lhbplc> lhbplc = new ArrayList<>();
                boolean need_button = false;//是否需要添加这个按钮
                for(SceneInfo tmp:children.get(key)){
                    System.out.println("lhb_pro" + tmp.getId() + tmp.getName());
                    //遍历父节点--调用了哪个节点
                    for (GroupInfo ignored :l_groups) {
                        System.out.println("lhb_pro+父节点" + ignored.getName() );
                        //判断有没有调用这个父节点
                        if(tmp.getName().equals(ignored.getName())) {
                            need_button = true;//这个按钮有内容
                            //调用了这个父节点，寻找这个父节点的子节点内容
                            for(ProductInfo L_get:l_children.get(ignored.getId())){
                                System.out.println("lhb_pro+子节点内容" + L_get.getId() + L_get.getName() + L_get.getDesc());
                                //ccc += "按钮" + L_get.getId() +  "地址" + L_get.getaddress() + "输出口" + L_get.getout() + "加备注" + L_get.getDesc() ;
                                boolean new_node = true;//是否需要创建新节点
                                //遍历数组有有没有相同的地址，有相同的地址就把他们放到一起
                                for (lhbplc plc :lhbplc) {
                                    //判断地址是否相同
                                    if(plc.getAddress().equals((L_get.getaddress()))) {
                                        byte[] plcvale = new byte[8];//byte数组;

                                        int num_arr  = 0;
                                        switch (L_get.getout()) {
                                            case "继电器1":
                                                num_arr = 0;
                                                if(L_get.getsw_bit()) {
                                                    plcvale[4] = 1;
                                                } else {
                                                    plcvale[4] = 0;
                                                }
                                                break;
                                            case "继电器2":
                                                num_arr = 1;
                                                if(L_get.getsw_bit()) {
                                                    plcvale[5] = 1;
                                                } else {
                                                    plcvale[5] = 0;
                                                }
                                                break;
                                            case "继电器3":
                                                num_arr = 2;
                                                if(L_get.getsw_bit()) {
                                                    plcvale[6] = 1;
                                                } else {
                                                    plcvale[6] = 0;
                                                }
                                                break;
                                            case "继电器4":
                                                num_arr = 3;
                                                if(L_get.getsw_bit()) {
                                                    plcvale[7] = 1;
                                                } else {
                                                    plcvale[7] = 0;
                                                }
                                                break;
                                            case "继电器5":
                                                num_arr = 4;
                                                if(L_get.getsw_bit()) {
                                                    plcvale[4] = 1;
                                                } else {
                                                    plcvale[4] = 0;
                                                }
                                                break;
                                            case "继电器6":
                                                num_arr = 5;
                                                if(L_get.getsw_bit()) {
                                                    plcvale[5] = 1;
                                                } else {
                                                    plcvale[5] = 0;
                                                }
                                                break;
                                            case "继电器7":
                                                num_arr = 6;
                                                if(L_get.getsw_bit()) {
                                                    plcvale[6] = 1;
                                                } else {
                                                    plcvale[6] = 0;
                                                }
                                                break;
                                            case "继电器8":
                                                num_arr = 7;
                                                if(L_get.getsw_bit()) {
                                                    plcvale[7] = 1;
                                                } else {
                                                    plcvale[7] = 0;
                                                }
                                                break;
                                            default:
                                                break;
                                        }

                                        boolean arrow_swo = false;//组判断

                                        plcvale = plc.getOut_name();
                                        if(plcvale[1] == 1) {
                                            if(num_arr < 4) {
                                                arrow_swo = true;
                                            }
                                        } else if(plcvale[1] == 2){
                                            if(num_arr >= 4) {
                                                arrow_swo = true;
                                            }
                                        }

                                        if(arrow_swo) {
                                            switch (L_get.getout()) {
                                                case "继电器1":
                                                    if (L_get.getsw_bit()) {
                                                        plcvale[4] = 1;
                                                    } else {
                                                        plcvale[4] = 0;
                                                    }
                                                    break;
                                                case "继电器2":
                                                    if (L_get.getsw_bit()) {
                                                        plcvale[5] = 1;
                                                    } else {
                                                        plcvale[5] = 0;
                                                    }
                                                    break;
                                                case "继电器3":
                                                    if (L_get.getsw_bit()) {
                                                        plcvale[6] = 1;
                                                    } else {
                                                        plcvale[6] = 0;
                                                    }
                                                    break;
                                                case "继电器4":
                                                    if (L_get.getsw_bit()) {
                                                        plcvale[7] = 1;
                                                    } else {
                                                        plcvale[7] = 0;
                                                    }
                                                    break;
                                                case "继电器5":
                                                    if (L_get.getsw_bit()) {
                                                        plcvale[4] = 1;
                                                    } else {
                                                        plcvale[4] = 0;
                                                    }
                                                    break;
                                                case "继电器6":
                                                    if (L_get.getsw_bit()) {
                                                        plcvale[5] = 1;
                                                    } else {
                                                        plcvale[5] = 0;
                                                    }
                                                    break;
                                                case "继电器7":
                                                    if (L_get.getsw_bit()) {
                                                        plcvale[6] = 1;
                                                    } else {
                                                        plcvale[6] = 0;
                                                    }
                                                    break;
                                                case "继电器8":
                                                    if (L_get.getsw_bit()) {
                                                        plcvale[7] = 1;
                                                    } else {
                                                        plcvale[7] = 0;
                                                    }
                                                    break;
                                                default:
                                                    break;
                                            }
                                            plc.setOut_name(plcvale);
                                            new_node = false;
                                        }
                                    }
                                }
                                //创建新节点
                                if(new_node) {
                                    byte[] plcvale = new byte[8];//byte数组;
                                    plcvale[0] = 1;
                                    plcvale[7] = 2;
                                    plcvale[6] = 2;
                                    plcvale[5] = 2;
                                    plcvale[4] = 2;
                                    switch (L_get.getout()) {
                                        case "继电器1":
                                            plcvale[1] = 1;
                                            if(L_get.getsw_bit()) {
                                                plcvale[4] = 1;
                                            } else {
                                                plcvale[4] = 0;
                                            }
                                            break;
                                        case "继电器2":
                                            plcvale[1] = 1;
                                            if(L_get.getsw_bit()) {
                                                plcvale[5] = 1;
                                            } else {
                                                plcvale[5] = 0;
                                            }
                                            break;
                                        case "继电器3":
                                            plcvale[1] = 1;
                                            if(L_get.getsw_bit()) {
                                                plcvale[6] = 1;
                                            } else {
                                                plcvale[6] = 0;
                                            }
                                            break;
                                        case "继电器4":
                                            plcvale[1] = 1;
                                            if(L_get.getsw_bit()) {
                                                plcvale[7] = 1;
                                            } else {
                                                plcvale[7] = 0;
                                            }
                                            break;
                                        case "继电器5":
                                            plcvale[1] = 2;
                                            if(L_get.getsw_bit()) {
                                                plcvale[4] = 1;
                                            } else {
                                                plcvale[4] = 0;
                                            }
                                            break;
                                        case "继电器6":
                                            plcvale[1] = 2;
                                            if(L_get.getsw_bit()) {
                                                plcvale[5] = 1;
                                            } else {
                                                plcvale[5] = 0;
                                            }
                                            break;
                                        case "继电器7":
                                            plcvale[1] = 2;
                                            if(L_get.getsw_bit()) {
                                                plcvale[6] = 1;
                                            } else {
                                                plcvale[6] = 0;
                                            }
                                            break;
                                        case "继电器8":
                                            plcvale[1] = 2;
                                            if(L_get.getsw_bit()) {
                                                plcvale[7] = 1;
                                            } else {
                                                plcvale[7] = 0;
                                            }
                                            break;
                                        default:
                                            break;
                                    }
                                    plcvale[2] = (byte)(Integer.parseInt( L_get.getaddress()) );
                                    lhbplc.add(new lhbplc(L_get.getaddress(),plcvale,L_get.getsw_bit()));
                                }
                            }
                        }
                    }
                }
                //遍历按钮1内容
                button[0]++;
                if(need_button) {
                    byte[] button_ll = new byte[8];//byte数组;
                    button_ll[0] = button[0];
                    button_ll[3] = 4;
                    download_lhb.add(button_ll);
                }

                for (lhbplc plc :lhbplc) {
                    ccc += plc.getAddress() + Arrays.toString(plc.getOut_name());
                   // byte[] add_plcvale = new byte[8];//byte数组;
                    //add_plcvale[0] = (byte)(Integer.parseInt( plc.getAddress()) );
                    //download_lhb.add(add_plcvale);
                    download_lhb.add(plc.getOut_name());
                }

                lhb.add(ccc);
            }

            byte[] add_lock = new byte[8];//byte数组;
            add_lock[7] = (byte)0xf5;
            add_lock[6] = (byte)0xf5;
            add_lock[5] = (byte)0xf5;
            add_lock[4] = (byte)0xf5;
            add_lock[3] = (byte)0xf5;
            add_lock[2] = (byte)0xf5;
            add_lock[1] = (byte)0xf5;
            add_lock[0] = (byte)0xf5;
            download_lhb.add(add_lock);

            for(String tmp:lhb){
                System.out.println("lhb_编译结果:   " + tmp);
            }
            for(byte[] tmp:download_lhb) {
                System.out.println("lhb_编译结果2:   " + Arrays.toString(tmp));
            }
            EventBus.getDefault().post(new downloadmsg(download_lhb));
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
