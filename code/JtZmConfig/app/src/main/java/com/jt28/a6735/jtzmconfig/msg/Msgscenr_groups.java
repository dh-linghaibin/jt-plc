package com.jt28.a6735.jtzmconfig.msg;

import com.jt28.a6735.jtzmconfig.model.Scene;
import com.jt28.a6735.jtzmconfig.model.Scenechild;

import java.util.List;

/**
 * Created by a6735 on 2017/7/31.
 */

public class Msgscenr_groups {
    private List<Scene> scenr_groups;
    private List<List<Scenechild>> scene_children;

    public Msgscenr_groups(List<Scene> scenr_groups, List<List<Scenechild>> scene_children) {
        this.scenr_groups = scenr_groups;
        this.scene_children = scene_children;
    }

    public void setScenr_groups(List<Scene> scenr_groups) {
        this.scenr_groups = scenr_groups;
    }

    public List<List<Scenechild>> getScene_children() {
        return scene_children;
    }

    public List<Scene> getScenr_groups() {
        return scenr_groups;
    }

    public void setScene_children(List<List<Scenechild>> scene_children) {
        this.scene_children = scene_children;
    }
}
