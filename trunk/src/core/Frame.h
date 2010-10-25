#ifndef FRAME_H
#define FRAME_H

#include <vector>

struct Frame
{
	//int idx;
    float m_time;
    float rotx, roty, rotz;
    float translationx, translationy, translationz;
};

#endif //FRAME_H