#include "CorePCH.h"
#include <algorithm>
#include <set>
#include <iterator>
#include <core/C3DParserEx.h>
#include <core/C3DChannels.h>
//#include <core/IMarker.h>
//#include <core/Marker.h>
#include <boost/algorithm/string.hpp>

using namespace core;

static const int markersSlots = 39;

C3DParser::C3DParser()
{
    //object = core::ObjectWrapper::create<C3D_Data>();
    // dodanie kana³ów
    for(int i=0; i<12; i++){
        GRFChannels.push_back(ObjectWrapper::create<GRFChannel>());
    }
    for(int i=12;i<28;i++){
        EMGChannels.push_back(ObjectWrapper::create<EMGChannel>());
    }

    MarkerChannels = ObjectWrapper::create<MarkerSet>();

    //MarkerChannels.resize(markersSlots);
    //for(int i = 0; i < markersSlots; i++){
    //    MarkerChannels[i] = ObjectWrapper::create<MarkerChannel>();
    //}


}

C3DParser::~C3DParser()
{
}

void C3DParser::parseFile(core::IDataManager* /*dataManager*/, const core::Filesystem::Path& path)
{
    // parsowanie pliku
    c3dParser parser;
    scoped_ptr<C3D_Data> data(parser.parseData(path.string()));

    for (int i = 0; i < 12; ++i) {
        GRFChannelPtr ptr(new GRFChannel(*data , i));
        //GRFChannels[i]->set<GRFChannel>(ptr);
        GRFChannels[i]->set(ptr);
        GRFChannels[i]->setName(ptr->getName());
        GRFChannels[i]->setSource(path.string());
    }
    for (int i = 12; i < 28; ++i) {
        EMGChannelPtr ptr(new EMGChannel(*data , i));
        //EMGChannels[i-12]->set<EMGChannel>(ptr);
        EMGChannels[i-12]->set(ptr);
        EMGChannels[i-12]->setName(ptr->getName());
        EMGChannels[i-12]->setSource(path.string());
    }

    MarkerSetPtr markers(new MarkerSet);
    const ::GroupData* pointsGroup = data->getParameters()->getGroup("POINT");
    const ::Parameter* labels = pointsGroup->getParameter("LABELS");
    int markersCount = data->getHeader()->getNumberOfC3DPoints();
    
    std::map<std::string, int> markerLabels;
        std::set<std::string> markersSet, markersToDrop, markers3D;
    for(int i = 0; i < markersCount; ++i) {
       std::string name = (boost::trim_copy(labels->getDataAsStringsArray(i)));
       markerLabels[name] = i;
       markersSet.insert(name);
    }

    const char* labels39[] = {
        "LFHD", "RFHD", "LBHD", "RBHD", "C7",   "T10",  "CLAV", "STRN", "RBAK", 
        "LSHO", "LUPA", "LELB", "LFRM", "LWRA", "LWRB", "LFIN", "RSHO", "LPSI",
        "RUPA", "RELB", "RFRM", "RWRA", "RWRB", "RFIN", "LASI", "RASI", "RPSI", 
        "LTHI", "LKNE", "LTIB", "LANK", "LHEE", "LTOE", "RTHI", "RKNE", "RTIB", 
        "RANK", "RHEE", "RTOE"};
    const int labels39Count =  sizeof(labels39) / sizeof(char*);
    std::set<std::string> set39(labels39, labels39 + labels39Count);

    std::set_difference(markersSet.begin(), markersSet.end(), 
        set39.begin(), set39.end(),
        std::inserter(markers3D, markers3D.end()));

    if (markersSet.size() - markers3D.size() == labels39Count) {
        markers3D = set39;
    } else {
        markers3D.clear();
        const char* labels53[] = {
            "ARIEL", "LFHD", "LBHD", "RFHD", "RBHD", "C7",   "T10",  "CLAV", "STRN" 
            "LFSH" , "LBSH", "LUPA", "LELB", "LIEL", "LOWR", "LIWR", "LWRE", "LIHAND",
            "LOHAND","RFSH", "RBSH", "RUPA", "RELB", "RIEL", "ROWR", "RIWR", "RWRE", 
            "RIHAND","ROHAND","LFWT","LMWT", "LBWT", "RFWT", "RMWT", "RBWT", "LHIP", 
            "LKNE",  "LKNI", "LSHN", "LANK", "LHEL", "LMT5", "LMT1", "LTOE", "RHIP", 
            "RKNE",  "RKNI", "RSHN", "RANK", "RHEL", "RMT5", "RMT1", "RTOE"};
        const int labels53Count =  sizeof(labels53) / sizeof(char*);
        std::set<std::string> set53(labels53, labels53 + labels53Count);

        std::set_difference(markersSet.begin(), markersSet.end(), 
            set53.begin(), set53.end(),
            std::inserter(markers3D, markers3D.end()));

        if (markersSet.size() - markers3D.size() == labels53Count) {
            markers3D = set53;
        } else {
            const int dropCount = 4;
            std::vector<const ::Parameter*> parameters(dropCount);

            parameters[0] = pointsGroup->getParameter("FORCES");
            parameters[1] = pointsGroup->getParameter("MOMENTS");
            parameters[2] = pointsGroup->getParameter("ANGLES");
            parameters[3] = pointsGroup->getParameter("POWERS");
    
            for (int i = 0; i < dropCount; ++i) {
                std::vector<short> dim = parameters[i]->getDimensions();
                int labelsCount = dim[1];
                for (int j = 0; j < labelsCount; ++j) {
                    markersToDrop.insert(boost::trim_copy(parameters[i]->getDataAsStringsArray(j)));
                }
            }

            //http://b-tk.googlecode.com/svn-history/r396/BTK/trunk/Code/BasicFilters/btkSeparateKnownVirtualMarkersFilter.cpp
            const char* prefixes[] = {
                "HED", "LCL", "LFE", "LFO", "LHN", "LHU", "LRA", "LTI", "LTO", "PEL",
                "RCL", "RFE", "RFO", "RHN", "RHU", "RRA", "RTI", "RTO", "TRX"
            };

    
            BOOST_FOREACH(const char* prefix, prefixes) {
                std::string s(prefix);
                markersToDrop.insert(s + "O");
                markersToDrop.insert(s + "L");
                markersToDrop.insert(s + "A");
                markersToDrop.insert(s + "P");
            }

            markersToDrop.insert("CentreOfMass");
            markersToDrop.insert("CentroOfMassFloor");

            std::set_difference(markersSet.begin(), markersSet.end(), 
                markersToDrop.begin(), markersToDrop.end(),
                std::inserter(markers3D, markers3D.end()));
        }
    }
    

    for (auto it = markers3D.begin(); it != markers3D.end(); it++) {
        int index = markerLabels[*it];
        MarkerChannelPtr ptr(new MarkerChannel(*data, index));
        markers->addMarker(ptr);
    }

    //markersCount = markersSlots < markersCount ? markersSlots : markersCount;
    ////MarkerChannels.resize(markersCount);
    //for (int i = 0; i < markersCount; ++i) {
    //    MarkerChannelPtr ptr(new MarkerChannel(*data, i));
    //    markers->addMarker(ptr);
    //}
    //MarkerChannels->set<MarkerSet>(markers);
    MarkerChannels->set(markers);
}

core::IParser* C3DParser::create()
{
    return new C3DParser();
}

std::string C3DParser::getSupportedExtensions() const
{
    return "c3d";
}

C3D_Data* C3DParser::getC3dData()
{
    return nullptr;
}

void C3DParser::getObjects( std::vector<core::ObjectWrapperPtr>& objects )
{
    objects.insert(objects.end(), GRFChannels.begin(), GRFChannels.end());
    objects.insert(objects.end(), EMGChannels.begin(), EMGChannels.end());
    objects.push_back(MarkerChannels);

}