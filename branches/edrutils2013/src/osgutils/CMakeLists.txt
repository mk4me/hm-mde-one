BEGIN_PROJECT("static")

SET_PUBLIC_HEADERS(
	CustomPrimitivesFactory.h
)

SET_PRIVATE_HEADERS( PCH.h )

SET_SOURCE_FILES(
	CustomPrimitivesFactory.cpp
	PCH.cpp
)
SET_PRECOMPILED_HEADER(PCH.h PCH.cpp)

END_PROJECT()