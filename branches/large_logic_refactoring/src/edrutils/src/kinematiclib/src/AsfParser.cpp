#include "stdafx.h"
#include <kinematiclib/SkeletalParsers.h>
#define PARSER_VERSION "0.11"

using namespace std;

namespace kinematic {

AsfParser::AsfParser() :
    forceRootXYZ(true),
    idCounter(0)
{
}

AsfParser::~AsfParser()
{
}

// główna metoda parsująca
void AsfParser::parse(SkeletalModelPtr model, const string& filename)
{
    // wskaźnik trzymany, aby metody nie musialy go przekazywac
    this->model = model;
    ifstream in(filename.c_str(), ios_base::in);
    // czy plik został poprawnie otwarty?
    if (!in) {
       throw UnableToOpenFileException(filename);
    }
    // skopiowanie zawartości pliku do pojedynczego stringa
    string storage;
    // nie omijaj bialych znakow
    in.unsetf(ios::skipws); 
    copy(
        istream_iterator<char>(in),
        istream_iterator<char>(),
        back_inserter(storage)
        );
    in.close();
    // usun komentarze z pliku
    storage = removeComments(storage);
    // podziel plik na sekcje
    bool result = splitAsf(storage);
    if (result) {
        result &= parseUnits(units);
        result &= parseRoot(root, model->getSkeleton());
        result &= parseJoints(bonedata);
        result &= parseHierarchy(hierarchy, model->getSkeleton());
    }
    if (!result) {
        throw WrongFileException("Error in file");
    }
}

// Dzieli wejściowy plik na poszczegolne sekcje
bool AsfParser::splitAsf (const string& asf) 
{
    istringstream is(asf);
    string line;
    string token;

    string* section = &this->unknown;
    while (getline(is, line)) {
        istringstream linestream(line);
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

void AsfParser::parseLimit(const string& token, vector<double>& limitValues) {
    string limit;
    int count = token.size();

    // na pewno jest jakas funkcja do tego...
    for (int i = 0; i < count; ++i) {
        char c = token[i];
        if (c != '(' && c != ')') {
            limit += (c != ',' ? c : ' ');
        }
    }
    
    double min = -numeric_limits<double>::infinity();
    double max = numeric_limits<double>::infinity();

    istringstream is (limit);

    for (int i = 0; i < 2; ++i)    {
        is >> limit;
        if (limit.find("inf") != string::npos) {
            limitValues.push_back(limit.find("-") != string::npos ? min : max);
        } else {
            limitValues.push_back(boost::lexical_cast<double>(limit));
        }
    }
}

bool AsfParser::parseSingleJoint(const string& singleBone, Joint& bone) {
    istringstream is(singleBone);

    vector<string> rawBones;
    Axis::Order order = Axis::YXZ;

    string token;
    // todo sprawdzic kopiowanie wektorow
    vector<DegreeOfFreedom::Channel> channels;
    vector<double> limitValues;
    string axis;

    while (is >> token)    {
        if (token.compare("id") == 0) {
            is >> bone.id;
            // niektóre pliki maja powtorzony unikalny id..
            bone.id = ++idCounter;
        } else if (token.compare("name") == 0) {
            is >> bone.name;
        } else if (token.compare("bodymass") == 0) {
            is >> bone.bodymass;
        } else if (token.compare("cofmass") == 0) {
            is >> bone.cofmass;
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
            bone.order = order;
        } else if (token.compare("dof") == 0) {
            getline(is, token);
            istringstream dofs(token);
            while (dofs >> token) {
                channels.push_back(DegreeOfFreedom::getChannel(token));
            }
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
            DegreeOfFreedom dof;
            dof.channel = channels[i];
            dof.minLimit = limitValues[2 * i];
            dof.maxLimit = limitValues[2 * i + 1];
            bone.dofs.push_back(dof);
        }
    } else if (limitsCount == 0) {
        LOGGER(Logger::Debug, "Bone " + bone.name + " has no limit values");
        double minf = -numeric_limits<double>::infinity();
        double pinf = numeric_limits<double>::infinity();
        for (unsigned int i = 0; i < count; ++i) {
            DegreeOfFreedom dof;
            dof.channel = channels[i];
            dof.minLimit = minf;
            dof.maxLimit = pinf;
            bone.dofs.push_back(dof);
        }
    } else {
        throw WrongFileException("dofs size != limits size");
    }
    return true;
}

bool AsfParser::parseJoints(const string& bones) {
    istringstream is (bones);
    string bone;
    SkeletalModel::JointIdMap& bonesIds = model->getJointIDMap();
    SkeletalModel::JointMap& bonesMap = model->getJointMap();
    string token;
    string::size_type shift = strlen("begin");
    string::size_type begin = 0;
    string::size_type end = 0;
    for(;;) {
        begin = bones.find("begin", end);
        end = bones.find("end", begin);
        begin += shift;
        if (begin != string::npos && end != string::npos && end > begin) {
            bone = bones.substr(begin, end - begin);
            JointPtr singleJoint(new Joint());
            bool jointParse = parseSingleJoint(bone, *singleJoint);

            if (jointParse) {
                if (bonesIds.find(singleJoint->id) == bonesIds.end()) {
                    bonesMap[singleJoint->name] = singleJoint;
                    bonesIds[singleJoint->id] = singleJoint;
                } else {
                    throw WrongFileException("Bone : " + singleJoint->name + " already added");
                }
            }
        } else {
            break;
        }
        
    }

    if (bonesMap.size() == 0) {
        throw WrongFileException("No bones defined");
    }

    return true;
}

bool AsfParser::parseUnits (const string& units) {
    istringstream is(units);
    string token;
    double number;

    while (is >> token) {
        if (token.compare("length")== 0) {
            is >> number;
            model->setLength(number);
        }
        else if (token.compare("mass")== 0) {
            is >> number;
            model->setMass(number);
        }
        else if (token.compare("angle")== 0) {
            is >> token;
            if (token.compare("deg") == 0) {
                model->setAngle(SkeletalModel::degrees);
            } else if (token.compare("rad")) {
                model->setAngle(SkeletalModel::radians);
            } else {
                LOGGER(Logger::Warning, "Unknown angle token");
            }
        } else {
           LOGGER(Logger::Warning, "Unknown units token");
        }
        
    }
    
    return true;
}

bool AsfParser::parseRoot(const string& root, Skeleton& skeleton) {
    osg::Vec3d position(0.0, 0.0, 0.0);
    osg::Vec3d orientation(0.0, 0.0, 0.0);

    Axis::Order order = Axis::XYZ;
    istringstream lines(root);
    string token;
    string line;
    
    //vector<DegreeOfFreedom::Channel> channels;
    channels.clear();
    while (getline(lines, line)) {
        istringstream is(line);
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
            LOGGER(Logger::Warning, "Unknown root token : " + token);
        }
    }
    
    string s;
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
    if (dataOrder == Axis::UnknownAxisOrder) {
        throw WrongFileException("Wrong root order");
    }

    skeleton.setAxis(order);
    skeleton.setOrder(dataOrder);
    //skeleton.setChannels(channels);
    skeleton.setOrientation(orientation);
    skeleton.setPosition(position);
    
    return true;//result;
}

bool AsfParser::parseHierarchy(const string& hierarchyString, Skeleton& skeleton)
{
    typedef vector<string> children;
    typedef pair<string, children> hierarchyLine;
    vector<hierarchyLine> hierarchy;

    istringstream stream(hierarchyString);
    string l;

    while(getline(stream, l)) {
        stringstream ss(l); 
        vector<string> tokens;
        string buffer;
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
    JointPtr root(new Joint());
    root->id = -1;
    root->name = "root";
    root->parent.reset();

    root->order = skeleton.getOrder();
    int count = channels.size();
    for (int i = 0; i < count; ++i) {
        DegreeOfFreedom dof;
        dof.channel = channels[i];
        root->dofs.push_back(dof);
    }

    skeleton.setRoot(root);
    model->getJointIDMap()[root->id] = root;
    model->getJointMap()[root->name] = root;
    
    for (int i = 0; i < linesCount; ++i) {
        hierarchyLine line = hierarchy[i];
        JointPtr parent = model->getJointByName(line.first);
        for (int j = line.second.size() - 1; j >= 0; --j) {
            JointPtr bone = model->getJointByName(line.second[j]);
            bone->parent = parent;
            parent->children.push_back(bone);
        }
    }
    if (root->children.size() == 0) {
        throw WrongFileException("root has no children");
    }
    
    return true;
}

string kinematic::AsfParser::removeComments( const string& txt )
{
    string result;
    string line;
    istringstream is(txt);

    string::size_type position;
   
    while( getline( is, line ) ) {
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

string* kinematic::AsfParser::getSectionContainter( const string& token ) {
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

    LOGGER(Logger::Warning, "Unknown section : " + token);
    return &this->unknown;
}

void kinematic::AsfParser::saveRoot( std::ostream& out ) const {
    out << ":root" << endl;
    out << "   " << "order";
    
    Skeleton& skeleton = model->getSkeleton();
    const std::vector<DegreeOfFreedom>& dofs = skeleton.getRoot()->dofs;
    int count = dofs.size();

    if (forceRootXYZ) {
        out << "  TX TY TZ RX RY RZ" << endl; 
    } else {
        for (int i = 0; i < count; ++i) {
            out << " " << DegreeOfFreedom::getChannelName(dofs[i].channel, true);
        }
    }
    out << endl;

    if (!forceRootXYZ) {
        out << "   " << "axis "  << Axis::getAxisOrderName(skeleton.getAxis()) <<  endl;
    } else {
        out << "   " << "axis XYZ" <<  endl;
    }

    osg::Vec3d position = skeleton.getPosition();
    out << "   " << "position "
        << position[0] << " "
        << position[1] << " "
        << position[2] << endl;

    osg::Vec3d orientation = skeleton.getOrientation();
    out << "   " << "orientation "  
        << orientation[0] << " "
        << orientation[1] << " "
        << orientation[2] << endl;
}

void kinematic::AsfParser::saveBones( std::ostream& out ) const
{
    out << ":bonedata" << endl;
    
    SkeletalModel::JointIdMap bonesIds = model->getJointIDMap();
    map<int, JointPtr>::iterator it;
    map<int, JointPtr>::iterator end = bonesIds.end();

    for (it = bonesIds.begin(); it != end; ++it) {
        if (it->second->id != -1) {
            saveSingleBone(out, *(it->second));
        } else {
            //std::cout << "ROOT";
        }

    }
}

void kinematic::AsfParser::saveSingleBone( std::ostream& out, const Joint& bone) const
{
    out << "  begin" << endl;
    out << "     " << "id " << bone.id << endl;
    out << "     " << "name " << bone.name << endl;
    if (bone.bodymass != std::numeric_limits<double>::infinity()) {
        out << "     " << "bodymass " << bone.bodymass;
    }
    if (bone.cofmass != std::numeric_limits<double>::infinity()) {
        out << "     " << "cofmass " << bone.cofmass;
    }
    out << "     " << "direction " << 
        bone.direction[0] << " " <<
        bone.direction[1] << " " <<
        bone.direction[2] << endl;

    out << "     " << "length " << bone.length << endl;
    out << "     " << "axis " << 
        bone.axis[0] << " " <<
        bone.axis[1] << " " <<
        bone.axis[2] << " " <<
        Axis::getAxisOrderName(bone.order) <<
        endl;

    int count = bone.dofs.size();

    if (count > 0) {
        out << right << "     "  << "dof";
        for (int i = 0; i < count; ++i) {
            out<< " " << DegreeOfFreedom::getChannelName(bone.dofs[i].channel) ;
        }
        out << endl;
        out << "     " << "limits ";
        double min = -numeric_limits<double>::infinity();
        double max = numeric_limits<double>::infinity();

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
            out << ")" << endl;
        }
    }
    out << "  end" << endl;
}

void kinematic::AsfParser::saveBoneInHierarchy(std::ostream& out, kinematic::JointConstPtr joint) const
{
    typedef std::vector<kinematic::JointPtr> BoneTable;
    BoneTable toInsert;
    
    //for (int i = children.size() - 1; i >= 0; --i) {
    for (BoneTable::const_iterator it = joint->children.cbegin(); it != joint->children.cend(); ++it) {   
        
        out << " " << (*it)->name;
        if ((*it)->children.size() > 0) {
           toInsert.push_back(*it);
        }
    }
    out << endl;
   
    int count = toInsert.size();
    for (int i = 0; i < count; ++i) {
        if (toInsert[i]->children.size() > 0) {
            out << "    " << toInsert[i]->name;
            saveBoneInHierarchy(out, toInsert[i]);
        }
    }
    
}

void kinematic::AsfParser::saveHierarchy( std::ostream& out ) const 
{
    out << ":hierarchy" << endl;
    out << "  begin" << endl;
    out << "    " << "root";
    saveBoneInHierarchy(out, model->getSkeleton().getRoot());

    out << "  end" << endl;
}

void kinematic::AsfParser::saveUnits( std::ostream& out ) const
{
    out << ":units" << endl;
    out << "  length " << model->getLength() << endl;
    out << "  mass " << model->getMass() << endl;
    out << "  angle ";
    if (model->getAngle() == SkeletalModel::degrees) {
        out << "deg";
    } else {
        out << "rad";
    }
    out << endl;
}

void AsfParser::save(const SkeletalModelPtr model, const std::string& filename ) {
    this->model = model;
    ofstream out;
    out.open(filename.c_str());
    out << "# Created by hmAnimation Parser (v. " << PARSER_VERSION << ")" << endl;
    out << ":version " << model->getVersion() << endl;
    out << ":name " << model->getName() << endl;
    saveUnits(out);
    out << ":documentation" << documentation << endl;
    saveRoot(out);
    saveBones(out);
    saveHierarchy(out);
    out.close();
}

}