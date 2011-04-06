#include "stdafx.h"
#include <iostream>
#include <tinyxml.h>
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
        TiXmlDocument doc(filename.c_str());
        bool loadOkay = doc.LoadFile();
        TiXmlElement* rootElement = nullptr;
        // jesli plik jest plikiem *xml ...
        if (loadOkay) {
            TiXmlElement* docElement = doc.FirstChildElement();
            while (docElement) {
                // szukanie glownego elementu - "KinematicModel"
                if (strcmp(docElement->Value(), "KinematicModel") == 0) {
                    rootElement = docElement;
                    break;
                }
                docElement = docElement->NextSiblingElement();
            }
        } else {
            // oczyt nie udal sie z jakiegos powodu
            throw UnableToOpenFileException(filename);
        }
        
        // jesli plik zawiera odpowiedni korzen (root)
        if (rootElement) {
            for (TiXmlElement* element = rootElement->FirstChildElement(); element != nullptr; element = element->NextSiblingElement()) {
                    // element Markers set zawiera dane dla polaczen i pojedynczych markerow
                    if (element && strcmp(element->Value(), "MarkerSet") == 0) {
                        for (
                            TiXmlElement* markersElement = element->FirstChildElement(); 
                            markersElement != nullptr; markersElement = markersElement->NextSiblingElement()) {
                                if (markersElement && strcmp(markersElement->Value(), "Markers") == 0) {
                                    // znaleziono element z markerami!
                                    readMarkers(markersElement);
                                } else if (markersElement && strcmp(markersElement->Value(), "Sticks") == 0) {
                                    // znaleziono element z polaczeniami markerow
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

    void VskParser::readSticks( TiXmlElement* sticksElement )
    {
        // odczyt wczystkich polaczen
        for (TiXmlElement* child = sticksElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
            if (child && strcmp(child->Value(), "Stick") == 0) {
                Stick s;
                TiXmlAttribute* attrib = child->FirstAttribute();
                while(attrib) {
                    if (strcmp(attrib->Name(), "MARKER1") == 0) {
                        s.name1 = attrib->Value();
                    } else if (strcmp(attrib->Name(), "MARKER2") == 0) {
                        s.name2 = attrib->Value();
                    } else if (strcmp(attrib->Name(), "RGB") == 0) {
                        istringstream iss(attrib->Value());
                        int r, g, b;
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

    void VskParser::readMarkers( TiXmlElement* markersElement )
    {
        // odczyt wszystkich markerow
        for (TiXmlElement* child = markersElement->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
           if (child && strcmp(child->Value(), "Marker") == 0) {
               Marker m;
               TiXmlAttribute* attrib = child->FirstAttribute();
               while(attrib) {
                   if (strcmp(attrib->Name(), "NAME") == 0) {
                       m.name = attrib->Value();
                   } else if (strcmp(attrib->Name(), "RADIUS") == 0) {
                       m.radius = attrib->DoubleValue();
                   } else if (strcmp(attrib->Name(), "RGB") == 0) {
                       istringstream iss(attrib->Value());
                       int r, g, b;
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