#!/bin/bash

VAR_FLAGS_MACOS_CFLAGS_BASE='
 -fpascal-strings -O2 -fmessage-length=0 
 -fdiagnostics-show-note-include-stack -fmacro-backtrace-limit=0
 -fno-common -fasm-blocks -fstrict-aliasing 
 -Wall -Wextra -Wcast-align -Wno-trigraphs -Wno-unused-parameter -Wno-format 
 -Wno-unused-function -Wno-sign-compare 
 -Wnon-modular-include-in-framework-module -Werror=non-modular-include-in-framework-module
 -Wno-missing-field-initializers -Wno-missing-prototypes -Werror=return-type -Wunreachable-code 
 -Werror=deprecated-objc-isa-usage -Werror=objc-root-class -Wno-missing-braces -Wparentheses 
 -Wswitch -Wunused-function -Wno-unused-label -Wno-unused-parameter -Wunused-variable 
 -Wunused-value -Wempty-body -Wconditional-uninitialized -Wno-unknown-pragmas -Wno-shadow 
 -Wno-four-char-constants -Wno-conversion -Wconstant-conversion -Wint-conversion -Wbool-conversion 
 -Wenum-conversion -Wshorten-64-to-32 -Wpointer-sign -Wno-newline-eof
 -Wdeprecated-declarations 
 -g
 '

VAR_FLAGS_MACOS_CFLAGS="$VAR_FLAGS_MACOS_CFLAGS_BASE -std=c99 "

VAR_FLAGS_MACOS_CXXFLAGS="$VAR_FLAGS_MACOS_CFLAGS_BASE 
 -stdlib=libc++ -fvisibility-inlines-hidden -fvisibility=hidden "

VAR_FLAGS_MACOS_COBJFLAGS="$VAR_FLAGS_MACOS_CFLAGS_BASE -Wno-arc-repeated-use-of-weak 
-Wno-missing-braces -Wno-selector -Wno-strict-selector-match -Wno-undeclared-selector
-fvisibility=hidden
 "

VAR_FLAGS_MACOS_LDFLAGS=' 
 '

VAR_FLAGS_MACOS_PLATFORM_SDK_ROOT_DIR='
/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/ '

VAR_FLAGS_MACOS_CC='
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang '

VAR_FLAGS_MACOS_CXX='
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang '

VAR_FLAGS_MACOS_COBJ='
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang '

VAR_FLAGS_MACOS_LIBTOOL='
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/libtool '

VAR_FLAGS_MACOS_AR='
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ar '

VAR_FLAGS_MACOS_STRIP='
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/strip '

VAR_FLAGS_MACOS_RANLIB='
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/ranlib '

VAR_FLAGS_MACOS_LIPO='
/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/lipo '

VAR_FLAGS_MACOS_DEPLOYMENT_TARGET='
10.7'

VAR_FLAGS_MACOS_INSTALL_DIR='
/Users/Shared/Jenkins/MacOS/'

case $1 in
	--cflags)
		echo $VAR_FLAGS_MACOS_CFLAGS
		;;
	--cxxflags)
		echo $VAR_FLAGS_MACOS_CXXFLAGS
		;;
	--cobjflags)
		echo $VAR_FLAGS_MACOS_COBJFLAGS
		;;
	--ldflags)
		echo $VAR_FLAGS_MACOS_LDFLAGS
		;;
	--sysroot)
		echo $VAR_FLAGS_MACOS_PLATFORM_SDK_ROOT_DIR
		;;
	--cc)
		echo $VAR_FLAGS_MACOS_CC
		;;
	--cxx)
		echo $VAR_FLAGS_MACOS_CXX
		;;
	--cobj)
		echo $VAR_FLAGS_MACOS_COBJ
		;;
	--ar)
		echo $VAR_FLAGS_MACOS_AR
		;;
	--strip)
		echo $VAR_FLAGS_MACOS_STRIP
		;;
	--ranlib)
		echo $VAR_FLAGS_MACOS_RANLIB
		;;
	--libtool)
		echo $VAR_FLAGS_MACOS_LIBTOOL
		;;
	--lipo)
		echo $VAR_FLAGS_MACOS_LIPO
		;;
	--minversion)
		echo $VAR_FLAGS_MACOS_DEPLOYMENT_TARGET
		;;
	--installdir)
		echo $VAR_FLAGS_MACOS_INSTALL_DIR
		;;
	*)
		echo "Invalid option!"
		exit 1
		;;
esac
