/**
@author Marek Daniluk
@brief Klasa Entity jest klasa bazowa dla obiektow reprezentujacych byty po stronie serwera.
*/

//<xsd:enumeration value=" performer"/>
//<xsd:enumeration value=" session"/>
//<xsd:enumeration value=" trial"/>
//<xsd:enumeration value="performer_conf"/>
//<xsd:enumeration value="measurement_conf"/>
//<xsd:enumeration value=" measurement"/>
//<xsd:enumeration value=" file"/>
#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "DateTime.h"

namespace communication {

	struct Entity
	{
		Entity() : id(0) {};
		int id;
		///**
		//Czysto wirtualna metoda toString
		//@return prezentuje obiekt w formie tekstowej
		//**/
		//virtual const std::string toString() const = 0;
		///**
		//Operator wyjscia dla Entity pod cout
		//*/
		//friend std::ostream& operator <<(std::ostream& out, const Entity& e) {return out << e.toString();}
	};

	struct File : public Entity
	{
		File() : Entity(), file_name(""), file_description(""), file_subdir(""), file_attribute_name("") {};

		std::string file_name;
		std::string file_description;
		std::string file_subdir;
		std::string file_attribute_name;
	};

	struct Trial : public Entity
	{
		Trial() : Entity(), session_id(0), trial_description("") {};

		int session_id;
		std::string trial_description;
		std::map<int, File> trial_files;
	};

	struct Session : public Entity
	{
		Session() : Entity(), user_id(0), lab_id(0), motion_kind(""), session_description(""), session_label(""), session_date() {};

		int user_id;
		int lab_id;
		std::string motion_kind;
		std::string session_description;
		std::string session_label;
		DateTime session_date;
		std::map<int, Trial> session_trials;
	};

	struct SessionGroup : public Entity
	{
		SessionGroup() : Entity(), sessiongroup_name("") {};

		std::string sessiongroup_name;
	};

	struct Performer : public Entity
	{
		Performer() : Entity(), performer_firstname(""), performer_lastname("") {};

		std::string performer_firstname;
		std::string performer_lastname;
	};

	//struct Measurement : public Entity
	//{
	//};

}
#endif
