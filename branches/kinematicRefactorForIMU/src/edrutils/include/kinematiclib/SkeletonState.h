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
#include <list>
#include <boost/bimap.hpp>
#include "osgutils/OsgSchemeDrawer.h"

namespace kinematic
{
	//! Klasa przedstawiaj¹ca stan szkieletu - pozycje i orientacje poszczególnych stawów
	class SkeletonState
	{
	public:

		//! Typ indeksu wêz³a
		typedef unsigned int NodeIDX;
		//! Typ bimapy indeksów wêz³ów do nazw wêz³ów
		typedef boost::bimap<NodeIDX, std::string> LinearizedNodesMapping;

		//! Struktura opisuj¹ca prosta zmianê w szkielecie - translacja roota i lokalne rotacje stawów
		//! pozycje rotacji w wektorze wg mapowania
		struct RigidCompleteStateChange
		{
			//! Translacja roota
			osg::Vec3 translation;
			//! Lokalne rotacje wszystkich stawów
			std::vector<osg::Quat> rotations;
		};

		//! Struktura opisuj¹ca prosta zmianê w szkielecie - translacja roota i lokalne rotacje stawów
		//! rotacje wg mapowania
		struct RigidPartialStateChange
		{
			//! Translacja roota
			osg::Vec3 translation;
			//! Lokalne rotacje wybranych stawów
			std::map<NodeIDX, osg::Quat> rotations;
		};

		//! Struktura opisuj¹ca zmianê stawu
		struct JointStateChange
		{
			//! Lokalna translacja
			osg::Vec3 translation;
			//! Lokalna rotacja
			osg::Quat rotation;
		};

		//! Typ opisuj¹cy pe³n¹ zmianê stanu szkieletu
		//! pozycja zmian wg mapowania
		typedef std::vector<JointStateChange> NonRigidCompleteStateChange;

		//! Typ opisuj¹cy pe³n¹ zmianê stanu szkieletu
		//! zmiany wg mapowania
		typedef std::map<NodeIDX, JointStateChange> NonRigidPartialStateChange;

		//! Klasa reprezentuj¹ca staw
		class JointData
		{
		private:
			//! Forward declaration
			class JointDataImpl;

			friend class SkeletonState;

		private:			
			//! \param name Nazwa wêz³a odpowiadaj¹ca szkieletowi
			//! \param translation Pozycja|Translacja lokalna stawu
			//! \param rotation Orientacja|Rotacja lokalna stawu			
			JointData(const std::string & name, const osg::Vec3 & translation,
					  const osg::Quat & rotation, const osg::Quat & prerotation );

		public:

			//! Destruktor
			~JointData();

			//! \return Nazwa stawu
			std::string name() const;

			//! \return Lokalna pzycja wzglêdem rodzica
			osg::Vec3 localPosition() const;
			//! \return Globalna pozycja
			osg::Vec3 globalPosition() const;

			//! \return Lokalna orientacja wzglêdem rodzica
			osg::Quat localOrientation() const;
			//! \return Globalna orientacja
			osg::Quat globalOrientation() const;

			//! \param translation Lokalne przesuniêcie
			//! \param rotation Lokalna rotacja
			void update(const osg::Vec3 & translation, const osg::Quat & rotation);


			//! \param translation Przesuniêcie
			void update(const osg::Vec3 & translation);
			//! \param position Globalna pozycja
			void setGlobal(const osg::Vec3 & position);
			//! \param position Lokalna pozycja
			void setLocal(const osg::Vec3 & position);

			//! \param rotation Rotacja
			void update(const osg::Quat & rotation);
			//! \param orientation Globalna orientacja
			void setGlobal(const osg::Quat & orientation);
			//! \param orientation Lokalna orientacja
			void setLocal(const osg::Quat & orientation);

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
		typedef utils::TreeNode<JointData> Joint;
		//! Typ wskaŸników do jointów
		DEFINE_SMART_POINTERS(Joint);

		typedef std::map<kinematic::SkeletonState::JointConstPtr, unsigned int> Joint2Index;
	private:

		//! \param root Staw rodzic stanu
		//! \param activeJointsCount Iloœæ aktywnych stawów które maja dzieci
		SkeletonState(JointPtr root);

	public:

		//! \param Other Stan który przejmujemy
		SkeletonState(SkeletonState && Other);

		//! \param skeleton Szkielet dla któego tworzymy stan
		//! \return Stan bazowy szkieletu
		static SkeletonState create(const Skeleton & skeleton);

		static RigidPartialStateChange convert(const acclaim::Skeleton & skeleton,
			const acclaim::MotionData::FrameData & motionData,
			const LinearizedNodesMapping & mapping);

		static RigidCompleteStateChange convert(const biovision::Skeleton & skeleton,
			const biovision::MotionData::FrameJointData & motionData);

		static LinearizedNodesMapping createMapping(const Skeleton & skeleton);

		//! \param skeletonState [out] Aktualizowany stan szkieletu
		//! \param stateChange Zmiana stanu szkieletu
		static void update(SkeletonState & skeletonState, const RigidCompleteStateChange & stateChange);
		//! \param skeletonState [out] Aktualizowany stan szkieletu
		//! \param stateChange Zmiana stanu szkieletu
		static void update(SkeletonState & skeletonState, const NonRigidCompleteStateChange & stateChange);

		//! \param skeletonState [out] Aktualizowany stan szkieletu
		//! \param stateChange Zmiana stanu szkieletu
		static void update(SkeletonState & skeletonState, const RigidPartialStateChange & stateChange);
		//! \param skeletonState [out] Aktualizowany stan szkieletu
		//! \param stateChange Zmiana stanu szkieletu
		static void update(SkeletonState & skeletonState, const NonRigidPartialStateChange & stateChange);

		//! \param skeletonState [out] Aktualizowany stan szkieletu
		//! \param newState Nowy stan szkieletu
		static void setLocal(SkeletonState & skeletonState, const RigidCompleteStateChange & newState);
		static void setGlobal(SkeletonState & skeletonState, const RigidCompleteStateChange & newState);
		//! \param skeletonState [out] Aktualizowany stan szkieletu
		//! \param newState Nowy stan szkieletu
		static void setLocal(SkeletonState & skeletonState, const NonRigidCompleteStateChange & newState);
		static void setGlobal(SkeletonState & skeletonState, const NonRigidCompleteStateChange & newState);

		//! \param skeletonState Stan szkieletu
		//! \return Globalny opis stanu szkieletu
		static NonRigidCompleteStateChange globalState(const SkeletonState & skeletonState);
		//! \param skeletonState Stan szkieletu
		//! \return Lokalny opis stanu szkieletu
		static NonRigidCompleteStateChange localState(const SkeletonState & skeletonState);


		static osgutils::SegmentsDescriptors createConnections(const kinematic::SkeletonState& skeleton, const Joint2Index& mapping);

		static Joint2Index createJoint2IndexMapping(const kinematic::SkeletonState &skeleton, const LinearizedNodesMapping& mapping);

		//! \return Staw - root stanu
		JointPtr root();
		//! \return Staw - root stanu
		JointConstPtr root() const;

	private:

		static JointPtr create(kinematic::JointConstPtr joint);

	private:
		//! Staw - root stanu
		JointPtr root_;
	};
	
}

#endif	// __HEADER_GUARD_KINEMATIC__SKELETONSTATE_H__
