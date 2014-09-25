How to compile idx/eblearn demo on android:

1- Get packages:
  - Android NDK: http://developer.android.com/sdk/ndk/index.html
  - Android SDK: http://developer.android.com/sdk/index.html
  - Java JDK (in particular jni.h)
  - Ant (to build application):
    sudo apt-get install ant

2- Export paths in your environment:
  - export your NDK root to ANDKROOT
  - export your SDK root to ASDKROOT
  - export your signing keystore root to AKEYSROOT (and create it)
  (http://developer.android.com/tools/publishing/app-signing.html)
  - add SDK tools to your PATH:
    export PATH=$PATH:$ASDKROOT/tools

3- Create your personal signing key:
  - export your java root to JAVA_HOME
  - call 'make key' from here

4- Create a link to this app in your NDK:
  mkdir -p ${ANDKROOT}/apps/
  cd ${ANDKROOT}/apps/
  ln -s HERE eblearn

5- Create build file:
  - make create
  You might have to modify the create: line in Makefile to add a target.
  For example, you might have to modify
      ${atools}/android update project -p `pwd`
  to
      ${atools}/android update project -p `pwd` --target 1
  
6- Compile:
  - make

7- Install on device:
  - start the emulator
  - make install
  - if failing because already installed, try make uninstall, then make install

8- Check logs:
  - make log | grep eblearn

9- Accessing the sd card
  - when creating device in emulator, specify an sdcard
  - when phone is off, mount sdcard file:
    cd ~/.android/avd/your_device/
    mkdir tmp && sudo mount -o loop sdcard.img tmp
