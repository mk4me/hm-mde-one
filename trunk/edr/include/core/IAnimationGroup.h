#ifndef I_ANIMATION_GROUP_H
#define I_ANIMATION_GROUP_H

#include <string>
class IAnimationNode;

class IAnimationGroup
{
public:
    virtual ~IAnimationGroup() {};

	virtual void setPosition(unsigned int p) = 0; // set position
    virtual void setLength(double length) = 0; // set length
    virtual bool valid() = 0; // this valid
    virtual bool next() = 0; // next exists?
    virtual bool prev() = 0; // prev exists?
    virtual bool goNext() = 0; // increases counter
    virtual bool goPrev() = 0; // decreases counter
    virtual double getLength() = 0; // get length
    virtual int getPosition() = 0; // get position
    virtual int getNumOfFrames() = 0; // get number of frames
	virtual std::string GetName() = 0; // get Name
    virtual IAnimationNode* getActNode() = 0; // get actual animation node
    virtual IAnimationNode* getNextNode() = 0; // get actual animation node
    virtual IAnimationNode* getPrevNode() = 0; // get previous animation node
};

#endif // I_ANIMATION_GROUP_H