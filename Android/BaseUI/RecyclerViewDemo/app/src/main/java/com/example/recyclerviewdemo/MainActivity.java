package com.example.recyclerviewdemo;

import androidx.appcompat.app.AppCompatActivity;
import androidx.recyclerview.widget.GridLayoutManager;
import androidx.recyclerview.widget.RecyclerView;

import android.os.Bundle;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        RecyclerView recyclerView = findViewById(R.id.recyclerView);

        // 创建列表数据
        List<RecycleViewItemData> data = new ArrayList<>();
        for(int i=0;i<10;i++){
            data.add(new RecycleViewItemData("Item "+i,i*10));
        }

        // 创建适配器
        MyAdapter adapter = new MyAdapter(data);

        // 设置LayoutManager为GridLayoutManager，指定列数为2
        GridLayoutManager gridLayoutManager = new GridLayoutManager(this, 2);
        recyclerView.setLayoutManager(gridLayoutManager);

        // 设置适配器
        recyclerView.setAdapter(adapter);
    }
}