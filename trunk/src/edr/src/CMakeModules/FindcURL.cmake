# przygotowanie do szukania
FIND_INIT(CURL curl)

# szukanie
FIND_SHARED(CURL "libcurl_imp" "libcurl")

# skopiowanie
FIND_FINISH(CURL)
