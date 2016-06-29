/********************************************************************
	created:	2014/05/22
	created:	22:5:2014   10:07
	filename: 	ValueLayer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__VALUELAYER_H__
#define HEADER_GUARD_DICOM__VALUELAYER_H__

namespace dicom {
	//! Interfejs powsta�, by da�o si� przeiterowa� po layerach z warto�ciami
	//! nie znaj�c implementacji i typ�w z kt�rymi s� skompilowane 
	class IValueLayer : public ILayerItem
	{
	public:
		IValueLayer(const int val) : ILayerItem(val) {}
		virtual ~IValueLayer() {}
		//! \return warto�� przedstawiona w formie stringu
		virtual std::string valueAsString() const = 0;
	};
	DEFINE_SMART_POINTERS(IValueLayer);
}
#endif
