package com.example.refreshdemo;

import androidx.appcompat.app.AppCompatActivity;
import androidx.swiperefreshlayout.widget.SwipeRefreshLayout;

import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private SwipeRefreshLayout mSwipeRefreshLayout;
    private TextView mtextView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mtextView=findViewById(R.id.tv_refresh);
        mSwipeRefreshLayout=findViewById(R.id.swipeRefreshLayout);
        //设置 SwipeRefreshLayout 刷新指示器（即旋转的圆形加载器）的颜色方案
        mSwipeRefreshLayout.setColorSchemeResources(
                android.R.color.holo_orange_light,
                android.R.color.holo_blue_light,
                android.R.color.holo_green_light
        );
        // 设置SwipeRefreshLayout的刷新监听器
        mSwipeRefreshLayout.setOnRefreshListener(new SwipeRefreshLayout.OnRefreshListener() {
            @Override
            public void onRefresh() {
                // 使用postDelayed来延迟执行关闭刷新和更新TextView的操作
                mSwipeRefreshLayout.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        // 关闭刷新
                        mSwipeRefreshLayout.setRefreshing(false);
                        mtextView.setText("刷新完成");
                    }
                },2000);// 延迟2000毫秒（即2秒）
            }
        });
    }
}