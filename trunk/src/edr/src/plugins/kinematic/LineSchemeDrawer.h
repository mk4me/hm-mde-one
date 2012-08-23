#ifndef HEADER_GUARD_LINESCHEMEDRAWER_H__
#define HEADER_GUARD_LINESCHEMEDRAWER_H__

#include <core/SmartPtr.h>
#include <osg/PositionAttitudeTransform>
#include "OsgSchemeDrawer.h"

//! Schemat odrysowywuj¹cy linie szkieletu
class LineSchemeDrawer : public OsgSchemeDrawer
{
public:
    //! Konstruktor
    LineSchemeDrawer() {}

public:
    //! Realizacja metody abstrakcyjnej (nieu¿ywana)
    virtual void draw();
    //! Wywo³ywane, gdy schemat zosta³ zmieniony i jego wizualizacja wymaga odswie¿enia
    virtual void update();
    //! Wywo³ywane przy zakoñczeniu dzia³ania, sprz¹tniêcie obiektów ze sceny
    virtual void deinit();
    //! Inicjalizacja drawera, tworzone s¹ wêz³y
    //! \param scheme Dostarczany schemat wizualizacji
    virtual void init(VisualizationSchemeConstPtr scheme);
    //! \return zwraca wêze³ ze stworzonymi po³¹czeniami
    virtual osg::ref_ptr<osg::Node> getNode();

private:
    //! wskaŸnik do wêz³a przechowywuj¹cego geometriê
    typedef osg::ref_ptr<osg::Geode> GeodePtr;
    //! wskaŸnik do wêz³a zawieraj¹cego transformacjê
    typedef osg::ref_ptr<osg::PositionAttitudeTransform> TransformPtr;
    //! po³¹czenie pomiêdzy stawami lub markerami
    typedef VisualizationScheme::Connection Connection;
    //! aktualny stan markera (kolor, pozycja, ...)
    typedef VisualizationScheme::State JointState;

private:
    //! Metoda tworzy liniê pomiêdzy punktami
    //! \param from pocz¹tek linii  
    //! \param to koniec linii
    //! \param color kolor linii
    void addLine(const osg::Vec3& from, const osg::Vec3& to, const osg::Vec4& color);

private:
    //! tutaj trafiaj¹ tworzone linie
    std::vector<GeodePtr> lines;
    //! bufory z wierzcho³kami dla linii
    std::vector< osg::ref_ptr<osg::Vec3Array> > buffers;
    //! wêze³ ze stworzonymi liniami
    osg::ref_ptr<osg::Group> node;
};

#endif
