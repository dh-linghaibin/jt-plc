package com.jt28.a6735.jtzmconfig.adapter.viewpager;

import android.content.Context;
import android.support.v4.view.ViewPager;
import android.util.AttributeSet;
import android.view.MotionEvent;

/**
 * Created by a6735 on 2016/12/14.
 */

public class SlideViewPager extends ViewPager {

    private int downX = 0;

    public SlideViewPager(Context context) {
        this(context,null);
    }

    public SlideViewPager(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent ev) {

        switch (ev.getAction()) {

            case MotionEvent.ACTION_DOWN:

                downX = (int)ev.getX();

                //注意！这里必须有，否则快速滑动会失灵
                getParent().requestDisallowInterceptTouchEvent(true);

                break;

            case MotionEvent.ACTION_MOVE:

                int moveX = (int) ev.getX();

                //在这里写事件拦截的条件，
                if (downX < 20) {
                    if (moveX > downX) {
                        //请求不中断事件
                        getParent().requestDisallowInterceptTouchEvent(false);
                    }else{
                        //请求中断事件
                        getParent().requestDisallowInterceptTouchEvent(true);
                    }
                }else{
                    getParent().requestDisallowInterceptTouchEvent(true);
                }

                break;
        }
        return super.dispatchTouchEvent(ev);
    }
}
