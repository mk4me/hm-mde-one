#ifndef HEADER_GUARD_KINEMATICMODEL__JOINT_H__
#define HEADER_GUARD_KINEMATICMODEL__JOINT_H__

#include "DegreeOfFreedom.h"
namespace kinematic
{
struct Joint;
typedef boost::shared_ptr<Joint> JointPtr;
typedef boost::weak_ptr<Joint> WeakPtr;
/*! 
 * \brief Struktura przechowuje informacje o stawie
 */
struct Joint {
   /*!
   * Inteligentny wskaznik do stawu
   */
   //typedef boost::shared_ptr<Joint> Ptr;
   /*!
   * Wskaznik do parenta (slaby zeby nie bylo zapetlen)
   */
   //typedef boost::weak_ptr<Joint> WeakPtr;

   int id;                              //!< unikalny identyfikator stawu
   std::string name;                    //!< nazwa stawu
   osg::Vec3d direction;                //!< kierunek kosci w globalnym ukladzie odniesienia
   double length;                       //!< dlugosc kosci
   osg::Vec3d axis;                     //!< okresla poczatkowa orientacje dla kosci
   Axis::Order order;                   //!< okresla kolejnosc rotacji
   WeakPtr parent;                      //!< wskaznik do rodzica
   std::vector<JointPtr> children;     //!< wskazniki do wszyskich dzieci w hierarchi
   std::vector<DegreeOfFreedom> dofs;   //!< stopnie swobody dla kosci (razem z limitami)
   double bodymass;                     //!< masa ciala zwiazana z ta koscia
   double cofmass;                      //!< pozycja srodka ciezkosci wzgledem kosci

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

    virtual ~Joint() {}
    
    /// \brief  Kopiuje zawartosc kosci bez kopiowania informacji o rodzicu i dzieciach
    /// \param  source      Kosc, z ktorej kopiujemy
    /// \param  destination Kosc, do ktorej trafiaja informacje. 
    static void copyContent(JointPtr source, JointPtr destination);

    /// \brief  Tworzy gleboka kopie hierarchii. 
    /// \param  bone    The bone. 
    /// \return A copy of this object. 
    static JointPtr clone(JointPtr bone);

private:
    static JointPtr clone(JointPtr bone, JointPtr clonedParent);
};

}

#endif