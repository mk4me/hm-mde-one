/********************************************************************
	created:  2010/07/08
	created:  8:7:2010   13:33
	filename: StreamsViewOSGWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__STREAMSVIEWOSGWIDGET_H__
#define __HEADER_GUARD__STREAMSVIEWOSGWIDGET_H__

#include <stdint.h>
#include <map>
#include <algorithm>
#include <osgWidget/WindowManager>
#include <osg/ref_ptr>
#include <plugins/video/core/PixelFormat.h>
#include <core/GridModel.h>
#include <core/OsgWidgetGridView.h>
#include "StreamOsgWidget.h"

class OsgWidgetStreamHelper : public osg::Referenced
{
private:
    //! TODO: do usuniêcia!
    osg::ref_ptr<osg::Program> yuvProgram;
    //! TODO: do usuniêcia!
    osg::ref_ptr<osg::Shader> yuvTextureRectShader;
    //! TODO: do usuniêcia!
    osg::ref_ptr<osg::Shader> yuvTexture2DShader;
    //! Czy ma u¿ywaæ texture rectów?
    bool useTextureRect;
    //! Lista stworzonych widgetów.
    std::vector<osg::observer_ptr<video::StreamOsgWidget> > widgets;

public:
    //!
    OsgWidgetStreamHelper(const std::string& shaderDirectory, bool useTextureRect);
    //! 
    video::StreamOsgWidget* createWidget(osg::Image* image);
    //! \return
    const bool getUseTextureRect() const;
    //!
    void setUseTextureRect( bool useTextureRect );
    //!
    void setFormat(vm::PixelFormat format);
};

////!
//class StreamViewOSGWidget : public osgWidget::WindowManager
//{
//public:
//
//
//private:
//    //! Liczba wierszy.
//    int rows;
//    //! Liczba kolumn.
//    int columns;
//
//
//    //! Grid.
//    osg::ref_ptr<core::OsgWidgetGrid> gridView;
//        
//// 
//// public:
////     //!
////     core::GridModel model;
//
//public:
//    //! \param view
//    //! \param useTextureRect
//    //! \param width
//    //! \param height
//    //! \param mask
//    //! \param flags
//    StreamViewOSGWidget(osgViewer::View * view, bool useTextureRect, 
//        float width, float height, unsigned int mask, unsigned int flags);
//
//    
//    //! Dodaje strumieñ do widoku.
//    //! \param image Obrazek reprezentuj¹cy strumieñ.
//    //! \param row Indeks wiersza.
//    //! \param column Indeks kolumny.
//    void addStream(osg::Image * image, int row, int column);
//
//    //! 
//    //! \param count
//    //! \param rows
//    //! \param columns
//    void makeGrid(int count, int& rows, int& columns);
//
//    //! \param rows Liczba wierszy.
//    //! \param columns Liczba kolumn.
//    void setDimensions(int rows, int columns)
//    {
//        this->rows = rows;
//        this->columns = columns;
//    }
//    //! \return
//    inline int getRows() const
//    { 
//        return rows;
//    }
//    //! \return
//    inline int getColumns() const
//    { 
//        return columns;
//    }
//
//    //! \return
//    inline const bool getUseTextureRect() const
//    { 
//        return useTextureRect;
//    }
//
//
//    void setUseTextureRect( bool useTextureRect );
//    
//
//
//    void clear();
//
//    // TODO: tymczasowe
//    void setFormat(vm::PixelFormat format);
//
//};





#endif  // __HEADER_GUARD__STREAMSVIEWOSGWIDGET_H__