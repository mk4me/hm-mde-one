/********************************************************************
	created:  2014/12/17	16:21:59
	filename: SkeletonState.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_KINEMATIC__SKELETONSTATE_H__
#define __HEADER_GUARD_KINEMATIC__SKELETONSTATE_H__

#include <kinematiclib/Skeleton.h>
#include <kinematiclib/LinearizedSkeleton.h>
#include <kinematiclib/Types.h>
#include <acclaimformatslib/MotionData.h>
#include <biovisionformatslib/MotionData.h>
#include <osg/Vec3>
#include <osg/Quat>
#include <osg/Matrix>
#include <list>
//#include <osgutils/OsgSchemeDrawer.h>

namespace kinematic
{
	//! Klasa przedstawiająca stan szkieletu - pozycje i orientacje poszczególnych stawów
	struct SkeletonState
	{
	public:
		//! Typ mapowania identyfikatorów kosci aktywnych szkieletu acclaim do odpowadajacych jointow naszego szkieletu
		using AcclaimActiveSkeletonMapping = boost::bimap < LinearizedSkeleton::NodeIDX, acclaim::Bone::ID > ;

		//! Orientacje dla ciała sztywnego
		using Orientations = std::vector<osg::Quat>;

		//! Pełny stan ciała sztywnego
		struct RigidCompleteState
		{
			//! Pozycja
			osg::Vec3d position;
			//! Orientacje aktywnych jointów
			Orientations orientations;
		};

		using RigidCompleteStateLocal = LocalData < RigidCompleteState > ;
		using RigidCompleteStateGlobal = GlobalData < RigidCompleteState >;

		//! Częsciowy stan ciała sztywnego
		struct RigidPartialState
		{
			//! Pozycja
			osg::Vec3d position;
			//! Orientacje aktywnych jointów
			std::map<LinearizedSkeleton::NodeIDX, osg::Quat> orientations;
		};

		using RigidPartialStateLocal = LocalData < RigidPartialState >;
		using RigidPartialStateGlobal = GlobalData < RigidPartialState >;

		//! Stan jointa ciała nie sztywnego
		struct NonRigidJointState
		{
			//! Pozycja
			osg::Vec3d position;
			//! Orientacja
			osg::Quat orientation;
		};

		//! Pełny stan ciała niesztywnego
		using NonRigidCompleteState = std::vector < NonRigidJointState >;

		using NonRigidCompleteStateLocal = LocalData < NonRigidCompleteState >;
		using NonRigidCompleteStateGlobal = GlobalData < NonRigidCompleteState >;

		//! Częściowy stan ciała niesztywnego
		using NonRigidPartialState = std::map <LinearizedSkeleton::NodeIDX, NonRigidJointState >;

		using NonRigidPartialStateLocal = LocalData < NonRigidPartialState >;
		using NonRigidPartialStateGlobal = GlobalData < NonRigidPartialState >;

		//! \param skeleton Szkielet dla którego tworzymy stan
		//! \return Globalny stan sztywnego szkieletu
		static RigidCompleteStateGlobal globalRigidState(const Skeleton & skeleton);
		//! \param skeleton Szkielet dla którego tworzymy stan
		//! \return Lokalny stan sztywnego szkieletu
		static RigidCompleteStateLocal localRigidState(const Skeleton & skeleton);
		//! \param skeleton Szkielet dla którego tworzymy stan
		//! \return Globalny stan nie-sztywnego szkieletu
		static NonRigidCompleteStateGlobal globalNonRigidState(const Skeleton & skeleton);
		//! \param skeleton Szkielet dla którego tworzymy stan
		//! \return Lokalny stan nie-sztywnego szkieletu
		static NonRigidCompleteStateLocal localNonRigidState(const Skeleton & skeleton);

		//! \param skeleton Szkielet dla którego ustawiamy stan
		//! \param state Stan szkieletu
		static void applyLocalState(Skeleton & skeleton, const RigidCompleteState & state);
		//! \param skeleton Szkielet dla którego ustawiamy stan
		//! \param state Stan szkieletu
		static void applyLocalState(Skeleton & skeleton, const RigidPartialState & state);
		//! \param skeleton Szkielet dla którego ustawiamy stan
		//! \param state Stan szkieletu
		static void applyLocalState(Skeleton & skeleton, const NonRigidCompleteState & state);
		//! \param skeleton Szkielet dla którego ustawiamy stan
		//! \param state Stan szkieletu
		static void applyLocalState(Skeleton & skeleton, const NonRigidPartialState & state);

		//! \param skeleton Szkielet dla którego ustawiamy stan
		//! \param state Stan szkieletu
		static void applyGlobalState(Skeleton & skeleton, const RigidCompleteState & state);
		//! \param skeleton Szkielet dla którego ustawiamy stan
		//! \param state Stan szkieletu
		static void applyGlobalState(Skeleton & skeleton, const RigidPartialState & state);
		//! \param skeleton Szkielet dla którego ustawiamy stan
		//! \param state Stan szkieletu
		static void applyGlobalState(Skeleton & skeleton, const NonRigidCompleteState & state);
		//! \param skeleton Szkielet dla którego ustawiamy stan
		//! \param state Stan szkieletu
		static void applyGlobalState(Skeleton & skeleton, const NonRigidPartialState & state);

		template<typename StateType>
		static void applyState(Skeleton & skeleton, const StateType & state)
		{
			applyState(skeleton, state, std::integral_constant<bool, StateType::type == Local>());
		}

		//! \param skeleton Szkielet
		//! \param bones Kości
		//! \return Zlinearyzowane mapowanie aktywnych kości
		static AcclaimActiveSkeletonMapping createAcclaimActiveMapping(const Skeleton & skeleton,
			const acclaim::Skeleton::Bones & bones);

		//! \param skeleton Szkielet dla którego tworzymy zmianę stanu szkieletu
		//! \param motionData Dane ruchu w formacie acclaim - ramka danych
		//! \param mapping Mapowanie danych acclaim do szkieletu
		//! \return Zmiana stanu szkieletu
		static RigidPartialStateLocal convert(const acclaim::Skeleton::Bones & bones,
			const acclaim::Skeleton::Mapping & aMapping,
			const acclaim::MotionData::BonesData & motionData,
			const AcclaimActiveSkeletonMapping & activeMapping,
			const acclaim::Skeleton::HelperMotionData & helperMotionData,
			const bool angleInRadians);

		//! \param skeleton Szkielet dla którego tworzymy zmianę stanu szkieletu
		//! \param motionData Dane ruchu w formacie acclaim - ramka danych
		//! \param mapping Mapowanie danych acclaim do szkieletu
		//! \return Zmiana stanu szkieletu
		static acclaim::MotionData::BonesData convert(const acclaim::Skeleton & skeleton,
			const RigidCompleteStateLocal & skeletonState,
			const AcclaimActiveSkeletonMapping & activeMapping,
			const acclaim::Skeleton::HelperMotionData & helperMotionData);

		//! \param skeleton Szkielet dla którego tworzymy zmianę stanu szkieletu
		//! \param motionData Dane ruchu w formacie biovision - ramka danych		
		//! \return Zmiana stanu szkieletu
		static RigidCompleteState convert(const biovision::Skeleton & skeleton,
			const biovision::MotionData::FrameJointData & motionData);

	private:

		template<typename StateType>
		static void applyState(Skeleton & skeleton, const StateType & state, std::true_type)
		{
			applyLocalState(skeleton, state);
		}

		template<typename StateType>
		static void applyState(Skeleton & skeleton, const StateType & state, std::false_type)
		{
			applyGlobalState(skeleton, state);
		}
	};
}

#endif	// __HEADER_GUARD_KINEMATIC__SKELETONSTATE_H__
