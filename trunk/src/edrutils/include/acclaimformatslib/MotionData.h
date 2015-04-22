#ifndef HEADER_GUARD_acclaimDATA__SKELETALMODEL_H__
#define HEADER_GUARD_acclaimDATA__SKELETALMODEL_H__

#include <acclaimformatslib/Bone.h>

namespace acclaim
{
	//! klasa przechowuje wczytane dane animacji
	struct MotionData
	{
	public:
		//! Agregat danych kanału
		typedef std::vector<double> ChannelValues;

		/// \brief  Struktura zawiera wartości kanałów dla
		/// 	    konkretnej kości i konkretnego kanału
		struct BoneData
		{
			//! nazwa stawu
			std::string name;
			/// \brief  wartości kanałów dla tej stawu (ich liczba powinna być rowna stopniom swobody stawu)
			ChannelValues channelValues;
		};

		//! Agregat danych kości
		typedef std::vector<BoneData> BonesData;

		/// \brief  struktura przechowuje wartości kanałów dla wszystkich kości w pojedynczej klatce animacji
		struct FrameData
		{
			unsigned int id; //!< numer klatki animacji
			BonesData bonesData; //!< wartości kanałów			
		};

		typedef std::vector<FrameData> FramesData;

	public:
		//! \param frameTime Czas trwania ramki w sekundach
		MotionData(const double frameTime = 0.01) :
			frameTime(frameTime)
		{}
		//! Destruktor
		~MotionData() {}

		//! kolecja z danymi
		FramesData frames;
		////! Czas trwania pojedynczej ramki
		double frameTime;
	};
	DEFINE_SMART_POINTERS(MotionData);
}
#endif
