package com.jt28.a6735.jtzmconfig.model;

import java.io.Serializable;

/**
 * Created by a6735 on 2017/7/8.
 */

public class Scenechild implements Serializable {
    private String name;

    public Scenechild(String name) {
        this.name = name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }
}
