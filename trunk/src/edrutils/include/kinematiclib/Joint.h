#ifndef HEADER_GUARD_KINEMATICMODEL__JOINT_H__
#define HEADER_GUARD_KINEMATICMODEL__JOINT_H__

#include <vector>
#include <osg/Vec3d>
#include <boost/smart_ptr.hpp>
#include <boost/utility.hpp>
#include <kinematiclib/DegreeOfFreedom.h>

namespace kinematic
{
struct Joint;
typedef boost::shared_ptr<Joint> JointPtr;
typedef boost::shared_ptr<const Joint> JointConstPtr;
typedef boost::weak_ptr<Joint> JointWeakPtr;

//! Struktura przechowuje informacje o stawie
struct Joint : boost::noncopyable 
{
	//! unikalny identyfikator stawu
   int id;                              
   //! nazwa stawu
   std::string name;                    
   //! kierunek kosci w globalnym ukladzie odniesienia
   osg::Vec3d direction;                
   //! dlugosc kosci
   double length;                       
   //! okresla poczatkowa orientacje dla kosci
   osg::Vec3d axis;                     
   //! okresla kolejnosc rotacji
   Axis::Order order;                   
   //! wskaznik do rodzica
   JointWeakPtr parent;                      
   //! wskazniki do wszyskich dzieci w hierarchi
   std::vector<JointPtr> children;      
   //! stopnie swobody dla kosci (razem z limitami)
   std::vector<DegreeOfFreedom> dofs;   
   //! masa ciala zwiazana z ta koscia
   double bodymass;                     
   //! pozycja srodka ciezkosci wzgledem kosci
   double cofmass;                      

   Joint() :
    axis(0.0, 0.0, 0.0),
    length(0.0),
    order(Axis::XYZ),
    direction(0.0, 0.0, 0.0),
    name("unknown"),
    id(-1),
    bodymass(std::numeric_limits<double>::infinity()),
    cofmass(std::numeric_limits<double>::infinity())
   {}

	static void copyContent(const Joint& source, Joint& destination) 
	{
		destination.id = source.id;                              
		destination.name = source.name;                    
		destination.direction = source.direction;                
		destination.length = source.length;                       
		destination.axis = source.axis;                     
		destination.order = source.order;     
		destination.dofs = source.dofs;   
		destination.bodymass = source.bodymass;                     
		destination.cofmass = source.cofmass;  
	}
};

}

#endif