#include "stdafx.h"
#include <iostream>
#include <tinyxml2.h>
#include <kinematiclib/VskParser.h>
#include <kinematiclib/hmException.h>

using namespace std;

namespace kinematic 
{
    VskParser::VskParser() :
        loaded(false)
    {
    }

    void VskParser::parse(const string& filename) 
    {
        tinyxml2::XMLDocument doc;
        tinyxml2::XMLError error = doc.LoadFile(filename.c_str());
        tinyxml2::XMLElement* rootElement = nullptr;
        // jeśli plik jest plikiem *xml ...
        if (error == tinyxml2::XML_NO_ERROR) {
            tinyxml2::XMLElement* docElement = doc.FirstChildElement();
            while (docElement) {
                // szukanie głównego elementu - "KinematicModel"
                if (strcmp(docElement->Value(), "KinematicModel") == 0) {
                    rootElement = docElement;
                    break;
                }
                docElement = docElement->NextSiblingElement();
            }
        } else {
            // oczyt nie udał się z jakiegos powodu
            throw UnableToOpenFileException(filename);
        }
        
        // jeśli plik zawiera odpowiedni korzeń (root)
        if (rootElement) {
            for (tinyxml2::XMLElement* element = rootElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
                    // element Markers set zawiera dane dla połączeń i pojedynczych markerow
                    if (element && strcmp(element->Value(), "MarkerSet") == 0) {
                        for (
                            tinyxml2::XMLElement* markersElement = element->FirstChildElement(); 
                            markersElement != nullptr; markersElement = markersElement->NextSiblingElement()) {
                                if (markersElement && strcmp(markersElement->Value(), "Markers") == 0) {
                                    // znaleziono element z markerami!
                                    readMarkers(markersElement);
                                } else if (markersElement && strcmp(markersElement->Value(), "Sticks") == 0) {
                                    // znaleziono element z połączeniami markerow
                                    readSticks(markersElement);
                                }
                        }
                    }
            }

            if (sticks.size() == 0 || markers.size() == 0) {
                throw kinematic::WrongFileException(filename + " has no sticks or markers");
            }
        } else {
            WrongFileException(filename + " does not have root element");
        }

        loaded = true;
    }

    void VskParser::readSticks( tinyxml2::XMLElement* sticksElement )
    {
        // odczyt wczystkich połączeń
        for (tinyxml2::XMLElement* child = sticksElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
            if (child && strcmp(child->Value(), "Stick") == 0) {
                Stick s;
                const tinyxml2::XMLAttribute* attrib = child->FirstAttribute();
                while(attrib) {
                    if (strcmp(attrib->Name(), "MARKER1") == 0) {
                        s.name1 = attrib->Value();
                    } else if (strcmp(attrib->Name(), "MARKER2") == 0) {
                        s.name2 = attrib->Value();
                    } else if (strcmp(attrib->Name(), "RGB") == 0) {
                        istringstream iss(attrib->Value());
                        int r = 0;
						int g = 0;
						int b = 0;
                        iss >> r >> g >> b;
                        s.color[0] = static_cast<float>(r / 256.0f);
                        s.color[1] = static_cast<float>(g / 256.0f);
                        s.color[2] = static_cast<float>(b / 256.0f);
                        s.color[3] = 1.0f;
                    }

                    attrib = attrib->Next();
                }
                if (s.name1.size() > 0 && s.name2.size() > 0) {
                    sticks.push_back(s);
                }
            }
        }
    }

    void VskParser::readMarkers( tinyxml2::XMLElement* markersElement )
    {
        // odczyt wszystkich markerow
        for (tinyxml2::XMLElement* child = markersElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
           if (child && strcmp(child->Value(), "Marker") == 0) {
               Marker m;
               const tinyxml2::XMLAttribute* attrib = child->FirstAttribute();
               while(attrib) {
                   if (strcmp(attrib->Name(), "NAME") == 0) {
                       m.name = attrib->Value();
                   } else if (strcmp(attrib->Name(), "RADIUS") == 0) {
                       m.radius = attrib->DoubleValue();
                   } else if (strcmp(attrib->Name(), "RGB") == 0) {
                       istringstream iss(attrib->Value());
					   int r = 0;
					   int g = 0;
					   int b = 0;
                       iss >> r >> g >> b;
                       m.color[0] = static_cast<float>(r / 256.0f);
                       m.color[1] = static_cast<float>(g / 256.0f);
                       m.color[2] = static_cast<float>(b / 256.0f);
                       m.color[3] = 1.0f;
                   }
                   
                   attrib = attrib->Next();
               }

               if (m.name.size() > 0) {
                   markers.push_back(m);
               }
           }
       }
    }

    VskParser::stickIterators VskParser::getSticks() const
    {
        UTILS_ASSERT(loaded, "file not loaded!");
        return std::make_pair(sticks.cbegin(), sticks.cend());
    }

    VskParser::markerIterators VskParser::getMarkers() const
    {
        UTILS_ASSERT(loaded, "file not loaded!");
        return std::make_pair(markers.cbegin(), markers.cend());
    }

}
