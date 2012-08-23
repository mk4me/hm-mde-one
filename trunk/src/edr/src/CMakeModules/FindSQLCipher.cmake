# przygotowanie do szukania
FIND_INIT(SQLCIPHER sqlcipher)

# szukanie
if (WIN32) 
FIND_SHARED(SQLCIPHER "sqlcipher" "sqlcipher")
elseif (UNIX)
FIND_SHARED(SQLCIPHER "sqlite3" "sqlite3")
endif()
add_definitions(-DSQLITE_HAS_CODEC)
# skopiowanie
FIND_FINISH(SQLCIPHER)
