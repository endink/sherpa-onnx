#!/usr/bin/env bash

set -ex

if [[ ! -f ../build/lib/libsherpa-onnx-jni.dylib  && ! -f ../build/lib/libsherpa-onnx-jni.so ]]; then
  mkdir -p ../build
  pushd ../build
  cmake \
    -DSHERPA_ONNX_ENABLE_PYTHON=OFF \
    -DSHERPA_ONNX_ENABLE_TESTS=OFF \
    -DSHERPA_ONNX_ENABLE_CHECK=OFF \
    -DBUILD_SHARED_LIBS=ON \
    -DSHERPA_ONNX_ENABLE_PORTAUDIO=OFF \
    -DSHERPA_ONNX_ENABLE_JNI=ON \
    ..

  make -j4
  ls -lh lib
  popd
fi

if [ ! -f ../sherpa-onnx/java-api/build/sherpa-onnx.jar ]; then
  pushd ../sherpa-onnx/java-api
  make
  popd
fi

if [ ! -f ./gtcrn_simple.onnx ]; then
  curl -SL -O https://github.com/k2-fsa/sherpa-onnx/releases/download/speech-enhancement-models/gtcrn_simple.onnx
fi

if [ ! -f ./inp_16k.wav ]; then
  curl -SL -O https://github.com/k2-fsa/sherpa-onnx/releases/download/speech-enhancement-models/inp_16k.wav
fi

java \
  -Djava.library.path=$PWD/../build/lib \
  -cp ../sherpa-onnx/java-api/build/sherpa-onnx.jar \
  NonStreamingSpeechEnhancementGtcrn.java
