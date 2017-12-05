package com.jt28.a6735.jtzmconfig.msg;

import java.util.List;

/**
 * Created by a6735 on 2017/7/6.
 */

public class downloadmsg {
    private String flag;
    private List<byte[]> download;
    public downloadmsg(String flag,List<byte[]> download) {
        this.download = download;
        this.flag = flag;
    }

    public List<byte[]> getDownload() {
        return download;
    }
    public void setDownload(List<byte[]> download) {
        this.download = download;
    }

    public String getFlag() {
        return flag;
    }

    public void setFlag(String flag) {
        this.flag = flag;
    }
}
