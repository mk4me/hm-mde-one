#include "DicomPCH.h"
#include "BackgroundLayer.h"

QPixmap dicom::BackgroundLayer::render() const
{
    return pixmap;
}

QString dicom::BackgroundLayer::getName() const
{
    return name;
}

dicom::BackgroundLayer::BackgroundLayer( const QPixmap& p, const QString& name /*= QString("Background")*/ ) :
    pixmap(p),
    name(name)
{

}

