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

namespace kinematic
{
	//! Klasa przedstawiaj¹ca stan szkieletu - pozycje i orientacje poszczególnych stawów
	class SkeletonState
	{
	public:

		//! Struktura opisuj¹ca prosta zmianê w szkielecie - translacja roota i lokalne rotacje stawów
		struct SimpleStateChange
		{
			//! Translacja roota
			osg::Vec3 translation;
			//! Lokalne rotacje stawów
			std::list<osg::Quat> rotations;
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
		typedef std::vector<JointStateChange> FullStateChange;

		//! Forward declaration
		class Joint;
		DEFINE_SMART_POINTERS(Joint);

		//! Klasa reprezentuj¹ca staw
		class Joint
		{
		private:
			//! Forward declaration
			class JointImpl;

			friend class SkeletonState;

		private:			
			//! \param name Nazwa wêz³a odpowiadaj¹ca szkieletowi
			//! \param translation Pozycja|Translacja lokalna stawu
			//! \param rotation Orientacja|Rotacja lokalna stawu			
			Joint(const std::string & name, const osg::Vec3 & translation,
				const osg::Quat & rotation);

		public:

			//! Destruktor
			~Joint();

			//! \return Staw rodzic
			JointPtr parent();

			//! \return Staw rodzic
			JointConstPtr parent() const;
			//! \return Nazwa stawu
			std::string name() const;
			//! \param idx Indeks stawu dziecka do pobrania
			//! \return Staw dziecko
			JointPtr child(const unsigned int idx);
			//! \param idx Indeks stawu dziecka do pobrania
			//! \return Staw dziecko
			JointConstPtr child(const unsigned int idx) const;
			//! \return Iloœæ stawów dzieci
			const unsigned int childrenCount() const;

			//! \return Lokalna pzycja wzglêdem rodzica
			osg::Vec3 localPosition() const;
			//! \return Globalna pozycja
			osg::Vec3 globalPosition() const;

			//! \return Lokalna orientacja wzglêdem rodzica
			osg::Quat localOrientation() const;
			//! \return Globalna orientacja
			osg::Quat globalOrientation() const;

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

			//! \param translation Lokalne przesuniêcie
			//! \param rotation Lokalna rotacja
			void update(const osg::Vec3 & translation, const osg::Quat & rotation);
			//! \param position Globalna pozycja
			//! \param orientation Globalna orientacja
			void setGlobal(const osg::Vec3 & position, const osg::Quat & orientation);
			//! \param position Lokalna pozycja
			//! \param orientation Lokalna orientacja
			void setLocal(const osg::Vec3 & position, const osg::Quat & orientation);

			template<typename T>
			static void visit(JointPtr joint, T & visitor)
			{
				visitor(joint);
				for (unsigned int i = 0; i < joint->childrenCount(); ++i)
				{
					visit(joint->child(i), visitor);
				}
			}

			template<typename T>
			static void visit(JointConstPtr joint, T & visitor)
			{
				visitor(joint.get());
				for (unsigned int i = 0; i < joint->childrenCount(); ++i)
				{
					visit(joint->child(i), visitor);
				}
			}

		private:
			//! Prywatna implementacja
			utils::shared_ptr<JointImpl> impl;
		};

	private:

		//! \param root Staw rodzic stanu
		//! \param activeJointsCount Iloœæ aktywnych stawów które maja dzieci
		SkeletonState(JointPtr root, const unsigned int activeJointsCount);

	public:

		//! \param Other Stan który przejmujemy
		SkeletonState(SkeletonState && Other);

		//! \param skeleton Szkielet dla któego tworzymy stan
		//! \return Stan bazowy szkieletu
		static SkeletonState create(const Skeleton & skeleton);

		static FullStateChange convert(const acclaim::Skeleton & skeleton,
			const acclaim::MotionData::FrameData & motionData);

		static FullStateChange convert(const biovision::Skeleton & skeleton,
			const biovision::MotionData::FrameJointData & motionData);

		//! \param skeletonState [out] Aktualizowany stan szkieletu
		//! \param stateChange Zmiana stanu szkieletu
		static void update(SkeletonState & skeletonState, const SimpleStateChange & stateChange);
		//! \param skeletonState [out] Aktualizowany stan szkieletu
		//! \param stateChange Zmiana stanu szkieletu
		static void update(SkeletonState & skeletonState, const FullStateChange & stateChange);

		//! \param skeletonState [out] Aktualizowany stan szkieletu
		//! \param newState Nowy stan szkieletu
		static void setLocal(SkeletonState & skeletonState, const SimpleStateChange & newState);
		static void setGlobal(SkeletonState & skeletonState, const SimpleStateChange & newState);
		//! \param skeletonState [out] Aktualizowany stan szkieletu
		//! \param newState Nowy stan szkieletu
		static void setLocal(SkeletonState & skeletonState, const FullStateChange & newState);
		static void setGlobal(SkeletonState & skeletonState, const FullStateChange & newState);

		//! \param skeletonState Stan szkieletu
		//! \return Globalny opis stanu szkieletu
		static FullStateChange globalState(const SkeletonState & skeletonState);
		//! \param skeletonState Stan szkieletu
		//! \return Lokalny opis stanu szkieletu
		static FullStateChange localState(const SkeletonState & skeletonState);

		//! \return Staw - root stanu
		JointPtr root();
		//! \return Staw - root stanu
		JointConstPtr root() const;

	private:

		static JointPtr create(kinematic::JointConstPtr joint, unsigned int & activeJointsCount);

	private:
		//! Staw - root stanu
		JointPtr root_;
		//! Iloœæ wêz³ów aktywnych (nie end effectorów) stanu
		unsigned int activeJointsCount;
	};
	
	struct SkeletonStates
	{
		std::vector<SkeletonState::FullStateChange> frames;
		std::vector<std::string> jointNames;
		double frameTime;
	};
	DEFINE_SMART_POINTERS(SkeletonStates);
}

#endif	// __HEADER_GUARD_KINEMATIC__SKELETONSTATE_H__
