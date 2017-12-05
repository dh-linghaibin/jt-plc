package com.jt28.a6735.jtconfig.Msg;

import java.util.List;

/**
 * Created by a6735 on 2017/7/6.
 */

public class downloadmsg {
    private List<byte[]> download;
    public downloadmsg(List<byte[]> download) {
        this.download = download;
    }

    public List<byte[]> getDownload() {
        return download;
    }
    public void setDownload(List<byte[]> download) {
        this.download = download;
    }
}
