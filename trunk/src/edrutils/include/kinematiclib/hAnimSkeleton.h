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
		//! \return pelna, pusta struktura h-anim
		static hAnimSkeletonPtr create();
		//! 
		~hAnimSkeleton() {}
	private:
		//! 
		hAnimSkeleton();

	public:
		// akcesory
		hAnimJointPtr getRoot() const { return root; }
		void setRoot(hAnimJointPtr val) { root = val; }
		//! towrzy powiazania pomiedzy aktywnymi jointami
		void createActiveHierarchy();

		// todo - jest to brzydki sposob przekazywania tych kolekcji
		std::map<std::string, hAnimJointPtr>& getJoints() { return joints; }
		std::map<std::string, hAnimBonePtr>& getBones() { return bones; }

		/// \brief  Wczytuje slownik z nazwami do mapowania. 
		/// \param  filename   Plik ze slownikiem. 
		void loadMappingDictionary(const std::string& filename);
		/// \brief  Metoda aktywuje lub dezaktywuje staw, operacja ta zmienia strukture szkieletu i dane o rotacjach 
		/// \param  joint   Aktywowany staw. 
		/// \param  active  true - staw staje sie aktywny , false - staw staje sie nieaktywny 
		void activateJoint(hAnimJointPtr joint, bool active);
		//! Metoda aktywuje lub dezaktywuje staw, operacja ta zmienia strukture szkieletu i dane o rotacjach 
		//! \param jointName nazwa aktywowanego stawu
		//! \param active  true - staw staje sie aktywny , false - staw staje sie nieaktywny 
		void activateJoint(const std::string& jointName, bool active);
		/// \brief  Pobiera joint z hierarchii o podanej nazwie (zgodnej z h-anim 1.1)
		/// \param  name    Nazwa pobieranego stawu 
		/// \return Wyszukany staw, jesli takiego nie ma to rzucany jest wyjatek
		hAnimJointPtr getJointByName(const std::string& name);
		/// \brief  Pobiera kosc z hierarchii o podanej nazwie (zgodnej z h-anim 1.1)
		/// \param  name    Nazwa pobieranej kosci
		/// \return Wyszukana kosc, jesli takiej nie ma to rzucany jest wyjatek
		hAnimBonePtr getBoneByName(const std::string& name);
		/// \brief  Executes the skeleton mapping operation. 
		/// \param  skeletalModel   The skeletal model. 
		void doSkeletonMapping(SkeletalModelConstPtr skeletalModel);
		//! realizuje mapowanie pojedynczego jointa na nazwe w h-anim
		//! \param given nazwa podanego jointa
		//! \return zmapowana na h-anim nazwa
		std::string mapJointName(const std::string& given);

		virtual hAnimSkeleton* clone() const;

	private:
		/// \brief  Wyszukuje kosc i staw na podstawie podanych nazw i zmienia referencje do wskaznikow.
		/// \details Pomocnicze przy tworzeniu drzewa h-anim 
		/// \param  newJointName       Nazwa stawu do wyszukania. 
		/// \param  newBoneName        Nazwa kosci do wyszukania. 
		/// \param [out] parentJoint Referencja do wskaznika, tutaj laduje znaleziony staw. 
		/// \param [out] parentBone  Referencja do wskaznika, tutaj laduje znaleziona kosc. 
		void backJointAndBone(std::string newJointName, std::string newBoneName, hAnimJointPtr& parentJoint, hAnimBonePtr& parentBone);
		/// \brief  Tworzy kosc i staw na podstawie podanych nazw i zmienia referencje do wskaznikow.
		/// \details Pomocnicze przy tworzeniu drzewa h-anim 
		/// \param  newJointName       Nazwa nowego stawu. 
		/// \param  newBoneName        Nazwa nowej kosci.
		/// \param [in,out] parentJoint Referencja do wskaznika, tutaj laduje tworzony staw. 
		/// \param [in,out] parentBone  Referencja do wskaznika, tutaj laduje tworzona kosc.
		void createJointAndBone(std::string newJointName, std::string newBoneName, hAnimJointPtr& parentJoint, hAnimBonePtr& parentBone);
		//! czysci wszystkie informacje o powiazaniu aktywnych jointow
		//! \param parent 
		void clearActiveHierarchy(hAnimJointPtr parent);
		//! tworzy powiazania pomiedzy aktywnymi jointami
		//! \param parent 
		//! \param lastActive 
		void createActiveHierarchy(hAnimJointPtr parent, hAnimJointPtr lastActive);
		hAnimJointPtr cloneHierarchy( hAnimJointConstPtr root, std::map<std::string, hAnimJointPtr>& joints, 
			std::map<std::string, hAnimBonePtr>& bones ) const;
		hAnimJointPtr cloneHierarchy( hAnimJointConstPtr currentSourceJoint, hAnimBonePtr destinationParentBone, 
			std::map<std::string, hAnimJointPtr>& joints, std::map<std::string, hAnimBonePtr>& bones ) const;

	private:
		//! korzen drzewa h-anim (powinien byc zawsze "Humanoid Root")
		hAnimJointPtr root;
		//! mapa wszystkich jointow h-anim                           
		std::map<std::string, hAnimJointPtr> joints;
		//! mapa wszystkich kosci h-anim                        
		std::map<std::string, hAnimBonePtr> bones;
		//! slowniki ze sposobem mapowania                                                 
		std::vector<SkeletonMappingSchemePtr> mappingSchemes;

	};
	
}

#endif
