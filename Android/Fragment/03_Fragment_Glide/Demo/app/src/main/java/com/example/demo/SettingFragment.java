package com.example.demo;

import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.ProgressBar;
import android.widget.TextView;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.bumptech.glide.Glide;
import com.bumptech.glide.request.target.ImageViewTarget;
import com.bumptech.glide.request.transition.Transition;

public class SettingFragment extends Fragment {
    private final String TAG="SettingFragment";
    private ImageView mImageview;
    private ProgressBar mProgressBar;
    private TextView mTextView;
    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container,
                             @Nullable Bundle savedInstanceState) {
        View view=inflater.inflate(R.layout.setting_fragment,container,false);
        mImageview= view.findViewById(R.id.image);
        mProgressBar=view.findViewById(R.id.progressBar);
        mTextView=view.findViewById(R.id.textView);
        Glide.with(getContext()).
                load("https://i-blog.csdnimg.cn/blog_migrate/aff0b6273a4adf84f57e5a4145fd4936.jpeg")
                .into(new ImageViewTarget<Drawable>(mImageview) {
                    @Override
                    public void onLoadStarted(@Nullable Drawable placeholder) {
                        super.onLoadStarted(placeholder);
                        Log.i(TAG,"onLoadStarted\n");
                        mProgressBar.setVisibility(View.VISIBLE);
                        mTextView.setVisibility(View.GONE);
                    }

                    @Override
                    public void onLoadFailed(@Nullable Drawable errorDrawable) {
                        super.onLoadFailed(errorDrawable);
                        Log.i(TAG,"onLoadFailed\n");
                        mImageview.setImageResource(R.drawable.error);
                        mTextView.setVisibility(View.VISIBLE);
                    }

                    @Override
                    public void onResourceReady(@NonNull Drawable resource, @Nullable Transition<? super Drawable> transition) {
                        super.onResourceReady(resource, transition);
                        mProgressBar.setVisibility(View.GONE);
                        mTextView.setVisibility(View.GONE);
                        mImageview.setImageDrawable(resource);
                        Log.i(TAG,"onResourceReady\n");
                    }

                    @Override
                    protected void setResource(@Nullable Drawable resource) {
                        mProgressBar.setVisibility(View.GONE);
                        mTextView.setVisibility(View.GONE);
                        mImageview.setImageDrawable(resource);
                    }
                });
        return view;
    }
}
