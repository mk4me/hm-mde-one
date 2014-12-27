#ifndef HEADER_GUARD_acclaimDATA__SKELETALMODEL_H__
#define HEADER_GUARD_acclaimDATA__SKELETALMODEL_H__

#include <acclaimformatslib/Bone.h>

namespace acclaim
{
	//! klasa przechowuje wczytane dane animacji
	struct MotionData
	{
	public:
		/// \brief  Struktura zawiera wartości kanałów dla
		/// 	    konkretnej kości i konkretnego kanału
		struct BoneData
		{
			//! nazwa stawu
			std::string name;
			/// \brief  wartości kanałów dla tej stawu (ich liczba powinna być rowna stopniom swobody stawu)
			std::vector<float> channelValues;
			//! konstruktor ustawia domyślny rozmiar dla kolekcji (6 stopni swobody)
			BoneData()
			{
				channelValues.reserve(6);
			}
		};

		/// \brief  struktura przechowuje wartości kanałów dla wszystkich kości w pojedynczej klatce animacji
		struct FrameData
		{
			int id; //!< numer klatki animacji
			std::vector<BoneData> bonesData; //!< wartości kanałów			
		};		

	public:
		//! konstruktor, ustawia domyślny okres pojedynczej ramki (1/100)
		MotionData(const float frameTime = 0.01) :
			frameTime(frameTime)
		{}
		~MotionData() {}

		//! kolecja z danymi
		std::vector<FrameData>  frames;
		//! Czas trwania pojedynczej ramki
		float frameTime;
	};
	DEFINE_SMART_POINTERS(MotionData);
}
#endif
