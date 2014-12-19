#ifndef HEADER_GUARD_KINEMATICMODEL__SKELETON_H__
#define HEADER_GUARD_KINEMATICMODEL__SKELETON_H__

#include <utils/SmartPtr.h>
#include <boost/utility.hpp>
#include "DegreeOfFreedom.h"
#include <osg/Vec3>
#include <osg/Matrix>
#include <kinematiclib/hAnimBone.h>
#include <kinematiclib/hAnimJoint.h>

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
		JointConstPtr getRoot() const               { return this->root; }
		JointPtr getRoot()                          { return this->root; }
		/// \brief  Ustawia korzeń, czyli kość od której zaczyna się hierarchia.
		/// \param  root  wskaźnik do kości roota.
		void setRoot(JointPtr root)                 { this->root = root; }
		/// \brief  Zwraca nazwę korzenia
		const std::string& getRootName() const      { return this->rootName; };
		/// \brief  Ustawia nazwę korzenia
		void setRootName(const std::string& name)   { this->rootName = name; }
		/// \brief  zwraca początkowa pozycje w globalnym układzie odniesienia
		osg::Vec3d getPosition() const              { return this->position; }
		/// \brief  ustawia początkowa pozycje w globalnym układzie odniesienia
		void setPosition(osg::Vec3d position)       { this->position = position; }
		/// \brief  zwraca początkowa orientacje w globalnym układzie odniesienia
		osg::Vec3d getOrientation() const           { return this->orientation; }
		/// \brief  ustawia początkowa orientacje w globalnym układzie odniesienia
		void setOrientation(osg::Vec3d orientation) { this->orientation = orientation; }
		/// \brief  zwraca kolejność rotacji (sekcja axis)
		Axis::Order getAxis() const                 { return this->axisOrder; }
		/// \brief  ustawia kolejność rotacji (sekcja axis)
		void setAxis(Axis::Order order)             { this->axisOrder = order; }
		/// \brief  zwraca kolejność rotacji (sekcja order)
		Axis::Order getOrder() const                { return this->dataOrder; }
		/// \brief  ustawia kolejność rotacji (sekcja order)
		void setOrder(Axis::Order order)            { this->dataOrder = order; }
		/// \return głęboka kopia obiektu
		virtual Skeleton* clone() const
		{
			std::auto_ptr<Skeleton> clone(new Skeleton());
			clone->position = this->position;
			clone->orientation = this->orientation;
			clone->axisOrder = this->axisOrder;
			clone->dataOrder = this->dataOrder;
			clone->rootName = this->rootName;
			clone->root = cloneHierarchy(this->root);
			return clone.release();
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
	typedef utils::shared_ptr<Skeleton> SkeletonPtr;
	typedef utils::shared_ptr<const Skeleton> SkeletonConstPtr;

	class SkeletonUtils
	{
	public:
		//! Metoda pomocnicza, do konwersji zapisu rotacji
		//! \param q kwaternion z liczona rotacja
		//! \return wyliczone katy Eulera
		static osg::Vec3 getEulerFromQuat(const osg::Quat& q);
		//! Metoda pomocnicza, do konwersji zapisu rotacji
		//! \param heading
		//! \param attitude
		//! \param bank
		//! \return wyliczony kwaternion
		static osg::Quat getQuatFromEuler(double heading, double attitude, double bank);
		//! Metoda pomocnicza, do konwersji zapisu rotacji
		//! \param euler katy eulera w postaci wektora
		//! \return wyliczony kwaternion
		static osg::Quat getQuatFromEuler(const osg::Vec3& euler);
		/// \brief  Wyszukuje i zwraca długość najdłuższej z kości w szkielecie
		/// \param  skeleton  Przeszukiwany szkielet
		/// \return długość najdłuższej z kości.
		static double getMaxBoneLength(const Skeleton& skeleton);
		/// \brief zwraca maksymalną długość kości
		/// \param  joint staw, dostarczający hierarchię
		/// \param  maxLength dotychczasowa, najdłuższa kość
		/// \return wyliczona długość.
		static double getMaxLength(const JointConstPtr & joint, double maxLength);
		//! obraca wektor o kąty eulera
		//! \param v obracany wektor
		//! \param rX kąt X
		//! \param rY kąt Y
		//! \param rZ kąt Z
		//! \return obliczony wektor
		static osg::Vec3 vectorRotation(osg::Vec3 v, double rX, double rY, double rZ);
		//! zwraca rotację względną pozmiędzy rodzicem a dzieckiem
		//! \param parent rodzic, dla którego liczona jest rotacja
		//! \param child dziecko, dla którego liczona jest rotacja
		//! \return kwaternion z obliczoną rotacją
		static osg::Quat rotationParentChild(hAnimJointPtr parent, hAnimJointPtr child);
		//! Tworzy rotację na podstawie kątów
		//! \param rX kąt X
		//! \param rY kąt Y
		//! \param rZ kąt Z
		//! \param order kolejność kątów
		//! \return wyliczony kwaternion
		static osg::Quat createRotation(const osg::Quat& rX, const osg::Quat& rY, const osg::Quat& rZ, Axis::Order order);
	};
}
#endif
