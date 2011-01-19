#include "Marker.h"



//--------------------------------------------------------------------------------------------------
Marker::Marker()
{
    m_index = 0;
    m_actualPosition = osg::Vec3f(0.0, 0.0, 0.0);
    m_animationList.clear();
}

//--------------------------------------------------------------------------------------------------
void Marker::Clear()
{
    m_index = 0;
    m_animationList.clear();
}

//--------------------------------------------------------------------------------------------------
int Marker::GetIndex() const
{
    return m_index;
}

//--------------------------------------------------------------------------------------------------
void Marker::SetIndex( int index )
{
    m_index = index;
}

//--------------------------------------------------------------------------------------------------
std::vector<MarkerFrame* > Marker::GetAnimationList() const
{
    return m_animationList;
}

//--------------------------------------------------------------------------------------------------
void Marker::SetAnimationList(std::vector<MarkerFrame* > animationList)
{
    m_animationList = animationList;
}

//--------------------------------------------------------------------------------------------------
void Marker::AddFrame(MarkerFrame *frame)
{
    m_animationList.push_back(frame);
}

//--------------------------------------------------------------------------------------------------
osg::Vec3f Marker::GetActualPossition()
{
    return m_actualPosition;
}

//--------------------------------------------------------------------------------------------------
void Marker::SetActualPossition( osg::Vec3f position )
{
    m_actualPosition = position;
}
