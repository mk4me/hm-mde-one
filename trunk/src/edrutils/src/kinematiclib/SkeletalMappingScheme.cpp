#include "stdafx.h"

#include <kinematiclib/SkeletalMappingScheme.h>
#include <kinematiclib/hmException.h>

using namespace std;

namespace kinematic {

	void SkeletonMappingScheme::readNode(TiXmlNode* node, vector<SkeletonMappingSchemePtr>& result) 
	{
		if (!node) {
			return;
		}

		TiXmlNode* pChild;
		int t = node->Type();

		switch (t) {
		case TiXmlNode::TINYXML_DOCUMENT:
			break;

		case TiXmlNode::TINYXML_ELEMENT:
			{
				TiXmlElement* element = node->ToElement();
				if (element && strcmp(element->Value(), "SkeletonScheme") == 0) {
					mappingDict dict = readMappingNode(element);
					if (dict.size() > 0) {
						SkeletonMappingSchemePtr sms(new SkeletonMappingScheme(dict));
						result.push_back(sms);
					}
					return;
				}
			}
			break;

		case TiXmlNode::TINYXML_UNKNOWN:
			UTILS_ASSERT(false, "Unable to load dictionary");
			break;

		case TiXmlNode::TINYXML_TEXT:
			break;

		case TiXmlNode::TINYXML_DECLARATION:
			break;
		default:
			break;
		}
		for ( pChild = node->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) {
			readNode( pChild, result);
		}
	}

	SkeletonMappingScheme::mappingDict SkeletonMappingScheme::readMappingNode(TiXmlNode* node)
	{
		mappingDict result;
		TiXmlNode* child;
		for ( child = node->FirstChild(); child != nullptr; child = child->NextSibling()) {
			int t = node->Type();
			switch (t) {
			case TiXmlNode::TINYXML_ELEMENT:
				{
					TiXmlElement* element = child ? child->ToElement() : nullptr;
					TiXmlAttribute* pAttrib= element ? element->FirstAttribute() : nullptr;
					string name;
					string val;
					while (pAttrib) {
						if (strcmp(pAttrib->Name(),"hanim") == 0) {
							name = pAttrib->Value();
						} else if (strcmp(pAttrib->Name(), "name") == 0) {
							val = pAttrib->Value();
						} else {
							UTILS_ASSERT(false, "Wrong file");
						}
						pAttrib=pAttrib->Next();
					}

					UTILS_ASSERT(val.size() > 0 && name.size() > 0, "Wrong file");
					result[val] = name;
				}
				break;

			default:
				UTILS_ASSERT(false, "wrong dictionary file");
				break;
			}
		}
		return result;
	}

	void SkeletonMappingScheme::loadFromXML(const std::string& filename, std::vector<SkeletonMappingSchemePtr>& outSchemes)
	{
		TiXmlDocument doc(filename.c_str());
		bool loadOkay = doc.LoadFile();
		if (loadOkay) {
			readNode(&doc, outSchemes);
		} else {
			throw UnableToOpenFileException(filename);
		}
	}

	kinematic::SkeletonMappingSchemePtr SkeletonMappingScheme::createDefault()
	{
		std::map<std::string, std::string> jointMappingDictionary;
		jointMappingDictionary["root0"] = "vl5";
		jointMappingDictionary["root1"] = "l_hip";
		jointMappingDictionary["root2"] = "r_hip";
		jointMappingDictionary["ToSpine"] = "vl4";
		jointMappingDictionary["Spine"] = "vt12";
		jointMappingDictionary["Spine1"] = "vt1";
		jointMappingDictionary["Spine10"] = "vc7";
		jointMappingDictionary["Spine11"] = "l_sternoclavicular";
		jointMappingDictionary["Spine12"] = "r_sternoclavicular";
		jointMappingDictionary["Neck"] = "vc1";
		jointMappingDictionary["Head"] = "skullbase";
		jointMappingDictionary["LeftShoulder"] = "l_shoulder";
		jointMappingDictionary["LeftArm"] = "l_elbow";
		jointMappingDictionary["LeftForeArm"] = "l_wrist";
		jointMappingDictionary["LeftHand"] = "l_middle0";
		jointMappingDictionary["RightShoulder"] = "r_shoulder";
		jointMappingDictionary["RightArm"] = "r_elbow";
		jointMappingDictionary["RightForeArm"] = "r_wrist";
		jointMappingDictionary["RightHand"] = "r_middle0";
		jointMappingDictionary["LeftUpLeg"] = "l_knee";
		jointMappingDictionary["LeftLeg"] = "l_ankle";
		jointMappingDictionary["LeftFoot"] = "l_subtalar";
		jointMappingDictionary["LeftToeBase"] = "l_metatarsal";
		jointMappingDictionary["RightUpLeg"] = "r_knee";
		jointMappingDictionary["RightLeg"] = "r_ankle";
		jointMappingDictionary["RightFoot"] = "r_subtalar";
		jointMappingDictionary["RightToeBase"] = "r_metatarsal";
		//loadMappingDictionary("dictionary.txt");
		jointMappingDictionary["root"] = "HumanoidRoot";
		jointMappingDictionary["lhipjoint"] = "l_hip";
		jointMappingDictionary["lfemur"] = "l_knee";
		jointMappingDictionary["ltibia"] = "l_ankle";
		jointMappingDictionary["lfoot"] = "l_subtalar";
		jointMappingDictionary["ltoes"] = "l_metatarsal";
		jointMappingDictionary["rhipjoint"] = "r_hip";
		jointMappingDictionary["rfemur"] = "r_knee";
		jointMappingDictionary["rtibia"] = "r_ankle";
		jointMappingDictionary["rfoot"] = "r_subtalar";
		jointMappingDictionary["rtoes"] = "r_metatarsal";
		jointMappingDictionary["lowerback"] = "vl5";
		jointMappingDictionary["upperback"] = "vt12";
		jointMappingDictionary["thorax"] = "vt1";
		jointMappingDictionary["lowerneck"] = "vc7";
		jointMappingDictionary["upperneck"] = "vc1";
		jointMappingDictionary["head"] = "skullbase";
		jointMappingDictionary["lclavicle"] = "l_sternoclavicular";
		jointMappingDictionary["lhumerus"] = "l_shoulder";
		jointMappingDictionary["lradius"] = "l_elbow";
		jointMappingDictionary["lwrist"] = "l_wrist";
		jointMappingDictionary["lhand"] = "l_middle0";
		jointMappingDictionary["lfingers"] = "l_middle1";
		jointMappingDictionary["lthumb"] = "l_thumb1";
		jointMappingDictionary["rclavicle"] = "r_sternoclavicular";
		jointMappingDictionary["rhumerus"] = "r_shoulder";
		jointMappingDictionary["rradius"] = "r_elbow";
		jointMappingDictionary["rwrist"] = "r_wrist";
		jointMappingDictionary["rhand"] = "r_middle0";
		jointMappingDictionary["rfingers"] = "r_middle1";
		jointMappingDictionary["rthumb"] = "r_thumb1";


		jointMappingDictionary["root"] = "HumanoidRoot"; 
		jointMappingDictionary["LeftUpLeg_dum"] = "l_hip"; 
		jointMappingDictionary["RightUpLeg_dum"] = "r_hip"; 
		jointMappingDictionary["Spine_dum"] = "vl5"; 
		jointMappingDictionary["LeftUpLeg"] = "l_knee"; 
		jointMappingDictionary["LeftLeg"] = "l_ankle"; 
		jointMappingDictionary["LeftFoot"] = "l_subtalar";  
		jointMappingDictionary["LeftToeBase"] = "l_metatarsal"; 
		jointMappingDictionary["RightUpLeg"] = "r_knee"; 
		jointMappingDictionary["RightLeg"] = "r_ankle"; 
		jointMappingDictionary["RightFoot"] = "r_subtalar"; 
		jointMappingDictionary["RightToeBase"] = "r_metatarsal"; 
		jointMappingDictionary["Spine"] = "vt12"; 
		jointMappingDictionary["Spine1"] = "vt1"; 
		jointMappingDictionary["Spine1_dum1"] = "vc7";  
		jointMappingDictionary["Spine1_dum2"] = "l_sternoclavicular";  
		jointMappingDictionary["Spine1_dum3"] = "r_sternoclavicular"; 
		jointMappingDictionary["Neck"] = "vc1";  
		jointMappingDictionary["Head"] = "skullbase"; 
		jointMappingDictionary["LeftShoulder"] = "l_acromioclavicular"; 
		jointMappingDictionary["LeftArm"] = "l_shoulder"; 
		jointMappingDictionary["LeftForeArm"] = "l_elbow"; 
		jointMappingDictionary["LeftHand"] = "l_wrist"; 
		jointMappingDictionary["LeftHand_dum2"] = "l_middle0"; 
		jointMappingDictionary["LeftHand_dum1"] = "l_thumb1";  
		jointMappingDictionary["LeftHandThumb"] = "l_thumb2"; 
		jointMappingDictionary["RightShoulder"] = "r_acromioclavicular";  
		jointMappingDictionary["RightArm"] = "r_shoulder"; 
		jointMappingDictionary["RightForeArm"] = "r_elbow"; 
		jointMappingDictionary["RightHand"] = "r_wrist"; 
		jointMappingDictionary["RightHand_dum2"] = "r_middle0"; 
		jointMappingDictionary["RightHand_dum1"] = "r_thumb1";  
		jointMappingDictionary["RightHandThumb"] = "r_thumb2"; 
		
		jointMappingDictionary["lshoulderjoint"] = "l_sternoclavicular";
		jointMappingDictionary["rshoulderjoint"] = "r_sternoclavicular";

		SkeletonMappingSchemePtr res(new SkeletonMappingScheme(jointMappingDictionary));
		return res;
	}

} //kinematic
