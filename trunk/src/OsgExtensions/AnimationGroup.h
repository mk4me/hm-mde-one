#ifndef ANIMATION_GROUP_H
#define ANIMATION_GROUP_H

#include <osg/Group>
#include <core/IAnimationNode.h>
#include <core/IAnimationGroup.h>

//////////////////////////////////////////////////////////////////////////
// group holding animations
class CAnimationGroup :public IAnimationGroup, public osg::Group
{
		double			_length;

		unsigned int	_frames;

		unsigned int	_counter;

	protected:

		// d - tor
		~CAnimationGroup();

	public:

		// get number of frames
		int		getNumOfFrames()			{ return _frames; }

		// set length
		void	setLength(double length)	{ _length = length; }

		// get length
		double	getLength()					{ return _length; }
	
		// this valid
		bool	valid()						{ return _counter < _frames; }

		// next exists?
		bool	next()						{ return (_counter < _frames - 1); }

		// prev exists?
		bool	prev()						{ return (_counter >= 0); }

		// increases counter
		bool	goNext()					{ ++_counter; return (_counter < _frames); }

		// decreases counter
		bool	goPrev()					{ --_counter; return (_counter >= 0); }

		// set position
		void	setPosition(unsigned int p)	{ _counter = p; }

		// get position
		int		getPosition()				{ return _counter; }

		virtual std::string GetName();

		// get actual animation node
		IAnimationNode*	getActNode()		{ return dynamic_cast<IAnimationNode*>(getChild(_counter)); }

		// get actual animation node
		IAnimationNode*	getNextNode();

		// get previous animation node
		IAnimationNode*	getPrevNode();

		// c - tor
		CAnimationGroup(unsigned int n);
};

#endif