# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.UDPChat.Debug:
/Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/Debug/libUDPChat.dylib:
	/bin/rm -f /Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/Debug/libUDPChat.dylib


PostBuild.UDPChat-static.Debug:
/Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/Debug/libUDPChat.a:
	/bin/rm -f /Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/Debug/libUDPChat.a


PostBuild.UDPChat.Release:
/Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/Release/libUDPChat.dylib:
	/bin/rm -f /Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/Release/libUDPChat.dylib


PostBuild.UDPChat-static.Release:
/Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/Release/libUDPChat.a:
	/bin/rm -f /Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/Release/libUDPChat.a


PostBuild.UDPChat.MinSizeRel:
/Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/MinSizeRel/libUDPChat.dylib:
	/bin/rm -f /Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/MinSizeRel/libUDPChat.dylib


PostBuild.UDPChat-static.MinSizeRel:
/Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/MinSizeRel/libUDPChat.a:
	/bin/rm -f /Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/MinSizeRel/libUDPChat.a


PostBuild.UDPChat.RelWithDebInfo:
/Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/RelWithDebInfo/libUDPChat.dylib:
	/bin/rm -f /Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/RelWithDebInfo/libUDPChat.dylib


PostBuild.UDPChat-static.RelWithDebInfo:
/Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/RelWithDebInfo/libUDPChat.a:
	/bin/rm -f /Users/mahuijie/Desktop/ChatLib/ios_arm64_build/lib/RelWithDebInfo/libUDPChat.a




# For each target create a dummy ruleso the target does not have to exist
