/********************************************************************
	created:  2011/01/03
	created:  3:1:2011   16:42
	filename: QOsgComponentWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__QOSGCOMPONENTWIDGET_H__
#define __HEADER_GUARD__QOSGCOMPONENTWIDGET_H__

#include <osg/GraphicsContext>
#include <osgQt/GraphicsWindowQt>
#include <core/QOsgWidgetsDeprecated.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class QOsgContextWidget : public osgQt::GraphWidget
{
public:
    //! \param parent
    //! \param name
    //! \param shareWidget
    //! \param f
    QOsgContextWidget(QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0);
    //!
    virtual ~QOsgContextWidget();

    //! \return Kontekst OSG.
    inline osgViewer::GraphicsWindow* getGraphicsWindow()
    {
        return _gw;
    }
    //! \return Kontekst OSG.
    inline const osgViewer::GraphicsWindow* getGraphicsWindow() const
    {
        return _gw;
    }

public:
    //! Tworzy kamerê dla zadanego okna.
    osg::ref_ptr<osg::Camera> createCamera();
    //! Tworzy kamerê dla zadanego okna.
    osg::ref_ptr<osg::Camera> createCamera( osg::GraphicsContext::Traits* traits );
    //! Tworzy cechy kontekstu na podstawie bie¿¹cego okna.
    osg::ref_ptr<osg::GraphicsContext::Traits> createTraits();
};

////////////////////////////////////////////////////////////////////////////////

class QOsgView : public QOsgContextWidget, public osgViewer::View
{
public:
    //! \param parent
    //! \param name
    //! \param shareWidget
    //! \param f
    QOsgView(QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0);

public:

    //! Metoda usprawniaj¹ca testowanie ró¿nych wariantów ³¹czenia OSG/QT.
    void onInit(osgViewer::CompositeViewer* composite)
    {
        composite->addView(this);
    }

    //! 
    bool isRenderingEnabled() const;
    //! \param renderingEnabled
    void setRenderingEnabled(bool renderingEnabled);

protected:
    //! \param camera
    osg::GraphicsOperation* createRenderer(osg::Camera* camera);

private:
    //! Inicjalizacja. Byæ mo¿e bêdzie musia³a byæ publiczna, zobaczymy.
    void init();
};

////////////////////////////////////////////////////////////////////////////////

class QOsgViewer : public QOsgContextWidget, public osgViewer::Viewer
{
public:
    //! Domyœlny okres timera.
    static const int defaultInterval = 16;

private:
    //! Timer odmierzaj¹cy kolejne ramki.
    QTimer frameTimer;
    //! Czy ramki maj¹ byæ omijane gdy widget jest niewidoczny? Dzia³a tylko dla automatycznego timera.
    bool skipFramesIfInvisible;

public:
    //! \param parent
    //! \param name
    //! \param shareWidget
    //! \param f
    QOsgViewer(QWidget * parent = 0, const char * name = 0, const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0);

    virtual ~QOsgViewer();

    //! \return Czy timer jest aktywny? Je¿eli nie ramki nie bêd¹ aktualizowane i nale¿y dodaæ go do rodzica, czyli
    //! do osgViewer::CompositeViewer
    inline bool isTimerActive() const
    {
        return frameTimer.isActive();
    }
    //! \params active Czy timer jest aktywny? Je¿eli nie ramki nie bêd¹ aktualizowane i nale¿y dodaæ go do rodzica, czyli
    //! do osgViewer::CompositeViewer
    inline void setTimerActive(bool active)
    {
        frameTimer.start();
    }

    //! Okres timera aktualizacyjnego.
    inline int getTimerInterval() const
    {
        return frameTimer.interval();
    }
    //! Okres timera aktualizacyjnego.
    inline void setTimerInterval(int msecs)
    {
        frameTimer.setInterval(msecs);
    }
    
    //! \return
    bool isSkippingFramesIfInvisible() const
    { 
        return skipFramesIfInvisible;
    }
    //! \param skipFramesIfInvisible
    void setSkipFramesIfInvisible(bool skipFramesIfInvisible) 
    { 
        this->skipFramesIfInvisible = skipFramesIfInvisible; 
    }

    //! Metoda usprawniaj¹ca testowanie ró¿nych wariantów ³¹czenia OSG/QT.
    void onInit(osgViewer::CompositeViewer* composite)
    {
        if ( !isTimerActive() ) {
            composite->addView(this);
        }
    }

    //! 
    bool isRenderingEnabled() const;
    //! \param renderingEnabled
    void setRenderingEnabled(bool renderingEnabled);

protected:
    //! 
    virtual void paintGL();

    //! \param camera
    osg::GraphicsOperation* createRenderer(osg::Camera* camera);

private:
    //! Inicjalizacja. Byæ mo¿e bêdzie musia³a byæ publiczna, zobaczymy.
    void init();
};

////////////////////////////////////////////////////////////////////////////////

typedef QOsgDeprecatedViewer QOsgDefaultWidget;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__QOSGCOMPONENTWIDGET_H__