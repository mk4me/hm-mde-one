#include "stdafx.h"
#include <osg/MatrixTransform>
#include <osg/Transform>
#include <osg/Math>
#include <osg/Matrix>
#include <osg/Matrixd>
#include <osg/Matrixf>
#include <kinematiclib/SkeletalParsers.h>

using namespace kinematic;
using namespace std;
//----------------------------------------------------------------------------------
std::string kinematic::BvhParser::getChannelName( DegreeOfFreedom::Channel channel ) const {
    switch (channel) {
        case DegreeOfFreedom::TX : return "Xposition";
        case DegreeOfFreedom::TY : return "Yposition";
        case DegreeOfFreedom::TZ : return "Zposition";
        case DegreeOfFreedom::RX : return "Xrotation";
        case DegreeOfFreedom::RY : return "Yrotation";
        case DegreeOfFreedom::RZ : return "Zrotation";
    }
    
    throw WrongFileException("Wrong channel name");
}
//----------------------------------------------------------------------------------
const std::string kinematic::BvhParser::space(const int no) {
    return string(no, ' ');
}
//-------------------------------------------------------------------------------------------
string kinematic::BvhParser::spaceL(int lvl) const {
    // podobno niektóre parsery nie potrafia sobie poradzic z plikiem *.bvh
    // jeśli będzie miał wciecia zrobione ze spacji
    if (tabs == false) {
        return space((lvl + 1) * 2);
    } else {
        string tabs;
        for (int i = 0; i < lvl; ++i) {
            tabs += '\t';
        }
        return tabs;
    }
}
//----------------------------------------------------------------------------------
SkeletalData::singleJointStatePtr kinematic::BvhParser::getSingleJointState( SkeletalDataConstPtr data,int frameNo, std::string boneName ) const
{
    // jeśli dane z konkretnej klatki zawieraja wpis o zadanej kości
    // to zostaje ona odnaleziona i zwrócona
    const vector<SkeletalData::singleFramePtr>& frames = data->getFrames();
    for (int i = frames[frameNo]->jointsData.size() - 1; i >= 0; --i) {
        if (frames[frameNo]->jointsData[i]->name == boneName) {
            return frames[frameNo]->jointsData[i];
        }
    }
    // w przeciwnym razie zwracany jest pusty stan, jest to przydatne jeśli wymusza się
    // aby każda kość miała dane z 3 lub 6 kanałów
    SkeletalData::singleJointStatePtr state(new SkeletalData::singleJointState);
    state->name = boneName;
    state->channelValues.push_back(0);
    state->channelValues.push_back(0);
    state->channelValues.push_back(0);
    return state;
}
//----------------------------------------------------------------------------------
bool kinematic::BvhParser::hasTranslation(const std::vector<DegreeOfFreedom>& dofs ) {
    for (int i = dofs.size() - 1; i >= 0; --i) {
        if (
            dofs[i].channel == DegreeOfFreedom::TX ||
            dofs[i].channel == DegreeOfFreedom::TY ||
            dofs[i].channel == DegreeOfFreedom::TZ
            )
        {
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------------
void kinematic::BvhParser::saveChannelsInHierarchy( std::ostream& out, JointConstPtr joint, int lvl) const
{
    if (forceZXY) {
        out << spaceL(lvl) << "CHANNELS 3 Zrotation Xrotation Yrotation" << endl;
    } else {
        int count = joint->dofs.size();
        if (count > 0) {
            out << spaceL(lvl) << "CHANNELS " << count;
            for (int i = 0; i < count; ++i) {
                out << " " << getChannelName((joint->dofs[i]).channel);
            }
            out << endl;
        }
    }
}
//----------------------------------------------------------------------------------
void kinematic::BvhParser::saveJoint (SkeletalModelConstPtr model, std::ostream& out, JointConstPtr joint, 
		JointConstPtr parent,  int lvl, std::list<std::string>& jointList) const 
{
    if (parent && parent->length == 0.0 && joint->children.size() == 1) {
        // jeśli przetwarzana kość to 'dummy bone' to zostanie ona zignorowana przy zapisie do bvh
        JointPtr null;
        saveJoint(model, out, joint->children[0], null, lvl, jointList);
    } else {
        // dodanie kości do listy z kolejnością wystapienia (ważne przy zapisie danych z kanałów)
        jointList.push_back(joint->name);
        int childrenCount = joint->children.size();
        out << spaceL(lvl) << "JOINT " << joint->name << endl;
        out << spaceL(lvl) << "{" << endl;
        ++lvl; // zwiekszenie wciecia
        out << spaceL(lvl) << "OFFSET ";
        // kość rodzica jest potrzebna aby obliczyć długość,
        // dlaczego parent nie jest brany z argumentu funkcji? poniewaz argument moze być nullem
        // (sztuczka robiona w przypadku gdy parent to dummy bone)
        JointConstPtr parent = boost::const_pointer_cast<const Joint>(joint->parent.lock());
        osg::Vec3d offset = parent->direction * parent->length * model->getLength();
        out << offset[0] << " " << offset[1] << " " << offset[2] << endl;
        saveChannelsInHierarchy(out, joint, lvl);
        // rekurencja
        for (int i = 0; i < childrenCount; ++i) {
            saveJoint(model, out, joint->children[i], joint, lvl, jointList);
        }
        // lisc jest traktowany inaczej
        if (childrenCount == 0) {
            out << spaceL(lvl) << "End Site" << endl;
            out << spaceL(lvl) << "{" << endl;
            out << spaceL(lvl + 1) << "OFFSET ";
            osg::Vec3d offset = joint->direction * joint->length;
            out << offset[0] << " " << offset[1] << " " << offset[2] << endl;
            out << spaceL(lvl) << "}" << endl;
        }

        lvl--;
        out << spaceL(lvl) << "}" << endl;
    }
}
//----------------------------------------------------------------------------------
void BvhParser::save(SkeletalModelConstPtr model, SkeletalDataConstPtr data, const std::string& filename ) {
    std::ofstream out;
    // tutaj trafiaja jointy zgodnie z kolejnością w której sa zapisywane
    std::list<std::string> jointList;
    const Skeleton& skeleton = model->getSkeleton();
    out.open(filename.c_str());
    int lvl = 0;
    out << "HIERARCHY" << endl;
    out << "ROOT " << skeleton.getRootName() << endl;
    out << "{" << endl;
    out << spaceL(lvl) << "OFFSET ";
    // pozycja roota w globalnym układzie odniesienia
    osg::Vec3d pos = skeleton.getPosition();
    out << pos[0] << " ";
    out << pos[1] << " ";
    out << pos[2];
    // todo flaga forceZXY
    out << endl << spaceL(lvl) << "CHANNELS 6";// << channels.size();
    out << " Xposition Yposition Zposition Zrotation Xrotation Yrotation" << endl;
    /*for (unsigned int i = 0; i < channels.size(); ++i) {
        out << " " << getChannelName(channels[i]);
    }*/
    out << endl;
    JointConstPtr root = skeleton.getRoot();
    jointList.push_back(root->name);
    int count = root->children.size();
    // rekurencja
    for (int i = 0; i < count; ++i) {
        saveJoint(model, out, root->children[i], root, lvl, jointList);
    }
    out << "}" << endl;

    // zapis danych animacji
    const vector<SkeletalData::singleFramePtr>& frames = data->getFrames();
    out << "MOTION" << endl;
    out << "Frames:\t" << frames.size() << endl;
    out << "Frame Time:\t" << data->getFrameTime() << endl;

    int framesCount = frames.size();
    // dla wszystkich klatek...
    for (int i = 0; i < framesCount; ++i) {
        std::list<std::string>::iterator it;
        // zgodnie z kolejnością zapisu jointów w pliku...
        for (it = jointList.begin(); it != jointList.end(); ++it) {
            SkeletalData::singleJointStatePtr jointState = getSingleJointState(data, i, *it);
            JointConstPtr bone = model->getJointByName(jointState->name);

            if (DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::L, bone->dofs) != -1) {
                LOGGER(Logger::Warning, "Acclaim 'L' degree of freedom is not supported");
            }
            // nastepuje zapis danych
            if (forceZXY) {
                int index = 0;
                if (hasTranslation(bone->dofs)) {
                    index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TX, bone->dofs);
                    out << " " << (index != -1 ? jointState->channelValues[index] : 0.0);
                    index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TY, bone->dofs);
                    out << " " << (index != -1 ? jointState->channelValues[index] : 0.0);
                    index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TZ, bone->dofs);
                    out << " " << (index != -1 ? jointState->channelValues[index] : 0.0);
                }
                double rx, ry, rz;
                index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RZ, bone->dofs);
                rz = (index != -1 ? jointState->channelValues[index] : 0.0);
                index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RX, bone->dofs);
                rx = (index != -1 ? jointState->channelValues[index] : 0.0);
                index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RY, bone->dofs);
                ry = (index != -1 ? jointState->channelValues[index] : 0.0);
                osg::Vec3d v; v[0] = rx; v[1] = ry; v[2] = rz;
                osg::Matrix mat;
                double mul = model->getAngle() == SkeletalModel::radians ? 1.0 : osg::DegreesToRadians(1.0);
                osg::Quat mX; mX.makeRotate(mul * (bone->axis[0]), osg::Vec3(1, 0, 0));
                osg::Quat mY; mY.makeRotate(mul * (bone->axis[1]), osg::Vec3(0, 1, 0));
                osg::Quat mZ; mZ.makeRotate(mul * (bone->axis[2]), osg::Vec3(0, 0, 1));
                osg::Quat m = mX * mY * mZ;
                v = m * v;
                rx = v[0]; ry = v[1]; rz = v[2];
                out << " " << rz << " " << rx << " " << ry;
            }
            // todo rozwiązać ten balagan
            /*else {
                for (unsigned int k = 0; k < frames[i].bonesData[j].channelValues.size(); k++) {
                    out << frames[i].bonesData[j].channelValues[k] << " ";
                }
            }*/
            
        }
        out << endl;
    }

    out.close();
}
//----------------------------------------------------------------------------------
void BvhParser::parse(SkeletalModelPtr model, SkeletalDataPtr data, const std::string& filename ) {
    // tutaj trafiaja jointy w kolejności wystapienia w pliku
    std::list<std::string> jointList;

    ifstream in(filename.c_str(), ios_base::in);
    
    if (!in) {
        throw UnableToOpenFileException(filename);
    }

    std::string line;
    while (in >> line)
    {
        if (line == "HIERARCHY") {
            // rekurencyjne przejście przez cala hierarchie w pliku
            readSingleBone(model, in, jointList);
        } else if (line == "MOTION") {
            // odczyt danych animacji
            int noOfFrames = -1;
            in >> line >> noOfFrames;
            double frameTime = 0.0;
            in >> line >> line >> frameTime;
            data->setFrameTime(frameTime);
            std::vector<SkeletalData::singleFramePtr>& frames = data->getFrames();
            // wartości dla kolejnych klatek
            for (int m=0; m < noOfFrames; ++m) {
                std::getline(in, line);
                SkeletalData::singleFramePtr frame(new SkeletalData::singleFrame);
                frame->frameNo = m + 1;
                std::list<std::string>::iterator it;
                // zgodnie z kolejnością wystapienia jointów w pliku
                for (it = jointList.begin(); it != jointList.end(); ++it) {
                    JointPtr bone = this->tempBonesMap[(*it)];
                    SkeletalData::singleJointStatePtr state(new SkeletalData::singleJointState);
                    state->name = bone->name;
                    int channelsCount = bone->dofs.size();
                    double number;
                    for (int k = 0; k < channelsCount; ++k ) {
                        in >> number;
                        state->channelValues.push_back(number);
                    }
                    frame->jointsData.push_back(state);
                }

                frames.push_back(frame);
            }

        }
    }

    // aby zachować zgodność z acclaim
    JointPtr root = tempBonesMap["root"];
    root->name = "root";
    root->id = -1;
    // utworzenie 'dummy bones' jeśli jest taka potrzeba
    //setBones(model);
    in.close();
}
//----------------------------------------------------------------------------------
kinematic::BvhParser::BvhParser()
:boneCounter(-1),
 forceZXY(true),
 tabs(true)
{

}
//----------------------------------------------------------------------------------
kinematic::BvhParser::~BvhParser()
{

}
//----------------------------------------------------------------------------------
void kinematic::BvhParser::readSingleBone(SkeletalModelPtr model, std::istream& in, std::list<std::string>& jointList ) {
    // dane o kościach na razie trafiaja do tymczasowych struktur (trafia do właściwych po utworzeniu dummy bones)
    SkeletalModel::JointIdMap& bonesIds = tempBonesID;
    SkeletalModel::JointMap& bonesMap = tempBonesMap;
    JointPtr root;
    std::string token;
    int intBuffer;

    in >> token;
    while (token.size() == 0 && (in >> token)) {}
    if (token == "}") {
        tempParentVector.pop_back();
        // koniec rekurencji
        return;
    }

    JointPtr j(new Joint());

    if (boost::iequals(token, "End")) {
        // natrafiono na lisc
        in >> token;
        string parentName = tempParentVector.back();
        tempParentVector.push_back("dummy");

        osg::Vec3d ofs;
        in >> token >> token >> ofs[0] >> ofs[1] >> ofs[2];
        j->length = ofs.normalize();
        j->direction = ofs;
        JointPtr parent = bonesMap[parentName];
        j->parent = parent;
        parent->children.push_back(j);
        
        j->name = parentName + "+effector";
    }
    else
    {
        JointPtr parent;
        if (token == "ROOT") {
            root = j;
        } else {
            // pobranie nazwy parenta ze stosu
            parent = bonesMap[tempParentVector.back()];
        }
        j->parent = parent;

        token = "";
        string name;
        while (in >> token) {
            if (token != "{") {
                name += token + ' ';
            } else {
                break;
            }
        }
        int size = name.size();
        if (size > 0) {
            name.resize(size - 1);
        }

        //LOGGER(Logger::Debug, name);
        if (bonesMap.find(name) != bonesMap.end()) {
            LOGGER(Logger::Warning, "Bone '" + name +  "' already exists! - renaming");
            // istnieje juz taka kość! zmiana nazwy.
            int c = 1;
            std::stringstream newline;
            newline << name << c;
            while (bonesMap.find(newline.str()) != bonesMap.end()) {
                newline.str(std::string());
                newline << name << ++c;
            }
            name = newline.str();
        }
        j->name = name;
        jointList.push_back(name);
        j->id = ++boneCounter;
        bonesMap[j->name] = j;
        bonesIds[j->id] = j;
        
        tempParentVector.push_back( name );
        
        if (parent) {
            parent->children.push_back(j);
        }

        in  >> token;
        osg::Vec3d ofs;
        in >> ofs[0] >> ofs[1] >> ofs[2];
        if (parent == false) {
            model->getSkeleton().setPosition(ofs);
        }
        else {
            j->length = ofs.normalize();
            j->direction = ofs;
        }
        in >> token;
        in >> intBuffer;
        for (int i = 0; i < intBuffer; ++i)
        {
            DegreeOfFreedom dof;
            in >> token;
            dof.channel = DegreeOfFreedom::getChannel(token);
            j ->dofs.push_back(dof);
        }
    }
    // rekurencja
    readSingleBone (model, in, jointList);

    in >> token;

    // po wczytaniu tokena trzeba się cofnąć (żeby nie zaburzyć przetwarzania pliku
    for (int i = token.size() - 1; i >= 0; --i) {
        in.putback(token[i]);
    }
    
    if (token == "MOTION") {
        // zakonczyla się sekcja z hierarchia
        model->getSkeleton().setRootName(root->name);
        // sztywna zmiana nazwy roota (zgodność z acclaim)
        bonesMap.erase(root->name);
        root->name = "root";
        bonesMap[root->name] = root;

        bonesIds.erase(root->id);
        root->id = -1;
        bonesIds[root->id] = root;

        *(jointList.begin()) = "root";
        // wyjscie z rekurencji
        return;
    } else {
        // przetworzenie kolejnego jointa.
        readSingleBone (model, in, jointList);
    }
}
//----------------------------------------------------------------------------------
void kinematic::BvhParser::HandleBone(SkeletalModelPtr model, JointPtr newBone)
{

    JointPtr nullBone;
    SkeletalModel::JointMap& boneMap = model->getJointMap();
    SkeletalModel::JointIdMap& boneIdMap = model->getJointIDMap();
    newBone->id = boneCounter++;
    boneMap[newBone->name] = newBone;
    boneIdMap[newBone->id] = newBone;

    const JointPtr bone = tempBonesMap[newBone->name];
    int size = bone->children.size();
    
    if (size > 1) {
        // tworzenie 'dummy bone'
        std::vector<JointPtr> children = bone->children;
        newBone->length = 0;
        newBone->direction[0] = newBone->direction[1] = newBone->direction[2] = 0.0;
        newBone->children.clear();
        for (int i = 0; i < size ; ++i) {
            JointPtr child(new Joint);
            child->parent = newBone;
            child->name = newBone->name + (char)('0' + i);
            
            child->direction = children[i]->direction;
            child->length = children[i]->length;
            child->id = boneCounter++;
            boneMap[child->name] = child;
            boneIdMap[child->id] = child;
            newBone->children.push_back(child);
            
            JointPtr child2(new Joint);            
			Joint::copyContent(*(children[i]), *child2);
            child2->parent = child;
            child2->children.clear();
            child->children.push_back(child2);
            HandleBone(model, child2);
        }
    } else if (size == 1) {
        // zwykle skopiowanie kości w nowej hierarchii.
        newBone->direction = bone->children[0]->direction;
        newBone->length = bone->children[0]->length;
        if (bone->children[0]->children.size() != 0) {
            JointPtr child(new Joint);
            //*child = *(bone->children[0]);
			Joint::copyContent(*(bone->children[0]), *child);
            child->parent = newBone;
            child->children.clear();
            child->name = bone->children[0]->name;
            newBone->children.push_back(child);
            HandleBone(model, child);
        }
    }
    else
    {
        /*bone->length = 0.0;
        bone->direction[0] = 0.0;
        bone->direction[1] = 0.0;
        bone->direction[2] = 0.00;*/
    }
}
//----------------------------------------------------------------------------------
void kinematic::BvhParser::setBones( SkeletalModelPtr model )
{
    boneCounter = -1;
    JointPtr root = tempBonesMap["root"];
    //model->getSkeleton().setRoot(root);
    JointPtr newRoot(new Joint);
    //(*newRoot) = *root;
	Joint::copyContent(*root, *newRoot);
    newRoot->children.clear();
    // do nowego korzenia trafia kości razem z utworzonymi dummies
    HandleBone(model, newRoot);
    model->getSkeleton().setRoot(newRoot);
    Axis::Order order = getAxisOrder(newRoot);
    model->getSkeleton().setAxis(order);
}
//----------------------------------------------------------------------------------
Axis::Order kinematic::BvhParser::getAxisOrder( JointPtr bone ) const
{
    DegreeOfFreedom::Channel channels[3];
    int count = bone->dofs.size();
    int index = -1;
    for (int i = 0; i < count; ++i) {
        DegreeOfFreedom::Channel c = bone->dofs[i].channel;
        if (c >= DegreeOfFreedom::RX && c <= DegreeOfFreedom::RZ ) {
                channels[++index] = c;
        }
    }

    if (index != 2) {
        throw WrongFileException("Root does not have all degrees of freedom");
    }

    switch (channels[0]) {
        case DegreeOfFreedom::RX:
            if (channels[1] == DegreeOfFreedom::RY && channels[2] == DegreeOfFreedom::RZ) { return Axis::XYZ; }
            if (channels[1] == DegreeOfFreedom::RZ && channels[2] == DegreeOfFreedom::RY) { return Axis::XZY; }
        case DegreeOfFreedom::RY:
            if (channels[1] == DegreeOfFreedom::RX && channels[2] == DegreeOfFreedom::RZ) { return Axis::YXZ; }
            if (channels[1] == DegreeOfFreedom::RZ && channels[2] == DegreeOfFreedom::RX) { return Axis::YZX; }
        case DegreeOfFreedom::RZ:
            if (channels[1] == DegreeOfFreedom::RX && channels[2] == DegreeOfFreedom::RY) { return Axis::ZXY; }
            if (channels[1] == DegreeOfFreedom::RY && channels[2] == DegreeOfFreedom::RX) { return Axis::ZYX; }
    }

    throw WrongFileException("Root has wrong axis order");
}
//----------------------------------------------------------------------------------
