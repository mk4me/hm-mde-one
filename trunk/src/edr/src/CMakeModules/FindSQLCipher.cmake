# przygotowanie do szukania
FIND_INIT(SQLCIPHER sqlcipher)

# szukanie
FIND_SHARED(SQLCIPHER "sqlcipher" "sqlcipher")
add_definitions(-DSQLITE_HAS_CODEC)
# skopiowanie
FIND_FINISH(SQLCIPHER)
