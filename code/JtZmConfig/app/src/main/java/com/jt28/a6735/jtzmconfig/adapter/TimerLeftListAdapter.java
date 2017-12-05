package com.jt28.a6735.jtzmconfig.adapter;

import android.content.Context;
import android.graphics.Color;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.jt28.a6735.jtzmconfig.R;
import com.jt28.a6735.jtzmconfig.model.PlcTimer;

import java.util.List;

/**
 * Created by a6735 on 2017/7/28.
 */

public class TimerLeftListAdapter extends BaseAdapter {
    private Context context;
    private List<PlcTimer> leftStr;

    public TimerLeftListAdapter(Context context,List<PlcTimer> leftStr) {
        this.leftStr = leftStr;
        this.context = context;
    }

    public void SetItem(List<PlcTimer> leftStr) {
        this.leftStr = leftStr;
    }

    @Override
    public int getCount() {
        return leftStr.size();
    }

    @Override
    public Object getItem(int arg0) {
        return leftStr.get(arg0);
    }

    @Override
    public long getItemId(int arg0) {
        return arg0;
    }

    @Override
    public View getView(final int arg0, View arg1, ViewGroup arg2) {
        Holder holder = null;
        if (arg1 == null) {
            holder = new Holder();
            arg1 = LayoutInflater.from(context).inflate(R.layout.timer_left_list_item, null);

            holder.item = (LinearLayout) arg1.findViewById(R.id.timeleft_list_item);
            holder.name = arg1.findViewById(R.id.timeleft_list_name);
            holder.month = arg1.findViewById(R.id.timeleft_list_month);
            holder.wday = arg1.findViewById(R.id.timeleft_list_wday);
            holder.hour = arg1.findViewById(R.id.timeleft_list_hour);
            holder.min = arg1.findViewById(R.id.timeleft_list_min);

            holder.menu= (LinearLayout)arg1.findViewById(R.id.timeleft_list_menu);
            holder.menu.setAnimation(AnimationUtil.moveToViewLocation());
            holder.but_add = arg1.findViewById(R.id.timeleft_list_menu_add);
            holder.but_sub = arg1.findViewById(R.id.timeleft_list_menu_sub);

            arg1.setTag(holder);
        } else {
            holder = (Holder) arg1.getTag();
        }
        final Holder finalHolder = holder;

        arg1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(leftStr.get(arg0).isFlagArray()) {
                    if (finalHolder.menu.getVisibility() == View.GONE) {
                        finalHolder.menu.setVisibility(View.VISIBLE);
                        finalHolder.menu.setAnimation(AnimationUtil.moveToViewLocation());
                        leftStr.get(arg0).setMenuflag(true);
                    } else {
                        finalHolder.menu.setVisibility(View.GONE);
                        leftStr.get(arg0).setMenuflag(false);
                    }

                }
                if(m_MenuInterface != null) {
                    m_MenuInterface.doItem(arg0);
                }
            }
        });
        finalHolder.but_add.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(m_MenuInterface != null) {
                    m_MenuInterface.doAdd(arg0);
                }
            }
        });

        finalHolder.but_sub.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(m_MenuInterface != null) {
                    m_MenuInterface.doSub(arg0);
                    finalHolder.menu.setVisibility(View.GONE);
                }
            }
        });

        holder.updataView(arg0);

        return arg1;
    }


    private class Holder {
        private LinearLayout item;
        private TextView name;
        private TextView month;
        private TextView wday;
        private TextView hour;
        private TextView min;

        private Button but_add;
        private Button but_sub;
        private LinearLayout menu;

        public void updataView(final int position) {
            name.setText(leftStr.get(position).getName());
            String s_mon = "";
            if(leftStr.get(position).getMonth() >= 12) {
                s_mon = "总是";
            } else {
                s_mon = String.valueOf(leftStr.get(position).getMonth()+1);
            }
            month.setText( s_mon );
            String s_wday = "";
            if(leftStr.get(position).getWday() >= 7) {
                s_wday = "总是";
            } else {
                s_wday = String.valueOf(leftStr.get(position).getWday()+1);
            }
            wday.setText(s_wday);
            hour.setText(String.valueOf(leftStr.get(position).getHour()));
            min.setText(String.valueOf(leftStr.get(position).getMin()));

            if (!leftStr.get(position).isMenuflag()) {
                menu.setVisibility(View.GONE);
            }

            if (leftStr.get(position).isFlagArray()) {
                item.setBackgroundColor(Color.rgb(255, 255, 255));
            } else {
                item.setBackgroundColor(Color.TRANSPARENT);
            }
        }
    }

    private MenuInterface m_MenuInterface;

    public void SetMenuInterface(MenuInterface m_MenuInterface) {
        this.m_MenuInterface = m_MenuInterface;
    }

    public interface MenuInterface {
        public void doItem(int groupPosition);
        public void doAdd(int groupPosition);
        public void doSub(int groupPosition);
    }
}
