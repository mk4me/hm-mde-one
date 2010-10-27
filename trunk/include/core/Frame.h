#ifndef FRAME_H
#define FRAME_H

#include <vector>

// struktura ramkai czyli animacji. s�u�a do przchowywania informacji zawarych w AMC file
struct Frame
{
	//int idx;
    float m_time;
    float rotx, roty, rotz;
    float translationx, translationy, translationz;
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
    std::vector<BoneAnimation* > m_boneAnimationList;
};

#endif //FRAME_H