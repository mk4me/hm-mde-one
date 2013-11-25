/********************************************************************
	created:	2013/10/12
	created:	12:10:2013   22:27
	filename: 	ILayer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__ILAYER_H__
#define HEADER_GUARD_DICOM__ILAYER_H__

namespace dicom {

class ILayer
{
public:
	virtual ~ILayer() {}

    // TODO: Zmienic (faktyczna obsluga warstw)
    virtual QPixmap render() const = 0; 
    virtual QString getName() const = 0;

};
DEFINE_SMART_POINTERS(ILayer);

}

#endif
