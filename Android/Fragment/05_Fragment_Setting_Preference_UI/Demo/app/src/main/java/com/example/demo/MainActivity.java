package com.example.demo;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;
import androidx.fragment.app.Fragment;
import androidx.viewpager.widget.ViewPager;

import android.os.Bundle;
import android.util.Log;
import android.widget.ImageView;
import android.widget.RadioButton;
import android.widget.RadioGroup;

import java.util.ArrayList;
import java.util.List;

public class MainActivity extends AppCompatActivity {
    private static final String TAG="MainActivity";
    private RadioButton mHome;
    private RadioButton mSetting;
    private RadioGroup mGroup;
    private ViewPager mViewPager;
    private ViewPagerAdapter mViewPagerAdapter;
    private List<Fragment> mFragments;
    private Toolbar mToolbar;
    @Override
    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        Log.i(TAG,"onCreate ...\n");
        setContentView(R.layout.activity_main);

        mFragments=new ArrayList<>();
        mGroup=findViewById(R.id.radioGroup);
        mHome=findViewById(R.id.Home);
        mSetting=findViewById(R.id.Setting);
        mViewPager=findViewById(R.id.viewPager);
//        //tableview 制作Setting界面
//        mFragments.add(new HomeFragment());

        mFragments.add(new SettingPreferenceFragment());
        mFragments.add(new SettingFragment());

        //关联ViewPager与Adapter
        mViewPagerAdapter=new ViewPagerAdapter(getSupportFragmentManager(),mFragments);
        mViewPager.setAdapter(mViewPagerAdapter);
        mViewPager.setCurrentItem(0);
        mHome.setChecked(true);
        //RadioGroup切换监听 关联ViewPager相关页面
        mGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup radioGroup, int i) {
                if(i==R.id.Home){
                    mViewPager.setCurrentItem(0);
                }
                if(i==R.id.Setting){
                    mViewPager.setCurrentItem(1);
                }
            }
        });

        //ViewPager滑动监听 关联相关RadioButton
        mViewPager.addOnPageChangeListener(new ViewPager.OnPageChangeListener() {
            @Override
            public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {

            }

            @Override
            public void onPageSelected(int position) {
                if(position==0){
                    mHome.setChecked(true);
                }
                if(position==1){
                    mSetting.setChecked(true);
                }
            }

            @Override
            public void onPageScrollStateChanged(int state) {

            }
        });
    }


    @Override
    protected void onStart() {
        super.onStart();
        Log.i(TAG,"onStart ...\n");
    }

    @Override
    protected void onResume() {
        super.onResume();
        Log.i(TAG,"onResume ...\n");
    }

    @Override
    protected void onPause() {
        super.onPause();
        Log.i(TAG,"onPause ...\n");
    }

    @Override
    protected void onStop() {
        super.onStop();
        Log.i(TAG,"onStop ...\n");
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        Log.i(TAG,"onDestroy ...\n");
    }

}