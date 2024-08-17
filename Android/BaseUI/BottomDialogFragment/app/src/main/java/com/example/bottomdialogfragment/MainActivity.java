package com.example.bottomdialogfragment;

import androidx.appcompat.app.AppCompatActivity;
import androidx.fragment.app.Fragment;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {

    private MBottomSheetDialogFragment mFragment;
    private Button mButton;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mButton=findViewById(R.id.btn_bottom_sheet_dialog_fragment);
        mFragment=new MBottomSheetDialogFragment();
        //getSupportFragmentManager().beginTransaction().add(R.id.container,mFragment,"HomeFragment").commit();
        mButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                mFragment.show(getSupportFragmentManager(),"HomeFragment");
            }
        });
    }
}