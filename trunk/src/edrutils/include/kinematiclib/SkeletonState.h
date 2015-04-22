/********************************************************************
	created:  2014/12/17	16:21:59
	filename: SkeletonState.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATIC__SKELETONSTATE_H__
#define __HEADER_GUARD_KINEMATIC__SKELETONSTATE_H__

#include <kinematiclib/Skeleton.h>
#include <acclaimformatslib/MotionData.h>
#include <biovisionformatslib/MotionData.h>
#include <osg/Vec3>
#include <osg/Quat>
#include <osg/Matrix>
#include <list>
#include <boost/bimap.hpp>
#include <osgutils/OsgSchemeDrawer.h>

namespace kinematic
{
	//! Klasa przedstawiaj�ca stan szkieletu - pozycje i orientacje poszczeg�lnych staw�w
	class SkeletonState
	{
	public:

		//! Typ indeksu w�z�a
		typedef unsigned int NodeIDX;
		//! Typ bimapy indeks�w w�z��w do nazw w�z��w
		typedef boost::bimap<NodeIDX, std::string> LinearizedNodesMapping;

		//! Struktura opisuj�ca prosta zmian� w szkielecie - translacja roota i lokalne rotacje staw�w
		//! pozycje rotacji w wektorze wg mapowania
		struct RigidCompleteStateChange
		{
			//! Translacja roota
			osg::Vec3 translation;
			//! Lokalne rotacje wszystkich staw�w
			std::vector<osg::Quat> rotations;
		};

		//! Struktura opisuj�ca prosta zmian� w szkielecie - translacja roota i lokalne rotacje staw�w
		//! rotacje wg mapowania
		struct RigidPartialStateChange
		{
			//! Translacja roota
			osg::Vec3 translation;
			//! Lokalne rotacje wybranych staw�w
			std::map<NodeIDX, osg::Quat> rotations;
		};

		//! Struktura opisuj�ca zmian� stawu
		struct JointStateChange
		{
			//! Lokalna translacja
			osg::Vec3 translation;
			//! Lokalna rotacja
			osg::Quat rotation;
		};

		//! Typ opisuj�cy pe�n� zmian� stanu szkieletu
		//! pozycja zmian wg mapowania
		typedef std::vector<JointStateChange> NonRigidCompleteStateChange;

		//! Typ opisuj�cy pe�n� zmian� stanu szkieletu
		//! zmiany wg mapowania
		typedef std::map<NodeIDX, JointStateChange> NonRigidPartialStateChange;

		//! Klasa reprezentuj�ca staw
		class JointData
		{
		private:
			//! Forward declaration
			class JointDataImpl;

			friend class SkeletonState;

		private:			
			//! \param name Nazwa w�z�a odpowiadaj�ca szkieletowi
			//! \param translation Pozycja|Translacja lokalna stawu
			//! \param rotation Orientacja|Rotacja lokalna stawu			
			JointData(const std::string & name, const osg::Vec3 & translation,
					  const osg::Quat & rotation);

		public:

			//! Destruktor
			~JointData();

			//! \return Nazwa stawu
			std::string name() const;

			//! \return Globalna pozycja
			osg::Vec3 globalPosition() const;

			//! \return Globalna orientacja
			osg::Quat globalOrientation() const;

			//! \return Lokalna pozycja względem rodzica
			osg::Vec3 localPosition() const;
			//! \return Lokalna orientacja względem rodzica
			osg::Quat localOrientation() const;

			//! \param translation Lokalne przesunięcie
			void localUpdate(const osg::Vec3 & translation);
			//! \param translation Globalne przesunięcie
			void globalUpdate(const osg::Vec3 & translation);
			//! \param position Globalna pozycja
			void setGlobal(const osg::Vec3 & position);
			//! \param position Lokalna pozycja
			void setLocal(const osg::Vec3 & position);

			//! \param rotation Rotacja lokalna
			void localUpdate(const osg::Quat & rotation);
			//! \param rotation Rotacja globalna
			void globalUpdate(const osg::Quat & rotation);
			//! \param orientation Globalna orientacja
			void setGlobal(const osg::Quat & orientation);
			//! \param orientation Lokalna orientacja
			void setLocal(const osg::Quat & orientation);

			//! \param translation Lokalne przesunięcie
			//! \param rotation Rotacja lokalna
			void localUpdate(const osg::Vec3 & translation, const osg::Quat & rotation);

			//! \param translation Globalne przesunięcie
			//! \param rotation Rotacja globalna
			void globalUpdate(const osg::Vec3 & translation, const osg::Quat & rotation);
		
			//! \param position Globalna pozycja
			//! \param orientation Globalna orientacja
			void setGlobal(const osg::Vec3 & position, const osg::Quat & orientation);
			//! \param position Lokalna pozycja
			//! \param orientation Lokalna orientacja
			void setLocal(const osg::Vec3 & position, const osg::Quat & orientation);
		private:
			//! Prywatna implementacja
			utils::shared_ptr<JointDataImpl> impl;
		};
		
		//! Typ jointa stanu
		typedef utils::TreeNodeT<JointData> Joint;
		//! Typ wska�nik�w do joint�w
		DEFINE_SMART_POINTERS(Joint);

		//! Typ opisujący mapowanie jointów do indeksów
		typedef std::map<kinematic::SkeletonState::JointConstPtr, unsigned int> Joint2Index;
	private:

		//! \param root Staw rodzic stanu
		SkeletonState(JointPtr root);

	public:

		//! \param Other Stan kt�ry przejmujemy
		SkeletonState(SkeletonState && Other);

		//! \param skeleton Szkielet dla kt�ego tworzymy stan
		//! \return Stan bazowy szkieletu
		static SkeletonState create(const Skeleton & skeleton);

		//! \param skeleton Szkielet dla któego tworzymy zmianę stanu szkieletu
		//! \param motionData Dane ruchu w formacie acclaim - ramka danych
		//! \param mapping Mapowanie danych acclaim do szkieletu
		//! \return Zmiana stanu szkieletu
		static RigidPartialStateChange convert(const acclaim::Skeleton & skeleton,
			const acclaim::MotionData::FrameData & motionData,
			const LinearizedNodesMapping & mapping);

		//! \param skeleton Szkielet dla któego tworzymy zmianę stanu szkieletu
		//! \param motionData Dane ruchu w formacie acclaim - ramka danych
		//! \param mapping Mapowanie danych acclaim do szkieletu
		//! \return Zmiana stanu szkieletu
		static acclaim::MotionData::BonesData convert(const acclaim::Skeleton & skeleton,
			const SkeletonState & skeletonState);

		//! \param skeleton Szkielet dla któego tworzymy zmianę stanu szkieletu
		//! \param motionData Dane ruchu w formacie biovision - ramka danych		
		//! \return Zmiana stanu szkieletu
		static RigidCompleteStateChange convert(const biovision::Skeleton & skeleton,
			const biovision::MotionData::FrameJointData & motionData);

		//! \param skeleton Szkielet dla którego generujemy mapowanie wszystkich jointów
		//! \return Mapowanie jointów szkieletu
		static LinearizedNodesMapping createMapping(const Skeleton & skeleton);

		//! \param skeleton Szkielet dla którego generujemy mapowanie aktywnych jointów (mających dzieci)
		//! \return Mapowanie aktywnych jointów szkieletu
		static LinearizedNodesMapping createActiveMapping(const Skeleton & skeleton);

		//! \param mapping Mapowanie jointów do indeksów
		//! \param sChange Zmiana stanu (tylko orientacje)
		//! \return Kompletna zmiana stanu (dodano zerowe translacje)
		static NonRigidCompleteStateChange convertStateChange(const LinearizedNodesMapping &mapping,
			const RigidPartialStateChange &sChange);

		//! \param skeletonState [out] Aktualizowany stan szkieletu
		//! \param stateChange Zmiana stanu szkieletu
		//! \param mapping Mapowanie jointów do indeksów
		static void update(SkeletonState & skeletonState, const NonRigidCompleteStateChange & stateChange,
			const LinearizedNodesMapping & mapping);

		////! \param skeletonState [out] Aktualizowany stan szkieletu
		////! \param newState Nowy stan szkieletu
		//! \param mapping Mapowanie jointów do indeksów
		static void setLocal(SkeletonState & skeletonState, const NonRigidCompleteStateChange & newState,
				const LinearizedNodesMapping & mapping);

		//! \param skeletonState [out] Aktualizowany stan szkieletu
		//! \param stateChange Zmiana stanu szkieletu
		//! \param mapping Mapowanie jointów do indeksów
		static void setLocal(SkeletonState & skeletonState, const RigidPartialStateChange & stateChange,
				const LinearizedNodesMapping & mapping);

	private:
		//! \param skeletonState [out] Aktualizowany stan szkieletu
		//! \param stateChange Zmiana stanu szkieletu
		static void update(SkeletonState & skeletonState, const RigidCompleteStateChange & stateChange);


		public:
		////! \param skeletonState [out] Aktualizowany stan szkieletu
		////! \param stateChange Zmiana stanu szkieletu
		//static void update(SkeletonState & skeletonState, const NonRigidPartialStateChange & stateChange);

		////! \param skeletonState [out] Aktualizowany stan szkieletu
		////! \param newState Nowy stan szkieletu
		//static void setLocal(SkeletonState & skeletonState, const RigidCompleteStateChange & newState);
		//static void setGlobal(SkeletonState & skeletonState, const RigidCompleteStateChange & newState);
		//static void setGlobal(SkeletonState & skeletonState, const NonRigidCompleteStateChange & newState);

		////! \param skeletonState Stan szkieletu
		////! \return Globalny opis stanu szkieletu
		//static NonRigidCompleteStateChange globalState(const SkeletonState & skeletonState);
		////! \param skeletonState Stan szkieletu
		////! \return Lokalny opis stanu szkieletu
		//static NonRigidCompleteStateChange localState(const SkeletonState & skeletonState);


		static osgutils::SegmentsDescriptors createConnections(const kinematic::SkeletonState& skeleton, const Joint2Index& mapping);

		static Joint2Index createJoint2IndexMapping(const kinematic::SkeletonState &skeleton, const LinearizedNodesMapping& mapping);

		//! \return Stan stawu roota całego stanu szkieletu
		JointPtr root();
		//! \return Stan stawu roota całego stanu szkieletu
		JointConstPtr root() const;

	private:

		//! \param joint Staw szkieletu dla którego tworzymy stan
		//! \return Stan szkieletu dla zadanego stawu i jego dzieci
		static JointPtr create(kinematic::JointConstPtr joint);

	private:
		//! Staw - root stanu
		JointPtr root_;
	};
	DEFINE_SMART_POINTERS(SkeletonState);
}

#endif	// __HEADER_GUARD_KINEMATIC__SKELETONSTATE_H__
