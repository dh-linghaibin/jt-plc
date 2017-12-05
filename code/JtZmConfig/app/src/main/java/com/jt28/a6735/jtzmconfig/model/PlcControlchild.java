package com.jt28.a6735.jtzmconfig.model;

import java.io.Serializable;

/**
 * Created by a6735 on 2017/7/8.
 */

public class PlcControlchild implements Serializable {
    private int id;
    private String name;
    private int address;
    private int out_num;
    private boolean out_num_val;
    public PlcControlchild(int id, String name, int address, int out_num, boolean out_num_val) {
        this.id = id;
        this.name = name;
        this.address = address;
        this.out_num = out_num;
        this.out_num_val = out_num_val;
    }

    public void setId(int id) {
        this.id = id;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setAddress(int address) {
        this.address = address;
    }

    public void setOut_num(int out_num) {
        this.out_num = out_num;
    }

    public void setOut_num_val(boolean out_num_val) {
        this.out_num_val = out_num_val;
    }

    public int getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public int getAddress() {
        return address;
    }

    public int getOut_num() {
        return out_num;
    }

    public boolean getOut_num_val() {
        return out_num_val;
    }
}
