package com.jt28.a6735.jtzmconfig.model;

import java.io.Serializable;
import java.util.List;

/**
 * Created by a6735 on 2017/7/11.
 */

public class PlcControlchildn implements Serializable {
    private String name;
    private int address;
    private List<Plcval> val;
    public PlcControlchildn(String name, int address, List<Plcval> val) {
        this.name = name;
        this.address = address;
        this.val = val;
    }
    public void setName(String name) {
        this.name = name;
    }

    public void setAddress(int address) {
        this.address = address;
    }

    public void setOut(List<Plcval> out_num) {
        this.val = out_num;
    }

    public String getName() {
        return name;
    }

    public int getAddress() {
        return address;
    }

    public List<Plcval> getOut() {
        return val;
    }
}
