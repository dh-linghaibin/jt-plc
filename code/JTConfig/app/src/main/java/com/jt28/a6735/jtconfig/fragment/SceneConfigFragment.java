package com.jt28.a6735.jtconfig.fragment;

import android.os.Bundle;
import android.support.annotation.Nullable;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.Spinner;
import android.widget.Switch;
import android.widget.Toast;

import com.jt28.a6735.jtconfig.Msg.CmdMsg;
import com.jt28.a6735.jtconfig.Msg.Msg;
import com.jt28.a6735.jtconfig.R;
import com.jt28.a6735.jtconfig.adapter.ShopcartExpandableListViewAdapter;
import com.jt28.a6735.jtconfig.bean.GroupInfo;
import com.jt28.a6735.jtconfig.bean.ProductInfo;
import com.jt28.a6735.jtconfig.widget.SuperExpandableListView;
import com.jt28.a6735.viewpager.PageFragment;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

/**
 * Created by a6735 on 2017/7/4.
 */

public class SceneConfigFragment extends PageFragment implements ShopcartExpandableListViewAdapter.CheckInterface, View.OnClickListener, ShopcartExpandableListViewAdapter.ModifyCountInterface{
    private SuperExpandableListView exListView;
    private ShopcartExpandableListViewAdapter selva;
    private List<GroupInfo> groups = new ArrayList<>();// 组元素数据列表
    private Map<String, List<ProductInfo>> children = new LinkedHashMap<>();// 子元素数据列表

    private android.app.AlertDialog.Builder builder = null;
    private View view_custom;
    private android.app.AlertDialog alert = null;

    private android.app.AlertDialog.Builder builder2 = null;
    private View view_custom2;
    private android.app.AlertDialog alert2 = null;

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);

//        for (int i = 0; i < 10; i++) {
//            groups.add(new GroupInfo(i + "","分组" + i));
//            List<ProductInfo> products = new ArrayList<>();
//            for (int j = 0; j <= i; j++) {
//                products.add(new ProductInfo( j + "", "灯" + j,"备注" + j,String.valueOf(j),"输出"+j,true ));
//            }
//            children.put(groups.get(i).getId(), products);// 将组元素的一个唯一值，这里取Id，作为子元素List的Key
//        }

        exListView = (SuperExpandableListView) getActivity().findViewById(R.id.sceneconfigfragment_exListView);

        selva = new ShopcartExpandableListViewAdapter(groups, children, getContext());
        selva.setCheckInterface(this);// 关键步骤1,设置复选框接口
        selva.setModifyCountInterface(this);// 关键步骤2,设置数量增减接口
        exListView.setAdapter(selva);
        for (int i = 0; i < selva.getGroupCount(); i++) {
            exListView.expandGroup(i);// 关键步骤3,初始化时，将ExpandableListView以展开的方式呈现
        }

        exListView.setOnItemLongClickListener(new AdapterView.OnItemLongClickListener() {
            @Override
            public boolean onItemLongClick(AdapterView<?> parent, View view, int position, long id) {
                String content = "";
                if ((int) view.getTag(R.layout.item_shopcart_product) == -1) {
                    content = "父类第" + view.getTag(R.layout.item_shopcart_group) + "项" + "被长按了";
                } else {
                    content = "父类第" + view.getTag(R.layout.item_shopcart_group) + "项" + "中的"
                            + "子类第" + view.getTag(R.layout.item_shopcart_product) + "项" + "被长按了";
                }

                Toast.makeText(getContext(), content, Toast.LENGTH_SHORT).show();

                return true;
            }
        });

        //初始化Builder
        builder = new android.app.AlertDialog.Builder(getActivity());
        //加载自定义的那个View,同时设置下'
        final LayoutInflater inflater = getActivity().getLayoutInflater();
        view_custom = inflater.inflate(R.layout.add_tal, null, false);
        builder.setView(view_custom);
        builder.setCancelable(false);
        alert = builder.create();
        final EditText name = (EditText) view_custom.findViewById(R.id.addbar_name);
        final EditText address = (EditText) view_custom.findViewById(R.id.add_tal_address);
        final Spinner jdq_switch = (Spinner)view_custom.findViewById(R.id.add_tal_spin);
        Switch sw= (Switch) view_custom.findViewById(R.id.add_sw1);
        sw.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                if(b) {
                    Toast.makeText(getContext(), "开", Toast.LENGTH_SHORT).show();
                } else {
                    Toast.makeText(getContext(), "关", Toast.LENGTH_SHORT).show();
                }
            }
        });
        view_custom.findViewById(R.id.add_but_close).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                alert.dismiss();
            }
        });
        view_custom.findViewById(R.id.add_but_ok).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //添加子 -- 继电器数据
                children.get(groups.get(xiugai).getId()).add(new ProductInfo(1 + "","1",name.getText().toString(),address.getText().toString(),jdq_switch.getSelectedItem().toString(),true));
                for (int i = 0; i < selva.getGroupCount(); i++) {
                    exListView.expandGroup(i);// 关键步骤3,初始化时，将ExpandableListView以展开的方式呈现
                }
                selva.notifyDataSetChanged();
                alert.dismiss();
            }
        });

        //初始化Builder
        builder2 = new android.app.AlertDialog.Builder(getContext());
        //加载自定义的那个View,同时设置下'
        final LayoutInflater inflater2 =getActivity().getLayoutInflater();
        view_custom2 = inflater2.inflate(R.layout.add_bar, null, false);
        builder2.setView(view_custom2);
        builder2.setCancelable(true);
        alert2 = builder2.create();
        final EditText changj = (EditText) view_custom2.findViewById(R.id.addbar_name);
        view_custom2.findViewById(R.id.addbar_but_close).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                alert2.dismiss();
            }
        });
        view_custom2.findViewById(R.id.addbar_but_ok).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                groups.add(new GroupInfo(groups.size()+1 + "",changj.getText().toString()));
                List<ProductInfo> products = new ArrayList<>();
                children.put(groups.get(groups.size() - 1).getId(), products);// 将组元素的一个唯一值，这里取Id，作为子元素List的Key
                selva.notifyDataSetChanged();
                alert2.dismiss();
            }
        });

        Button add = (Button) getActivity().findViewById(R.id.sceneconfigfragment_add);
        add.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                alert2.show();
            }
        });
    }

    @Nullable
    @Override
    public View onCreateView(LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.sceneconfigfragment, container, false);
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

    //组添加小
    private int xiugai = 0;
    @Override
    public void checkButAdd(int groupPosition) {
        xiugai = groupPosition;
        alert.show();
    }

    @Override
    public void doChange(int groupPosition) {

    }

    @Override
    public void doIncrease(int groupPosition, int childPosition, View showCountView, boolean isChecked) {

    }

    @Override
    public void doDecrease(int groupPosition, int childPosition, View showCountView, boolean isChecked) {

    }

    @Override
    public void doChange(int groupPosition, int childPosition, View showCountView, boolean isChecked) {

    }

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(Msg event) {
        /* Do something */
        System.out.println("lhb_a" + event.GetGroups());
    };

    @Subscribe(threadMode = ThreadMode.MAIN)
    public void onMessageEvent(CmdMsg event) {
        /* Do something */
        System.out.println("lhb_cmd" + event.getCmd());
        if(event.getCmd().equals("GetScene")) {
            EventBus.getDefault().post(new Msg(groups,children));
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
