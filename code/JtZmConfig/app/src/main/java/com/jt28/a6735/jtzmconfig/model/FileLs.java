package com.jt28.a6735.jtzmconfig.model;

import java.io.Serializable;

/**
 * Created by a6735 on 2017/7/28.
 */

public class FileLs implements Serializable {
    private String url;
    private String name;
    public FileLs(String name,String url) {
        this.name = name;
        this.url = url;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void setUrl(String url) {
        this.url = url;
    }

    public String getUrl() {
        return url;
    }
}
