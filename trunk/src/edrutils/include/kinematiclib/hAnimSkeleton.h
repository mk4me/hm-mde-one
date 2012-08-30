/********************************************************************
	created:	2011/07/17
	created:	17:7:2011   11:27
	filename: 	hAnimSkeleton.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_KINEMATIC__HANIMSKELETON_H__
#define HEADER_GUARD_KINEMATIC__HANIMSKELETON_H__


#include <kinematiclib/hAnimBone.h>
#include <kinematiclib/hAnimJoint.h>
#include <kinematiclib/SkeletalMappingScheme.h>
#include <kinematiclib/SkeletalModel.h>

namespace kinematic {

	class hAnimSkeleton;
	typedef boost::shared_ptr<hAnimSkeleton> hAnimSkeletonPtr;
	typedef boost::shared_ptr<const hAnimSkeleton> hAnimSkeletonConstPtr;

	/// \brief  klasa reprezentuje szkielet h-anim
	class hAnimSkeleton : boost::noncopyable
	{
	public:
		//! \return całkowita, pusta struktura h-anim
		static hAnimSkeletonPtr create();
		//! 
		~hAnimSkeleton() {}
	private:
		//! 
		hAnimSkeleton();

	public:
		//! \return korzeń drzewa h-anim (powinien być zawsze "Humanoid Root")
		hAnimJointPtr getRoot() const { return root; }
        //! ustawia korzeń drzewa h-anim (powinien być zawsze "Humanoid Root")
		void setRoot(hAnimJointPtr val) { root = val; }
		//! tworzy powiązania pomiędzy aktywnymi jointami
		void createActiveHierarchy();

        // TODO zmienic ten dostęp
		//! \return mapa nazwa -> staw
		std::map<std::string, hAnimJointPtr>& getJoints() { return joints; }
        //! \return mapa nazwa -> kość
		std::map<std::string, hAnimBonePtr>& getBones() { return bones; }

		/// \brief  Wczytuje słownik z nazwami do mapowania. 
		/// \param  filename   Plik ze słownikiem. 
		void loadMappingDictionary(const std::string& filename);
		/// \brief  Metoda aktywuje lub dezaktywuje staw, operacja ta zmienia strukture szkieletu i dane o rotacjach 
		/// \param  joint   Aktywowany staw. 
		/// \param  active  true - staw staje się aktywny , false - staw staje się nieaktywny 
		void activateJoint(hAnimJointPtr joint, bool active);
		//! Metoda aktywuje lub dezaktywuje staw, operacja ta zmienia strukture szkieletu i dane o rotacjach 
		//! \param jointName nazwa aktywowanego stawu
		//! \param active  true - staw staje się aktywny , false - staw staje się nieaktywny 
		void activateJoint(const std::string& jointName, bool active);
		/// \brief  Pobiera joint z hierarchii o podanej nazwie (zgodnej z h-anim 1.1)
		/// \param  name    Nazwa pobieranego stawu 
		/// \return Wyszukany staw, jeśli takiego nie ma to rzucany jest wyjątek
		hAnimJointPtr getJointByName(const std::string& name);
		/// \brief  Pobiera kość z hierarchii o podanej nazwie (zgodnej z h-anim 1.1)
		/// \param  name    Nazwa pobieranej kości
		/// \return Wyszukana kość, jeśli takiej nie ma to rzucany jest wyjątek
		hAnimBonePtr getBoneByName(const std::string& name);
		/// \brief  mapowanie nazw szkieletu 
		/// \param  skeletalModel struktura szkieletu 
		void doSkeletonMapping(SkeletalModelConstPtr skeletalModel);
		//! realizuje mapowanie pojedynczego jointa na nazwę w h-anim
		//! \param given nazwa podanego jointa
		//! \return zmapowana na h-anim nazwa
		std::string mapJointName(const std::string& given);
        //! \return głęboka kopia obiektu
		virtual hAnimSkeleton* clone() const;

	private:
		/// \brief  Wyszukuje kość i staw na podstawie podanych nazw i zmienia referencje do wskaźnikow.
		/// \details Pomocnicze przy tworzeniu drzewa h-anim 
		/// \param  newJointName       Nazwa stawu do wyszukania. 
		/// \param  newBoneName        Nazwa kości do wyszukania. 
		/// \param [out] parentJoint Referencja do wskaźnika, tutaj laduje znaleziony staw. 
		/// \param [out] parentBone  Referencja do wskaźnika, tutaj laduje znaleziona kość. 
		void backJointAndBone(std::string newJointName, std::string newBoneName, hAnimJointPtr& parentJoint, hAnimBonePtr& parentBone);
		/// \brief  Tworzy kość i staw na podstawie podanych nazw i zmienia referencje do wskaźnikow.
		/// \details Pomocnicze przy tworzeniu drzewa h-anim 
		/// \param  newJointName       Nazwa nowego stawu. 
		/// \param  newBoneName        Nazwa nowej kości.
		/// \param [in,out] parentJoint Referencja do wskaźnika, tutaj laduje tworzony staw. 
		/// \param [in,out] parentBone  Referencja do wskaźnika, tutaj laduje tworzona kość.
		void createJointAndBone(std::string newJointName, std::string newBoneName, hAnimJointPtr& parentJoint, hAnimBonePtr& parentBone);
		//! czysci wszystkie informacje o powiązaniu aktywnych jointow
		//! \param parent 
		void clearActiveHierarchy(hAnimJointPtr parent);
		//! tworzy powiązania pomiędzy aktywnymi jointami
		//! \param parent 
		//! \param lastActive 
		void createActiveHierarchy(hAnimJointPtr parent, hAnimJointPtr lastActive);
		//! tworzy kopię hierarchii, metoda pomocnicza
		hAnimJointPtr cloneHierarchy( hAnimJointConstPtr root, std::map<std::string, hAnimJointPtr>& joints, 
			std::map<std::string, hAnimBonePtr>& bones ) const;
		//! tworzy kopię hierarchii, metoda pomocnicza
		hAnimJointPtr cloneHierarchy( hAnimJointConstPtr currentSourceJoint, hAnimBonePtr destinationParentBone, 
			std::map<std::string, hAnimJointPtr>& joints, std::map<std::string, hAnimBonePtr>& bones ) const;

	private:
		//! korzeń drzewa h-anim (powinien być zawsze "Humanoid Root")
		hAnimJointPtr root;
		//! mapa wszystkich jointów h-anim                           
		std::map<std::string, hAnimJointPtr> joints;
		//! mapa wszystkich kości h-anim                        
		std::map<std::string, hAnimBonePtr> bones;
		//! słowniki ze sposobem mapowania                                                 
		std::vector<SkeletonMappingSchemePtr> mappingSchemes;

	};
	
}

#endif
