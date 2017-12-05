package com.jt28.a6735.jtconfig.bean;

/**
 * Created by a6735 on 2017/6/30.
 */

public class SceneInfo {
    private String Id;
    private String name;
    private boolean isChoosed;

    public SceneInfo(String id,String name,boolean isChoosed) {
        this.Id = id;
        this.name = name;
        this.isChoosed = isChoosed;
    }

    public String getId() {
        return Id;
    }
    public void setId(String id) {
        Id = id;
    }

    public String getName() {
        return name;
    }
    public void setName(String name) {
        this.name = name;
    }

    public boolean getChoosed() {
        return isChoosed;
    }
    public void setChoosed(boolean choosed) {
        isChoosed = choosed;
    }

}
