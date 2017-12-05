package com.jt28.a6735.jtzmconfig.model;

import java.io.Serializable;

/**
 * Created by a6735 on 2017/7/11.
 */

public class Plcval implements Serializable {
    private int out_num;
    private int out_num_val;
    public Plcval(int out_num, int out_num_val) {
        this.out_num = out_num;
        this.out_num_val = out_num_val;
    }

    public int getOut_num() {
        return out_num;
    }

    public int getOut_num_val() {
        return out_num_val;
    }

    public void setOut_num(int out_num) {
        this.out_num = out_num;
    }

    public void setOut_num_val(int out_num_val) {
        this.out_num_val = out_num_val;
    }
}
