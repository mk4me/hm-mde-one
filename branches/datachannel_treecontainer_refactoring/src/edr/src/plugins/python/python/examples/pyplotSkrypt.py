#Skrypt wczytuje pierwsz� za�adowan� tablic� wektor�w
import plugin_python
import sys
sys.path += ["C:\\Users\\Wojciech\\Anaconda\\Lib\\site-packages"]

import matplotlib.pyplot as plt

#listowanie za�adowanych danych
v = mde.listLoadedVectors()
if len(v)>1:
	#pobieramy pierwsz� tablic�
	vc = mde.getVectorChannel(v[0].first, v[0].second)
	data = vc.getData();
	#wype�nianie wykresu danymi
	plt.plot([v.x() for v in data])
	plt.ylabel('some numbers')
	#rysowanie wykresu
	plt.show()
else:
	print "no data loaded"






