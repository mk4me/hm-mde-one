#ifndef FRAME_H
#define FRAME_H

#include <vector>
#include <osg/Vec3f>
#include <osg/Quat>

// struktura ramkai czyli animacji. s�u�a do przchowywania informacji zawarych w AMC file
struct Frame
{
    Frame() {m_time = rotx = roty = rotz = translationx = translationy = translationz = 0; };
	//int idx;
    float m_time;
    float rotx, roty, rotz;
    float translationx, translationy, translationz;

    osg::Quat rotation;
    osg::Vec3f translation;
};

// struktura zawieraj�ca liste ramek - animacji z AMC file
// idx s�u�y do identyfikowania animcji do w�a�ciwej ko�ci.
struct BoneAnimation
{
    int idx;

    std::vector<Frame* > m_frames;

};

// struktura zawieraj�ca liste ko�ci
struct SkeletonAnimation
{
    std::string m_animationName;
    std::vector<BoneAnimation* > m_boneAnimationList;
};

struct SkeletonAnimationList
{
    std::vector<SkeletonAnimation* > m_SkeletonAnimationList;
};

#endif //FRAME_H