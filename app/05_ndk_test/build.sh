#/bin/bash
# 说明：设置 Android NDK 路径，下载完直接解压到根目录即可。
# https://developer.android.google.cn/ndk/downloads?hl=zh-cn
export ANDROID_NDK=/home/zhaoc/Downloads/android-ndk-r28c

rm -r build
mkdir build && cd build

# # 设置NDK的根目录的绝对路径
# set(ANDROID_NDK /root/libs/NDK/android-ndk-r26d)
# # 设置工具链
# set(CMAKE_TOOLCHAIN_FILE ${ANDROID_NDK}/build/cmake/android.toolchain.cmake)
# # 目标ABI
# set(ANDROID_ABI armeabi-v7a) #arm64-v8a
# # 最低Android平台
# set(ANDROID_PLATFORM android-22)

cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
	-DANDROID_ABI="arm64-v8a" \
	-DANDROID_NDK=$ANDROID_NDK \
	-DANDROID_PLATFORM=android-32 \
	-DANDROID_STL=c++_shared \
	..

make

cd ..
