#include "FileReader2Motion.h"
#include "FileChunkReader.h"
#include "Model.h"
#include "Mesh.h"

#include "FModel.h"
#include "FAnimation.h"
#include "AnimationNode.h"
#include "AnimationGroup.h"
#include "SkeletonNode.h"

// helper - this name is quite long...
#define pPat osg::PositionAttitudeTransform*

#define POSITION 0
#define NORMALS  1


using namespace std;

FileChunkReader *m_pFileReader;

//--------------------------------------------------------------------------------------------------
void FileReader2Motion::ReadFile( const std::string& file, Model* model )
{
	wstring fmodel(file.begin(), file.end());

	SFModel* fmodel_file = new SFModel(fmodel);
	if (!fmodel_file->properly_loaded)
		return;

	fmodel_file->path = fmodel.substr(0, fmodel.find_last_of(L'/'));

	// path of file containing mesh
	wstring model_name = fmodel_file->path + L'/' + fmodel_file->file_name;

	LoadMesh(&model_name, model);
	model->InicializeMesh();

    if (!LoadSkeleton(model))
        return;

    LoadAnimation(fmodel_file, model);
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::LoadMesh(std::wstring* address, Model* model)
{
	FILE* meshFile = NULL;
	std::string straddress(address->begin(), address->end());

	m_pFileReader = new FileChunkReader(straddress);
	m_pFileReader->LoadMesh(model);

	return true;
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::LoadAnimation( SFModel* fmodel, Model* model )
{
        SFAnimation* animations = new SFAnimation();

        vector<SkeletonNode*> bones;
        CreateArrayHoldingBones(&bones, model);

        for(vector<std::wstring>::iterator i = fmodel->animation_list.begin(); i != fmodel->animation_list.end(); ++i)
        {
            wstring address = fmodel->path + L'/' + *i;

            if(IsSkeletalAnimation(&address) && !bones.empty())
            {
                SSkeletonAnimation* animation = new SSkeletonAnimation();
                if (!LoadSkeletalAnimationFromFile(&address, animation))
                {
                    delete animation;
                    return false;
                }

                animations->m_skeletonAanimation.insert(make_pair<wstring, SSkeletonAnimation*>(*i, animation));
                InicializeSkeletalAnimation(&(*i), animation, &bones);

            }
            else if(IsMeshAnimation(&address))
            {
                // TODO:
                // na razie zakldam, ze nie jest to potrzebne
                // aczkolwiek...
                assert(false);
            }
            else
                return false;
        }

        model->SetAnimation(animations);
        return true;
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::CreateArrayHoldingBones	
(std::vector<SkeletonNode*>* bones, Model* model)
{
    SkeletonNode* skeleteon = NULL;

    // find root bone
    int childNum = model->getNumChildren();
    for (int i = 0; i < childNum; ++i)
    {		
        skeleteon = dynamic_cast<SkeletonNode*>(model->getChild(i));
        if (skeleteon)
            break;
    }
    if (!skeleteon) return false; // no skeleton?!

    // get every bone & put them to list/whatever where they are sorted by id
    struct Obj
    {
        static void createArrayOfBones(SkeletonNode* bone, 
            vector<SkeletonNode*>* bones)
        {
            bones->push_back(bone);
            for (unsigned int b = 0; b < bone->getNumChildren(); ++b)
                createArrayOfBones((SkeletonNode*)bone->getChild(b), bones);
        }
    };
    Obj::createArrayOfBones(skeleteon, bones);

    return true;
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::IsSkeletalAnimation(std::wstring* address)
{
    FILE* file = NULL;

    std::string straddress(address->begin(), address->end());
    if (file = fopen(straddress.c_str(), "rb"))
    {
        int chunk_header = 0;
        int length = 0;

        // another chunk
        if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}
        if (chunk_header != FMD_HEADER)
        {	fclose(file); return false;	}
        if (fread(&length, SIZES_INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}

        if (length > 6)
        {
            // another chunk
            if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}
            if (chunk_header != FMD_PLUGIN)
            {	fclose(file); return false;	}
            if (fread(&length, SIZES_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}

            if (length > 6)
            {
                unsigned char clsid[16];
                if (fread(clsid, 16, 1, file) != 1)			
                {	fclose(file); return false;	}

                if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
                {	fclose(file); return false;	}

                fclose(file);

                if (chunk_header != FMD_SKEL_ANIM)
                    return false;
                else
                    return true;
            }
        }
        fclose(file);		
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
// load skeletal animation from file
bool FileReader2Motion::LoadSkeletalAnimationFromFile(std::wstring* address, SSkeletonAnimation* anim)
{
    FILE* file = NULL;

    std::string straddress(address->begin(), address->end());
    if (file = fopen(straddress.c_str(), "rb"))
    {
        // chunk header
        int chunkHeader = 0;
        int fileSize = 0;

        // FMD
        if (fread(&chunkHeader, SHORT_INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}
        if (fread(&fileSize, SIZES_INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}

        //CLSID + FMD_PLUGIN
        byte* header = new byte [22];
        if (fread(header, 22, 1, file) != 1)			
        {	fclose(file); return false;	}
        delete [] header;

        // n * FMD_SKEL_ANIM
        do 
        {
            int length = 0;

            if (fread(&chunkHeader, SHORT_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}
            if (chunkHeader != FMD_SKEL_ANIM)
            {	fclose(file); return false;	}
            if (fread(&length, SIZES_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}

            // FMD_STRING - animation name
            if (fread(&chunkHeader, SHORT_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}
            if (chunkHeader != FMD_STRING)
            {	fclose(file); return false;	}
            if (fread(&length, SIZES_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}

            length -= HEADER_SIZE;
            char* buff = new char [length];
            if (fread(buff, SIZES_CHAR_SIZE * length, 1, file) != 1)
            {	delete [] buff; fclose(file); return false;	}
            anim->name = string(buff);
            delete [] buff;

            // bones count		
            if (fread(&anim->bones_count, SIZES_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}

            anim->bones = new SSkeletalAnimationBone [anim->bones_count];

            // bones...
            for (int b = 0; b < anim->bones_count; ++b)
            {
                // bone id
                if (fread(&anim->bones[b].bone_id, SIZES_INT_SIZE, 1, file) != 1)
                {	fclose(file); return false;	}

                // key count
                if (fread(&anim->bones[b].n, SIZES_INT_SIZE, 1, file) != 1)
                {	fclose(file); return false;	}

                anim->bones[b].frames = new SKeyFrame [anim->bones[b].n];

                // for every key
                for (int k = 0; k < anim->bones[b].n; ++k)
                {
                    if (fread(&anim->bones[b].frames[k], sizeof(SKeyFrame), 1, file) != 1)
                    {	fclose(file); return false;	}					
                }
            }

        } while (!feof(file) && (fileSize - ftell(file) > 1));

        fclose(file);
        return true;
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
// push animation to OSG
bool FileReader2Motion::InicializeSkeletalAnimation	
(std::wstring* name, SSkeletonAnimation* anim, std::vector<SkeletonNode*>* bones)
{
    std::wstring animationName = name->substr(0, name->find_first_of('.'));

    // for every animated (?) bone...
    for (int b = 0; b < anim->bones_count; ++b)
    {
        // attach new animation to bone
        CAnimationGroup* animation = new CAnimationGroup(anim->bones[b].n);
        animation->setName(string(animationName.begin(), animationName.end()));

        // for every frame
        double animLength = 0.0;
        for (int f = 0; f < anim->bones[b].n; ++f)
        {
            CAnimationNode* frame = new CAnimationNode((double)anim->bones[b].frames[f].time);

            animLength = animLength < anim->bones[b].frames[f].time ? anim->bones[b].frames[f].time : animLength;

            frame->setPosition(osg::Vec3(anim->bones[b].frames[f].trans[0], anim->bones[b].frames[f].trans[1],
                anim->bones[b].frames[f].trans[2]));									// translation
            frame->setAttitude(osg::Quat(anim->bones[b].frames[f].quat[0], anim->bones[b].frames[f].quat[1],
                anim->bones[b].frames[f].quat[2], anim->bones[b].frames[f].quat[3]));	// attitude

            animation->addChild(frame);
        }

        // length of entire animation...

        // Moze sie zdazyc tak i z animacja rozpocznie sie od pewnego momentu - wtedy dlugosc poczatkowych wezlow bedzie rowna zera - moze sie rozsypac program. 
        // TODO:  cos z tym zrobic,  stworzycadtwarzanie animacji uwzgledaniajac ten przyapdek
        //	if (animLength > 0.0)
        //	{
        animation->setLength(animLength);
        (*bones)[anim->bones[b].bone_id]->AddAnimation(animation);	
        //	}
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
// is mesh animation?
bool FileReader2Motion::IsMeshAnimation(std::wstring* address)
{
    FILE* file = NULL;

    std::string straddress(address->begin(), address->end());
    if (file = fopen(straddress.c_str(), "rb"))
    {
        int chunk_header = 0;
        int length = 0;

        // another chunk
        if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}
        if (chunk_header != FMD_HEADER)
        {	fclose(file); return false;	}
        if (fread(&length, SIZES_INT_SIZE, 1, file) != 1)
        {	fclose(file); return false;	}

        if (length > 6)
        {
            // another chunk
            if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}
            if (chunk_header != FMD_PLUGIN)
            {	fclose(file); return false;	}
            if (fread(&length, SIZES_INT_SIZE, 1, file) != 1)
            {	fclose(file); return false;	}

            if (length > 6)
            {
                unsigned char clsid[16];
                if (fread(clsid, 16, 1, file) != 1)			
                {	fclose(file); return false;	}

                if (fread(&chunk_header, SHORT_INT_SIZE, 1, file) != 1)
                {	fclose(file); return false;	}

                fclose(file);

                if (chunk_header != FMD_MESH_ANIM)
                    return false;
                else
                    return true;
            }
        }
        fclose(file);		
    }

    return false;
}

//--------------------------------------------------------------------------------------------------
bool FileReader2Motion::LoadSkeleton(Model* model)
{
    SSkeleton* skeleton = model->GetSkeleton();

    if (model->GetSkeleton() && skeleton->bones_count > 0)
    {
        // skeleton

        //ref_ptr<Group> skeleton_group = new Group();
        //skeleton_group->setName("skeleton");

        osg::ref_ptr<osg::Group> root_bone = new SkeletonNode();
        int roots = 0;

        for (int b = 0; b < skeleton->bones_count; ++b)
        {	
            // load skeleton
            // set parents
            if (skeleton->bones[b].parent_id == -1) // handle root
            {
                skeleton->bones[b].node = root_bone;				
                ++roots;
            }
            else
            {
                skeleton->bones[b].node = new SkeletonNode();

                // i assume that bones are in order defined by their hierarchy
                skeleton->bones[skeleton->bones[b].parent_id].node->addChild(skeleton->bones[b].node);
            }

            // get pos and rot
            osg::Vec3d bpos = osg::Vec3(skeleton->bones[b].translation[0], skeleton->bones[b].translation[1],skeleton->bones[b].translation[2]);
            osg::Quat  brot = osg::Quat(osg::Vec4(skeleton->bones[b].quaternion[0], skeleton->bones[b].quaternion[1], 
                skeleton->bones[b].quaternion[2], skeleton->bones[b].quaternion[3]));

            // translation
            ((pPat)skeleton->bones[b].node.get())->setPosition(bpos);

            // quaternion
            ((pPat)skeleton->bones[b].node.get())->setAttitude(brot);

            // name
            skeleton->bones[b].node.get()->setName(skeleton->bones[b].name);

            // id
            ((SkeletonNode*)skeleton->bones[b].node.get())->SetId(b);
        }

        // imho there should be only one root...
        if (roots != 1)   
            return false;

        //skeleton_group->addChild(root_bone);
        //root->addChild(skeleton_group);
        model->addChild(root_bone);
    }

    return true;
}