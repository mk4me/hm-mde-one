/********************************************************************
	created:  
	created:  
	filename: FileReader2Modion.h
	author:	  Rafal Zowal
	
    purpose: Inderface do Marker.h

*********************************************************************/

#ifndef I_MARKER_H
#define I_MARKER_H

#include <vector>


// struktura ramkai czyli animacji. s³u¿a do przchowywania informacji zawarych w C3D file
struct MarkerFrame
{
    MarkerFrame() {m_time = positionx = positiony = positionz = 0; };

    float m_time;
    float positionx, positiony, positionz;

    osg::Vec3f m_position;
};


class IMarker
{
public:
    virtual ~IMarker() {};

    virtual void Clear() = 0;

    virtual int GetIndex() const = 0;
    virtual void SetIndex(int index) = 0;

    virtual std::vector<MarkerFrame* > GetAnimationList() const = 0;
    virtual void SetAnimationList(std::vector<MarkerFrame* > animationList) = 0;

    virtual void AddFrame(MarkerFrame *frame) = 0;

    virtual osg::Vec3f GetActualPossition() = 0;
    virtual void SetActualPossition(osg::Vec3f position) = 0;
};

#endif  //I_MARKER_H