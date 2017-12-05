package com.jt28.a6735.jtzmconfig.model;

import java.io.Serializable;

/**
 * Created by a6735 on 2017/7/7.
 */

public class Scene implements Serializable {
    private String name;
    private int address;
    private int keynum;
    private int type;//面板型号
    private boolean flagArray;
    private boolean menuflag;

    public Scene(String name,int type,int address,int keynum,boolean flagArray,boolean menuflag) {
        this.name = name;
        this.type = type;
        this.address = address;
        this.keynum = keynum;
        this.flagArray = flagArray;
        this.menuflag = menuflag;
    }

    public String getName() {
        return name;
    }

    public int getType() {
        return type;
    }

    public int getAddress() {
        return address;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setType(int type) {
        this.type = type;
    }

    public void setAddress(int address) {
        this.address = address;
    }

    public int getKeynum() {
        return keynum;
    }

    public void setKeynum(int kaynum) {
        this.keynum = kaynum;
    }

    public void setFlagArray(boolean flagArray) {
        this.flagArray = flagArray;
    }

    public boolean isFlagArray() {
        return flagArray;
    }

    public void setMenuflag(boolean menuflag) {
        this.menuflag = menuflag;
    }

    public boolean isMenuflag() {
        return menuflag;
    }
}
