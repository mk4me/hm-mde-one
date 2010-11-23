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
		std::string file_name;
		std::string file_description;
		std::string file_subdir;
		std::string file_attribute_name;
	};

	struct Trial : public Entity
	{
		int session_id;
		std::string trial_description;
		std::map<int, File> trial_files;
	};

	struct Session : public Entity
	{
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
		std::string sessiongroup_name;
	};

	struct Performer : public Entity
	{
		std::string performer_firstname;
		std::string performer_lastname;
	};

	struct Measurement : public Entity
	{
	};

}
#endif
