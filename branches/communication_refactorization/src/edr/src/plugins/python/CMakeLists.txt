BEGIN_PROJECT("dynamic")

SET_PRIVATE_HEADERS(
	PythonService.h
	PythonConsole.h
	PythonDataChannel.h
	PythonProcessingBlock.h
	PythonHighlighter.h
	PythonLogic.h
	MdeBridge.h
	PythonEditor.h
	PythonPCH.h
)

SET_SOURCE_FILES (
	PythonService.cpp
	PythonConsole.cpp
	PythonDataChannel.cpp
	PythonProcessingBlock.cpp
	PythonPlugin.cpp
	PythonHighlighter.cpp
	PythonLogic.cpp
	MdeBridge.cpp
	PythonEditor.cpp
	PythonPCH.cpp
)

SET_MOC_FILES (
	PythonHighlighter.h
	PythonService.h
	PythonEditor.h
	PythonConsole.h
)

SET_PRECOMPILED_HEADER( PythonPCH.h PythonPCH.cpp)

END_PROJECT()