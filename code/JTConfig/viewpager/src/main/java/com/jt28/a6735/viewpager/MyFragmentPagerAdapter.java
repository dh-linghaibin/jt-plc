package com.jt28.a6735.viewpager;

import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentStatePagerAdapter;

import java.util.ArrayList;

/**
 * Created by zhaoxuan.li on 2015/10/10.
 */
public class MyFragmentPagerAdapter extends FragmentStatePagerAdapter {
    private ArrayList<PageFragment> listFragments;
    private FragmentManager mFragmentManager;

    public MyFragmentPagerAdapter(FragmentManager fm, ArrayList<PageFragment> al) {
        super(fm);
        mFragmentManager = fm;
        listFragments = al;
        notifyDataSetChanged();
    }

    /*
    * 通常Fragment在onResume中做了数据刷新工作，缺点是当前正在显示的Fragment不能及时刷新数据
    * （FragmentStatePagerAdapter 会同时加载2-3个Fragment，这三个Fragment相互切换不会调用onResume方法，一直为显示状态）
    * 如何通知正在显示的Fragment刷新，一般采用广播机制来实现。（但感觉广播在底层进行遍历判断很费力）
    * 所以这里对FragmentList进行遍历，找到当前正在显得Fragment进行刷新
    *
    * 疑问：这个方法是放在adapter里合适还是Activity里合适
    * */
    public void refreshData(){
        for (PageFragment fragment:listFragments) {
            if (fragment.isVisible()){
                fragment.refreshView();
            }
        }
    }


    @Override
    public Fragment getItem(int position) {
        return listFragments.get(position);
    }

    @Override
    public int getCount() {
        return listFragments.size();
    }

    @Override
    public int getItemPosition(Object object) {
        return super.getItemPosition(object);
    }



}
