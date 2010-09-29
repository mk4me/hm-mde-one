/********************************************************************
	created:  2010/07/08
	created:  8:7:2010   13:33
	filename: StreamsViewOSGWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__STREAMSVIEWOSGWIDGET_H__
#define __HEADER_GUARD__STREAMSVIEWOSGWIDGET_H__

//!
class StreamViewOSGWidget : public osgWidget::WindowManager
{
private:
  int rows;
  int columns;
  osg::ref_ptr<osg::Program> yuvShader;
  const bool useTextureRect;
public:
  //!
  StreamViewOSGWidget(osgViewer::View * view, int rows, int columns, bool useTextureRect, float width, float height, unsigned int mask, unsigned int flags);
  //!
  void addStream(osg::Image * image, int row, int column);

  //! \return
  inline int getRows() const
  { 
    return rows;
  }
  //! \return
  inline int getColumns() const
  { 
    return columns;
  }
};





#endif  // __HEADER_GUARD__STREAMSVIEWOSGWIDGET_H__