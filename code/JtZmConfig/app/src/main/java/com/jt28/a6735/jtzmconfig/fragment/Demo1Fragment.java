package com.jt28.a6735.jtzmconfig.fragment;


import android.app.Dialog;
import android.content.Context;
import android.os.Bundle;
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
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.Switch;

import com.jt28.a6735.jtzmconfig.R;
import com.jt28.a6735.jtzmconfig.View.PinnedHeaderListView;
import com.jt28.a6735.jtzmconfig.adapter.LeftListAdapter;
import com.jt28.a6735.jtzmconfig.adapter.MainSectionedAdapter;
import com.jt28.a6735.jtzmconfig.model.PlcControl;
import com.jt28.a6735.jtzmconfig.model.PlcControlchildn;
import com.jt28.a6735.jtzmconfig.model.Plcval;
import com.jt28.a6735.jtzmconfig.model.Proplc;
import com.jt28.a6735.jtzmconfig.msg.Msgcoir_groups;
import com.jt28.a6735.jtzmconfig.msg.Notice;
import com.jt28.a6735.jtzmconfig.util.DensityUtil;
import com.jt28.a6735.viewpager.PageFragment;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;


public class Demo1Fragment extends PageFragment {
    private String TAG = "jt128";

    private ListView leftListview;
    private PinnedHeaderListView pinnedListView;
    private boolean isScroll = true;
    private LeftListAdapter adapter;
    private MainSectionedAdapter sectionedAdapter;

    private List<PlcControl> groups;
    private List<List<PlcControlchildn>> children;

    private FloatingActionButton but_add;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        return inflater.inflate(R.layout.fragment_demo1, container, false);
    }

    @Override
    public void onViewCreated(final View view, @Nullable Bundle savedInstanceState) {
        super.onViewCreated(view, savedInstanceState);

        initDate();

        but_add = view.findViewById(R.id.mode1_mode);
        //final LTitleBehaviorAnim show = new LTitleBehaviorAnim(but_add);
        leftListview = view.findViewById(R.id.left_listview);
        pinnedListView = view.findViewById(R.id.pinnedListView);

        sectionedAdapter = new MainSectionedAdapter(getContext(), groups, children);
        pinnedListView.setAdapter(sectionedAdapter);
        adapter = new LeftListAdapter(getContext(), groups);
        leftListview.setAdapter(adapter);

        but_add.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                //but_add.hide();
                //but_add.hide();
                ShowPlc_add();
                //but_add.show();
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

        adapter.SetMenuInterface(new LeftListAdapter.MenuInterface() {
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
                ShowPlc_menu_add(groupPosition);
            }
            @Override
            public void doSub(int groupPosition) {
                groups.remove(groupPosition);
                children.remove(groupPosition);

                adapter.SetItem(groups);
                sectionedAdapter.SetItem(groups,children);

                adapter.notifyDataSetChanged();
                sectionedAdapter.notifyDataSetChanged();

                EventBus.getDefault().post(new Msgcoir_groups(groups,children));
            }
        });

        sectionedAdapter.SetMenuInterface(new MainSectionedAdapter.MenuInterface() {
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

                EventBus.getDefault().post(new Msgcoir_groups(groups,children));
            }

            @Override
            public void dochange(int gr, int ch) {
                ShowPlc_menu_chagne(gr,ch);
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
            @Override
            public void onScroll(AbsListView view, int firstVisibleItem, int visibleItemCount, int totalItemCount) {

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
                } else {
                    isScroll = true;
                }
            }
        });
    }

    private void initDate(){
        groups = new ArrayList<>();
        children = new ArrayList<>();
        for(int i = 0;i < 0;i++) {
            boolean arr = false;
            if(i == 0) {
                arr = true;
            }
            groups.add(new PlcControl(i,"场景" + i,arr,false));
            List<PlcControlchildn> products = new ArrayList<>();
            List<Plcval> out_val = new ArrayList<>();
            if((i%2) == 0) {
                out_val.add(new Plcval(0, 0));
                out_val.add(new Plcval(1, 1));
                out_val.add(new Plcval(2, 2));
                out_val.add(new Plcval(3, 0));
            } else {
                out_val.add(new Plcval(0, 0));
                out_val.add(new Plcval(1, 1));
                out_val.add(new Plcval(2, 2));
                out_val.add(new Plcval(3, 0));
                out_val.add(new Plcval(4, 1));
                out_val.add(new Plcval(5, 2));
                out_val.add(new Plcval(6, 0));
                out_val.add(new Plcval(7, 1));
            }
            products.add(new PlcControlchildn("场景1",1,out_val));
            products.add(new PlcControlchildn("场景2",2,out_val));
            products.add(new PlcControlchildn("场景3",3,out_val));
            products.add(new PlcControlchildn("场景4",4,out_val));
            children.add(products);
        }
    }

    List<byte[]> download = new ArrayList<>();
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Notice event) {
        Log.d(TAG,event.getNotice());
        if(event.getNotice().equals("sence_time")) {
            download.clear();
            byte[] lock_1 = new byte[8];//byte数组;
            lock_1[7] = (byte)0x1a;
            lock_1[6] = (byte)0x25;
            lock_1[5] = (byte)0x3a;
            lock_1[4] = (byte)0x45;
            lock_1[3] = (byte)0x5a;
            lock_1[2] = (byte)0x65;
            lock_1[1] = (byte)0x7a;
            lock_1[0] = (byte)0x85;
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

            for(int i = 0;i < groups.size(); i ++) {
                Log.d(TAG,"场景：" + groups.get(i).getName());
                List<Proplc> m_proplc = new ArrayList<>();//缓存按钮

                for(int sc_i = 0;sc_i < children.get(i).size(); sc_i ++){
                    Log.d(TAG,"------内容：" + children.get(i).get(sc_i).getName());
                    //创建节点
                    for (Plcval val : children.get(i).get(sc_i).getOut()) {
                        byte[] plcval = new byte[8];//byte数组;
                        plcval[0] = 1;
                        plcval[2] = (byte) children.get(i).get(sc_i).getAddress();
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
                            m_proplc.add(new Proplc(children.get(i).get(sc_i).getAddress(), plcval, true));
                        }
                    }
                }

                byte[] key_num = new byte[8];//byte数组;
                key_num[7] = (byte)0x77;
                key_num[6] = (byte)0x77;
                key_num[5] = (byte)0x77;
                key_num[4] = (byte)0x77;
                key_num[3] = (byte)0x77;
                key_num[2] = (byte)0x77;
                key_num[1] = (byte)i;
                key_num[0] = (byte)0x77;
                download.add(key_num);
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
                Log.d(TAG,"场景结果:  " + Arrays.toString(tmp));
            }
        }
    }
    //场景
    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Msgcoir_groups event) {
        groups = event.getCoir_groups();
        children = event.getCoir_children();
        adapter.SetItem(groups);
        sectionedAdapter.SetItem(groups,children);

        adapter.notifyDataSetChanged();
        sectionedAdapter.notifyDataSetChanged();
    }

    private void ShowPlc_add() {
        final Dialog bottomDialog = new Dialog(getContext(), R.style.BottomDialog);
        final View contentView = LayoutInflater.from(getContext()).inflate(R.layout.dialog_newplc_circle, null);
        bottomDialog.setContentView(contentView);
        ViewGroup.MarginLayoutParams params = (ViewGroup.MarginLayoutParams) contentView.getLayoutParams();
        params.width = getResources().getDisplayMetrics().widthPixels - DensityUtil.dp2px(getContext(), 16f);
        params.bottomMargin = DensityUtil.dp2px(getContext(), 8f);
        contentView.setLayoutParams(params);
        bottomDialog.setCanceledOnTouchOutside(true);
        bottomDialog.getWindow().setGravity(Gravity.BOTTOM);
        bottomDialog.getWindow().setWindowAnimations(R.style.BottomDialog_Animation);
        //绑定内部控件
        final EditText name = contentView.findViewById(R.id.dialog_newplc_name);
        final Button enter = contentView.findViewById(R.id.dialog_newplc_enter);
        enter.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if("".equals(name.getText().toString().trim())) {
                    enter.setBackgroundResource(R.color.red);
                    enter.setText("场景名称不能为空！");
                } else {
                    int num = 0;
                    if(groups.size() == 0) {
                        num = 0;
                    } else {
                        String name = groups.get(groups.size()-1).getName();
                        num = Integer.valueOf( name.substring(name.length()-1,name.length()) ) + 1;
                    }
                    groups.add(new PlcControl(groups.size(),name.getText().toString() + num,false,false ));
                    List<PlcControlchildn> products = new ArrayList<>();
                    children.add(products);
                    adapter.SetItem(groups);
                    sectionedAdapter.SetItem(groups,children);
                    adapter.notifyDataSetChanged();
                    sectionedAdapter.notifyDataSetChanged();

                    EventBus.getDefault().post(new Msgcoir_groups(groups,children));

                    bottomDialog.dismiss();
                }
            }
        });
        bottomDialog.show();
    }

    public class outnum_info {
        private String title;   //信息标题
        private boolean sel;
        private int val;
        public outnum_info(String title,boolean sel,int val) {
            this.title = title;
            this.val = val;
            this.sel = sel;
        }

        public void setTitle(String title) {
            this.title = title;
        }

        public void setVal(int val) {
            this.val = val;
        }

        public void setSel(boolean sel) {
            this.sel = sel;
        }

        public String getTitle() {
            return title;
        }

        public int getVal() {
            return val;
        }

        public boolean getSel() {
            return sel;
        }
    }

    private List<outnum_info> out_num_list = new ArrayList<>();
    //新建自相
    private void ShowPlc_menu_chagne(final int gr, final int ch) {
        final Dialog bottomDialog = new Dialog(getContext(), R.style.BottomDialog);
        final View contentView = LayoutInflater.from(getContext()).inflate(R.layout.dialog_newplc_menu_add_circle, null);
        bottomDialog.setContentView(contentView);
        ViewGroup.MarginLayoutParams params = (ViewGroup.MarginLayoutParams) contentView.getLayoutParams();
        params.width = getResources().getDisplayMetrics().widthPixels - DensityUtil.dp2px(getContext(), 16f);
        params.bottomMargin = DensityUtil.dp2px(getContext(), 8f);
        contentView.setLayoutParams(params);
        bottomDialog.setCanceledOnTouchOutside(true);
        bottomDialog.getWindow().setGravity(Gravity.BOTTOM);
        bottomDialog.getWindow().setWindowAnimations(R.style.BottomDialog_Animation);

        final EditText tal = (EditText)contentView.findViewById(R.id.dialog_newplc_add_name);
        final EditText address = (EditText)contentView.findViewById(R.id.dialog_newplc_add_address);

        final Button enter =(Button)contentView.findViewById(R.id.dialog_newplc_add_enter);
        enter.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                if("".equals(address.getText().toString().trim())) {
                    enter.setBackgroundResource(R.color.red);
                    enter.setText("地址不能为空");
                } else {
                    int i_num = 0;
                    List<Plcval> out_val = new ArrayList<>();
                    for (outnum_info outnum :out_num_list) {
                        System.out.println("lhb" + outnum.getSel());
                        if(outnum.getSel()) {
                            out_val.add(new Plcval(i_num,outnum.getVal()));
                        }
                        i_num++;
                    }
                    children.get(gr).set(ch,new PlcControlchildn(tal.getText().toString(),
                            Integer.valueOf(address.getText().toString()),out_val));
//                    children.get(pos).add(new PlcControlchildn(tal.getText().toString(),
//                            Integer.valueOf(address.getText().toString()),out_val));
//
                    adapter.SetItem(groups);
                    sectionedAdapter.SetItem(groups,children);

                    adapter.notifyDataSetChanged();
                    sectionedAdapter.notifyDataSetChanged();

                    EventBus.getDefault().post(new Msgcoir_groups(groups,children));

                    bottomDialog.dismiss();
                }
            }
        });

        out_num_list.clear();

        //赋值
        tal.setText(children.get(gr).get(ch).getName());
        address.setText(String.valueOf(children.get(gr).get(ch).getAddress()));

        for(int i = 0;i < 8;i++) {
            boolean have = true;
            for(Plcval tmp: children.get(gr).get(ch).getOut()) {
                Log.d(TAG, String.valueOf(tmp.getOut_num()));
                Log.d(TAG, String.valueOf(tmp.getOut_num_val()));
                if(i == tmp.getOut_num()) {
                    out_num_list.add(new outnum_info("线圈" + i,true,tmp.getOut_num_val() ) );
                    have = false;
                }
            }
            if(have) {
                out_num_list.add(new outnum_info("线圈" + i,false, 0));
            }
        }
        RecyclerView recyclerView = (RecyclerView) contentView.findViewById(R.id.dialog_newplc_add_out_num);
        recyclerView.setItemAnimator(new DefaultItemAnimator());
        recyclerView.setLayoutManager(new GridLayoutManager(getContext(),1));
        CoilSet myAadapter = new CoilSet(getContext(),out_num_list);
        recyclerView.setAdapter(myAadapter);

        bottomDialog.show();
    }

    private void ShowPlc_menu_add(final int pos) {
        final Dialog bottomDialog = new Dialog(getContext(), R.style.BottomDialog);
        final View contentView = LayoutInflater.from(getContext()).inflate(R.layout.dialog_newplc_menu_add_circle, null);
        bottomDialog.setContentView(contentView);
        ViewGroup.MarginLayoutParams params = (ViewGroup.MarginLayoutParams) contentView.getLayoutParams();
        params.width = getResources().getDisplayMetrics().widthPixels - DensityUtil.dp2px(getContext(), 16f);
        params.bottomMargin = DensityUtil.dp2px(getContext(), 8f);
        contentView.setLayoutParams(params);
        bottomDialog.setCanceledOnTouchOutside(true);
        bottomDialog.getWindow().setGravity(Gravity.CENTER);
        bottomDialog.getWindow().setWindowAnimations(R.style.BottomDialog_Animation);

        final EditText tal = (EditText)contentView.findViewById(R.id.dialog_newplc_add_name);
        final EditText address = (EditText)contentView.findViewById(R.id.dialog_newplc_add_address);

        final Button enter =(Button)contentView.findViewById(R.id.dialog_newplc_add_enter);
        enter.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if("".equals(address.getText().toString().trim())) {
                    enter.setBackgroundResource(R.color.red);
                    enter.setText("地址不能为空");
                } else {
                    int i_num = 0;
                    List<Plcval> out_val = new ArrayList<>();
                    for (outnum_info outnum :out_num_list) {
                        System.out.println("lhb" + outnum.getSel());
                        if(outnum.getSel()) {
                            out_val.add(new Plcval(i_num,outnum.getVal()));
                        }
                        i_num++;
                    }
                    children.get(pos).add(new PlcControlchildn(tal.getText().toString(),
                            Integer.valueOf(address.getText().toString()),out_val));

                    adapter.SetItem(groups);
                    sectionedAdapter.SetItem(groups,children);

                    adapter.notifyDataSetChanged();
                    sectionedAdapter.notifyDataSetChanged();


                    EventBus.getDefault().post(new Msgcoir_groups(groups,children));

                    bottomDialog.dismiss();
                }
            }
        });

        out_num_list.clear();
        for(int i = 0;i < 8;i++) {
            out_num_list.add(new outnum_info("线圈" + i,false,0));
        }
        RecyclerView recyclerView = (RecyclerView) contentView.findViewById(R.id.dialog_newplc_add_out_num);
        recyclerView.setItemAnimator(new DefaultItemAnimator());
        recyclerView.setLayoutManager(new GridLayoutManager(getContext(),1));
        CoilSet myAadapter = new CoilSet(getContext(),out_num_list);
        recyclerView.setAdapter(myAadapter);

        bottomDialog.show();
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

    public class CoilSet extends RecyclerView.Adapter{
        private LayoutInflater inflater;
        private List<outnum_info> list;

        public CoilSet(Context context , List<outnum_info> list) {
            this.list = list;
            inflater = LayoutInflater.from(context);
        }

        @Override
        public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
            return new ViewHolder(inflater.inflate(R.layout.item_listview_outnum,parent,false));
        }

        @Override
        public void onBindViewHolder(RecyclerView.ViewHolder holder, final int position) {
            final ViewHolder viewHolder = (ViewHolder) holder;
            //viewHolder.info.setText(list.get(position).getTitle());
            viewHolder.Selected.setText(list.get(position).getTitle());
            viewHolder.Selected.setChecked(list.get(position).getSel());
            if(list.get(position).getVal() == 0) {
                viewHolder.val.setChecked(false);
                viewHolder.vol_show.setBackgroundResource(R.drawable.bullet_orange);
            } else if(list.get(position).getVal() == 1) {
                viewHolder.val.setChecked(true);
                viewHolder.vol_show.setBackgroundResource(R.drawable.bullet_green);
            } else if(list.get(position).getVal() == 2) {
                viewHolder.val.setChecked(true);
                viewHolder.vol_show.setBackgroundResource(R.drawable.bullet_red);
            }
            viewHolder.Selected.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                    list.get(position).setSel(b);
                }
            });
            viewHolder.val.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                    if(list.get(position).getVal() != 2) {
                        if (b) {
                            list.get(position).setVal(1);
                            viewHolder.vol_show.setBackgroundResource(R.drawable.bullet_green);
                        } else {
                            list.get(position).setVal(0);
                            viewHolder.vol_show.setBackgroundResource(R.drawable.bullet_orange);
                        }
                    }
                }
            });
            viewHolder.vol_toggle.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                    if (b) {
                        list.get(position).setVal(2);
                        viewHolder.vol_show.setBackgroundResource(R.drawable.bullet_red);
                    } else {
                        if(viewHolder.val.isChecked()) {
                            list.get(position).setVal(1);
                            viewHolder.vol_show.setBackgroundResource(R.drawable.bullet_green);
                        } else {
                            list.get(position).setVal(0);
                            viewHolder.vol_show.setBackgroundResource(R.drawable.bullet_orange);
                        }
                    }
                }
            });
        }

        @Override
        public int getItemCount() {
            return list.size();
        }

        private class ViewHolder extends RecyclerView.ViewHolder{
            //private TextView info;
            private CheckBox Selected;
            private Switch val;
            private ImageView vol_show;
            private CheckBox vol_toggle;

            private ViewHolder(View itemView) {
                super(itemView);
                //info=(TextView) itemView.findViewById(R.id.list_outnum_num);
                Selected=(CheckBox) itemView.findViewById(R.id.list_outnum_sel);
                val=(Switch) itemView.findViewById(R.id.list_outnum_val);

                vol_show=(ImageView) itemView.findViewById(R.id.list_outnum_outshow);
                vol_toggle=(CheckBox) itemView.findViewById(R.id.list_outnum_val_toggle);
            }
        }
    }
}
