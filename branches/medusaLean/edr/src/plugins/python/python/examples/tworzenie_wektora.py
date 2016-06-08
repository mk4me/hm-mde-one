#skrypt tworzy dane wektorowe kompatybilne z aplikacj� i wrzuca je do drzewa analiz
import plugin_python

#tworzymy tablic� wektor�w
v = v3vector()

'''wype�niamy wektor danymi
  2000 pr�bek
  x - funkcja liniowa
  y - funkcja kwadratowa
  z - funkcja trygonometryczna'''
for x in range(1,2000):
	a = Vec3(x*0.01, x*x*0.001, 3*sin(x/200.0))
	v.append(a)

#tworzymy DataChannel...
vc = mde.createVectorChannel()
#... wype�niamy danymi ...
vc.setData(v)
#... ustalamy pr�bkowanie ...
vc.setFrequency(100)
#... i nazw�
vc.setName("Test1")
# utworzenie wpisu w drzewie analiz
mde.addVectorChannel(vc)


