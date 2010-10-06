/********************************************************************
	created:  2010/09/28
	created:  28:9:2010   14:46
	filename: VideoWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOWIDGET_H__
#define __HEADER_GUARD__VIDEOWIDGET_H__

#include <stdint.h>
#include <map>
#include <vector>
#include <osg/ImageStream>
#include <osgViewer/Viewer>
#include <core/QOSGWidget.h>
#include "ui_video.h"
#include "StreamsViewOSGWidget.h"


class VideoWidget : public QWidget, Ui::VideoWidget
{
    Q_OBJECT
public:
    //! Tryby widoku.
    enum ViewType {
        ViewTypeGrid            = 0,
        ViewTypeHorizontal      = 1,
        ViewTypeVertical        = 2,
        ViewTypeSingle          = 3,
        ViewTypeSingleIdxMask   = 0xFFFF0000,
        ViewTypeSingleIdxShift  = 16
    };

    // //! Obrazki pogrupowane wg. po³o¿enia.
    // typedef std::map< std::pair<int, int>, osg::ref_ptr<osg::Image> > ImagesMap;
    // //! Typy widoku.
    // typedef std::map<ViewType, osg::ref_ptr<osg::Node> > ViewsList; 
    //! Lista strumieni.
    typedef std::vector<osg::ref_ptr<osg::Image> > ImagesList;

private:
    //! Bie¿¹cy typ widoku. W postaci unii, aby ³atwiejszy
    //! by³ podgl¹d w debuggerze.
    union {
        ViewType viewType;
        struct {
            uint16_t type;
            uint16_t idx;
        } rawViewType;
    };
    //! Obrazki pogrupowane wg. po³o¿enia.
    ImagesList images;
    //! Widget z widokiem OSG.
    QOSGViewer* viewer;
    //! Widok w³aœciwy.
    StreamViewOSGWidget* view;

    struct {
        ImagesList images;
        int rows;
        int columns;
    } last;

public:
    //! Konstruktor inicjalizujacu UI.
    VideoWidget();

    void init( std::vector<std::string> &files );
    virtual ~VideoWidget()
    {
    }

    void configureView(int rows, int columns, ImagesList & images);

public:

    //! \param selected Opcjonalny wskaŸnik na miejse, gdzie zapisaæ indeks 
    //!                 aktualnie wyœwietlanego strumienia.
    //! \return Typ widoku.
    ViewType getViewType(int * selected = NULL) const;
    //! \param viewType Typ widoku.
    //! \param selected Indeks strumienia. Ignorowany, gdy 
    //!                 viewType != ViewTypeSingle
    void setViewType(ViewType viewType, int selected = 0);

public slots: 
    void displayTypeChanged(int index);
    void outputFormatChanged(int index);
    void textureRectangleChecked(int checked);
};

#endif  // __HEADER_GUARD__VIDEOWIDGET_H__