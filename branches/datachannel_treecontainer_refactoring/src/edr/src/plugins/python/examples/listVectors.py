
#Skrypt tworzy list� wszystkich kolekcji wektor�w, kt�re zosta�y za�adowane do aplikacji
import plugin_simplePythond
# pobieranie tablic wraz z opisem 
v = mde.listLoadedVectors()
# konwersja do listy
dataList = [(x.first, x.second) for x in v]
# wypisanie wynik�w na konsol�
print dataList



