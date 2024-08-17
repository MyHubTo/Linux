package com.example.snackbar;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import com.google.android.material.snackbar.Snackbar;

public class MainActivity extends AppCompatActivity {

    private final String TAG="MainActivity";
    private Button mButton1;
    private Button mButton2;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mButton1=findViewById(R.id.button);
        View rootLayout = findViewById(R.id.constraint_layout);
        mButton1.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(rootLayout,"已加入行程", Snackbar.LENGTH_LONG).show();
            }
        });

        mButton2=findViewById(R.id.button2);
        mButton2.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar snackbar = Snackbar.make(rootLayout,"已加入行程",  Snackbar.LENGTH_INDEFINITE);
                snackbar.setAction("知道了", new View.OnClickListener() {
                    @Override
                    public void onClick(View view) {
                        Toast.makeText(MainActivity.this, "知道了", Toast.LENGTH_LONG).show();
                        Log.i(TAG,"onClick\n");
                    }
                }).show();
            }
        });
    }
}