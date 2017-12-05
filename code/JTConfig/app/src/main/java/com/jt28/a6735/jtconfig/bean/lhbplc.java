package com.jt28.a6735.jtconfig.bean;

/**
 * Created by a6735 on 2017/7/5.
 */

public class lhbplc {
    private String address;
    private byte[]  out_name;
    private boolean state;
    public lhbplc(String address,byte[] out_name,boolean state) {
        this.address = address;
        this.out_name = out_name;
        this.state = state;
    }

    public String getAddress() {
        return address;
    }

    public void setAddress(String address) {
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
