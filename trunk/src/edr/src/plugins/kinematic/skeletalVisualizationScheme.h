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

//! Klasa stanowi polaczenie miedzy jointami a ich wizualizacja
class SkeletalVisualizationScheme : public VisualizationScheme
{
public:
    //! Konstruktor ustawia domyslne wartosci w tym kolory
    SkeletalVisualizationScheme();
	
public:
    //! \return zwraca strukture z jointami
    kinematic::JointAnglesCollectionConstPtr getJoints() const { return joints; }
    //! Ustawienie struktury z jointami, dokonywana jest pelna inicjalizacja schematu
    //! \param val ustawiane jointy
    void setJoints(kinematic::JointAnglesCollectionConstPtr val);
    //! \return liczba klatek animacji
    virtual int getNumFrames() const;
    //! \return dlugosc trwania jednej klatki
    double getFrameTime() const;
    //! \return czas trwania animacji
    virtual double getDuration() const;
    //! czy schemat jest poprawnie zainicjalizowany
    virtual bool hasData() const { return joints.get(); }
    //! \return srodek geometryczny
    virtual osg::Vec3 getCurrentPosition() const { return currentPosition; }
    //! Zwraca pozycje dla glownego wezla animacji
    //! \param time czas, dla ktorego jest pobierana pozycja
    //! \return pozycja roota
    osg::Vec3 getRootPosition(double time);

protected:
    //! aktualizacja schematu
    //! \param time czas, dla ktorego ma byc zaktualizowany schemat
    virtual void update( double time );

private:
    //! odswiezenie informacji o jointach
    void updateJoints();
    //! obliczenie transformacji szkieletu. Metoda wywoluje rekurencje
    //! \param time czas, dla ktorego maja byc liczone transformacje
    void updateJointTransforms(double time);
    //! obliczenie transformacji dla podanego czasu
    //! \param rotations rotacje jointow dla konkretnej klatki
    //! \param joint przetwarzany staw
    //! \param parentRot rotacja rodzica
    //! \param parentPos pozycja rodzica
    void updateJointTransforms(const std::vector<osg::Quat>& rotations, kinematic::hAnimJointPtr joint, 
        osg::Quat parentRot, osg::Vec3 parentPos);
    //! Metoda wywolywana rekurencyjnie, tworzy polaczenia miedzy jointami
    //! \param joint aktualnie przetwarzany joint
    void createSkeletonConnections(kinematic::hAnimJointPtr joint);

private:
    //! ulatwia ineksowanie jointow
    std::map<kinematic::hAnimJointPtr, int> visJoints;
    //! ustawiona struktura z jointami
	kinematic::JointAnglesCollectionConstPtr joints;
    //! pomocne przy zapelnianiu kolekcji jointow
    int counterHelper;
    //! aktualny srodek geometryczny
    osg::Vec3 currentPosition;
    //! kolor polaczen
    osg::Vec4 connectionColor;
    //! kolor markerow
    osg::Vec4 dotColor;
};
typedef core::shared_ptr<SkeletalVisualizationScheme> SkeletalVisualizationSchemePtr;
typedef core::shared_ptr<const SkeletalVisualizationScheme> SkeletalVisualizationSchemeConstPtr;

#endif
