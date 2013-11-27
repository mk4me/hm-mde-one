#include "DicomPCH.h"
#include "Adnotations.h"
#include <boost/serialization/shared_ptr.hpp>

dicom::adnotations::AdnotationsTypePtr dicom::adnotations::getDefault()
{
    AdnotationsTypePtr adn = utils::make_shared<AdnotationsType>();
    adn->insert(AdnotationsType::value_type(-1, QObject::tr("unknown"))); 
    adn->insert(AdnotationsType::value_type(0, QObject::tr("other"))); 
    adn->insert(AdnotationsType::value_type(1, QObject::tr("skin"))); 
    adn->insert(AdnotationsType::value_type(2, QObject::tr("bone"))); 
    adn->insert(AdnotationsType::value_type(3, QObject::tr("tendon"))); 
    adn->insert(AdnotationsType::value_type(4, QObject::tr("joint"))); 
    adn->insert(AdnotationsType::value_type(5, QObject::tr("region inflammatory synovitis degree of inflammation / hyperplasia"))); 
    adn->insert(AdnotationsType::value_type(6, QObject::tr("the degree of intensity/ hyperaemia")));
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
