keystore=${AKEYSROOT}
keyname=${USER}
ndk=${ANDKROOT}
sdk=${ASDKROOT}
name=eblearn
pkg=com.eblearn
app=${ndk}/apps/${name}
atools=${sdk}/tools
aptools=${sdk}/platform-tools
apkname=${name}-release-unsigned
dapkname=${name}-debug

all: lib
	cd ${app} && ant release

lib:
	cd ${app}/../.. && make APP=${name} APP_OPTIM=release APP_STL=gnustl_static APP_CPPFLAGS="-fexceptions -frtti -D__PTHREAD__"

debug:
	cd ${app}/../.. && make APP=${name} APP_OPTIM=debug V=1  NDK_DEBUG=1  APP_STL=gnustl_static APP_CPPFLAGS="-fexceptions -frtti -D__PTHREAD__"
	cd ${app} && ant debug

gdb: debug dinstall
	cd ${ndk} && ${ndk}/ndk-gdb --verbose --project=${app} --launch=.${name}
	#cd ${app} && ndk-gdb --launch=.${name}

create: # create project files
	${atools}/android update project -p `pwd`
# ${atools}/android create project --package ${pkg} \
# 	--activity ${name} --target 1 \
# 	--path ${app}

install:
	jarsigner -sigalg MD5withRSA -digestalg SHA1 \
	-verbose -keystore ${keystore}/${keyname}.keystore \
	-signedjar ${app}/bin/${name}-signed.apk \
	${app}/bin/${apkname}.apk ${keyname}.keystore
	cp ${app}/bin/${name}-signed.apk ${atools}/
	cd ${atools} && ${aptools}/adb install -r ${name}-signed.apk

dinstall: # install debug package
	jarsigner -sigalg MD5withRSA -digestalg SHA1 \
	 -verbose -keystore ${keystore}/${keyname}.keystore \
	-signedjar ${app}/bin/${name}-signed.apk \
	${app}/bin/${dapkname}.apk ${keyname}.keystore
	cp ${app}/bin/${name}-signed.apk ${atools}/
	cd ${atools} && ${aptools}/adb install -r ${name}-signed.apk

uninstall:
	cd ${atools} && ${aptools}/adb uninstall ${pkg}

clean:
	cd ${app} && ${ndk}/ndk-build clean
	rm -Rf ${app}/bin/*

log:
	${aptools}/adb logcat

key:
	@@echo "Your JAVA_HOME is: ${JAVA_HOME}"
	@@echo "If not set, set it to your java root to use the correct keytool"
	${JAVA_HOME}/bin/keytool -genkey -alias ${keyname}.keystore \
		-keyalg RSA -validity 20000 \
		-keystore ${keystore}/${keyname}.keystore
