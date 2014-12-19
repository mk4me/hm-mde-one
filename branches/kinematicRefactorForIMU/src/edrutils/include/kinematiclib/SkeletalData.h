#ifndef HEADER_GUARD_KINEMATICDATA__SKELETALMODEL_H__
#define HEADER_GUARD_KINEMATICDATA__SKELETALMODEL_H__

#include <kinematiclib/Joint.h>
#include <kinematiclib/Skeleton.h>
#include <threadingUtils/StreamData.h>
#include <utils/Debug.h>
namespace kinematic
{
	//! klasa przechowuje wczytane dane animacji
	struct SkeletalData
	{
		/// \brief  struktura przechowuje wartości kanałów dla wszystkich kości w pojedynczej klatce animacji
		struct Frame
		{
			osg::Vec3 translation;
			std::vector<osg::Quat> rotations;
		};
	
		//! konstruktor, ustawia domyślny okres pojedynczej ramki (1/100)
		SkeletalData(const float duration = 0.01) :	frameTime(duration)	{}
		~SkeletalData() {}
		
		//! kolecja z danymi
		std::vector<Frame>  frames;
		//! okres pojedynczej ramki
		float frameTime;
	};
	typedef utils::shared_ptr<SkeletalData> SkeletalDataPtr;
	typedef utils::shared_ptr<const SkeletalData> SkeletalDataConstPtr;

	//! Klatka dla danych strumieniowych
	struct StreamSingleFrame
	{
		//! Pozycja roota
		osg::Vec3 rootPosition;
		int frameNo; //!< numer klatki animacji
		std::vector<SkeletalData::singleJointStatePtr> jointsData; //!< wartości kanałów

		StreamSingleFrame* clone() const
		{
			std::unique_ptr<StreamSingleFrame> clone(new StreamSingleFrame());
			clone->rootPosition = this->rootPosition;
			clone->frameNo = this->frameNo;
			const int count = this->jointsData.size();
			clone->jointsData.resize(count);
			for (int i = 0; i < count; ++i) {
				clone->jointsData[i] = SkeletalData::singleJointStatePtr(this->jointsData[i]->clone());
			}
			return clone.release();
		}
	};

	struct StreamSkeletonDataFrame
	{
		//! Pozycja roota
		osg::Vec3 rootPosition;
		int frameNo; //!< numer klatki animacji
		std::vector<osg::Quat> rotations; //!< rotacje
	};

	typedef threadingUtils::StreamT<StreamSingleFrame> SkeletalFramesStream;

	typedef threadingUtils::StreamT<StreamSkeletonDataFrame> SkeletalDataStream;

	typedef utils::shared_ptr<SkeletalDataStream> SkeletalDataStreamPtr;
	typedef utils::shared_ptr<const SkeletalDataStream> SkeletalDataStreamConstPtr;
}
#endif