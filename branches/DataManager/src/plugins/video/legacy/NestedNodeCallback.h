/********************************************************************
	created:  2010/06/11
	created:  11:6:2010   11:09
	filename: NestedNodeCallback.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__NESTEDNODECALLBACK_H__
#define __HEADER_GUARD__NESTEDNODECALLBACK_H__

#include <osg/NodeCallback>

template <class Node>
struct NestedCallback : public osg::NodeCallback
{
  //! W�ze� w�a�ciwy.
  Node * node;
  //! \param node W�ze� do kt�rego si� podpi��.
  NestedCallback(Node * node) : node(node) {}
  //! Operator przekazywania callbacka do w�z�a.
  //! \param node
  //! \param nv
  void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    (*this->node)(node, nv);
  }
};

#endif  // __HEADER_GUARD__NESTEDNODECALLBACK_H__