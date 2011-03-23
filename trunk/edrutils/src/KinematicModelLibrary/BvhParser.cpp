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
std::string kinematic::BvhParser::space(int no) const {
    // ugly, na pewno jest jakas funkcja do tego
    string res;
    for (int i = 0; i < no; i++) {
        res += ' ';
    }
    return res;
}
//-------------------------------------------------------------------------------------------
string kinematic::BvhParser::spaceL(int lvl) const {
    // podobno niektore parsery nie potrafia sobie poradzic z plikiem *.bvh
    // jesli bedzie mial wciecia zrobione ze spacji
    if (tabs == false) {
        return space((lvl + 1) * 2);
    } else {
        string tabs;
        for (int i = 0; i < lvl; i++) {
            tabs += '\t';
        }
        return tabs;
    }
}
//----------------------------------------------------------------------------------
SkeletalModel::singleBoneState kinematic::BvhParser::getSingleBoneState( int frameNo, std::string boneName ) const
{
    // jesli dane z konkretnej klatki zawieraja wpis o zadanej kosci
    // to zostaje ona odnaleziona i zwrocona
    vector<SkeletalModel::singleFramePtr>& frames = model->getFrames();
    for (int i = frames[frameNo]->bonesData.size() - 1; i >= 0; --i) {
        if (frames[frameNo]->bonesData[i].name == boneName) {
            return frames[frameNo]->bonesData[i];
        }
    }
    // w przeciwnym razie zwracany jest pusty stan, jest to przydatne jesli wymusza sie
    // aby kazda kosc miala dane z 3 lub 6 kanalow
    SkeletalModel::singleBoneState state;
    state.name = boneName;
    state.channelValues.push_back(0);
    state.channelValues.push_back(0);
    state.channelValues.push_back(0);
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
void kinematic::BvhParser::saveChannelsInHierarchy( std::ostream& out, JointPtr bone, int lvl) const
{
    if (forceZXY) {
        out << spaceL(lvl) << "CHANNELS 3 Zrotation Xrotation Yrotation" << endl;
    } else {
        int count = bone->dofs.size();
        if (count > 0) {
            out << spaceL(lvl) << "CHANNELS " << count;
            for (int i = 0; i < count; i++) {
                out << " " << getChannelName((bone->dofs[i]).channel);
            }
            out << endl;
        }
    }
}
//----------------------------------------------------------------------------------
void kinematic::BvhParser::saveBone (
    std::ostream& out,
    JointPtr bone,
    JointPtr parent,
    int lvl,
    std::list<std::string>& jointList)
    const {
    if (parent && parent->length == 0.0 && bone->children.size() == 1) {
        // jesli przetwarzana kosc to 'dummy bone' to zostanie ona zignorowana przy zapisie do bvh
        JointPtr null;
        saveBone(out, bone->children[0], null, lvl, jointList);
    } else {
        // dodanie kosci do listy z kolejnoscia wystapienia (wazne przy zapisie danych z kanalow)
        jointList.push_back(bone->name);
        int childrenCount = bone->children.size();
        out << spaceL(lvl) << "JOINT " << bone->name << endl;
        out << spaceL(lvl) << "{" << endl;
        lvl++; // zwiekszenie wciecia
        out << spaceL(lvl) << "OFFSET ";
        // kosc rodzica jest potrzebna aby obliczyc dlugosc,
        // dlaczego parent nie jest brany z argumentu funkcji? poniewaz argument moze byc nullem
        // (sztuczka robiona w przypadku gdy parent to dummy bone)
        JointPtr parent = bone->parent.lock();
        osg::Vec3d offset = parent->direction * parent->length * model->getLength();
        out << offset[0] << " " << offset[1] << " " << offset[2] << endl;
        saveChannelsInHierarchy(out, bone, lvl);
        // rekurencja
        for (int i = 0; i < childrenCount; i++) {
            saveBone(out, bone->children[i], bone, lvl, jointList);
        }
        // lisc jest traktowany inaczej
        if (childrenCount == 0) {
            out << spaceL(lvl) << "End Site" << endl;
            out << spaceL(lvl) << "{" << endl;
            out << spaceL(lvl + 1) << "OFFSET ";
            osg::Vec3d offset = bone->direction * bone->length;
            out << offset[0] << " " << offset[1] << " " << offset[2] << endl;
            out << spaceL(lvl) << "}" << endl;
        }

        lvl--;
        out << spaceL(lvl) << "}" << endl;
    }
}
//----------------------------------------------------------------------------------
void BvhParser::save(const SkeletalModelPtr model, const std::string& filename ) {
    // do this->model moga odwolac sie metody pomocnicze
    this->model = model;
    std::ofstream out;
    // tutaj trafiaja jointy zgodnie z kolejnoscia w ktorej sa zapisywane
    std::list<std::string> jointList;
    Skeleton& skeleton = model->getSkeleton();
    out.open(filename.c_str());
    int lvl = 0;
    out << "HIERARCHY" << endl;
    out << "ROOT " << skeleton.getRootName() << endl;
    out << "{" << endl;
    out << spaceL(lvl) << "OFFSET ";
    // pozycja roota w globalnym ukladzie odniesienia
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
    JointPtr root = skeleton.getRoot();
    jointList.push_back(root->name);
    int count = root->children.size();
    // rekurencja
    for (int i = 0; i < count; i++) {
        saveBone(out, root->children[i], root, lvl, jointList);
    }
    out << "}" << endl;

    // zapis danych animacji
    vector<SkeletalModel::singleFramePtr>& frames = model->getFrames();
    out << "MOTION" << endl;
    out << "Frames:\t" << frames.size() << endl;
    out << "Frame Time:\t" << model->getFrameTime() << endl;

    int framesCount = frames.size();
    // dla wszystkich klatek...
    for (int i = 0; i < framesCount; i++) {
        std::list<std::string>::iterator it;
        // zgodnie z kolejnoscia zapisu jointow w pliku...
        for (it = jointList.begin(); it != jointList.end(); it++) {

            SkeletalModel::singleBoneState boneState = getSingleBoneState(i, *it);
            JointPtr bone = model->getJointByName(boneState.name);

            if (DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::L, bone->dofs) != -1) {
                LOGGER(Logger::Warning, "Acclaim 'L' degree of freedom is not supported");
            }
            // nastepuje zapis danych
            if (forceZXY) {
                int index = 0;
                if (hasTranslation(bone->dofs)) {
                    index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TX, bone->dofs);
                    out << " " << (index != -1 ? boneState.channelValues[index] : 0.0);
                    index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TY, bone->dofs);
                    out << " " << (index != -1 ? boneState.channelValues[index] : 0.0);
                    index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::TZ, bone->dofs);
                    out << " " << (index != -1 ? boneState.channelValues[index] : 0.0);
                }
                double rx, ry, rz;
                index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RZ, bone->dofs);
                rz = (index != -1 ? boneState.channelValues[index] : 0.0);
                index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RX, bone->dofs);
                rx = (index != -1 ? boneState.channelValues[index] : 0.0);
                index = DegreeOfFreedom::getChannelIndex(DegreeOfFreedom::RY, bone->dofs);
                ry = (index != -1 ? boneState.channelValues[index] : 0.0);
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
            // todo rozwiazac ten balagan
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
void BvhParser::parse(SkeletalModelPtr model, const std::string& filename ) {
    // tutaj trafiaja jointy w kolejnosci wystapienia w pliku
    std::list<std::string> jointList;
    // metody pomocnicze korzystac moga z this->model
    this->model = model;
    
    ifstream in(filename.c_str(), ios_base::in);
    
    if (in == false) {
        throw UnableToOpenFileException(filename);
    }

    std::string line;
    while (in >> line)
    {
        if (line == "HIERARCHY") {
            // rekurencyjne przejscie przez cala hierarchie w pliku
            readSingleJoint(in, jointList);
        } else if (line == "MOTION") {
            // odczyt danych animacji
            int noOfFrames = -1;
            in >> line >> noOfFrames;
            double frameTime = 0.0;
            in >> line >> line >> frameTime;
            model->setFrameTime(frameTime);
            std::vector<SkeletalModel::singleFramePtr>& frames = model->getFrames();
            // wartosci dla kolejnych klatek
            for (int m=0; m < noOfFrames; m++) {
                std::getline(in, line);
                SkeletalModel::singleFramePtr frame(new SkeletalModel::singleFrame);
                frame->frameNo = m + 1;
                std::list<std::string>::iterator it;
                // zgodnie z kolejnoscia wystapienia jointow w pliku
                for (it = jointList.begin(); it != jointList.end(); it++) {
                    JointPtr bone = this->tempBonesMap[(*it)];
                    SkeletalModel::singleBoneState state;
                    state.name = bone->name;
                    int channelsCount = bone->dofs.size();
                    double number;
                    for (int k = 0; k < channelsCount; k++ ) {
                        in >> number;
                        state.channelValues.push_back(number);
                    }
                    frame->bonesData.push_back(state);
                }

                frames.push_back(frame);
            }

        }
    }

    // aby zachowac zgodnosc z acclaim
    JointPtr root = tempBonesMap["root"];
    root->name = "root";
    root->id = -1;
    // utworzenie 'dummy bones' jesli jest taka potrzeba
    setBones();
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
void kinematic::BvhParser::readSingleJoint( std::istream& in, std::list<std::string>& jointList ) {
    // dane o kosciach na razie trafiaja do tymczasowych struktur (trafia do wlasciwych po utworzeniu dummy bones)
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
            // istnieje juz taka kosc! zmiana nazwy.
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
        for (int i = 0; i < intBuffer; i++)
        {
            DegreeOfFreedom dof;
            in >> token;
            dof.channel = DegreeOfFreedom::getChannel(token);
            j ->dofs.push_back(dof);
        }
    }
    // rekurencja
    readSingleJoint (in, jointList);

    in >> token;

    // po wczytaniu tokena trzeba sie cofnac (zeby nie zaburzyc przetwarzania pliku
    for (int i = token.size() - 1; i >= 0; --i) {
        in.putback(token[i]);
    }
    
    if (token == "MOTION") {
        // zakonczyla sie sekcja z hierarchia
        model->getSkeleton().setRootName(root->name);
        // sztywna zmiana nazwy roota (zgodnosc z acclaim)
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
        readSingleJoint (in, jointList);
    }
}
//----------------------------------------------------------------------------------
void kinematic::BvhParser::HandleBone(JointPtr newBone)
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
        for (int i = 0; i < size ; i++) {
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
            (*child2) = *(children[i]);
            child2->parent = child;
            child2->children.clear();
            child->children.push_back(child2);
            HandleBone(child2);
        }
    } else if (size == 1) {
        // zwykle skopiowanie kosci w nowej hierarchii.
        newBone->direction = bone->children[0]->direction;
        newBone->length = bone->children[0]->length;
        if (bone->children[0]->children.size() != 0) {
            JointPtr child(new Joint);
            *child = *(bone->children[0]);
            child->parent = newBone;
            child->children.clear();
            child->name = bone->children[0]->name;
            newBone->children.push_back(child);
            HandleBone(child);
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
void kinematic::BvhParser::setBones()
{
    boneCounter = -1;
    JointPtr root = tempBonesMap["root"];
    //model->getSkeleton().setRoot(root);
    JointPtr newRoot(new Joint);
    (*newRoot) = *root;
    newRoot->children.clear();
    // do nowego korzenia trafia kosci razem z utworzonymi dummies
    HandleBone(newRoot);
    model->getSkeleton().setRoot(newRoot);
    Axis::Order order = getAxisOrder(newRoot);
    model->getSkeleton().setAxisOrder(order);
}
//----------------------------------------------------------------------------------
Axis::Order kinematic::BvhParser::getAxisOrder( JointPtr bone ) const
{
    DegreeOfFreedom::Channel channels[3];
    int count = bone->dofs.size();
    int index = -1;
    for (int i = 0; i < count; i++) {
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
