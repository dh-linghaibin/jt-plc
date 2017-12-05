package com.jt28.a6735.jtconfig.Msg;

import com.jt28.a6735.jtconfig.bean.GroupInfo;
import com.jt28.a6735.jtconfig.bean.ProductInfo;

import java.util.List;
import java.util.Map;

/**
 * Created by a6735 on 2017/6/27.
 */

public class Msg {
    public Msg(List<GroupInfo> groups,Map<String, List<ProductInfo>> children){
        this.groups = groups;
        this.children = children;
    }
    private List<GroupInfo> groups;// 组元素数据列表
    private Map<String, List<ProductInfo>> children;// 子元素数据列表

    public List<GroupInfo> GetGroups() {
        return groups;
    }
    public void SetGroups(List<GroupInfo> name) {
        this.groups = name;
    }

    public Map<String, List<ProductInfo>> Getchildren() {
        return children;
    }
    public void Setchildren(Map<String, List<ProductInfo>> name) {
        this.children = name;
    }
}
