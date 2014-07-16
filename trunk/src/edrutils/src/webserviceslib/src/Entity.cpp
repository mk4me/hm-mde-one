#include <webserviceslib/Entity.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <tinyxml2.h>
#include <utils/Debug.h>
#include <webserviceslib/DateTimeUtils.h>

using namespace webservices;
using namespace webservices::xmlWsdl;

const Entity::Type Entity::convert(const std::string & entity)
{
	Entity::Type ret = Entity::Unspecified;
	
	std::string e = boost::algorithm::to_lower_copy(entity);
	
	if(e == "session"){
		ret = Entity::Session;
	}else if(e == "trial"){
		ret = Entity::Trial;
	}else if(e == "file"){
		ret = Entity::File;
	}else if(e == "measurement_configuration"){
		ret = Entity::MeasurementConfiguration;
	}else if(e == "performer"){
		ret = Entity::Performer;
	}else if(e == "performer_configuration"){
		ret = Entity::PerformerConfiguration;
	}

	return ret;
}

const std::string Entity::convert(const Type & entity)
{
	std::string ret("unspecified");

	switch(entity){

	case Entity::Session:
		ret = "session";
		break;

	case Entity::Trial:
		ret = "trial";
		break;

	case Entity::File:
		ret = "file";
		break;

	case Entity::MeasurementConfiguration:
		ret = "measurement_conf";
		break;

	case Entity::Performer:
		ret = "performer";
		break;

	case Entity::PerformerConfiguration:
		ret = "performer_conf";
		break;
	}

	return ret;
}

const Gender::Type Gender::convert(const std::string & gender)
{
	Gender::Type ret = Gender::Unspecified;

	std::string e = boost::algorithm::to_lower_copy(gender);

	if(e == "man" || e == "male" || e == "m"){
		ret = Gender::Male;
	}else if(e == "woman" || e == "female" || e == "f" || e == "w" || e == "k"){
		ret = Gender::Female;
	}

	return ret;
}

const std::string Gender::convert(const Type & gender)
{
	std::string ret("unspecified");

	switch(gender){

	case Gender::Male:
		ret = "m";
		break;

	case Gender::Female:
		ret = "k";
		break;
	}

	return ret;
}

const AttributeType::Type AttributeType::convert(const std::string & attributeType)
{
	AttributeType::Type ret = AttributeType::Unspecified;

	std::string e = boost::algorithm::to_lower_copy(attributeType);

	if(e == "int"){
		ret = AttributeType::Int;
	}else if(e == "decimal"){
		ret = AttributeType::Decimal;
	}else if(e == "nonnegativeinteger"){
		ret = AttributeType::NonNegativeInteger;
	}else if(e == "nonnegativedecimal"){
		ret = AttributeType::NonNegativeDecimal;
	}else if(e == "datetime"){
		ret = AttributeType::DateTime;
	}else if(e == "date"){
		ret = AttributeType::Date;
	}else if(e == "timecode"){
		ret = AttributeType::TimeCode;
	}else if(e == "float"){
		ret = AttributeType::Float;
	}

	return ret;
}

const std::string AttributeType::convert(const Type & attributeType)
{
	std::string ret("unspecified");

	switch(attributeType){

	case AttributeType::Int:
		ret = "int";
		break;

	case AttributeType::Decimal:
		ret = "decimal";
		break;

	case AttributeType::NonNegativeInteger:
		ret = "nonNegativeInteger";
		break;

	case AttributeType::NonNegativeDecimal:
		ret = "nonNegativeDecimal";
		break;

	case AttributeType::DateTime:
		ret = "dateTime";
		break;

	case AttributeType::Date:
		ret = "date";
		break;

	case AttributeType::TimeCode:
		ret = "TIMECODE";
		break;

	case AttributeType::Float:
		ret = "float";
		break;
	}

	return ret;
}

const BooleanType::Type BooleanType::convert(const std::string & booleanType)
{
	BooleanType::Type ret = BooleanType::Unspecified;

	std::string e = boost::algorithm::to_lower_copy(booleanType);

	if(e == "true"){
		ret = BooleanType::True;
	}else if(e == "false"){
		ret = BooleanType::False;
	}

	return ret;
}

const std::string BooleanType::convert(const Type & booleanType)
{
	std::string ret("unspecified");

	switch(booleanType){

	case BooleanType::True:
		ret = "true";
		break;

	case BooleanType::False:
		ret = "false";
		break;
	}

	return ret;
}

const AnnotationStatus::Type AnnotationStatus::convert(const std::string & statusType)
{
	Type ret = Unspecified;

	try{
		int e = boost::lexical_cast<int>(statusType);
		if(e > -1 && e < 5){
			ret = static_cast<Type>(e);
		}
	}catch(...){

	}

	return ret;
}

const std::string AnnotationStatus::convert(const Type & statusType)
{
	std::string ret("unspecified");

	switch(statusType){

	case UnderConstruction:
		ret = "1";
		break;

	case ReadyForReview:
		ret = "2";
		break;

	case UnderReview:
		ret = "3";
		break;

	case Approved:
		ret = "4";
		break;

	case Rejected:
		ret = "0";
		break;
	}

	return ret;
}

struct XMLHelper
{
public:
	template<class T>
	static const bool extractTagValue(tinyxml2::XMLElement * root, const std::string & tag, T & value)
	{
		tinyxml2::XMLHandle rE(root);

		tinyxml2::XMLElement * element = rE.FirstChildElement(tag.c_str()).ToElement();

		if(element != nullptr){
			try{
				value = boost::lexical_cast<T>(element->GetText());
				return true;
			}catch(...){
				return false;
			}
		}

		return false;
	}

	template<class T>
	static const bool extractAttributeValue(tinyxml2::XMLElement * root, const std::string & attribute, T & value)
	{
        auto ptr = root->Attribute(attribute.c_str());
		if(ptr){
			try{
		        std::string val = ptr;
				value = boost::lexical_cast<T>(val);
				return true;
			}catch(...){
				return false;
			}
		}

		return false;
	}

	template<>
	static const bool extractTagValue<std::string>(tinyxml2::XMLElement * root, const std::string & tag, std::string & value)
	{
		tinyxml2::XMLHandle rE(root);

		tinyxml2::XMLElement * element = rE.FirstChildElement(tag.c_str()).ToElement();

		if(element != nullptr){
			auto ret = element->GetText();
			if(ret != nullptr){
				value = std::string(ret);
			}
			
			return true;
		}

		return false;
	}

	template<>
	static const bool extractAttributeValue<std::string>(tinyxml2::XMLElement * root, const std::string & attribute, std::string & value)
	{
        auto ptr = root->Attribute(attribute.c_str());
		if(ptr) {
            value = ptr;
			return true;			
		}

		return false;
	}

	static const bool extractAttributes(tinyxml2::XMLElement * root, Attributes & attributes)
	{
		bool ret = true;

		Attributes locAttributes;

		tinyxml2::XMLHandle rE(root);

		tinyxml2::XMLElement * element = rE.FirstChildElement("Attributes").FirstChildElement("Attribute").ToElement();

		while(element != nullptr){

			Attribute attribute;

			attribute.name = utils::safeString( element->Attribute("Name"));
			attribute.value = utils::safeString( element->Attribute("Value"));

			std::string entity;
			entity = utils::safeString( element->Attribute("Value"));
			attribute.entity = Entity::convert(entity);

            attribute.attributeGroup = utils::safeString( element->Attribute("AttributeGroup "));

			std::string type = utils::safeString( element->Attribute("Type"));
			attribute.type = AttributeType::convert(type);

			locAttributes.push_back(attribute);

			element = element->NextSiblingElement();
		}

		if(ret == true){
			attributes.insert(attributes.end(), locAttributes.begin(), locAttributes.end());
		}

		return ret;
	}

	static const bool extractEnumValues(tinyxml2::XMLElement * root, EnumValueList & enumValues)
	{
		bool ret = true;

		EnumValueList locEnumValues;

		tinyxml2::XMLHandle rE(root);

		tinyxml2::XMLElement * element = rE.FirstChildElement("EnumValueList").FirstChildElement("EnumValue").ToElement();

		while(element != nullptr){

			EnumValue enumValue;

			enumValue = element->GetText();

			locEnumValues.push_back(enumValue);

			element = element->NextSiblingElement();
		}

		if(ret == true){
			enumValues.insert(enumValues.end(), locEnumValues.begin(), locEnumValues.end());
		}

		return ret;
	}

	static void extractAnnotation(tinyxml2::XMLElement * element, Annotation & annotation)
	{	
		XMLHelper::extractTagValue(element, "TrialID", annotation.trialID);		
		XMLHelper::extractTagValue(element, "UserID", annotation.userID);

		std::string status;
		XMLHelper::extractTagValue(element, "Status", status);
		annotation.status = AnnotationStatus::convert(status);

		XMLHelper::extractTagValue(element, "Comment", annotation.comment);
		XMLHelper::extractTagValue(element, "Note", annotation.note);
	}

	static void extractUserGroup(tinyxml2::XMLElement * element, UserGroup & userGroup)
	{		
		XMLHelper::extractAttributeValue(element, "UserGroupID", userGroup.id);		
		XMLHelper::extractAttributeValue(element, "UserGroupName", userGroup.name);		
	}

	static void extractUserDetails(tinyxml2::XMLElement * element, UserDetails & userDetails)
	{		
		XMLHelper::extractTagValue(element, "ID", userDetails.id);		
		XMLHelper::extractTagValue(element, "Login", userDetails.login);		
		XMLHelper::extractTagValue(element, "FirstName", userDetails.firstName);
		XMLHelper::extractTagValue(element, "LastName", userDetails.lastName);
	}

	//! TODO
	//! ta metoda albo poprzednia musi znikn¹æ po ujednoliceniu opisu usera w odpowiedzi us³ug
	static void _extractUserDetails(tinyxml2::XMLElement * element, UserDetails & userDetails)
	{		
		XMLHelper::extractAttributeValue(element, "ID", userDetails.id);		
		XMLHelper::extractAttributeValue(element, "Login", userDetails.login);		
		XMLHelper::extractAttributeValue(element, "FirstName", userDetails.firstName);
		XMLHelper::extractAttributeValue(element, "LastName", userDetails.lastName);
	}

	static void extractUserData(tinyxml2::XMLElement * element, User & user)
	{		
		extractUserDetails(element, user);
		XMLHelper::extractTagValue(element, "Email", user.email);
	}

	static void extractPerformer(tinyxml2::XMLElement * element, PerformerDetails & performer)
	{
		XMLHelper::extractTagValue(element, "PerformerID", performer.id);
		XMLHelper::extractTagValue(element, "FirstName", performer.firstName);
		XMLHelper::extractTagValue(element, "LastName", performer.lastName);
	}

	static void extractSession(tinyxml2::XMLElement * element, SessionDetails & session)
	{
		XMLHelper::extractTagValue(element, "SessionID", session.id);
		XMLHelper::extractTagValue(element, "UserID", session.userID);
		XMLHelper::extractTagValue(element, "LabID", session.labID);
		XMLHelper::extractTagValue(element, "MotionKind", session.motionKind);

		std::string sessionDate;
		XMLHelper::extractTagValue(element, "LastName", sessionDate);
		session.dateTime = toTime(sessionDate);

		XMLHelper::extractTagValue(element, "SessionDescription", session.description);
		XMLHelper::extractTagValue(element, "SessionLabel", session.label);
	}

	static void extractTrial(tinyxml2::XMLElement * element, TrialDetails & trial)
	{
		XMLHelper::extractTagValue(element, "TrialID", trial.id);
		XMLHelper::extractTagValue(element, "SessionID", trial.sessionID);
		XMLHelper::extractTagValue(element, "TrialDescription", trial.description);	
	}

	static void extractFile(tinyxml2::XMLElement * element, FileDetails & file)
	{
		element->QueryIntAttribute("FileID", &file.id);
		file.name = utils::safeString( element->Attribute("FileName"));
		file.description = utils::safeString( element->Attribute("FileDescription"));
		file.subdirPath = utils::safeString( element->Attribute("SubdirPath"));
		file.attributeName = utils::safeString( element->Attribute("AttributeName"));		
	}

	static void extractMeasurementConf(tinyxml2::XMLElement * element, MeasurementConfDetails & mcDetails)
	{		
		XMLHelper::extractTagValue(element, "MeasurementConfID", mcDetails.id);
		XMLHelper::extractTagValue(element, "MeasurementConfName", mcDetails.name);
		XMLHelper::extractTagValue(element, "MeasurementConfKind", mcDetails.kind);
		XMLHelper::extractTagValue(element, "MeasurementConfDescription", mcDetails.description);
	}

	static void extractPerformerConf(tinyxml2::XMLElement * element, PerformerConfDetails & pcDetails)
	{		
		XMLHelper::extractTagValue(element, "PerformerConfID", pcDetails.id);
		XMLHelper::extractTagValue(element, "SessionID", pcDetails.sessionID);
		XMLHelper::extractTagValue(element, "PerformerID ", pcDetails.performerID);
	}

	static void extractAttribute(tinyxml2::XMLElement * element, AttributeDefinition & attributeDef)
	{		
		XMLHelper::extractTagValue(element, "AttributeName", attributeDef.name);

		std::string attributeType;
		XMLHelper::extractTagValue(element, "AttributeType", attributeType);
		attributeDef.type = AttributeType::convert(attributeType);

		XMLHelper::extractTagValue(element, "AttributeEnum ", attributeDef.enumerate);
		XMLHelper::extractTagValue(element, "AttributeGroupName ", attributeDef.groupName);
		XMLHelper::extractTagValue(element, "Unit ", attributeDef.unit);
	}

	static void extractAttributeGroup(tinyxml2::XMLElement * element, AttributeGroupDefinition & attributeGroupDef)
	{		
		XMLHelper::extractTagValue(element, "AttributeGroupName", attributeGroupDef.name);

		std::string entityType;
		XMLHelper::extractTagValue(element, "DescribedEntity", entityType);
		attributeGroupDef.describedEntity = Entity::convert(entityType);		
	}

	static void extractSessionGroup(tinyxml2::XMLElement * element, SessionGroupDefinition & sessionGroupDef)
	{		
		XMLHelper::extractTagValue(element, "SessionGroupID", sessionGroupDef.id);		
		XMLHelper::extractTagValue(element, "SessionGroupName", sessionGroupDef.name);		
	}

	static void extractMotionKind(tinyxml2::XMLElement * element, MotionKindDefinition & motionKind)
	{		
		XMLHelper::extractTagValue(element, "MotionKindID", motionKind.id);		
		XMLHelper::extractTagValue(element, "MotionKindName", motionKind.name);
	}
};

const AnnotationsList webservices::xmlWsdl::parseAnnotations(const std::string & xmlResponse)
{
	AnnotationsList ret;

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;
	tinyxml2::XMLHandle hElement(0);

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);

	tinyxml2::XMLElement* annotationElement = hElement.FirstChildElement("Annotation").ToElement();

	while(annotationElement != nullptr) {
		Annotation a;

		XMLHelper::extractAnnotation(annotationElement, a);

		ret.push_back(a);

		annotationElement = annotationElement->NextSiblingElement();
	}

	return ret;
}

const UserGroupsList webservices::xmlWsdl::parseUserGroups(const std::string & xmlResponse)
{
	UserGroupsList ret;

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;
	tinyxml2::XMLHandle hElement(0);

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);

	tinyxml2::XMLElement* userGroupElement = hElement.FirstChildElement("UserGroup").ToElement();

	while(userGroupElement != nullptr) {
		UserGroup ug;

		XMLHelper::extractUserGroup(userGroupElement, ug);

		ret.push_back(ug);

		userGroupElement = userGroupElement->NextSiblingElement();
	}

	return ret;
}

const UsersList webservices::xmlWsdl::parseUsersList(const std::string & xmlResponse)
{
	UsersList ret;

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;
	tinyxml2::XMLHandle hElement(0);

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);

	tinyxml2::XMLElement* userDetailsElement = hElement.FirstChildElement("UserDetails").ToElement();

	while(userDetailsElement != nullptr) {
		UserDetails ud;

		XMLHelper::_extractUserDetails(userDetailsElement, ud);

		ret.push_back(ud);

		userDetailsElement = userDetailsElement->NextSiblingElement();
	}

	return ret;
}

const User webservices::xmlWsdl::parseUser(const std::string & xmlResponse)
{
	User ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;

	_element = hDocument.FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	XMLHelper::extractUserDetails(_element, ret);

	return ret;
}

const AttributeGroupDefinitionList webservices::xmlWsdl::parseAttributeGroupDefinitions(const std::string & xmlResponse)
{
	AttributeGroupDefinitionList ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;
	tinyxml2::XMLHandle hElement(0);

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);

	tinyxml2::XMLElement* attributeGroupDef = hElement.FirstChildElement("AttributeGroupDefinition").ToElement();

	while(attributeGroupDef != nullptr) {
		AttributeGroupDefinition agd;

		XMLHelper::extractTagValue(attributeGroupDef, "AttributeGroupName", agd.name);

		std::string entityType;
		XMLHelper::extractTagValue(attributeGroupDef, "DescribedEntity", entityType);
		agd.describedEntity = Entity::convert(entityType);

		ret.push_back(agd);

		attributeGroupDef = attributeGroupDef->NextSiblingElement();
	}

	return ret;
}

const PerformerList webservices::xmlWsdl::parsePerfomers(const std::string & xmlResponse)
{
	PerformerList ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;
	tinyxml2::XMLHandle hElement(0);

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);

	tinyxml2::XMLElement* performerDetails = hElement.FirstChildElement("PerformerDetails").ToElement();

	while(performerDetails != nullptr) {
		PerformerDetails pd;

		performerDetails->QueryIntAttribute("PerformerID", &pd.id);
		pd.firstName = utils::safeString( performerDetails->Attribute("FirstName"));
		pd.lastName = utils::safeString( performerDetails->Attribute("LastName"));

		ret.push_back(pd);

		performerDetails = performerDetails->NextSiblingElement();
	}

	return ret;
}

const PerformerDetailsWithAttributes webservices::xmlWsdl::parsePerformer(const std::string & xmlResponse)
{
	PerformerDetailsWithAttributes ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	XMLHelper::extractPerformer(_element, ret.performerDetails);
	XMLHelper::extractAttributes(_element, ret.attributes);

	return ret;
}

const PerformerWithAttributesList webservices::xmlWsdl::parsePerfomersWithAttributes(const std::string & xmlResponse)
{
	PerformerWithAttributesList ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;	

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}	

	while(_element != nullptr){

		PerformerDetailsWithAttributes pdwa;

		XMLHelper::extractPerformer(_element, pdwa.performerDetails);
		XMLHelper::extractAttributes(_element, pdwa.attributes);

		ret.push_back(pdwa);

		_element = _element->NextSiblingElement();
	}

	return ret;
}

const SessionList webservices::xmlWsdl::parseSessions(const std::string & xmlResponse)
{
	SessionList ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;
	tinyxml2::XMLHandle hElement(0);

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);

	_element = hElement.FirstChildElement("SessionDetails").ToElement();

	while(_element != nullptr){

		SessionDetails sd;

		_element->QueryIntAttribute("SessionID", &sd.id);
		_element->QueryIntAttribute("UserID", &sd.userID);
		_element->QueryIntAttribute("LabID", &sd.labID);
        sd.motionKind = utils::safeString( _element->Attribute("MotionKind"));

        std::string sessionDate = utils::safeString( _element->Attribute("SessionDate"));
		
		sd.dateTime = toTime(sessionDate);

		sd.description = utils::safeString( _element->Attribute("SessionDescription"));
		sd.label = utils::safeString( _element->Attribute("SessionLabel "));

		ret.push_back(sd);

		_element = _element->NextSiblingElement();
	}

	return ret;
}

const SessionDetailsWithAttributes webservices::xmlWsdl::parseSession(const std::string & xmlResponse)
{
	SessionDetailsWithAttributes ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	XMLHelper::extractSession(_element, ret.sessionDetails);
	XMLHelper::extractAttributes(_element, ret.attributes);

	return ret;
}

const SessionWithAttributesList webservices::xmlWsdl::parseSessionsWithAttributes(const std::string & xmlResponse)
{
	SessionWithAttributesList ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;	
	tinyxml2::XMLHandle hElement(0);

    _element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);
    _element = hElement.FirstChildElement("SessionDetailsWithAttributes").ToElement();

	while(_element != nullptr){

		SessionDetailsWithAttributes sdwa;

		XMLHelper::extractSession(_element, sdwa.sessionDetails);
		XMLHelper::extractAttributes(_element, sdwa.attributes);

		ret.push_back(sdwa);

		_element = _element->NextSiblingElement();
	}

	return ret;
}

const SessionGroupDefinitionList webservices::xmlWsdl::parseSessionGroups(const std::string & xmlResponse)
{
	SessionGroupDefinitionList ret;

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;	
	tinyxml2::XMLHandle hElement(0);

    _element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);
    _element = hElement.FirstChildElement("SessionGroupDefinition").ToElement();

	while(_element != nullptr){

		SessionGroupDefinition sgd;

		XMLHelper::extractTagValue(_element, "SessionGroupID", sgd.id);
		XMLHelper::extractTagValue(_element, "SessionGroupName", sgd.name);

		ret.push_back(sgd);

		_element = _element->NextSiblingElement();
	}

	return ret;
}

const MotionKindDefinitionList webservices::xmlWsdl::parseMotionKinds(const std::string & xmlResponse)
{
	MotionKindDefinitionList ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;	
	tinyxml2::XMLHandle hElement(0);

    _element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);
    _element = hElement.FirstChildElement("MotionKindDefinition").ToElement();

	while(_element != nullptr){

		MotionKindDefinition mkd;

		XMLHelper::extractTagValue(_element, "MotionKindID", mkd.id);
		XMLHelper::extractTagValue(_element, "MotionKindName", mkd.name);

		ret.push_back(mkd);

		_element = _element->NextSiblingElement();
	}

	return ret;
}

const TrialList webservices::xmlWsdl::parseTrials(const std::string & xmlResponse)
{
	TrialList ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;	
	tinyxml2::XMLHandle hElement(0);

    _element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);
    _element = hElement.FirstChildElement("TrialDetails").ToElement();

	while(_element != nullptr){

		TrialDetails td;

		_element->QueryIntAttribute("TrialID", &td.id);
		_element->QueryIntAttribute("SessionID", &td.sessionID);
		td.description = utils::safeString( _element->Attribute("TrialDescription"));		

		ret.push_back(td);

		_element = _element->NextSiblingElement();
	}

	return ret;
}

const TrialDetailsWithAttributes webservices::xmlWsdl::parseTrial(const std::string & xmlResponse)
{
	TrialDetailsWithAttributes ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;

    _element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	XMLHelper::extractTrial(_element, ret.trialDetails);
	XMLHelper::extractAttributes(_element, ret.attributes);

	return ret;
}

const TrialsWithAttributesList webservices::xmlWsdl::parseTrialsWithAttributes(const std::string & xmlResponse)
{
	TrialsWithAttributesList ret;

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;
	tinyxml2::XMLHandle hElement(0);

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);
	_element = hElement.FirstChildElement("TrialDetailsWithAttributes").ToElement();

	while(_element != nullptr){

		TrialDetailsWithAttributes tdwa;

		XMLHelper::extractTrial(_element, tdwa.trialDetails);
		XMLHelper::extractAttributes(_element, tdwa.attributes);

		ret.push_back(tdwa);

		_element = _element->NextSiblingElement();
	}

	return ret;
}

const MeasurementConfDetailsWithAttributes webservices::xmlWsdl::parseMeasurementConf(const std::string & xmlResponse)
{
	MeasurementConfDetailsWithAttributes ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	XMLHelper::extractMeasurementConf(_element, ret.measurementConfDetails);
	XMLHelper::extractAttributes(_element, ret.attributes);

	return ret;
}

const MeasurementConfWithAttributesList webservices::xmlWsdl::parseMeasurementsConfWithAttributes(const std::string & xmlResponse)
{
	MeasurementConfWithAttributesList ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;
	tinyxml2::XMLHandle hElement(0);

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);
	_element = hElement.FirstChildElement("MeasurementConfDetailsWithAttributes").ToElement();

	while(_element != nullptr){

		MeasurementConfDetailsWithAttributes mcwa;

		XMLHelper::extractMeasurementConf(_element, mcwa.measurementConfDetails);
		XMLHelper::extractAttributes(_element, mcwa.attributes);

		ret.push_back(mcwa);

		_element = _element->NextSiblingElement();
	}

	return ret;
}

const PerformerConfDetailsWithAttributes webservices::xmlWsdl::parsePerfomerConf(const std::string & xmlResponse)
{
	PerformerConfDetailsWithAttributes ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	XMLHelper::extractPerformerConf(_element, ret.performerConfDetails);
	XMLHelper::extractAttributes(_element, ret.attributes);

	return ret;
}

const PerformerConfWithAttributesList webservices::xmlWsdl::parsePerformersConfWithAttributes(const std::string & xmlResponse)
{
	PerformerConfWithAttributesList ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;
	tinyxml2::XMLHandle hElement(0);

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);
	_element = hElement.FirstChildElement("PerformerConfDetailsWithAttributes").ToElement();

	while(_element != nullptr){

		PerformerConfDetailsWithAttributes pcwa;

		XMLHelper::extractPerformerConf(_element, pcwa.performerConfDetails);
		XMLHelper::extractAttributes(_element, pcwa.attributes);

		ret.push_back(pcwa);

		_element = _element->NextSiblingElement();
	}

	return ret;
}

const FileList webservices::xmlWsdl::parseFiles(const std::string & xmlResponse)
{
	FileList ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;
	tinyxml2::XMLHandle hElement(0);

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);
	_element = hElement.FirstChildElement("FileDetails").ToElement();

	while(_element != nullptr){

		FileDetails fd;

		XMLHelper::extractFile(_element, fd);		

		ret.push_back(fd);

		_element = _element->NextSiblingElement();
	}

	return ret;
}

const FileWithAttributeList webservices::xmlWsdl::parseFilesWithAttributes(const std::string & xmlResponse)
{
	FileWithAttributeList ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;
	tinyxml2::XMLHandle hElement(0);

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);
	_element = hElement.FirstChildElement("FileDetailsWithAttributes").ToElement();

	while(_element != nullptr){

		FileDetailsWithAttribute fdwa;

		XMLHelper::extractFile(_element, fdwa.fileDetails);		
		XMLHelper::extractAttributes(_element, fdwa.attributes);		

		ret.push_back(fdwa);

		_element = _element->NextSiblingElement();
	}

	return ret;
}

const EnumValueList webservices::xmlWsdl::parseEnumValues(const std::string & xmlResponse)
{
	EnumValueList ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	XMLHelper::extractEnumValues(_element, ret);	

	return ret;
}

const AttributeDefinitionList webservices::xmlWsdl::parseAttributesDefinitions(const std::string & xmlResponse)
{
	AttributeDefinitionList ret;	

	tinyxml2::XMLDocument document;
	document.Parse(xmlResponse.c_str());

	if(document.Error()) {
		UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
	}

	tinyxml2::XMLHandle hDocument(&document);
	tinyxml2::XMLElement* _element;
	tinyxml2::XMLHandle hElement(0);

	_element = hDocument.FirstChildElement().FirstChildElement().ToElement();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}

	hElement = tinyxml2::XMLHandle(_element);
	_element = hElement.FirstChildElement("AttributeDefinition").ToElement();

	while(_element != nullptr){

		AttributeDefinition ad;

		XMLHelper::extractAttribute(_element, ad);
		XMLHelper::extractEnumValues(_element, ad.enumValues);

		ret.push_back(ad);

		_element = _element->NextSiblingElement();
	}

	return ret;
}