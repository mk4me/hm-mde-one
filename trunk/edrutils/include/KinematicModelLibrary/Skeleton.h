#ifndef HEADER_GUARD_KINEMATICMODEL__SKELETON_H__
#define HEADER_GUARD_KINEMATICMODEL__SKELETON_H__

#include <boost/smart_ptr.hpp>
#include "DegreeOfFreedom.h"
namespace hmAnimation
{
    /// \brief  Klasa przechowuje hierarchie kosci 
    class Skeleton
    {
    public:
        Skeleton() : 
          position(0.0,0.0,0.0),
          orientation(0.0,0.0,0.0),
          axisOrder(Axis::XYZ),
          rootName("root")
          {
          }

    public: 

        /// \brief  Zwraca korzen, czyli kosc od ktorej zaczyna sie hierarchia. 
        /// \return Wskaznik do kosci. 
        const Joint::Ptr getRoot() const { return this->root; }
        /// \brief  Ustawia korzen, czyli kosc od ktorej zaczyna sie hierarchia. 
        /// \param  root  Wskaznik do kosci roota. 
        void setRoot(Joint::Ptr root) { this->root = root; }
        /// \brief  Zwraca nazwe korzenia
        std::string getRootName() { return this->rootName; };
        /// \brief  Ustawia nazwe korzenia
        void setRootName(const std::string& name) { this->rootName = name; }
        /// \brief  zwraca poczatkowa pozycje w globalnym ukladzie odniesienia
        osg::Vec3d getPosition() { return this->position; }
        /// \brief  ustawia poczatkowa pozycje w globalnym ukladzie odniesienia
        void setPosition(osg::Vec3d position) { this->position = position; }
        /// \brief  zwraca poczatkowa orientacje w globalnym ukladzie odniesienia
        osg::Vec3d getOrientation() { return this->orientation; }
        /// \brief  ustawia poczatkowa orientacje w globalnym ukladzie odniesienia
        void setOrientation(osg::Vec3d orientation) { this->orientation = orientation; }
        /// \brief  zwraca kolejnosc rotacji
        Axis::Order getAxisOrder() { return this->axisOrder; }
        /// \brief  ustawia kolejnosc rotacji
        void setAxisOrder(Axis::Order order) {this->axisOrder = order; }

    protected:
        Joint::Ptr       root;          //!< korzen, czyli kosc od ktorej zaczyna sie hierarchia. 
        osg::Vec3d       position;      //!< poczatkowa pozycja w globalnym ukladzie odniesienia
        osg::Vec3d       orientation;   //!< poczatkowa orientacja w globalnym ukladzie odniesienia
        Axis::Order      axisOrder;     //!< kolejnosc rotacji
        std::string      rootName;      //!< nazwa korzenia (w bvh)
    };
}
#endif