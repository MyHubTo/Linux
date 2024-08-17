package com.example.bottomnavigation;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.TextView;

import com.google.android.material.bottomnavigation.BottomNavigationItemView;
import com.google.android.material.bottomnavigation.BottomNavigationMenuView;
import com.google.android.material.bottomnavigation.BottomNavigationView;
import com.google.android.material.navigation.NavigationBarView;

public class MainActivity extends AppCompatActivity {

    private BottomNavigationView mBottomNavigationView1;
    private BottomNavigationView mBottomNavigationView2;
    private BottomNavigationView mBottomNavigationView3;

    private TextView mTextView;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mTextView = findViewById(R.id.message);

        BottomNavigationView[] bottomNavigationViews = {
                findViewById(R.id.navigation),
                findViewById(R.id.navigation2),
                findViewById(R.id.navigation3)
        };

        // 遍历底部导航视图数组，为每个视图设置相同的监听器
        for (BottomNavigationView navigationView : bottomNavigationViews) {
            setupBottomNavigationView(navigationView);
        }

        //navigation3
        BottomNavigationMenuView menuView = (BottomNavigationMenuView) bottomNavigationViews[2].getChildAt(0);

        // 获取第2个itemView，注意索引从0开始，所以第2个的索引是1
        BottomNavigationItemView itemView = (BottomNavigationItemView) menuView.getChildAt(1);

        View badgeView = LayoutInflater.from(this).inflate(R.layout.layout_badge_view, menuView, false);
        itemView.addView(badgeView);
        TextView countView= badgeView.findViewById(R.id.tv_badge);
        countView.setText("99");

    }

    private void setupBottomNavigationView(BottomNavigationView navigationView) {
        navigationView.setOnItemSelectedListener(new NavigationBarView.OnItemSelectedListener() {
            @Override
            public boolean onNavigationItemSelected(@NonNull MenuItem item) {
                if(item.getItemId()==R.id.navigation_home){
                    mTextView.setText(R.string.title_home);
                }
                if(item.getItemId()==R.id.navigation_dashboard){
                    mTextView.setText(R.string.title_dashboard);
                }
                if(item.getItemId()==R.id.navigation_notifications){
                    mTextView.setText(R.string.title_notifications);
                }
                if(item.getItemId()==R.id.navigation_test){
                    mTextView.setText(R.string.title_test);
                }
                return true;
            }
        });
    }
}