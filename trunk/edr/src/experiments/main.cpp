#include <iostream>
#include <core/C3DChannels.h>
#include <core/C3DParserEx.h>
using namespace std;
using namespace core;
using namespace osg;

int main()
{
    const int markersCount = 10;
    const int perSecond = 1000;
    MarkerSetPtr msp(new MarkerSet);
    for (int i = 0; i < markersCount; i++) {
        MarkerChannelPtr m(new MarkerChannel(perSecond));
        for (int j = 0; j < perSecond; j++) {
            osg::Vec3 t(j / 1000.0, j / 500.0, j / 33.0);
            m->addPoint(t);
        }
        msp->addMarker(m);
    }

    for (int i = 0; i < markersCount; i++) {
        Vec3 v =  msp->getPosition(i, 1.0);
        cout << i << " : " << v[0] << endl;
    }
    cout << "test" << endl;
    system("pause");
    return 0;
}