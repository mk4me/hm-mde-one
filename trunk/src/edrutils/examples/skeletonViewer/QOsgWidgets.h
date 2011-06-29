/********************************************************************
	created:  2011/01/03
	created:  3:1:2011   16:42
	filename: QOsgComponentWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__QOSGCOMPONENTWIDGET_H__
#define __HEADER_GUARD__QOSGCOMPONENTWIDGET_H__
#include <osgViewer/CompositeViewer>
#include <osg/GraphicsContext>
#include <osgQt/GraphicsWindowQt>
#include "QOsgWidgetsDeprecated.h"

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////

class QOsgContextWidget : public osgQt::GLWidget
{
public:
    //! T�umaczy format osg do formatu qt
    static QGLFormat translateFormat(const osg::GraphicsContext::Traits& traits);

public:
    //! \param parent
    //! \param name
    //! \param shareWidget
    //! \param f
    QOsgContextWidget(QWidget * parent, const osg::GraphicsContext::Traits* traits = 0, Qt::WindowFlags f = 0);
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
};

////////////////////////////////////////////////////////////////////////////////

class QOsgView : public QOsgContextWidget, public osgViewer::View
{
public:
    //! \param parent
    //! \param name
    //! \param shareWidget
    //! \param f
    QOsgView(QWidget * parent = 0, const osg::GraphicsContext::Traits* traits = 0, Qt::WindowFlags f = 0);

public:

    //! Metoda usprawniaj�ca testowanie r�nych wariant�w ��czenia OSG/QT.
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
    //! Inicjalizacja. By� mo�e b�dzie musia�a by� publiczna, zobaczymy.
    void init();
};

////////////////////////////////////////////////////////////////////////////////

class QOsgViewer : public QOsgContextWidget, public osgViewer::Viewer
{
public:
    //! Domy�lny okres timera.
    static const int defaultInterval = 16;

private:
    //! Timer odmierzaj�cy kolejne ramki.
    QTimer frameTimer;
    //! Czy ramki maj� by� omijane gdy widget jest niewidoczny? Dzia�a tylko dla automatycznego timera.
    bool skipFramesIfInvisible;

public:
    //! \param parent
    //! \param name
    //! \param shareWidget
    //! \param f
    QOsgViewer(QWidget * parent = 0, const osg::GraphicsContext::Traits* traits = 0, Qt::WindowFlags f = 0);

    virtual ~QOsgViewer();

    //! \return Czy timer jest aktywny? Je�eli nie ramki nie b�d� aktualizowane i nale�y doda� go do rodzica, czyli
    //! do osgViewer::CompositeViewer
    inline bool isTimerActive() const
    {
        return frameTimer.isActive();
    }
    //! \params active Czy timer jest aktywny? Je�eli nie ramki nie b�d� aktualizowane i nale�y doda� go do rodzica, czyli
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

    //! Metoda usprawniaj�ca testowanie r�nych wariant�w ��czenia OSG/QT.
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
    //! Inicjalizacja. By� mo�e b�dzie musia�a by� publiczna, zobaczymy.
    void init();
};

////////////////////////////////////////////////////////////////////////////////

typedef QOsgViewer QOsgDefaultWidget;

////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////


#endif  // __HEADER_GUARD__QOSGCOMPONENTWIDGET_H__