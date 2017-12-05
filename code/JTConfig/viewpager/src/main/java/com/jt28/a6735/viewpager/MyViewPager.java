package com.jt28.a6735.viewpager;

import android.content.Context;
import android.support.v4.view.ViewPager;
import android.util.AttributeSet;
import android.view.MotionEvent;

/**
 * Created by a6735 on 2016/11/3.
 */

public class MyViewPager extends ViewPager {
    private boolean scrollble = true;

    public void setScrollble(boolean scrollble) {
        this.scrollble = scrollble;
    }

    private boolean isCanScroll = false;   //false表示不滑动

    public MyViewPager(Context context) {
        super(context);
    }

    public MyViewPager(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    public void setIsCanScroll(boolean isCanScroll) {
        this.isCanScroll = isCanScroll;
    }

    /*
    重写后，能实现不滑动，但是点击按钮时，页面照样不切换
     */
    /*@Override
    public void scrollTo(int x, int y) {
        if (isCanScroll) {
            super.scrollTo(x, y);
        }
    }*/

    @Override
    public boolean onTouchEvent(MotionEvent ev) {
        if (isCanScroll)
            return super.onTouchEvent(ev);
        else
            return false;
    }

    @Override
    public boolean onInterceptTouchEvent(MotionEvent ev) {
        if (isCanScroll)
            return super.onInterceptTouchEvent(ev);
        else
            return false;
    }

    @Override
    public void setCurrentItem(int item, boolean smoothScroll) {
        super.setCurrentItem(item, smoothScroll);
    }

    @Override
    public void setCurrentItem(int item) {
        super.setCurrentItem(item,false);   //表示切换的时候，不需要切换时间。
        //避免点击标签切换多个page页面时出现闪烁，影响用户体验
    }
}
