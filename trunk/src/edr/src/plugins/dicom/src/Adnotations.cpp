#include "DicomPCH.h"
#include "Adnotations.h"
#include <boost/serialization/shared_ptr.hpp>

dicom::adnotations::BloodLevelsTypePtr dicom::adnotations::getDefaultBloodLevels()
{
	BloodLevelsTypePtr adn = utils::make_shared<BloodLevelsType>();
	adn->insert(BloodLevelsType::value_type(unknownBloodLevel, QObject::tr("unknown blood level"))); 
	adn->insert(BloodLevelsType::value_type(bloodLevel0, QObject::tr("bloodLevel0"))); 
	adn->insert(BloodLevelsType::value_type(bloodLevel1, QObject::tr("bloodLevel1"))); 
	adn->insert(BloodLevelsType::value_type(bloodLevel2, QObject::tr("bloodLevel2"))); 
	adn->insert(BloodLevelsType::value_type(bloodLevel3, QObject::tr("bloodLevel3"))); 
	adn->insert(BloodLevelsType::value_type(bloodLevel4, QObject::tr("bloodLevel4")));	
	return adn;
}

dicom::adnotations::BloodLevelsTypePtr dicom::adnotations::instanceBloodLevels()
{
	return getDefaultBloodLevels();
}

dicom::adnotations::ArthritisLevelsTypePtr dicom::adnotations::getDefaultArthritisLevels()
{
	ArthritisLevelsTypePtr adn = utils::make_shared<ArthritisLevelsType>();
	adn->insert(ArthritisLevelsType::value_type(unknownArthritisLevel, QObject::tr("unknown arthritis level"))); 
	adn->insert(ArthritisLevelsType::value_type(arthritisLevel0, QObject::tr("arthritisLevel0"))); 
	adn->insert(ArthritisLevelsType::value_type(arthritisLevel1, QObject::tr("arthritisLevel1"))); 
	adn->insert(ArthritisLevelsType::value_type(arthritisLevel2, QObject::tr("arthritisLevel2"))); 
	adn->insert(ArthritisLevelsType::value_type(arthritisLevel3, QObject::tr("arthritisLevel3"))); 
	adn->insert(ArthritisLevelsType::value_type(arthritisLevel4, QObject::tr("arthritisLevel4")));

	return adn;
}

dicom::adnotations::ArthritisLevelsTypePtr dicom::adnotations::instanceArthritisLevels()
{
	return getDefaultArthritisLevels();
}


dicom::adnotations::AdnotationsTypePtr dicom::adnotations::getDefault()
{
    AdnotationsTypePtr adn = utils::make_shared<AdnotationsType>();
    adn->insert(AdnotationsType::value_type(unknownAnnotation, QObject::tr("unknown annotation"))); 
    adn->insert(AdnotationsType::value_type(otherAnnotation, QObject::tr("other"))); 
    adn->insert(AdnotationsType::value_type(skin, QObject::tr("skin"))); 
    adn->insert(AdnotationsType::value_type(bone, QObject::tr("bone"))); 
    adn->insert(AdnotationsType::value_type(tendon, QObject::tr("tendon"))); 
    adn->insert(AdnotationsType::value_type(joint, QObject::tr("joint"))); 
    adn->insert(AdnotationsType::value_type(inflammatory, QObject::tr("region inflammatory synovitis degree of inflammation / hyperplasia"))); 
    adn->insert(AdnotationsType::value_type(intensity, QObject::tr("the degree of intensity/ hyperaemia")));
    adn->insert(AdnotationsType::value_type(noise, QObject::tr("noise")));
	adn->insert(AdnotationsType::value_type(bloodLevel, QObject::tr("blood level")));
	adn->insert(AdnotationsType::value_type(arthritisLevel, QObject::tr("arthritis level")));
    return adn;
}

dicom::adnotations::AdnotationsTypePtr dicom::adnotations::load( const core::Filesystem::Path& p )
{
    AdnotationsTypePtr adnotations;
    std::ifstream ifs(p.c_str());
    if (ifs.good()) {
        adnotations = utils::make_shared<AdnotationsType>();
        boost::archive::xml_iarchive xmlIn(ifs);
        xmlIn >> BOOST_SERIALIZATION_NVP(adnotations);
        ifs.close();
    }

    return adnotations;
}

void dicom::adnotations::save( const core::Filesystem::Path& p, AdnotationsTypeConstPtr adnotations )
{
    std::ofstream ofs(p.c_str());
    if(ofs.good()) {
        boost::archive::xml_oarchive oa(ofs);
        oa << BOOST_SERIALIZATION_NVP(adnotations);
        ofs.close();
    }
}

dicom::adnotations::AdnotationsTypePtr dicom::adnotations::instance()
{
    static adnotations::AdnotationsTypePtr adn;

    if (!adn) {
        adn = adnotations::load(adnotationsFile());
        if (!adn) {
            adn = adnotations::getDefault();
        }
    }

    return adn;

}

void dicom::adnotations::autoSave()
{
    save(adnotationsFile(), instance());
}

core::Filesystem::Path dicom::adnotations::adnotationsFile()
{
    return plugin::getUserApplicationDataPath("DicomAdnotations.xml");
}
