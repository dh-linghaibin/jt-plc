package com.jt28.a6735.jtzmconfig.model;

import java.io.Serializable;

/**
 * Created by a6735 on 2017/7/28.
 */

public class PlcTimer implements Serializable {
    private String name;
    private int month;
    private int wday;
    private int hour;
    private int min;
    private boolean flagArray;
    private boolean menuflag;

    public PlcTimer(String name,int month,int wday,int hour,int min,boolean flagArray,boolean menuflag) {
        this.name = name;
        this.month = month;
        this.wday = wday;
        this.hour = hour;
        this.min = min;
        this.flagArray = flagArray;
        this.menuflag = menuflag;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public int getMonth() {
        return month;
    }
    public int getWday() {
        return wday;
    }

    public int getHour() {
        return hour;
    }

    public int getMin() {
        return min;
    }

    public void setMonth(int month) {
        this.month = month;
    }

    public void setWday(int wday) {
        this.wday = wday;
    }

    public void setHour(int hour) {
        this.hour = hour;
    }

    public void setMin(int min) {
        this.min = min;
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
