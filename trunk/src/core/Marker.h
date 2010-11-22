#ifndef MARKER_H
#define MARKER_H

#include <osg/Vec3f>

#include <core/IMarker.h>
#include <vector>



class Marker: public IMarker
{
public:
    Marker();

    virtual void Clear();

    virtual int GetIndex() const;
    virtual void SetIndex(int index);

    virtual std::vector<MarkerFrame* > GetAnimationList() const;
    virtual void SetAnimationList(std::vector<MarkerFrame* > animationList);

    virtual void AddFrame(MarkerFrame *frame);

    virtual osg::Vec3f GetActualPossition();
    virtual void SetActualPossition(osg::Vec3f position);

private:
    int m_index;

    osg::Vec3f m_actualPosition;
    std::vector<MarkerFrame* > m_animationList;
};

#endif //MARKER_H