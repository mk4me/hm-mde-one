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
	IJob.h
	IJobManager.h
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
	Serializable.h
	Synchronized.h
	SynchronizationPolicies.h
	IThread.h
	IThreadFactory.h
	TypeInfo.h
	ObjectWrapper.h
	Export.h
	SmartPtr.h
	ICommand.h
	ICommandStack.h
	CommandStack.h
)

#------------------------------------------------------------------------------
# Ÿród³a
SET_SOURCE_FILES(
	CommandStack.cpp
	Enum.cpp
	Debug.cpp
	NamedTree.cpp
	Profiler.cpp
	TypeInfo.cpp
	ObjectWrapper.cpp
)

SET_CONFIGURATION_INPUT_FILES(
	"Config.h.in"
)

SET_MOC_FILES(
	CommandStack.h
)

CONFIGURE_PUBLIC_HEADER("Config.h.in" "Config.h")

END_PROJECT()