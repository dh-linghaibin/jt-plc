package com.jt28.a6735.jtzmconfig.adapter;

import android.content.Context;
import android.support.v7.widget.RecyclerView;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.TextView;

import com.jt28.a6735.jtzmconfig.R;
import com.jt28.a6735.jtzmconfig.model.FileLs;

import java.util.List;

/**
 * Created by a6735 on 2017/7/28.
 */

public class FileAadapter extends RecyclerView.Adapter {
    private LayoutInflater inflater;
    public List<FileLs> list;

    public FileAadapter(Context context , List<FileLs> list) {
        this.list = list;
        inflater = LayoutInflater.from(context);
    }

    @Override
    public RecyclerView.ViewHolder onCreateViewHolder(ViewGroup parent, int viewType) {
        return new ViewHolder(inflater.inflate(R.layout.item_filels,parent,false));
    }

    @Override
    public void onBindViewHolder(RecyclerView.ViewHolder holder, final int position) {
        final ViewHolder viewHolder = (ViewHolder) holder;
        viewHolder.file_name.setText(list.get(position).getName());
        viewHolder.file_name.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(m_ClickInterface != null) {
                    m_ClickInterface.Click(position);
                }
            }
        });
    }

    @Override
    public int getItemCount() {
        return list.size();
    }

    private class ViewHolder extends RecyclerView.ViewHolder{

        private TextView file_name;

        public ViewHolder(View itemView) {
            super(itemView);
            file_name = (TextView) itemView.findViewById(R.id.item_file_name);
        }
    }

    private ClickInterface m_ClickInterface;

    public void SetClickInterface(ClickInterface m_ClickInterface) {
        this.m_ClickInterface = m_ClickInterface;
    }

    public interface ClickInterface {
        public void Click(int position);
    }
}
