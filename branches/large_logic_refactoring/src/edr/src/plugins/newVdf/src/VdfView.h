/********************************************************************
	created:	2012/12/20
	created:	20:12:2012   11:47
	filename: 	VdfView.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__VDFVIEW_H__
#define HEADER_GUARD_NEWVDF__VDFVIEW_H__

#include <QtGui/QWidget>
#include <QtGui/QGraphicsView>
#include <QtGui/QWheelEvent>

namespace vdf {

class VdfView : public QGraphicsView
{
    Q_OBJECT;
public:
    VdfView(QGraphicsScene* scene);

protected:
    virtual void wheelEvent(QWheelEvent *event);

private:
    qreal viewScale;
};

}

#endif
