/********************************************************************
	created:	2012/04/23
	created:	23:4:2012   12:45
	filename: 	skeletalVisualizationScheme.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__SKELETALVISUALIZATIONSCHEME_H__
#define HEADER_GUARD_KINEMATIC__SKELETALVISUALIZATIONSCHEME_H__

#include "VisualizationScheme.h"

//! Klasa stanowi połączenie między jointami a ich wizualizacja
class SkeletalVisualizationScheme : public VisualizationScheme
{
public:
    //! Konstruktor ustawia domyślne wartości w tym kolory
    SkeletalVisualizationScheme();
	
public:
    //! \return zwraca strukture z jointami
    kinematic::JointAnglesCollectionConstPtr getJoints() const { return joints; }
    //! Ustawienie struktury z jointami, dokonywana jest pelna inicjalizacja schematu
    //! \param val ustawiane jointy
    void setJoints(kinematic::JointAnglesCollectionConstPtr val);
    //! \return liczba klatek animacji
    virtual int getNumFrames() const;
    //! \return długość trwania jednej klatki
    double getFrameTime() const;
    //! \return czas trwania animacji
    virtual double getDuration() const;
    //! czy schemat jest poprawnie zainicjalizowany
    virtual bool hasData() const { return joints.get(); }
    //! \return środek geometryczny
    virtual osg::Vec3 getCurrentPosition() const { return currentPosition; }
    //! Zwraca pozycje dla głównego węzła animacji
    //! \param time czas, dla którego jest pobierana pozycja
    //! \return pozycja roota
    osg::Vec3 getRootPosition(double time);

protected:
    //! aktualizacja schematu
    //! \param time czas, dla którego ma być zaktualizowany schemat
    virtual void update( double time );

private:
    //! odświeżenie informacji o jointach
    void updateJoints();
    //! obliczenie transformacji szkieletu. Metoda wywołuje rekurencje
    //! \param time czas, dla którego maja być liczone transformacje
    void updateJointTransforms(double time);
    //! obliczenie transformacji dla podanego czasu
    //! \param rotations rotacje jointów dla konkretnej klatki
    //! \param joint przetwarzany staw
    //! \param parentRot rotacja rodzica
    //! \param parentPos pozycja rodzica
    void updateJointTransforms(const std::vector<osg::Quat>& rotations, kinematic::hAnimJointPtr joint, 
        osg::Quat parentRot, osg::Vec3 parentPos);
    //! Metoda wywoływana rekurencyjnie, tworzy połączenia między jointami
    //! \param joint aktualnie przetwarzany joint
    void createSkeletonConnections(kinematic::hAnimJointPtr joint);

private:
    //! ułatwia ineksowanie jointów
    std::map<kinematic::hAnimJointPtr, int> visJoints;
    //! ustawiona struktura z jointami
	kinematic::JointAnglesCollectionConstPtr joints;
    //! pomocne przy zapełnianiu kolekcji jointów
    int counterHelper;
    //! aktualny środek geometryczny
    osg::Vec3 currentPosition;
    //! kolor połączen
    osg::Vec4 connectionColor;
    //! kolor markerów
    osg::Vec4 dotColor;
};
typedef utils::shared_ptr<SkeletalVisualizationScheme> SkeletalVisualizationSchemePtr;
typedef utils::shared_ptr<const SkeletalVisualizationScheme> SkeletalVisualizationSchemeConstPtr;

#endif
