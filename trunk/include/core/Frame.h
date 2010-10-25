#ifndef FRAME_H
#define FRAME_H

#include <vector>
struct Srot
{
	float rotx, roty, rotz;
};

struct Stran
{
	float translationx, translationy, translationz;
};

struct Frame
{
	int idx;
    float m_time;
	std::vector<Srot*> rotation;
    std::vector<Stran*> translation;
};

#endif //FRAME_H