package com.example.demo;

import android.os.Bundle;
import android.util.Log;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.preference.CheckBoxPreference;
import androidx.preference.Preference;
import androidx.preference.PreferenceFragmentCompat;
import androidx.preference.SwitchPreferenceCompat;

public class SettingPreferenceFragment extends PreferenceFragmentCompat {
    private static final String TAG="SettingPreferenceFragment";
    @Override
    public void onCreatePreferences(@Nullable Bundle savedInstanceState, @Nullable String rootKey) {
        setPreferencesFromResource(R.xml.setting,rootKey);

        // 获取 CheckBoxPreference 实例
        CheckBoxPreference checkBoxPreference = findPreference("CheckBoxPreference");

        // 为 CheckBoxPreference 设置监听器
        if (checkBoxPreference != null) {
            checkBoxPreference.setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
                @Override
                public boolean onPreferenceChange(Preference preference, Object newValue) {
                    boolean isChecked = (Boolean) newValue;
                    if(isChecked){
                        Log.i(TAG,"checkBoxPreference is checked\n");
                    }else{
                        Log.i(TAG,"checkBoxPreference is not checked\n");
                    }
                    return true;
                }
            });
        }
        SwitchPreferenceCompat switchPreferenceCompat=findPreference("notifications");
        if(switchPreferenceCompat!=null){
            ((Preference) switchPreferenceCompat).setOnPreferenceChangeListener(new Preference.OnPreferenceChangeListener() {
                @Override
                public boolean onPreferenceChange(@NonNull Preference preference, Object newValue) {
                    boolean isChecked = (Boolean) newValue;
                    if(isChecked){
                        Log.i(TAG,"SwitchPreferenceCompat is checked\n");
                    }else{
                        Log.i(TAG,"SwitchPreferenceCompat is not checked\n");
                    }
                    return true;
                }
            });
        }
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }
}
