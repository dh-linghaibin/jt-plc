package com.jt28.a6735.viewpager;

/**
 * Created by a6735 on 2016/11/3.
 */

public class PageFragment extends BaseFragment {
    /*通常需要载入*/
    @Override
    public void onResume() {
        super.onResume();
        refreshView();
    }

    public void refreshView() {

    }
    /**
     * Activity 供Fragment回调的接口
     */
    public interface IMainCallBack{
        /*取得Fragment共用的数据*/
       // ServerData getCommonData();
        /*要求Fragment刷新数据*/
        void refreshCommonData();
    }
}
