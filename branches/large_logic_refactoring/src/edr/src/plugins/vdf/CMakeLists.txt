BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	VdfService.h
)

SET_PRIVATE_HEADERS(
	VdfWidget.h
	VdfQtModel.h
	DeleteConnectionContextMenu.h
	VdfCustomQOsgWidget.h
	EDRPin.h
	EDRSourceNode.h
	EDRProcessingNode.h
	EDRNodesFactory.h
	VdfPCH.h
)

SET_SOURCE_FILES (
	VdfService.cpp
	VdfPlugin.cpp
	VdfWidget.cpp
	VdfQtModel.cpp
	DeleteConnectionContextMenu.cpp
	VdfCustomQOsgWidget.cpp
	EDRPin.cpp
	EDRSourceNode.cpp
	EDRProcessingNode.cpp
	EDRNodesFactory.cpp
	VdfPCH.cpp
)

SET_MOC_FILES(
	VdfWidget.h
	DeleteConnectionContextMenu.h
	VdfQtModel.h
	VdfCustomQOsgWidget.h
)

SET_UI_FILES
	VdfWidget.ui
)

SET_PRECOMPILED_HEADER( VdfPCH.h VdfPCH.cpp)

END_PROJECT()
