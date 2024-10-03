package com.example.demo;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;

import android.os.Bundle;
import android.util.Log;
import android.widget.RadioButton;
import android.widget.RadioGroup;

public class MainActivity extends AppCompatActivity {
    private static final String TAG="MainActivity";
    private Fragment mFragment;
    private RadioButton mHome;
    private RadioButton mSetting;
    private RadioGroup mGroup;
    @Override
    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        Log.i(TAG,"onCreate ...\n");
        setContentView(R.layout.activity_main);
        mGroup=findViewById(R.id.radioGroup);
        mHome=findViewById(R.id.Home);
        mSetting=findViewById(R.id.Setting);

        //初始化默认的Fragment
        mFragment=new HomeFragment();
        //将HomeFragment添加到布局中
        getSupportFragmentManager().beginTransaction().add(R.id.container,mFragment,"HomeFragment").commit();

        //设置RadioGroup Listener
        mGroup.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(RadioGroup radioGroup, int i) {
                if(i==R.id.Home){
                    Log.i(TAG,"Switch to Home\n");
                    switchFragment(new HomeFragment());
                }
                if(i==R.id.Setting){
                    Log.i(TAG,"Switch to Setting\n");
                    switchFragment(new SettingFragment());
                }
            }
        });
    }

    private void switchFragment(Fragment fragment){
        Log.d(TAG,"switchFragment\n");
        //判断当前显示的Fragment是不是切换的Fragment
        if(mFragment!=fragment){
        //判断切换的Fragment是否已经添加过
            if(!fragment.isAdded()){
                Log.d(TAG,"Fragment is not isAdded\n");
                //如果没有,则先把当前的Fragment隐藏,把切换的Fragment添加上
                getSupportFragmentManager().beginTransaction().hide(mFragment)
                        .add(R.id.container,fragment).commit();
            }else{
                Log.d(TAG,"Fragment is isAdded\n");
                //如果已经添加过,则先把当前的Fragment隐藏,把切换的Fragment显示出来
                getSupportFragmentManager().beginTransaction()
                        .hide(mFragment).show(fragment).commit();
            }
            mFragment=fragment;
        }

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