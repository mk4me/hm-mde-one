#ifndef HEADER_GUARD_KINEMATICMODEL__SKELETON_H__
#define HEADER_GUARD_KINEMATICMODEL__SKELETON_H__

#include <boost/smart_ptr.hpp>
#include <boost/utility.hpp>
#include "DegreeOfFreedom.h"
namespace kinematic
{
    /// \brief  Klasa przechowuje hierarchie kosci 
    class Skeleton : boost::noncopyable
    {
    public:
        Skeleton();

    public: 
        /// \brief  Zwraca korzen, czyli kosc od ktorej zaczyna sie hierarchia. 
        /// \return Wskaznik do kosci. 
        JointConstPtr getRoot() const { return this->root; }
		JointPtr getRoot() { return this->root; }
        /// \brief  Ustawia korzen, czyli kosc od ktorej zaczyna sie hierarchia. 
        /// \param  root  Wskaznik do kosci roota. 
        void setRoot(JointPtr root) { this->root = root; }
        /// \brief  Zwraca nazwe korzenia
        const std::string& getRootName() const { return this->rootName; };
        /// \brief  Ustawia nazwe korzenia
        void setRootName(const std::string& name) { this->rootName = name; }
        /// \brief  zwraca poczatkowa pozycje w globalnym ukladzie odniesienia
        osg::Vec3d getPosition() const { return this->position; }
        /// \brief  ustawia poczatkowa pozycje w globalnym ukladzie odniesienia
        void setPosition(osg::Vec3d position) { this->position = position; }
        /// \brief  zwraca poczatkowa orientacje w globalnym ukladzie odniesienia
        osg::Vec3d getOrientation() const { return this->orientation; }
        /// \brief  ustawia poczatkowa orientacje w globalnym ukladzie odniesienia
        void setOrientation(osg::Vec3d orientation) { this->orientation = orientation; }
        /// \brief  zwraca kolejnosc rotacji
        Axis::Order getAxis() const { return this->axisOrder; }
        /// \brief  ustawia kolejnosc rotacji
        void setAxis(Axis::Order order) {this->axisOrder = order; }

        Axis::Order getOrder() const { return this->dataOrder; }
        void setOrder(Axis::Order order) {this->dataOrder = order; }

		virtual Skeleton* clone() const
		{
			Skeleton* clone = new Skeleton();      
			clone->	position = this->position;   
			clone->orientation = this->orientation;
			clone->axisOrder = this->axisOrder;  
			clone->dataOrder = this->dataOrder;  
			clone->rootName = this->rootName;
			clone->root = cloneHierarchy(this->root);
			return clone;
		}

	private:
		JointPtr cloneHierarchy(JointConstPtr root) const {
			JointPtr current(new Joint());
			Joint::copyContent(*root, *current);
			for (auto it = root->children.cbegin(); it != root->children.cend(); ++it) {
				cloneHierarchy(*it, current);
			}
			return current;
		}

		void cloneHierarchy(JointConstPtr currentSourceJoint, JointPtr destinationParent) const
		{
			JointPtr current(new Joint());
			Joint::copyContent(*currentSourceJoint, *current);
			current->parent = JointWeakPtr(destinationParent);
			if (destinationParent) {
				destinationParent->children.push_back(current);
			}
			for (auto it = currentSourceJoint->children.cbegin(); it != currentSourceJoint->children.cend(); ++it) {
				cloneHierarchy(*it, current);
			}
			
		}

    private:							
		//! korzen, czyli kosc od ktorej zaczyna sie hierarchia. 
        JointPtr root;          
		//! poczatkowa pozycja w globalnym ukladzie odniesienia
        osg::Vec3d position;      
		//! poczatkowa orientacja w globalnym ukladzie odniesienia
        osg::Vec3d orientation;   
		//! kolejnosc rotacji (sekcja axis)
        Axis::Order axisOrder;     
		//! kolejnosc rotacji (sekcja order)
        Axis::Order dataOrder;     
		//! nazwa korzenia (w bvh)
        std::string rootName;      
    };
	typedef boost::shared_ptr<Skeleton> SkeletonPtr;
	typedef boost::shared_ptr<const Skeleton> SkeletonConstPtr;
}
#endif