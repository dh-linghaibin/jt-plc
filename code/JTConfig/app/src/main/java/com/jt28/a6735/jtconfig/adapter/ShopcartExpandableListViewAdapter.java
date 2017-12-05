package com.jt28.a6735.jtconfig.adapter;

import android.content.Context;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.CompoundButton;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;

import com.jt28.a6735.jtconfig.R;
import com.jt28.a6735.jtconfig.bean.GroupInfo;
import com.jt28.a6735.jtconfig.bean.ProductInfo;
import com.jt28.a6735.jtconfig.widget.SlideView;

import java.util.List;
import java.util.Map;


public class ShopcartExpandableListViewAdapter extends BaseExpandableListAdapter {
    private List<GroupInfo> groups;
    private Map<String, List<ProductInfo>> children;
    private Context context;
    //HashMap<Integer, View> groupMap = new HashMap<Integer, View>();
    //HashMap<Integer, View> childrenMap = new HashMap<Integer, View>();
    private CheckInterface checkInterface;
    private ModifyCountInterface modifyCountInterface;

    /**
     * 构造函数
     *
     * @param groups   组元素列表
     * @param children 子元素列表
     * @param context
     */
    public ShopcartExpandableListViewAdapter(List<GroupInfo> groups, Map<String, List<ProductInfo>> children, Context context) {
        super();
        this.groups = groups;
        this.children = children;
        this.context = context;
    }

    public void setCheckInterface(CheckInterface checkInterface) {
        this.checkInterface = checkInterface;
    }

    public void setModifyCountInterface(ModifyCountInterface modifyCountInterface) {
        this.modifyCountInterface = modifyCountInterface;
    }

    @Override
    public int getGroupCount() {
        return groups.size();
    }

    @Override
    public int getChildrenCount(int groupPosition) {
        String groupId = groups.get(groupPosition).getId();
        return children.get(groupId).size();
    }

    @Override
    public Object getGroup(int groupPosition) {
        return groups.get(groupPosition);
    }

    @Override
    public Object getChild(int groupPosition, int childPosition) {
        List<ProductInfo> childs = children.get(groups.get(groupPosition).getId());

        return childs.get(childPosition);
    }

    @Override
    public long getGroupId(int groupPosition) {
        return groupPosition;
    }

    @Override
    public long getChildId(int groupPosition, int childPosition) {
        return childPosition;
    }

    @Override
    public boolean hasStableIds() {
        return false;
    }

    @Override
    public View getGroupView(final int groupPosition, boolean isExpanded, View convertView, ViewGroup parent) {
        SlideView slideView2 = null;
        GroupHolder gholder;
        if (convertView == null) {
            gholder = new GroupHolder();
            convertView = View.inflate(context, R.layout.item_shopcart_group, null);
            slideView2 = new SlideView(context, context.getResources(), convertView);

            convertView = slideView2;

            convertView.setTag(R.layout.item_shopcart_group, groupPosition);
            convertView.setTag(R.layout.item_shopcart_product, -1);

            gholder.cb_check = (CheckBox) convertView.findViewById(R.id.determine_chekbox);
            gholder.tv_group_name = (TextView) convertView.findViewById(R.id.tv_source_name);
            gholder.b_add = (Button) convertView.findViewById(R.id.tv_source_add);

            gholder.tv_change = (TextView) convertView.findViewById(R.id.shenhe);
            gholder.tv_delete = (TextView) convertView.findViewById(R.id.back);
            //groupMap.put(groupPosition, convertView);
            convertView.setTag(gholder);
        } else {
            // convertView = groupMap.get(groupPosition);
            gholder = (GroupHolder) convertView.getTag();
        }
        final GroupInfo group = (GroupInfo) getGroup(groupPosition);
        if (group != null) {
            gholder.tv_group_name.setText(group.getName());
            gholder.cb_check.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v)

                {
                    group.setChoosed(((CheckBox) v).isChecked());
                    checkInterface.checkGroup(groupPosition, ((CheckBox) v).isChecked());// 暴露组选接口
                }
            });
            gholder.cb_check.setChecked(group.isChoosed());
            gholder.b_add.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View view) {
                    checkInterface.checkButAdd(groupPosition);
                }
            });
            gholder.tv_delete.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View view) {
                    checkInterface.doChange(groupPosition);
                }
            });
        }
        return convertView;
    }

    @Override
    public View getChildView(final int groupPosition, final int childPosition, boolean isLastChild, View convertView, ViewGroup parent) {
        SlideView slideView = null;
        final ChildHolder cholder;
        if (convertView == null) {
            cholder = new ChildHolder();
            View view = View.inflate(context, R.layout.item_shopcart_product, null);
            slideView = new SlideView(context, context.getResources(), view);
            convertView = slideView;

            view.setTag(R.layout.item_shopcart_group, groupPosition);
            view.setTag(R.layout.item_shopcart_product, childPosition);

            cholder.cb_check = (CheckBox) convertView.findViewById(R.id.check_box);

            cholder.tv_product_desc = (TextView) convertView.findViewById(R.id.tv_intro);
//            cholder.tv_price = (TextView) convertView.findViewById(R.id.tv_price);
//            cholder.iv_increase = (TextView) convertView.findViewById(R.id.tv_add);
//            cholder.iv_decrease = (TextView) convertView.findViewById(R.id.tv_reduce);
//            cholder.tv_count = (TextView) convertView.findViewById(R.id.tv_num);
            cholder.tv_sw = (Switch) convertView.findViewById(R.id.tv_sw);
            cholder.tv_delete = (TextView) convertView.findViewById(R.id.back);
            cholder.tv_change = (TextView) convertView.findViewById(R.id.shenhe);
            cholder.tv_product_mage = (ImageView) convertView.findViewById(R.id.iv_adapter_list_pic);

            cholder.tv_product_address = (TextView) convertView.findViewById(R.id.iv_adapter_list_address);
            cholder.tv_product_out = (TextView) convertView.findViewById(R.id.tv_intro_id);

            if( cholder.tv_sw.isChecked() ) {
                cholder.tv_product_mage.setBackgroundResource(R.mipmap.light62);
            } else {
                cholder.tv_product_mage.setBackgroundResource(R.mipmap.lightbulb14);
            }
            // childrenMap.put(groupPosition, convertView);
            convertView.setTag(cholder);
        } else {
            // convertView = childrenMap.get(groupPosition);
            cholder = (ChildHolder) convertView.getTag();
        }
        final ProductInfo product = (ProductInfo) getChild(groupPosition, childPosition);

        if (product != null) {
            cholder.tv_product_address.setText(product.getaddress());
            cholder.tv_product_out.setText(product.getout());
            cholder.tv_sw.setChecked(product.getsw_bit());
            cholder.tv_product_desc.setText(product.getDesc());
//            cholder.tv_price.setText("￥" + product.getPrice() + "");
//            cholder.tv_count.setText(product.getCount() + "")
            cholder.cb_check.setChecked(product.isChoosed());
            cholder.cb_check.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View v) {
                    product.setChoosed(((CheckBox) v).isChecked());
                    cholder.cb_check.setChecked(((CheckBox) v).isChecked());
                    checkInterface.checkChild(groupPosition, childPosition, ((CheckBox) v).isChecked());// 暴露子选接口
                }
            });
//            cholder.iv_increase.setOnClickListener(new OnClickListener() {
//                @Override
//                public void onClick(View v) {
//                    modifyCountInterface.doIncrease(groupPosition, childPosition, cholder.tv_count, cholder.cb_check.isChecked());// 暴露增加接口
//                }
//            });
//            cholder.iv_decrease.setOnClickListener(new OnClickListener() {
//                @Override
//                public void onClick(View v) {
//                    modifyCountInterface.doDecrease(groupPosition, childPosition, cholder.tv_count, cholder.cb_check.isChecked());// 暴露删减接口
//                }
//            });
            cholder.tv_sw.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
                @Override
                public void onCheckedChanged(CompoundButton compoundButton, boolean b) {
                    product.setsw_bit(b);
                    if( b ) {
                        cholder.tv_product_mage.setBackgroundResource(R.mipmap.light62);
                    } else {
                        cholder.tv_product_mage.setBackgroundResource(R.mipmap.lightbulb14);
                    }
                }
            });
            cholder.tv_change.setOnClickListener(new OnClickListener() {
                @Override
                public void onClick(View view) {
                    modifyCountInterface.doChange(groupPosition, childPosition, cholder.tv_change, cholder.tv_sw.isChecked());
                }
            });
        }
        cholder.tv_delete.setOnClickListener(new OnClickListener() {

            @Override
            public void onClick(View v) {
                List<ProductInfo> childs = children.get(groups.get(groupPosition).getId());
                childs.remove(childPosition);
                if(childs.size() ==0){//child没有了，group也就没有了
                    groups.remove(groupPosition);
                }
                notifyDataSetChanged();
            }
        });
        return convertView;
    }

    @Override
    public boolean isChildSelectable(int groupPosition, int childPosition) {
        return false;
    }

    /**
     * 组元素绑定器
     */
    private class GroupHolder {
        CheckBox cb_check;
        TextView tv_group_name;
        Button b_add;
        TextView tv_delete;
        TextView tv_change;
    }

    /**
     * 子元素绑定器
     */
    private class ChildHolder {
        CheckBox cb_check;//输出状态
        TextView tv_product_name;
        TextView tv_product_desc;//备注
        ImageView tv_product_mage;//图片
        TextView tv_product_address;//地址
        TextView tv_product_out;//输出点
        //TextView tv_price;
        //TextView iv_increase;
        //TextView tv_count;
        //TextView iv_decrease;
        TextView tv_delete;
        TextView tv_change;
        Switch tv_sw;
    }
    /**
     * 复选框接口
     */
    public interface CheckInterface {
        /**
         * 组选框状态改变触发的事件
         *
         * @param groupPosition 组元素位置
         * @param isChecked     组元素选中与否
         */
        public void checkGroup(int groupPosition, boolean isChecked);

        /**
         * 子选框状态改变时触发的事件
         *
         * @param groupPosition 组元素位置
         * @param childPosition 子元素位置
         * @param isChecked     子元素选中与否
         */
        public void checkChild(int groupPosition, int childPosition, boolean isChecked);

        /**
         * 子选框状态改变时触发的事件
         *
         * @param groupPosition 组元素位置
         */
        public void checkButAdd(int groupPosition);
        /**
         * 修改操作
         *
         * @param groupPosition 组元素位置
         */
        public void doChange(int groupPosition);
    }

    /**
     * 改变数量的接口
     */
    public interface ModifyCountInterface {
        /**
         * 增加操作
         *
         * @param groupPosition 组元素位置
         * @param childPosition 子元素位置
         * @param showCountView 用于展示变化后数量的View
         * @param isChecked     子元素选中与否
         */
        public void doIncrease(int groupPosition, int childPosition, View showCountView, boolean isChecked);

        /**
         * 删减操作
         *
         * @param groupPosition 组元素位置
         * @param childPosition 子元素位置
         * @param showCountView 用于展示变化后数量的View
         * @param isChecked     子元素选中与否
         */
        public void doDecrease(int groupPosition, int childPosition, View showCountView, boolean isChecked);
        /**
         * 修改操作
         *
         * @param groupPosition 组元素位置
         * @param childPosition 子元素位置
         * @param showCountView 用于展示变化后数量的View
         * @param isChecked     子元素选中与否
         */
        public void doChange(int groupPosition, int childPosition, View showCountView, boolean isChecked);
    }

}
