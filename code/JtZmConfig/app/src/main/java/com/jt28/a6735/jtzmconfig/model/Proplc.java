package com.jt28.a6735.jtzmconfig.model;

import java.io.Serializable;

/**
 * Created by a6735 on 2017/7/15.
 */

public class Proplc implements Serializable {
    private int address;
    private byte[]  out_name;
    private boolean state;
    public Proplc(int address, byte[] out_name, boolean state) {
        this.address = address;
        this.out_name = out_name;
        this.state = state;
    }

    public int getAddress() {
        return address;
    }

    public void setAddress(int address) {
        this.address = address;
    }

    public byte[]  getOut_name() {
        return out_name;
    }
    public void setOut_name(byte[] out_name) {
        this.out_name = out_name;
    }

    public boolean geState() {
        return state;
    }
    public void setState(boolean state) {
        this.state = state;
    }
}
