package com.jt28.a6735.jtconfig.adapter;

import android.content.Context;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseExpandableListAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.TextView;

import com.jt28.a6735.jtconfig.R;
import com.jt28.a6735.jtconfig.bean.SceneInfo;
import com.jt28.a6735.jtconfig.widget.SlideView;

import java.util.List;
import java.util.Map;

/**
 * Created by a6735 on 2017/6/30.
 */

public class SceneListViewAdapter extends BaseExpandableListAdapter {
    private List<SceneInfo> groups;
    private Map<String, List<SceneInfo>> children;
    private Context context;
    private CheckInterface checkInterface;
    private ModifyCountInterface modifyCountInterface;

    /**
     * 构造函数
     *
     * @param groups   组元素列表
     * @param children 子元素列表
     * @param context
     */
    public SceneListViewAdapter(List<SceneInfo> groups, Map<String, List<SceneInfo>> children, Context context) {
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
        List<SceneInfo> childs = children.get(groups.get(groupPosition).getId());

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

    //list列表
    @Override
    public View getGroupView(final int groupPosition, boolean isExpanded, View convertView, ViewGroup parent) {
        SceneListViewAdapter.GroupHolder gholder;
        SlideView slideView2 = null;
        if (convertView == null) {
            gholder = new SceneListViewAdapter.GroupHolder();
            convertView = View.inflate(context, R.layout.item_scene_product, null);
            slideView2 = new SlideView(context, context.getResources(), convertView);
            convertView = slideView2;

            gholder.cb_check = (CheckBox) convertView.findViewById(R.id.item_senepr_chekbox);
            gholder.tv_group_name = (TextView) convertView.findViewById(R.id.item_senepr_name);
            gholder.b_add = (Button) convertView.findViewById(R.id.item_senepr_add);

            convertView.setTag(gholder);
        } else {
            gholder = (SceneListViewAdapter.GroupHolder) convertView.getTag();
        }
        final SceneInfo group = (SceneInfo) getGroup(groupPosition);
        if (group != null) {
            gholder.tv_group_name.setText(group.getName());
            gholder.cb_check.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    group.setChoosed(((CheckBox) v).isChecked());
                    checkInterface.checkGroup(groupPosition, ((CheckBox) v).isChecked());// 暴露组选接口
                }
            });
            gholder.cb_check.setChecked(group.getChoosed());
            gholder.b_add.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    checkInterface.checkButAdd(groupPosition);
                }
            });
        }
        return convertView;
    }

    //子选项的样式
    @Override
    public View getChildView(final int groupPosition, final int childPosition, boolean isLastChild, View convertView, ViewGroup parent) {
        SlideView slideView = null;
        final SceneListViewAdapter.ChildHolder cholder;
        if (convertView == null) {
            cholder = new SceneListViewAdapter.ChildHolder();
            View view = View.inflate(context, R.layout.item_scene_group, null);
            slideView = new SlideView(context, context.getResources(), view);
            convertView = slideView;

            cholder.cb_check = (CheckBox) convertView.findViewById(R.id.item_sene_chekbox);
            cholder.tv_delete = (TextView) convertView.findViewById(R.id.back);
            cholder.tv_change = (TextView) convertView.findViewById(R.id.shenhe);
            cholder.cb_name = (TextView) convertView.findViewById(R.id.item_sene_name);

            convertView.setTag(cholder);
        } else {
            cholder = (SceneListViewAdapter.ChildHolder) convertView.getTag();
        }
        final SceneInfo product = (SceneInfo) getChild(groupPosition, childPosition);

        if (product != null) {

            cholder.cb_check.setChecked(product.getChoosed());
            cholder.cb_name.setText(product.getName());
            cholder.cb_check.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    product.setChoosed(((CheckBox) v).isChecked());
                    cholder.cb_check.setChecked(((CheckBox) v).isChecked());
                    checkInterface.checkChild(groupPosition, childPosition, ((CheckBox) v).isChecked());// 暴露子选接口
                }
            });
            cholder.tv_change.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    modifyCountInterface.doChange(groupPosition, childPosition, cholder.tv_change);
                }
            });
        }
        cholder.tv_delete.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                List<SceneInfo> childs = children.get(groups.get(groupPosition).getId());
                childs.remove(childPosition);
                if(childs.size() ==0){//child没有了，group也就没有了
                   // groups.remove(groupPosition);
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
    }

    /**
     * 子元素绑定器
     */
    private class ChildHolder {
        CheckBox cb_check;//输出状态
        TextView cb_name;
        TextView tv_delete;
        TextView tv_change;
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
         */
        public void doChange(int groupPosition, int childPosition, View showCountView);
    }
}
