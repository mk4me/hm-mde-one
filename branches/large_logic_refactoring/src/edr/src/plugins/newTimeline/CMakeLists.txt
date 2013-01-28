BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	ITimelineService.h
	TimeSliderWidget.h
	TimelineControlsWidget.h
	VisualizerSerieTimelineChannel.h
)

SET_PRIVATE_HEADERS(
	TimelineService.h
	TimelineWidget.h
	ChannelWidget.h
	ChannelsTreeItem.h
	ChannelCheckbox.h
	TimelinePCH.h
)

SET_SOURCE_FILES (
	TimelineWidget.cpp
	ChannelWidget.cpp
	ChannelsTreeItem.cpp
	ChannelCheckbox.cpp
	TimelineService.cpp
	TimelinePlugin.cpp
	TimeSliderWidget.cpp
	TimelineControlsWidget.cpp
	TimelinePCH.cpp
	VisualizerSerieTimelineChannel.cpp
)

SET_MOC_FILES (
	TimelineWidget.h
	ChannelWidget.h
	ChannelCheckbox.h
	TimeSliderWidget.h
	TimelineControlsWidget.h
)

SET_UI_FILES (
	TimelineWidget.ui
	TimelineControlsWidget.ui
)

SET_RC_FILES (
	newTimelineIcons.qrc
)
SET_PRECOMPILED_HEADER( TimelinePCH.h TimelinePCH.cpp)

END_PROJECT()