#ifndef HEADER_GUARD_KINEMATICMODEL__SKELETON_H__
#define HEADER_GUARD_KINEMATICMODEL__SKELETON_H__

#include <boost/smart_ptr.hpp>
#include <boost/utility.hpp>
#include "DegreeOfFreedom.h"
namespace kinematic
{
    /// \brief  Klasa przechowuje hierarchie kości 
    class Skeleton : boost::noncopyable
    {
    public:
        Skeleton();

    public: 
        /// \brief  Zwraca korzeń, czyli kość od której zaczyna się hierarchia. 
        /// \return wskaźnik do kości. 
        JointConstPtr getRoot() const { return this->root; }
		JointPtr getRoot() { return this->root; }
        /// \brief  Ustawia korzeń, czyli kość od której zaczyna się hierarchia. 
        /// \param  root  wskaźnik do kości roota. 
        void setRoot(JointPtr root) { this->root = root; }
        /// \brief  Zwraca nazwę korzenia
        const std::string& getRootName() const { return this->rootName; };
        /// \brief  Ustawia nazwę korzenia
        void setRootName(const std::string& name) { this->rootName = name; }
        /// \brief  zwraca początkowa pozycje w globalnym układzie odniesienia
        osg::Vec3d getPosition() const { return this->position; }
        /// \brief  ustawia początkowa pozycje w globalnym układzie odniesienia
        void setPosition(osg::Vec3d position) { this->position = position; }
        /// \brief  zwraca początkowa orientacje w globalnym układzie odniesienia
        osg::Vec3d getOrientation() const { return this->orientation; }
        /// \brief  ustawia początkowa orientacje w globalnym układzie odniesienia
        void setOrientation(osg::Vec3d orientation) { this->orientation = orientation; }
        /// \brief  zwraca kolejność rotacji (sekcja axis)
        Axis::Order getAxis() const { return this->axisOrder; }
        /// \brief  ustawia kolejność rotacji (sekcja axis)
        void setAxis(Axis::Order order) {this->axisOrder = order; }
        /// \brief  zwraca kolejność rotacji (sekcja order)
        Axis::Order getOrder() const { return this->dataOrder; }
        /// \brief  ustawia kolejność rotacji (sekcja order)
        void setOrder(Axis::Order order) {this->dataOrder = order; }
        /// \return głęboka kopia obiektu
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
        /// \brief metoda pomocniczna przy tworzeniu głębokiej kopii
        /// \return kopia hierarchii poniżej roota
		JointPtr cloneHierarchy(JointConstPtr root) const {
			JointPtr current(new Joint());
			Joint::copyContent(*root, *current);
			for (auto it = root->children.cbegin(); it != root->children.cend(); ++it) {
				cloneHierarchy(*it, current);
			}
			return current;
		}
        /// \brief metoda pomocniczna przy tworzeniu głębokiej kopii
        /// \return kopia całej hierarchii 
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
		//! korzeń, czyli kość od której zaczyna się hierarchia. 
        JointPtr root;          
		//! początkowa pozycja w globalnym układzie odniesienia
        osg::Vec3d position;      
		//! początkowa orientacja w globalnym układzie odniesienia
        osg::Vec3d orientation;   
		//! kolejność rotacji (sekcja axis)
        Axis::Order axisOrder;     
		//! kolejność rotacji (sekcja order)
        Axis::Order dataOrder;     
		//! nazwa korzenia (w bvh)
        std::string rootName;      
    };
	typedef boost::shared_ptr<Skeleton> SkeletonPtr;
	typedef boost::shared_ptr<const Skeleton> SkeletonConstPtr;
}
#endif
