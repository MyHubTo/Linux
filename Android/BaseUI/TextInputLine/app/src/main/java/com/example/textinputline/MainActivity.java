package com.example.textinputline;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.text.Editable;
import android.text.InputFilter;
import android.text.TextWatcher;
import android.util.Log;

import com.google.android.material.textfield.TextInputEditText;
import com.google.android.material.textfield.TextInputLayout;

public class MainActivity extends AppCompatActivity {
    private TextInputLayout mTextInputLayout;
    private TextInputEditText mTextInputEditText;
    private final String TAG="MainActivity";
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mTextInputLayout=findViewById(R.id.title_name);
        //设置单行输入
        mTextInputLayout.getEditText().setSingleLine();
        mTextInputEditText=findViewById(R.id.et_name);

        //设置最大字数为TextInputLayout的CounterMaxLength
        InputFilter[] filters = new InputFilter[1];
        filters[0] = new InputFilter.LengthFilter(mTextInputLayout.getCounterMaxLength());
        mTextInputLayout.getEditText().setFilters(filters);

        //设置TextWatcher
        mTextInputEditText.addTextChangedListener(mTextWatcher);
    }

    TextWatcher mTextWatcher=new TextWatcher() {
        @Override
        public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

        }

        @Override
        public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
            //Log.i(TAG,"onTextChanged:"+charSequence+" i: "+i+" i1: "+i1+" i2: "+i2+"\n");
        }

        @Override
        public void afterTextChanged(Editable editable) {
            if(mTextInputLayout.getEditText().getText().length()>mTextInputLayout.getCounterMaxLength()){
                mTextInputLayout.setError("输入内容超过上限");
            }
        }
    };
}