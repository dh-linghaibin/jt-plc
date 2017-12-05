package com.jt28.a6735.jtzmconfig.adapter;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.jt28.a6735.jtzmconfig.R;
import com.jt28.a6735.jtzmconfig.model.Scene;
import com.jt28.a6735.jtzmconfig.model.Scenechild;

import java.util.List;

/**
 * Created by a6735 on 2017/7/27.
 */

public class ButSectionedAdapter extends SectionedBaseAdapter {
    private Context mContext;

    private List<Scene> leftStr;
    private List<List<Scenechild>> rightStr;

    public ButSectionedAdapter(Context context, List<Scene> leftStr, List<List<Scenechild>> rightStr) {
        this.mContext = context;
        this.leftStr = leftStr;
        this.rightStr = rightStr;
    }

    public void SetItem(List<Scene> leftStr, List<List<Scenechild>> rightStr) {
        this.leftStr = leftStr;
        this.rightStr = rightStr;
    }

    @Override
    public Object getItem(int section, int position) {
        return rightStr.get(section).get(position);
    }

    @Override
    public long getItemId(int section, int position) {
        return position;
    }

    @Override
    public int getSectionCount() {
        return leftStr.size();
    }

    @Override
    public int getCountForSection(int section) {
        return rightStr.get(section).size();
    }

    @Override
    public View getItemView(final int section, final int position, View convertView, ViewGroup parent) {
        RelativeLayout layout = null;
        if (convertView == null) {
            LayoutInflater inflator = (LayoutInflater) parent.getContext()
                    .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            layout = (RelativeLayout) inflator.inflate(R.layout.right_list_item_scene, null);
        } else {
            layout = (RelativeLayout) convertView;
        }

        ((TextView) layout.findViewById(R.id.right_list_scene_name)).setText(rightStr.get(section).get(position).getName());

        final LinearLayout menu= (LinearLayout)layout.findViewById(R.id.right_list_scene_menu);
        menu.setAnimation(AnimationUtil.moveToViewLocation());

        TextView but_sub = (TextView) layout.findViewById(R.id.right_list_scene_menu_sub);

        but_sub.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(m_MenuInterface != null) {
                    m_MenuInterface.doSub(section,position);
                }
            }});

        layout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View arg0) {
                if(menu.getVisibility() == View.GONE) {
                    menu.setVisibility(View.VISIBLE);
                } else {
                    menu.setVisibility(View.GONE);
                }
                menu.setAnimation(AnimationUtil.moveToViewLocation());
            }
        });
        return layout;
    }

    @Override
    public View getSectionHeaderView(int section, View convertView, ViewGroup parent) {
        LinearLayout layout = null;
        if (convertView == null) {
            LayoutInflater inflator = (LayoutInflater) parent.getContext()
                    .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
            layout = (LinearLayout) inflator.inflate(R.layout.header_item, null);
        } else {
            layout = (LinearLayout) convertView;
        }
        layout.setClickable(false);
        ((TextView) layout.findViewById(R.id.textItem)).setText(leftStr.get(section).getName());
        return layout;
    }

    private MenuInterface m_MenuInterface;

    public void SetMenuInterface(MenuInterface m_MenuInterface) {
        this.m_MenuInterface = m_MenuInterface;
    }

    public interface MenuInterface {
        public void doSub(int gr,int ch);
    }
}
