package com.jt28.a6735.jtzmconfig.adapter;


import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.jt28.a6735.jtzmconfig.R;
import com.jt28.a6735.jtzmconfig.model.PlcControl;
import com.jt28.a6735.jtzmconfig.model.PlcControlchildn;
import com.jt28.a6735.jtzmconfig.model.Plcval;

import java.util.List;


/**
 * 基本功能：右侧Adapter
 * 创建：王杰
 * 创建时间：16/4/14
 * 邮箱：w489657152@gmail.com
 */
public class MainSectionedAdapter extends SectionedBaseAdapter {
    private Context mContext;

    private List<PlcControl> leftStr;
    private List<List<PlcControlchildn>> rightStr;

    public MainSectionedAdapter(Context context, List<PlcControl> leftStr, List<List<PlcControlchildn>> rightStr) {
        this.mContext = context;
        this.leftStr = leftStr;
        this.rightStr = rightStr;
    }

    public void SetItem(List<PlcControl> leftStr, List<List<PlcControlchildn>> rightStr) {
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
            layout = (RelativeLayout) inflator.inflate(R.layout.right_list_item, null);
        } else {
            layout = (RelativeLayout) convertView;
        }

        ((TextView) layout.findViewById(R.id.textItem)).setText(rightStr.get(section).get(position).getName());
        ((TextView) layout.findViewById(R.id.imageItem)).setText(String.valueOf(rightStr.get(section).get(position).getAddress()));
        final LinearLayout menu= (LinearLayout)layout.findViewById(R.id.right_list_menu);
        //menu.setVisibility(View.GONE);
        menu.setAnimation(AnimationUtil.moveToViewLocation());

        ImageView[] Img = new ImageView[10];
        Img[0] = (ImageView) layout.findViewById(R.id.img_t1);
        Img[1] = (ImageView) layout.findViewById(R.id.img_t2);
        Img[2] = (ImageView) layout.findViewById(R.id.img_t3);
        Img[3] = (ImageView) layout.findViewById(R.id.img_t4);
        Img[4] = (ImageView) layout.findViewById(R.id.img_t5);
        Img[5] = (ImageView) layout.findViewById(R.id.img_t6);
        Img[6] = (ImageView) layout.findViewById(R.id.img_t7);
        Img[7] = (ImageView) layout.findViewById(R.id.img_t8);
        TextView but_sub = (TextView) layout.findViewById(R.id.right_list_menu_sub);
        TextView but_change = (TextView) layout.findViewById(R.id.right_list_menu_change);

        for(int i = 0;i < 8;i++) {
            Img[i].setBackgroundResource(R.drawable.bullet_white);
        }

        for(Plcval xx: rightStr.get(section).get(position).getOut()) {
            if(xx.getOut_num_val() == 0) {
                Img[xx.getOut_num()].setBackgroundResource(R.drawable.bullet_orange);
            } else if(xx.getOut_num_val() == 1) {
                Img[xx.getOut_num()].setBackgroundResource(R.drawable.bullet_green);
            } else if(xx.getOut_num_val() == 2) {
                Img[xx.getOut_num()].setBackgroundResource(R.drawable.bullet_red);
            }
        }

        but_sub.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                if(m_MenuInterface != null) {
                    m_MenuInterface.doSub(section,position);
                }
        }});
        but_change.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View view) {
                if(m_MenuInterface != null) {
                    m_MenuInterface.dochange(section,position);
                }
            }
        });

        layout.setOnClickListener(new OnClickListener() {
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
        public void dochange(int gr,int ch);
    }
}
