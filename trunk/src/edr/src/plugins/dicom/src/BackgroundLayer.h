/********************************************************************
	created:	2013/10/12
	created:	12:10:2013   22:32
	filename: 	BackgroundLayer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__BACKGROUNDLAYER_H__
#define HEADER_GUARD_DICOM__BACKGROUNDLAYER_H__

#include <plugins/dicom/ILayer.h>

namespace dicom {

class BackgroundLayer : public ILayer
{
public:
    BackgroundLayer(const QPixmap& p, const QString& name = QString("Background"));
	virtual ~BackgroundLayer() {}

public:
    virtual QPixmap render() const;
    virtual QString getName() const;

private:
    QPixmap pixmap;
    QString name;
};
DEFINE_SMART_POINTERS(BackgroundLayer);

}

#endif
