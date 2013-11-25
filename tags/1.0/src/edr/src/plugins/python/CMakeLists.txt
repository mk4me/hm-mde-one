BEGIN_PROJECT("dynamic")

SET_PRIVATE_HEADERS(
	PythonService.h
	PythonHighlighter.h
	PythonLogic.h
	PythonPCH.h
)

SET_SOURCE_FILES (
	PythonService.cpp
	PythonPlugin.cpp
	PythonHighlighter.cpp
	PythonLogic.cpp
	PythonPCH.cpp
)

SET_MOC_FILES (
	PythonHighlighter.h
	PythonService.h
)

SET_PRECOMPILED_HEADER( PythonPCH.h PythonPCH.cpp)

END_PROJECT()