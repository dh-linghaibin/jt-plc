package com.jt28.a6735.jtzmconfig;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;

import org.greenrobot.eventbus.EventBus;

/**
 * Created by a6735 on 2016/10/13.
 */

public class AppStart extends AppCompatActivity {
    private static final long DELAY_TIME = 1000L;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.app_start);
        redirectByTime();
    }
    @Override
    protected void onDestroy() {
        EventBus.getDefault().unregister(this);
        super.onDestroy();
    }

    private void redirectByTime() {
        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
            Intent intent = new Intent(AppStart.this, MainActivity.class);//AlarmAnalysisMain  Login
            intent.addFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP);
            startActivity(intent);
            finish();
            }
        }, DELAY_TIME);
    }

}
