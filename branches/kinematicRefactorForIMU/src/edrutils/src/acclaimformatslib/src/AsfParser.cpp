#include <acclaimformatslib/AsfParser.h>
#include <boost/lexical_cast.hpp>
#include <fstream>

#define PARSER_VERSION "0.12"

using namespace acclaim;

AsfParser::AsfParser() :
    forceRootXYZ(true),
    idCounter(0)
{
}

AsfParser::~AsfParser()
{
}

// główna metoda parsująca
Skeleton AsfParser::parse(const std::string& filename)
{
	std::ifstream in(filename.c_str(), std::ios_base::in);
    // czy plik został poprawnie otwarty?
    if (!in) {
       throw std::runtime_error("Unable to open file: " + filename);
    }
    // skopiowanie zawartości pliku do pojedynczego stringa
	std::string storage;
    // nie omijaj bialych znakow
	in.unsetf(std::ios::skipws);
	std::copy(
        std::istream_iterator<char>(in),
        std::istream_iterator<char>(),
        std::back_inserter(storage)
        );
    in.close();
    // usun komentarze z pliku
    storage = removeComments(storage);
    // podziel plik na sekcje
    bool result = splitAsf(storage);
    if (result) {
        result &= parseUnits(units);
        result &= parseRoot(root, model);
        result &= parseBones(bonedata);
        result &= parseHierarchy(hierarchy, model);
    }
    if (!result) {
		throw std::runtime_error("Error in file");
    }

	return model;
}

// Dzieli wejściowy plik na poszczegolne sekcje
bool AsfParser::splitAsf (const std::string& asf) 
{
    std::istringstream is(asf);
    std::string line;
    std::string token;

	std::string* section = &unknown;
	while (std::getline(is, line)) {
		std::istringstream linestream(line);
        linestream >> token;
        if (token[0] == ':') {
            section = getSectionContainter(token);
            while(linestream >> token) *section += token;
            *section += '\n';
            continue;
        }
        
        (*section) += line;
        *section += '\n';
    }

    int size = name.size();
    if (size > 0 ) {
        name.resize(size - 1, ' '); 
    }
    size = documentation.size();
    if (size > 0 ) { 
        documentation.resize(size - 1, ' ');
    }
    size = version.size();
    if (size > 0) {
        version.resize(size - 1, ' ');
    }

    return true;
}

void AsfParser::parseLimit(const std::string& token, std::vector<float>& limitValues) {
	std::string limit;
    int count = token.size();

    // na pewno jest jakas funkcja do tego...
    for (int i = 0; i < count; ++i) {
        char c = token[i];
        if (c != '(' && c != ')') {
            limit += (c != ',' ? c : ' ');
        }
    }	

	std::istringstream is(limit);

    for (int i = 0; i < 2; ++i)    {
        is >> limit;
		if (limit.find("inf") != std::string::npos) {
			limitValues.push_back(limit.find("-") != std::string::npos ? -std::numeric_limits<float>::infinity() : std::numeric_limits<float>::infinity());
        } else {
            limitValues.push_back(boost::lexical_cast<float>(limit));
        }
    }
}

bool AsfParser::parseSingleBone(const std::string& singleBone, Bone& bone) {
	std::istringstream is(singleBone);

	std::vector<std::string> rawBones;
	kinematicUtils::AxisOrder::Type order = kinematicUtils::AxisOrder::XYZ;

	std::string token;
    // todo sprawdzic kopiowanie wektorow
	std::vector<kinematicUtils::Channel> channels;
    std::vector<float> limitValues;
    std::string axis;

    while (is >> token)    {
        if (token.compare("id") == 0) {
            is >> bone.id;
            // niektóre pliki maja powtorzony unikalny id..
            bone.id = ++idCounter;
        } else if (token.compare("name") == 0) {
            is >> bone.name;
        } else if (token.compare("bodymass") == 0) {
            //is >> bone.bodymass;
        } else if (token.compare("cofmass") == 0) {
            //is >> bone.cofmass;
        }else if (token.compare("direction") == 0) {
            is >> bone.direction._v[0];
            is >> bone.direction._v[1];
            is >> bone.direction._v[2];
        } else if (token.compare("length") == 0) {
            is >> bone.length;
        } else if (token.compare("axis") ==0) {
            is >> bone.axis._v[0]; 
            is >> bone.axis._v[1];
            is >> bone.axis._v[2];
            is >> axis;
            order = Axis::getAxisOrder(axis);
            bone.axisOrder = order;
        } else if (token.compare("dof") == 0) {
            std::getline(is, token);
            std::istringstream dofs(token);
			std::string rotOrder;
			unsigned int ti = 0;
			unsigned int ri = 0;
            while (dofs >> token) {
				auto channel = DegreeOfFreedom::getChannel(token);
                channels.push_back(channel);
				if (channel == kinematicUtils::ChannelType::TX || channel == kinematicUtils::ChannelType::TY
					|| channel == kinematicUtils::ChannelType::TZ)
				{
					++ti;
				}
				else if (channel == kinematicUtils::ChannelType::RX || channel == kinematicUtils::ChannelType::RY
					|| channel == kinematicUtils::ChannelType::RZ)
				{
					rotOrder += tokenToAxis(token);
					++ri;
				}
            }

			if (!(ti == 0 || ti == 3) || !(ri == 0 || ri == 3))
			{
				throw std::runtime_error("Invalid acclaim degrees of freedom configuration. Incomplete rotation or translation");
			}

			bone.rotationOrder = Axis::getAxisOrder(rotOrder);

        } else if (token.compare("limits") == 0) {
            for (int i = channels.size() - 1; i >= 0; --i) {
                getline(is, token);
                parseLimit(token, limitValues);
            }
        }
    }
    unsigned int count = channels.size();
    unsigned int limitsCount = limitValues.size();
    if (limitsCount == (2 * count)) {
        for (unsigned int i = 0; i < count; ++i) {
			DegreeOfFreedom dof(channels[i]);            
            dof.minLimit = limitValues[2 * i];
            dof.maxLimit = limitValues[2 * i + 1];
            bone.dofs.push_back(dof);
        }
    } else if (limitsCount == 0) {
		float minf = -std::numeric_limits<float>::infinity();
		float pinf = std::numeric_limits<float>::infinity();
        for (unsigned int i = 0; i < count; ++i) {
			DegreeOfFreedom dof(channels[i]);            
            dof.minLimit = minf;
            dof.maxLimit = pinf;
            bone.dofs.push_back(dof);
        }
    } else {
        throw std::runtime_error("dofs size != limits size");
    }
    return true;
}

bool AsfParser::parseBones(const std::string& bones) {
    std::istringstream is (bones);
    std::string boneData;
    std::string token;
    std::string::size_type shift = strlen("begin");
    std::string::size_type begin = 0;
    std::string::size_type end = 0;
    for(;;) {
        begin = bones.find("begin", end);
        end = bones.find("end", begin);
        begin += shift;
		if (begin != std::string::npos && end != std::string::npos && end > begin) {
			boneData = bones.substr(begin, end - begin);
            Bone bone;
			bool boneParse = parseSingleBone(boneData, bone);

            if (boneParse) {
				if (model.bones.find(bone.id) == model.bones.end()) {
					model.bones[bone.id] = bone;
                } else {
					throw std::runtime_error("Bone : " + bone.name + " already added");
                }
            }
        } else {
            break;
        }        
    }

	if (model.bones.size() == 0) {
		throw std::runtime_error("No bones defined");
    }

    return true;
}

bool AsfParser::parseUnits(const std::string& units) {
    std::istringstream is(units);
    std::string token;
	std::string value;
    float number;

    while (is >> token) {

		is >> value;

		try{
			number = boost::lexical_cast<float>(value);
			model.units.defaultValues[token] = number;
		}
		catch (...){
			model.units.typeValueUnits[token] = value;
		}

		/*
        if (token.compare("length")== 0) {
            is >> number;
            //model->setLength(number);
			model.units.defaultValues["length"] = number;
        }
        else if (token.compare("mass")== 0) {
            is >> number;
            //model->setMass(number);
			model.units.defaultValues["mass"] = number;
        }
        else if (token.compare("angle")== 0) {
            is >> token;
			model.units.typeValueUnits["angle"] = token;
            if (token.compare("deg") == 0) {
				
            } else if (token.compare("rad")) {
                
            } else {

            }
        } else {

        }
        */
    }
    
    return true;
}

bool AsfParser::parseRoot(const std::string& root, Skeleton& skeleton) {
    osg::Vec3 position(0.0, 0.0, 0.0);
    osg::Vec3 orientation(0.0, 0.0, 0.0);

	kinematicUtils::AxisOrder::Type order = kinematicUtils::AxisOrder::XYZ;
    std::istringstream lines(root);
    std::string token;
    std::string line;
    
    channels.clear();
	while (std::getline(lines, line)) {
		std::istringstream is(line);
        if (line.size() == 0) {
            token = "";
        } else {
            is >> token;
        }
        if (token.compare("axis") == 0) {
            is >> token;
            order = Axis::getAxisOrder(token);
        } else if (token.compare("order")== 0) {
            while (is >> token) {
                channels.push_back(DegreeOfFreedom::getChannel(token));
            }
        } else if (token.compare("position")== 0) {
            is >> position[0] >> position[1] >> position[2];
        } else if (token.compare("orientation")== 0) {
            is >> orientation[0] >> orientation[1] >> orientation[2];
        } else if (token.size() != 0) {
            
        }
    }
    
	/*
	std::string s;
    int count = channels.size();
    for (int i = 0; i < count; ++i) {
        if (channels[i] == DegreeOfFreedom::RX) {
            s += "X";
        } else if (channels[i] == DegreeOfFreedom::RY) {
            s += "Y";
        } else if (channels[i] == DegreeOfFreedom::RZ) {
            s += "Z";
        }
    }
    Axis::Order dataOrder = Axis::getAxisOrder(s);
	*/

    skeleton.axisOrder = order;
    skeleton.dataOrder = channels;
    skeleton.orientation = orientation;
    skeleton.position = position;
    
    return true;
}

bool AsfParser::parseHierarchy(const std::string& hierarchyString, Skeleton& skeleton)
{
	typedef std::vector<std::string> children;
    typedef std::pair<std::string, children> hierarchyLine;
	std::vector<hierarchyLine> hierarchy;

    std::istringstream stream(hierarchyString);
    std::string l;

	while (std::getline(stream, l)) {
        std::stringstream ss(l); 
        std::vector<std::string> tokens;
        std::string buffer;
        while (ss >> buffer) {
            tokens.push_back(buffer);
        }

        hierarchyLine line;
        int count = tokens.size();
        if (count > 1) {
            line.first = tokens[0];
            for (int i = 1; i < count; ++i) {
                line.second.push_back(tokens[i]);
            }
            hierarchy.push_back(line);
        }
    }

    int linesCount = hierarchy.size();
    Bone root;
    root.id = -1;
    root.name = "root";

    root.axisOrder = model.axisOrder;
    int count = channels.size();
    for (int i = 0; i < count; ++i) {
		DegreeOfFreedom dof(channels[i]);        
        root.dofs.push_back(dof);
    }

    model.root = -1;
	model.bones[root.id] = root;
    
    for (auto& line : hierarchy) {
		auto parentID = std::find_if(model.bones.begin(), model.bones.end(), [&line](const Skeleton::Bones::value_type & val)
		{
			return val.second.name == line.first;
		})->first;

        for (auto& name : line.second) {
			auto boneID = std::find_if(model.bones.begin(), model.bones.end(), [&name](const Skeleton::Bones::value_type & val)
			{
				return val.second.name == name;
			})->first;          
			model.hierarchy.insert(Skeleton::Hierarchy::value_type(parentID, boneID));
        }
    }
    if (model.bones.size() < 2) {
		throw std::runtime_error("root has no children");
    }
    
    return true;
}

std::string acclaim::AsfParser::removeComments(const std::string& txt)
{
    std::string result;
    std::string line;
    std::istringstream is(txt);

	std::string::size_type position;
   
	while (std::getline(is, line)) {
        position = line.find("#");
        if (position > 0) {
            result += line.substr(0, position);
            result += '\n';
        } else if (position < 0) {
            result += line;
        }
    }

    return result;
}

std::string* acclaim::AsfParser::getSectionContainter(const std::string& token) {
    if (token.compare(":name") == 0) 
        return &this->name;
    if (token.compare(":documentation") == 0) 
        return &this->documentation;
    if (token.compare(":version") == 0) 
        return &this->version;
    if (token.compare(":units") == 0)
        return &this->units;
    if (token.compare(":root") == 0) 
        return &this->root;
    if (token.compare(":bonedata") == 0) 
        return &this->bonedata;
    if (token.compare(":hierarchy") == 0) 
        return &this->hierarchy;

    return &this->unknown;
}

void acclaim::AsfParser::saveRoot( std::ostream& out ) const {
	out << ":root" << std::endl;
    out << "   " << "order";
    
	auto rootIT = model.bones.find(model.root);

	const std::vector<DegreeOfFreedom>& dofs = rootIT->second.dofs;
    int count = dofs.size();

    if (forceRootXYZ) {
		out << "  TX TY TZ RX RY RZ" << std::endl;
    } else {
        for (int i = 0; i < count; ++i) {
            out << " " << DegreeOfFreedom::getChannelName(dofs[i].channel, true);
        }
    }
	out << std::endl;

    if (!forceRootXYZ) {
		out << "   " << "axis " << Axis::getAxisOrderName(model.axisOrder) << std::endl;
    } else {
		out << "   " << "axis XYZ" << std::endl;
    }

    osg::Vec3 position = model.position;
    out << "   " << "position "
        << position[0] << " "
        << position[1] << " "
		<< position[2] << std::endl;

    osg::Vec3 orientation = model.orientation;
    out << "   " << "orientation "  
        << orientation[0] << " "
        << orientation[1] << " "
		<< orientation[2] << std::endl;
}

void acclaim::AsfParser::saveBones( std::ostream& out ) const
{
	out << ":bonedata" << std::endl;

	for (auto it = model.bones.begin(); it != model.bones.end(); ++it) {
        if (it->second.id != -1) {
            saveSingleBone(out, it->second);
        } else {
            //std::cout << "ROOT";
        }

    }
}

void acclaim::AsfParser::saveSingleBone( std::ostream& out, const Bone& bone) const
{
	out << "  begin" << std::endl;
	out << "     " << "id " << bone.id << std::endl;
	out << "     " << "name " << bone.name << std::endl;
	/*
    if (bone.bodymass != std::numeric_limits<double>::infinity()) {
        out << "     " << "bodymass " << bone.bodymass;
    }
    if (bone.cofmass != std::numeric_limits<double>::infinity()) {
        out << "     " << "cofmass " << bone.cofmass;
    }
	*/
    out << "     " << "direction " << 
        bone.direction[0] << " " <<
        bone.direction[1] << " " <<
		bone.direction[2] << std::endl;

	out << "     " << "length " << bone.length << std::endl;
    out << "     " << "axis " << 
        bone.axis[0] << " " <<
        bone.axis[1] << " " <<
        bone.axis[2] << " " <<
        Axis::getAxisOrderName(bone.axisOrder) <<
		std::endl;

    int count = bone.dofs.size();

    if (count > 0) {
        out << std::right << "     "  << "dof";
        for (int i = 0; i < count; ++i) {
            out<< " " << DegreeOfFreedom::getChannelName(bone.dofs[i].channel) ;
        }
		out << std::endl;
        out << "     " << "limits ";
		float min = -std::numeric_limits<float>::infinity();
		float max = std::numeric_limits<float>::infinity();

        for (int i = 0; i < count; ++i) {
            i != 0 ? out << "          " : out;
            out << "(";
            if (bone.dofs[i].minLimit == min) {
                out << "-inf";
            } else {
                out << bone.dofs[i].minLimit;
            }
            out << ", ";
            if (bone.dofs[i].maxLimit == max) {
                out << "inf";
            } else {
                out << bone.dofs[i].maxLimit;
            }
			out << ")" << std::endl;
        }
    }
	out << "  end" << std::endl;
}

void acclaim::AsfParser::saveBoneInHierarchy(std::ostream& out, const Bone & bone) const
{
	//TODO
	/*
    BoneTable toInsert;
    
    //for (int i = children.size() - 1; i >= 0; --i) {
    for (BoneTable::const_iterator it = joint->children.cbegin(); it != joint->children.cend(); ++it) {   
        
        out << " " << (*it)->name;
        if ((*it)->children.size() > 0) {
           toInsert.push_back(*it);
        }
    }
	out << std::endl;
   
    int count = toInsert.size();
    for (int i = 0; i < count; ++i) {
        if (toInsert[i]->children.size() > 0) {
            out << "    " << toInsert[i]->name;
            saveBoneInHierarchy(out, toInsert[i]);
        }
    }
    */
}

void acclaim::AsfParser::saveHierarchy( std::ostream& out ) const 
{
	out << ":hierarchy" << std::endl;
	out << "  begin" << std::endl;
    out << "    " << "root";

	auto rootIT = model.bones.find(model.root);
    saveBoneInHierarchy(out, rootIT->second);

	out << "  end" << std::endl;
}

void acclaim::AsfParser::saveUnits( std::ostream& out ) const
{
	out << ":units" << std::endl;
	//TODO
	/*
	out << "  length " << model->getLength() << std::endl;
	out << "  mass " << model->getMass() << std::endl;
    out << "  angle ";
    if (model->getAngle() == SkeletalModel::degrees) {
        out << "deg";
    } else {
        out << "rad";
    }
	*/
	out << std::endl;
}

std::string acclaim::AsfParser::tokenToAxis(const std::string& token)
{
	if (token == "rx") return "X";
	if (token == "ry") return "Y";
	if (token == "rz") return "Z";
	if (token == "tx") return "X";
	if (token == "ty") return "Y";
	if (token == "tz") return "Z";
	throw std::runtime_error("Wrong token passed");
}

void AsfParser::save(const Skeleton & model, const std::string& filename ) {
    this->model = model;
	std::ofstream out;
    out.open(filename.c_str());
	out << "# Created by hmAnimation Parser (v. " << PARSER_VERSION << ")" << std::endl;
	out << ":version " << model.version << std::endl;
	out << ":name " << model.name << std::endl;
    saveUnits(out);
	out << ":documentation" << documentation << std::endl;
    saveRoot(out);
    saveBones(out);
    saveHierarchy(out);
    out.close();
}