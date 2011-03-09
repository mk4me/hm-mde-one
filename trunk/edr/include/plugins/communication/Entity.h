/**
@author Marek Daniluk
@brief Dokument z encjami bazodanowymi.
*/

//<xsd:enumeration value=" performer"/>
//<xsd:enumeration value=" session"/>
//<xsd:enumeration value=" trial"/>
//<xsd:enumeration value="performer_conf"/>
//<xsd:enumeration value="measurement_conf"/>
//<xsd:enumeration value=" measurement"/>
//<xsd:enumeration value=" file"/>
#ifndef HEADER_GUARD_COMMUNICATION_ENTITY_H__
#define HEADER_GUARD_COMMUNICATION_ENTITY_H__

#include <core/DateTime.h>

namespace communication
{
	struct Entity
	{
		Entity() : id(0) {};
		int id;
	};

	struct File : public Entity
	{
		File()
            : Entity() {};

		std::string fileName;
		std::string fileDescription;
		std::string fileSubdir;
		std::string fileAttributeName;
	};

	struct Trial : public Entity
	{
		Trial()
            : Entity(), sessionID(0) {};

		int sessionID;
		std::string trialDescription;
		std::vector<int> trialFiles;
		//std::map<int, File> trialFiles;
	};

	struct Session : public Entity
	{
		Session()
            : Entity(), userID(0), labID(0), sessionDate() {};

		int userID;
		int labID;
		std::string motionKind;
		std::string sessionDescription;
		std::string sessionLabel;
		DateTime sessionDate;
		std::map<int, Trial> sessionTrials;
	};

	struct SessionGroup : public Entity
	{
		SessionGroup()
            : Entity() {};

		std::string sessionGroupName;
	};

	struct Performer : public Entity
	{
		Performer()
            : Entity() {};

		std::string performerFirstname;
		std::string performerLastname;
	};
}
#endif
