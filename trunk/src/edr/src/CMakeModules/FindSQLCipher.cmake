# przygotowanie do szukania
FIND_INIT(SQLCIPHER sqlcipher)

# szukanie
FIND_SHARED(SQLCIPHER "sqlite3" "sqlite3")
add_definitions(-DSQLITE_HAS_CODEC)
# skopiowanie
FIND_FINISH(SQLCIPHER)
