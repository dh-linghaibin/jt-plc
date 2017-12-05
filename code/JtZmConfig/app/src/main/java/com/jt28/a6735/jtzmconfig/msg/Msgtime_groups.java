package com.jt28.a6735.jtzmconfig.msg;

import com.jt28.a6735.jtzmconfig.model.PlcTimer;
import com.jt28.a6735.jtzmconfig.model.Scenechild;

import java.util.List;

/**
 * Created by a6735 on 2017/7/31.
 */

public class Msgtime_groups {
    private List<PlcTimer> time_groups;
    private List<List<Scenechild>> time_children;

    public Msgtime_groups(List<PlcTimer> time_groups, List<List<Scenechild>> time_children) {
        this.time_groups = time_groups;
        this.time_children = time_children;
    }

    public void setTime_groups(List<PlcTimer> time_groups) {
        this.time_groups = time_groups;
    }

    public void setTime_children(List<List<Scenechild>> time_children) {
        this.time_children = time_children;
    }

    public List<List<Scenechild>> getTime_children() {
        return time_children;
    }

    public List<PlcTimer> getTime_groups() {
        return time_groups;
    }
}
