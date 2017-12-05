package com.jt28.a6735.jtzmconfig.msg;

import com.jt28.a6735.jtzmconfig.model.PlcControl;
import com.jt28.a6735.jtzmconfig.model.PlcControlchildn;

import java.util.List;

/**
 * Created by a6735 on 2017/7/31.
 */

public class Msgcoir_groups {
    private List<PlcControl> coir_groups;
    private List<List<PlcControlchildn>> coir_children;

    public Msgcoir_groups(List<PlcControl> coir_groups, List<List<PlcControlchildn>> coir_children){
        this.coir_groups = coir_groups;
        this.coir_children = coir_children;
    }

    public List<List<PlcControlchildn>> getCoir_children() {
        return coir_children;
    }
    public List<PlcControl> getCoir_groups() {
        return coir_groups;
    }

    public void setCoir_children(List<List<PlcControlchildn>> coir_children) {
        this.coir_children = coir_children;
    }

    public void setCoir_groups(List<PlcControl> coir_groups) {
        this.coir_groups = coir_groups;
    }
}
