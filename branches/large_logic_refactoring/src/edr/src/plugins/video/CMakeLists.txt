BEGIN_PROJECT("dynamic")

SET_PRIVATE_HEADERS(
	StreamOsgWidget.h
	VideoWidget.h
	VideoWidgetOptions.h
	VideoParser.h
	VideoService.h
	VideoVisualizer.h
	VideoPCH.h
)

SET_PUBLIC_HEADERS(
	Wrappers.h
)

SET_SOURCE_FILES (
	StreamOsgWidget.cpp
	VideoPlugin.cpp
	VideoService.cpp
	VideoWidget.cpp
	VideoWidgetOptions.cpp
	VideoParser.cpp
	VideoVisualizer.cpp
	VideoPCH.cpp
)

SET_MOC_FILES (
	VideoWidget.h
	VideoWidgetOptions.h
)

SET_UI_FILES (
	video.ui
	VideoWidgetQt.ui
	VideoWidgetOptions.ui
)

SET_RC_FILES(
	videoIcons.qrc
)

SET_PRECOMPILED_HEADER( VideoPCH.h VideoPCH.cpp )

END_PROJECT()