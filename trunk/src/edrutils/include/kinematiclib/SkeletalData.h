#ifndef HEADER_GUARD_KINEMATICDATA__SKELETALMODEL_H__
#define HEADER_GUARD_KINEMATICDATA__SKELETALMODEL_H__

#include <boost/utility.hpp>
#include <kinematiclib/Joint.h>
#include <kinematiclib/Skeleton.h>
#include <utils/Debug.h>
namespace kinematic 
{

	class SkeletalData : public boost::noncopyable
	{
	public:
		/// \brief  Struktura zawiera wartosci kanalow dla
		/// 	    konkretnej kosci i konkretnego kanalu
		struct singleJointState : boost::noncopyable
		{
			std::string name; //!< nazwa stawu 
			/// \brief  wartosci kanalow dla tej stawu (ich liczba powinna byc rowna stopniom swobody stawu)
			std::vector<double> channelValues; 
			//! konstruktor ustawia domyslny rozmiar dla kolekcji (6 stopni swobody)
			singleJointState()
			{
				channelValues.reserve(6);
			}

			singleJointState* clone() const
			{
				singleJointState* clone = new singleJointState();
				clone->channelValues = this->channelValues;
				clone->name = this->name;
				return clone;
			}
		};
		typedef boost::shared_ptr<singleJointState> singleJointStatePtr;
		typedef boost::shared_ptr<const singleJointState> singleJointStateConstPtr;

		/// \brief  struktura przechowuje wartosci kanalow dla wszystkich kosci w pojedynczej klatce animacji
		struct singleFrame : boost::noncopyable
		{
			int frameNo; //!< numer klatki animacji
			std::vector<singleJointStatePtr> jointsData; //!< wartosci kanalow

			singleFrame* clone() const 
			{
				singleFrame* clone = new singleFrame();
				clone->frameNo = this->frameNo;
				int count = this->jointsData.size();
				clone->jointsData.resize(count);
				for (int i = 0; i < count; ++i) {
					clone->jointsData[i] = singleJointStatePtr(this->jointsData[i]->clone());
				}
				return clone;
			}
		};
		typedef boost::shared_ptr<singleFrame> singleFramePtr;

	public:
		SkeletalData() :
		  frameTime(0.01)
		{}
		virtual ~SkeletalData() {}

	public:
		// akcesory
		const std::vector<singleFramePtr>& getFrames() const { return frames; }
		std::vector<singleFramePtr>& getFrames() { return frames; }
		void setFrames(std::vector<singleFramePtr> val) { frames = val; }
		double getFrameTime() const { return frameTime; }
		void setFrameTime(double val) { UTILS_ASSERT(val > 0.0); frameTime = val; }

	public:
		virtual SkeletalData* clone() const 
		{
			SkeletalData* clone = new SkeletalData();
			int count = this->frames.size();
			clone->frames.resize(count);
			for (int i = 0; i < count; ++i) {
				clone->frames[i] = singleFramePtr(frames[i]->clone());
			}
			return clone;
		}
		
	private:
		//! kolecja z danymi
		std::vector<singleFramePtr>  frames;
		//! okres pojedynczej ramki
		double frameTime;
		
	};
	typedef boost::shared_ptr<SkeletalData> SkeletalDataPtr;
	typedef boost::shared_ptr<const SkeletalData> SkeletalDataConstPtr;

}
#endif