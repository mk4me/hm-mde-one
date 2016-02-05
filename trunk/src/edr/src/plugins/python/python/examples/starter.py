#skrypt tworzy dane wektorowe kompatybilne z aplikacją i wrzuca je do drzewa analiz

import imp

    
try:
	imp.find_module('plugin_mdePython')
	from plugin_mdePython import *
except ImportError:
	from plugin_mdePythond import *
	

from math import sin

#tworzymy tablicę wektorów
v = v3vector()

'''wypełniamy wektor danymi
  2000 próbek
  x - funkcja liniowa
  y - funkcja kwadratowa
  z - funkcja trygonometryczna'''
for x in range(1,2000):
	a = Vec3(x*0.001, x*x*0.000001, 3*sin(x/200.0))
	v.append(a)
	
mde = MdeBridge()
#tworzymy DataChannel...
vc = mde.createVectorChannel()
#... wypełniamy danymi ...
vc.setData(v)
#... ustalamy próbkowanie ...
vc.setFrequency(100)
#... i nazwę
vc.setName("Test1")
# utworzenie wpisu w drzewie analiz
mde.addVectorChannel(vc)
helpers = mde.getHelpers()
for h in helpers:
    h.createVisualizer()
#mde.close(1)