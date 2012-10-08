#ifndef HEADER_GUARD_KINEMATICDATA__SKELETALMODEL_H__
#define HEADER_GUARD_KINEMATICDATA__SKELETALMODEL_H__

#include <boost/utility.hpp>
#include <kinematiclib/Joint.h>
#include <kinematiclib/Skeleton.h>
#include <utils/Debug.h>
namespace kinematic 
{
    //! klasa przechowuje wczytane dane animacji
	class SkeletalData : public boost::noncopyable
	{
	public:
		/// \brief  Struktura zawiera wartości kanałów dla
		/// 	    konkretnej kości i konkretnego kanału
		struct singleJointState : boost::noncopyable
		{
            //! nazwa stawu
			std::string name;  
			/// \brief  wartości kanałów dla tej stawu (ich liczba powinna być rowna stopniom swobody stawu)
			std::vector<double> channelValues; 
			//! konstruktor ustawia domyślny rozmiar dla kolekcji (6 stopni swobody)
			singleJointState()
			{
				channelValues.reserve(6);
			}
            //! \return głęboka kopia obiektu
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

		/// \brief  struktura przechowuje wartości kanałów dla wszystkich kości w pojedynczej klatce animacji
		struct singleFrame : boost::noncopyable
		{
			int frameNo; //!< numer klatki animacji
			std::vector<singleJointStatePtr> jointsData; //!< wartości kanałów
            //! \return głęboka kopia obiektu
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
        //! konstruktor, ustawia domyślny okres pojedynczej ramki (1/100)
		SkeletalData() :
		  frameTime(0.01)
		{}
		virtual ~SkeletalData() {}

	public:
		//! \return kolecja z danymi
		const std::vector<singleFramePtr>& getFrames() const { return frames; }
        //! \return kolecja z danymi
		std::vector<singleFramePtr>& getFrames() { return frames; }
        //! ustawia kolekcję z danymi
		void setFrames(std::vector<singleFramePtr> val) { frames = val; }
        //! return okres pojedynczej ramki
		double getFrameTime() const { return frameTime; }
        //! ustawia okres pojedynczej ramki
		void setFrameTime(double val) { UTILS_ASSERT(val > 0.0); frameTime = val; }

	public:
        //! \return głęboka kopia obiektu
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
