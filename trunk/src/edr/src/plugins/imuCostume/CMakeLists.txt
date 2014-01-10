BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS (
	Wrappers.h
)

SET_PRIVATE_HEADERS (
	IMUCostumeListWidget.h
	DataSource.h
	PCH.h
)

SET_SOURCE_FILES(
	IMUCostumeListWidget.cpp
	DataSource.cpp
	PCH.cpp
	Plugin.cpp
)

SET_MOC_FILES(
	IMUCostumeListWidget.h
)

SET_UI_FILES(
	IMUCostumeListWidget.ui
)

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
SET_PRECOMPILED_HEADER( PCH.h PCH.cpp )

END_PROJECT()