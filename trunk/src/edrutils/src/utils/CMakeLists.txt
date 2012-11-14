BEGIN_PROJECT("static")

# Opcje konfiguracji
CONFIG_OPTION(UTILS_ENABLE_PROFILER "Enable profiling?" OFF)
if(WIN32)
	CONFIG_OPTION(UTILS_ENABLE_LEAK_DETECTION_VLD "Enable leak detection with VLD? (http://sites.google.com/site/dmoulding/vld)" OFF)
endif()

SET_PUBLIC_HEADERS(
	Align.h
	ClonePolicies.h
	DataChannel.h
	DataChannelAccessors.h
	DataChannelTimers.h
	DataChannelDescriptors.h
	DataChannelCollection.h
	DataChannelModifiers.h
	DataChannelStats.h
	Pool.h
	Enum.h
	NamedTree.h
	PtrPolicyRaw.h
	PtrPolicyBoost.h
	PtrPolicyOSG.h
	PtrPolicyHelper.h
	PtrWrapper.h
	ObserverPattern.h
	Macros.h
	Utils.h
	Debug.h
	Profiler.h
	System.h
	LeakDetection.h
	Push.h
	Synchronized.h
	SynchronizationPolicies.h
)

configure_file(configuration/Config.h.in  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}/Config.h" )

#------------------------------------------------------------------------------
# Ÿród³a
SET_SOURCE_FILES(
	Enum.cpp
	Debug.cpp
	NamedTree.cpp
	Profiler.cpp
)

END_PROJECT()