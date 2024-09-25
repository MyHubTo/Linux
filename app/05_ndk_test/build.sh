#/bin/bash

export ANDROID_NDK=/root/libs/NDK/android-ndk-r26d

rm -r build
mkdir build && cd build

# # 设置NDK的根目录的绝对路径
# set(ANDROID_NDK /root/libs/NDK/android-ndk-r26d)
# # 设置工具链
# set(CMAKE_TOOLCHAIN_FILE ${ANDROID_NDK}/build/cmake/android.toolchain.cmake)
# # 目标ABI
# set(ANDROID_ABI armeabi-v7a)
# # 最低Android平台
# set(ANDROID_PLATFORM android-22)

cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake \
	-DANDROID_ABI="armeabi-v7a" \
	-DANDROID_NDK=$ANDROID_NDK \
	-DANDROID_PLATFORM=android-22 \
	..

make

cd ..
