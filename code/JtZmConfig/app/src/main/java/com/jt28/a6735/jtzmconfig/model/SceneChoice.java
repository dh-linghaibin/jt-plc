package com.jt28.a6735.jtzmconfig.model;

import java.io.Serializable;

/**
 * Created by a6735 on 2017/7/15.
 */

public class SceneChoice implements Serializable {
    private String name;
    private boolean choice;

    public SceneChoice(String name, boolean choice) {
        this.name = name;
        this.choice = choice;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void setChoice(boolean choice) {
        this.choice = choice;
    }

    public boolean getChoice() {
        return choice;
    }
}
