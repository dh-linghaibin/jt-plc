package com.jt28.a6735.jtzmconfig.model;

import java.io.Serializable;

/**
 * Created by a6735 on 2017/7/7.
 */

public class PlcControl implements Serializable {
    private int id;
    private String name;
    private boolean flagArray;
    private boolean menuflag;
    public PlcControl(int id,String name,boolean flagArray,boolean menuflag) {
        this.id = id;
        this.name = name;
        this.flagArray = flagArray;
        this.menuflag = menuflag;
    }

    public int getId() {
        return id;
    }
    public String getName() {
        return name;
    }

    public void setId(int id) {
        this.id = id;
    }
    public void setName(String name) {
        this.name = name;
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
