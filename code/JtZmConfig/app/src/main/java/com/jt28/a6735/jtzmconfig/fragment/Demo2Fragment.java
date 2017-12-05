package com.jt28.a6735.jtzmconfig.fragment;

import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
import android.support.annotation.IdRes;
import android.support.annotation.Nullable;
import android.support.design.widget.FloatingActionButton;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.RecyclerView;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import com.jt28.a6735.jtzmconfig.R;
import com.jt28.a6735.jtzmconfig.View.PinnedHeaderListView;
import com.jt28.a6735.jtzmconfig.adapter.ButSectionedAdapter;
import com.jt28.a6735.jtzmconfig.adapter.SceneLeftListAdapter;
import com.jt28.a6735.jtzmconfig.adapter.check.PickerView;
import com.jt28.a6735.jtzmconfig.model.PlcControl;
import com.jt28.a6735.jtzmconfig.model.PlcControlchildn;
import com.jt28.a6735.jtzmconfig.model.Plcval;
import com.jt28.a6735.jtzmconfig.model.Proplc;
import com.jt28.a6735.jtzmconfig.model.Scene;
import com.jt28.a6735.jtzmconfig.model.SceneChoice;
import com.jt28.a6735.jtzmconfig.model.Scenechild;
import com.jt28.a6735.jtzmconfig.msg.Msgcoir_groups;
import com.jt28.a6735.jtzmconfig.msg.Msgscenr_groups;
import com.jt28.a6735.jtzmconfig.msg.Notice;
import com.jt28.a6735.jtzmconfig.msg.downloadmsg;
import com.jt28.a6735.jtzmconfig.util.DensityUtil;
import com.jt28.a6735.viewpager.PageFragment;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * Created by a6735 on 2017/7/27.
 */

public class Demo2Fragment extends PageFragment {
    private String TAG = "jt128";

    private List<Scene> groups;
    private List<List<Scenechild>> children;

    ListView leftListview;
    PinnedHeaderListView pinnedListView;
    private boolean isScroll = true;
    private SceneLeftListAdapter adapter;
    private ButSectionedAdapter sectionedAdapter;
    private FloatingActionButton but_add;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_demo2, container, false);
    }

    @Override
    public void onViewCreated(View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);
        intDate();
        but_add = view.findViewById(R.id.demo2_menu);
        leftListview = view.findViewById(R.id.demo2_left_listview);
        pinnedListView = view.findViewById(R.id.demo2_pinnedListView);

        sectionedAdapter = new ButSectionedAdapter(getContext(), groups, children);
        pinnedListView.setAdapter(sectionedAdapter);
        adapter = new SceneLeftListAdapter(getContext(), groups);
        leftListview.setAdapter(adapter);

        but_add.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                show_add_sene();
                //but_add.hide();
            }
        });

        leftListview.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            @Override
            public void onItemClick(AdapterView<?> arg0, View view, int position, long arg3) {
                isScroll = false;

                for (int i = 0; i < groups.size(); i++) {
                    if (i == position) {
                        groups.get(i).setFlagArray(true);
                    } else {
                        groups.get(i).setFlagArray(false);
                    }
                    Log.d(TAG,"切换" + position);
                }
                adapter.notifyDataSetChanged();
                int rightSection = 0;
                for (int i = 0; i < position; i++) {
                    rightSection += sectionedAdapter.getCountForSection(i) + 1;
                }
                pinnedListView.setSelection(rightSection);
            }

        });

        adapter.SetMenuInterface(new SceneLeftListAdapter.MenuInterface() {
            @Override
            public void doItem(int groupPosition) {
                isScroll = false;

                for (int i = 0; i < groups.size(); i++) {
                    if (i == groupPosition) {
                        groups.get(i).setFlagArray(true);
                    } else {
                        groups.get(i).setFlagArray(false);
                        groups.get(i).setMenuflag(false);
                    }
                    Log.d(TAG,"切换" + groupPosition);
                }
                adapter.SetItem(groups);
                adapter.notifyDataSetChanged();
                int rightSection = 0;
                for (int i = 0; i < groupPosition; i++) {
                    rightSection += sectionedAdapter.getCountForSection(i) + 1;
                }
                pinnedListView.setSelection(rightSection);
            }

            @Override
            public void doAdd(int groupPosition) {
               // show.show();
                show_add_menu(groupPosition);
            }

            @Override
            public void doSub(int groupPosition) {
                groups.remove(groupPosition);
                children.remove(groupPosition);

                adapter.SetItem(groups);
                sectionedAdapter.SetItem(groups,children);

                adapter.notifyDataSetChanged();
                sectionedAdapter.notifyDataSetChanged();

                EventBus.getDefault().post(new Msgscenr_groups(groups,children));
            }
        });

        sectionedAdapter.SetMenuInterface(new ButSectionedAdapter.MenuInterface() {
            @Override
            public void doSub(int gr, int ch) {

                children.get(gr).remove(ch);
                if(children.get(gr).size() == 0) {
                    groups.remove(gr);
                    children.remove(gr);
                }

                adapter.SetItem(groups);
                sectionedAdapter.SetItem(groups,children);

                adapter.notifyDataSetChanged();
                sectionedAdapter.notifyDataSetChanged();

                EventBus.getDefault().post(new Msgscenr_groups(groups,children));
            }
        });

        pinnedListView.setOnScrollListener(new AbsListView.OnScrollListener() {
            @Override
            public void onScrollStateChanged(AbsListView arg0, int scrollState) {
                switch (scrollState) {
                    // 当不滚动时
                    case AbsListView.OnScrollListener.SCROLL_STATE_IDLE:
                        // 判断滚动到底部
                        if (pinnedListView.getLastVisiblePosition() == (pinnedListView.getCount() - 1)) {
                            leftListview.setSelection(ListView.FOCUS_DOWN);
                        }

                        // 判断滚动到顶部
                        if (pinnedListView.getFirstVisiblePosition() == 0) {
                            leftListview.setSelection(0);
                        }
                        break;
                }
            }
            int y = 0;
            int x = 0;
            int z = 0;
            int scroll_dr = 0;//判断方向
            @Override
            public void onScroll(AbsListView view, int firstVisibleItem, int visibleItemCount, int totalItemCount) {
                //Log.d(TAG,"位置：" + firstVisibleItem + "位置：" + visibleItemCount);

                if (isScroll) {
                    for (int i = 0; i < groups.size(); i++) {
                        if (i == sectionedAdapter.getSectionForPosition(pinnedListView.getFirstVisiblePosition())) {
                            groups.get(i).setFlagArray(true);
                            x = i;
                        } else {
                            groups.get(i).setFlagArray(false);
                        }
                    }
                    if (x != y) {
                        adapter.notifyDataSetChanged();
                        y = x;
                        int cc = 4;

                        if (y == leftListview.getLastVisiblePosition()) {
                            leftListview.setSelection(y);
                        }
                        if (x == leftListview.getFirstVisiblePosition()) {
                            int c = 0;
                            if(y > (c+cc)) {
                                c = y-cc;
                            }
                            leftListview.setSelection(c);
                        }
                        if (firstVisibleItem + visibleItemCount == totalItemCount - 1) {
                            leftListview.setSelection(ListView.FOCUS_DOWN);
                        }
                    }
                    if(scroll_dr > firstVisibleItem) {
                        scroll_dr = firstVisibleItem;
                        Log.d(TAG,"向下");
                        //but_add.show();
                    } else if(scroll_dr < firstVisibleItem) {
                        scroll_dr = firstVisibleItem;
                        Log.d(TAG,"向上");
                        //but_add.hide();
                    }
                } else {
                    isScroll = true;
                }
            }
        });
    }

    private List<PlcControl> s_groups;
    private List<List<PlcControlchildn>> s_children;
    //接收设置的场景消息
    //场景
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Msgcoir_groups event) {
        s_groups = event.getCoir_groups();
        s_children = event.getCoir_children();
        m_SceneChoice.clear();
        for(PlcControl val :s_groups) {
            Log.d(TAG,val.getName());
            m_SceneChoice.add(new SceneChoice(val.getName(),false));
        }
    }

    List<byte[]> download = new ArrayList<>();
    //下载程序消息
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Notice event) {
        //Log.d(TAG,event.getNotice());
        if(event.getNotice().equals("download_but")) {
            download.clear();
            byte[] lock_1 = new byte[8];//byte数组;
            lock_1[7] = (byte)0x5a;
            lock_1[6] = (byte)0xa5;
            lock_1[5] = (byte)0x5a;
            lock_1[4] = (byte)0xa5;
            lock_1[3] = (byte)0xaa;
            lock_1[2] = (byte)0xa5;
            lock_1[1] = (byte)0x5a;
            lock_1[0] = (byte)0xa5;
            download.add(lock_1);

            byte[] lock_2 = new byte[8];//byte数组;
            lock_2[7] = (byte)0x55;
            lock_2[6] = (byte)0x45;
            lock_2[5] = (byte)0x35;
            lock_2[4] = (byte)0x25;
            lock_2[3] = (byte)0x15;
            lock_2[2] = (byte)0x55;
            lock_2[1] = (byte)0x54;
            lock_2[0] = (byte)0x53;
            download.add(lock_2);

            for(int i = 0;i < groups.size(); i ++){
                boolean key_en = false;//这个按钮是否有命令

                Log.d(TAG, String.valueOf(groups.get(i).getAddress()));//按钮地址
                Log.d(TAG, String.valueOf(groups.get(i).getKeynum()));//哪个按钮

                List<Proplc> m_proplc = new ArrayList<>();//缓存按钮

                for(int c_i = 0;c_i < children.get(i).size(); c_i ++){
                    Log.d(TAG,children.get(i).get(c_i).getName());
                    //调用场景名称
                    String c_name = children.get(i).get(c_i).getName();
                    //判断场景调用
                    for(int s_i = 0;s_i < s_groups.size(); s_i ++){
                        String s_name = s_groups.get(s_i).getName();
                        if(c_name.equals(s_name)) {
                            key_en = true;//这个按钮有内容
                            //调用了这个场景
                            for(int sc_i = 0;sc_i < s_children.get(s_i).size(); sc_i ++){
                                //遍历这个场景的东西
                                Log.d(TAG,"编程内容");
                                Log.d(TAG,s_children.get(s_i).get(sc_i).getName());
                                Log.d(TAG, "场景地址" + String.valueOf(s_children.get(s_i).get(sc_i).getAddress()));

                                //创建节点
                                for (Plcval val : s_children.get(s_i).get(sc_i).getOut()) {
                                    byte[] plcval = new byte[8];//byte数组;
                                    plcval[0] = 1;
                                    plcval[2] = (byte) s_children.get(s_i).get(sc_i).getAddress();
                                    plcval[7] = 4;
                                    plcval[6] = 4;
                                    plcval[5] = 4;
                                    plcval[4] = 4;
                                    if(val.getOut_num() < 4) {
                                        plcval[1] = 1;
                                        plcval[val.getOut_num()+4] = (byte)val.getOut_num_val();
                                    } else {
                                        plcval[1] = 2;
                                        plcval[val.getOut_num()] = (byte)val.getOut_num_val();
                                    }

                                    boolean add_new = true;
                                    for (Proplc plc :m_proplc) {
                                        if(plc.getOut_name()[2] == plcval[2]) {
                                            if(plc.getOut_name()[1] == plcval[1]) {
                                                byte[] plcchange = new byte[8];//byte数组;
                                                plcchange = plc.getOut_name();
                                                if(val.getOut_num() < 4) {
                                                    plcchange[val.getOut_num()+4] = (byte)val.getOut_num_val();
                                                } else {
                                                    plcchange[val.getOut_num()] = (byte)val.getOut_num_val();
                                                }
                                                plc.setOut_name(plcchange);
                                                add_new = false;
                                            }
                                        }
                                    }
                                    if(add_new) {
                                        m_proplc.add(new Proplc(s_children.get(s_i).get(sc_i).getAddress(), plcval, true));
                                    }
                                }
                            }
                        }
                    }
                }
                //一个按钮判断完毕
                if(key_en) {
                    byte[] key_num = new byte[8];//byte数组;
                    key_num[0] = (byte)groups.get(i).getKeynum();//第几个按钮
                    key_num[7] = (byte)0xa5;
                    if (groups.get(i).getType() != 1) {
                        //自己的面板
                        //key_num[3] = (byte)groups.get(i).getAddress();//地址
                        key_num[3] = (byte)254;
                        key_num[2] = (byte)groups.get(i).getAddress();//地址
                        key_num[1] = (byte)m_proplc.size();
                    } else {
                        //第三方厂家面板
                        key_num[3] = (byte)254;
                        key_num[2] = (byte)groups.get(i).getAddress();//地址
                        key_num[1] = (byte)m_proplc.size();
                    }
                    download.add(key_num);
                }
                //添加按钮数据
                for (Proplc plc :m_proplc) {
                    String ccc = plc.getAddress() + Arrays.toString(plc.getOut_name());
                    Log.d(TAG,ccc);
                    download.add(plc.getOut_name());
                }
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
            download.add(add_lock);

            for(byte[] tmp:download) {
                Log.d(TAG,"lhb_编译结果2:  " + Arrays.toString(tmp));
            }
            EventBus.getDefault().post(new downloadmsg("download_but",download));
        }
    }
    //按钮
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Msgscenr_groups event) {
        groups = event.getScenr_groups();
        children = event.getScene_children();
        adapter.SetItem(groups);
        sectionedAdapter.SetItem(groups,children);

        adapter.notifyDataSetChanged();
        sectionedAdapter.notifyDataSetChanged();
    }
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

    private void intDate() {
        groups = new ArrayList<>();
        children = new ArrayList<>();
        for(int i = 0;i < 0;i++) {
            boolean arr = false;
            if(i == 0) {
                arr = true;
            }
            groups.add(new Scene("按钮"+i,0,i,i,arr,false) );
            List<Scenechild> products = new ArrayList<>();
            products.add(new Scenechild("场景1"));
            products.add(new Scenechild("场景2"));
            products.add(new Scenechild("场景3"));
            products.add(new Scenechild("场景4"));
            children.add(products);
        }
        //m_SceneChoice.add(new SceneChoice("123",false));
    }
    private void show_add_sene() {
        final Dialog bottomDialog = new Dialog(getContext(), R.style.BottomDialog);
        View contentView = LayoutInflater.from(getContext()).inflate(R.layout.dialog_add_sene_circle, null);
        bottomDialog.setContentView(contentView);
        ViewGroup.MarginLayoutParams params = (ViewGroup.MarginLayoutParams) contentView.getLayoutParams();
        params.width = getResources().getDisplayMetrics().widthPixels - DensityUtil.dp2px(getContext(), 16f);
        params.bottomMargin = DensityUtil.dp2px(getContext(), 8f);
        contentView.setLayoutParams(params);
        bottomDialog.setCanceledOnTouchOutside(true);
        bottomDialog.getWindow().setGravity(Gravity.BOTTOM);
        bottomDialog.getWindow().setWindowAnimations(R.style.BottomDialog_Animation);

        final int[] key_num = {3};
        final int[] type_num = {0};
        final PickerView second_pv;
        second_pv = (PickerView) contentView.findViewById(R.id.dialog_add_sene_list);
        final List<String> seconds = new ArrayList<String>();
        for (int i = 0; i < 4; i++) {
            seconds.add("按钮"+(i+1));
        }
        second_pv.setData(seconds);
        second_pv.setOnSelectListener(new PickerView.onSelectListener() {
            @Override
            public void onSelect(String text) {
                String key = text.substring(2);
                key_num[0] = Integer.valueOf(key);
            }
        });

        //获取实例
        RadioGroup  radioGroup = (RadioGroup)contentView.findViewById(R.id.dialog_add_sene_radioGroupID);
        final RadioButton femaleRadioButton = (RadioButton)contentView.findViewById(R.id.dialog_add_sene_originalfactory);
        final RadioButton maleRadioButton = (RadioButton)contentView.findViewById(R.id.dialog_add_sene_other);
        femaleRadioButton.setChecked(true);
        radioGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup radioGroup, @IdRes int c_i) {
                if (c_i==femaleRadioButton.getId()){
                    Log.d(TAG,"选中了female!");
                    type_num[0] = 0;
                    seconds.clear();
                    for (int i = 0; i < 4; i++) {
                        seconds.add("按钮"+(i+1));
                    }
                    second_pv.setData(seconds);
                }else if (c_i==maleRadioButton.getId()){
                    Log.d(TAG,"选中了male!");
                    type_num[0] = 1;
                    seconds.clear();
                    for (int i = 0; i < 8; i++) {
                        seconds.add("触摸"+(i+1));
                    }
                    second_pv.setData(seconds);
                }
            }
        });

        final EditText name = (EditText)contentView.findViewById(R.id.dialog_add_sene_name);
        final EditText address = (EditText)contentView.findViewById(R.id.dialog_add_sene_address);

        final Button enter = (Button) contentView.findViewById(R.id.dialog_add_sene_enter);
        enter.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if("".equals(address.getText().toString().trim())) {
                    enter.setBackgroundResource(R.color.red);
                    enter.setText("请设置按钮名称");
                } else {
                    groups.add(new Scene(name.getText().toString(), type_num[0],Integer.valueOf(address.getText().toString()),key_num[0],false,false) );
                    List<Scenechild> products = new ArrayList<>();
                    children.add(products);

                    adapter.SetItem(groups);
                    sectionedAdapter.SetItem(groups,children);

                    adapter.notifyDataSetChanged();
                    sectionedAdapter.notifyDataSetChanged();

                    EventBus.getDefault().post(new Msgscenr_groups(groups,children));

                    bottomDialog.dismiss();
                }
            }
        });

        bottomDialog.show();
    }

    List<SceneChoice> m_SceneChoice = new ArrayList<>();
    private void show_add_menu(final int pos) {
        final Dialog bottomDialog = new Dialog(getContext(), R.style.BottomDialog);
        View contentView = LayoutInflater.from(getContext()).inflate(R.layout.dialog_add_menu_circle, null);
        bottomDialog.setContentView(contentView);
        ViewGroup.MarginLayoutParams params = (ViewGroup.MarginLayoutParams) contentView.getLayoutParams();
        params.width = getResources().getDisplayMetrics().widthPixels - DensityUtil.dp2px(getContext(), 16f);
        params.bottomMargin = DensityUtil.dp2px(getContext(), 8f);
        contentView.setLayoutParams(params);
        bottomDialog.setCanceledOnTouchOutside(true);
        bottomDialog.getWindow().setGravity(Gravity.BOTTOM);
        bottomDialog.getWindow().setWindowAnimations(R.style.BottomDialog_Animation);

        RecyclerView recyclerView = (RecyclerView) contentView.findViewById(R.id.i_dialog_add_menu_list);
        recyclerView.setItemAnimator(new DefaultItemAnimator());
        recyclerView.setLayoutManager(new GridLayoutManager(getContext(),3));
        MyAadapter myAadapter = new MyAadapter(getContext(),m_SceneChoice);
        recyclerView.setAdapter(myAadapter);

        Button enter = (Button) contentView.findViewById(R.id.i_dialog_add_menu_enter);
        enter.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                for (SceneChoice val :m_SceneChoice) {
                    if(val.getChoice()) {
                        children.get(pos).add(new Scenechild(val.getName()));
                    }
                }
                adapter.SetItem(groups);
                sectionedAdapter.SetItem(groups,children);

                adapter.notifyDataSetChanged();
                sectionedAdapter.notifyDataSetChanged();

                EventBus.getDefault().post(new Msgscenr_groups(groups,children));

                bottomDialog.dismiss();
            }
        });

        bottomDialog.show();
    }

    public class MyAadapter extends RecyclerView.Adapter{
        private LayoutInflater inflater;
        public List<SceneChoice> list;

        public MyAadapter(Context context , List<SceneChoice> list) {
            this.list = list;
            inflater = LayoutInflater.from(context);
        }

        @Override
        public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            return new ViewHolder(inflater.inflate(R.layout.item_grid_scenechoice,parent,false));
        }

        @Override
        public void onBindViewHolder(RecyclerView.ViewHolder holder, final int position) {
            final ViewHolder viewHolder = (ViewHolder) holder;
            viewHolder.textview.setText(list.get(position).getName());
            viewHolder.choice.setChecked(list.get(position).getChoice());
            viewHolder.choice.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                    list.get(position).setChoice(b);
                }
            });
        }

        @Override
        public int getItemCount() {
            return list.size();
        }

        private class ViewHolder extends RecyclerView.ViewHolder{

            private TextView textview;
            private CheckBox choice;

            public ViewHolder(View itemView) {
                super(itemView);
                textview = (TextView) itemView.findViewById(R.id.item_scenechoice_name);
                choice = (CheckBox) itemView.findViewById(R.id.item_scenechoice_val);
            }
        }
    }
}
