/********************************************************************
	created:  2010/10/15
	created:  15:10:2010   15:04
	filename: QOSGWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__QOSGWIDGET_H__
#define __HEADER_GUARD__QOSGWIDGET_H__

#include <QtOpenGL/QGLWidget>
#include <QtCore/QTimer>
#include <osgViewer/Viewer>
#include <osgViewer/GraphicsWindow>

/**
 *	Adapter pozwalaj¹cy na w³¹czenie widgetów z Qt do logiki Osg.
 */
class QOSGWidget : public QGLWidget
{
protected:
    //! Okno z punktu widzenia Osg.
    osg::ref_ptr<osgViewer::GraphicsWindowEmbedded> graphicsWindow;

public:
    //! Konstruktor w postaci w³aœciwej Qt.
    QOSGWidget( QWidget * parent = 0, const char * name = 0, 
        const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0 );

    //! Pusty polimorficzny destruktor.
    virtual ~QOSGWidget() {}

public:
    //! \return Okno z punktu widzenia Osg.
    inline osgViewer::GraphicsWindow* getGraphicsWindow() 
    { 
        return graphicsWindow.get(); 
    }
    //! \return Okno z punktu widzenia Osg.
    inline const osgViewer::GraphicsWindow* getGraphicsWindow() const 
    { 
        return graphicsWindow.get(); 
    }

protected:
    virtual void resizeGL( int width, int height );
    virtual void keyPressEvent( QKeyEvent* event );
    virtual void keyReleaseEvent( QKeyEvent* event );
    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void wheelEvent( QWheelEvent* event );

    osgGA::GUIEventAdapter::KeySymbol translateKey( QKeyEvent* event );
};

class QOSGViewer : public QOSGWidget, public osgViewer::Viewer
{
public:
    //! TODO: tymczasowo publiczne!
    QTimer _timer;

public:
    QOSGViewer(QWidget * parent = 0, const char * name = 0, 
        const QGLWidget * shareWidget = 0, Qt::WindowFlags f = 0) 
        : QOSGWidget( parent, name, shareWidget, f )
    {
        getCamera()->setViewport(new osg::Viewport(0,0,width(),height()));
        getCamera()->setProjectionMatrixAsPerspective(45.0, double(width())/height(), 1.0, 10000.0);
        getCamera()->setGraphicsContext(getGraphicsWindow());
        setThreadingModel(osgViewer::Viewer::SingleThreaded);
        connect(&_timer, SIGNAL(timeout()), this, SLOT(updateGL()));
        _timer.start(34);
    }

protected:
    virtual void paintGL()
    {
        frame();
    }
};

#endif  // __HEADER_GUARD__QOSGWIDGET_H__