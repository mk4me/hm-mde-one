#include "FileChunkReader.h"
#include <core/ModelData.h>

#include "Model.h"
#include "Mesh.h"



//--------------------------------------------------------------------------------------------------
FileChunkReader::FileChunkReader(std::string name):
  m_pCurrentChunk(NULL)
, m_pFileName(name)
, m_pFile(NULL)
, m_pMeshFile(NULL)
{

    // Tworzenie pliku
//    fopen_s(&m_pFile, name,"rb"); // otwieramy plik do czytania
}

//--------------------------------------------------------------------------------------------------
bool FileChunkReader::LoadMesh( Model* model )
{
        m_pMeshFile = NULL;
    
        if (m_pMeshFile = fopen(m_pFileName.c_str(), "rb"))
        {
            // read file header
            short header = 0;
            int fileSize = 0;
            int chunkSize = 0;
    
            BeginChunk(fileSize); 
            ReadNextChunk(header, chunkSize);
    
            do 
            {
                switch (header)
                {
                case FMODEL_MESH:
                    {
                        Mesh* mesh = new Mesh();
    
                        // read material index in material set library
                        ReadInt(mesh->GetMaterialIndex());
                        short chunk_header = 0;
    
                        do 
                        {
                            // EOF - if there isn't any plugin
                            // EOF - if there is nothing more or file terminated by 0
                            if (feof(m_pMeshFile) || (fileSize - ftell(m_pMeshFile) < 2))
                            {			
                                model->AddMesh(mesh);
                                fclose(m_pMeshFile);			
                                return true;
                            }
    
                            int length = 0;
    
                            // read header of chunk
                            ReadNextChunk(chunk_header, length);
    
                            switch (chunk_header)
                            {
                            case FMODEL_STRING:
                                {
                                    length -= HEADER_SIZE;
                                    char* buff = new char [length];
    
                                    if(!ReadString(buff, (SIZES_CHAR_SIZE * length)))
                                    {
                                        delete [] buff; 
                                        delete mesh; 
                                        return false;
                                    }
                                    mesh->SetName(buff);
    
                                    delete [] buff;
                                }
                                break;
    
                            case FMODEL_MESH_BUFFER:
                                {
                                    ReadInt(mesh->GetBufferSize());
    
                                    if (mesh->GetBufferSize() > 0)
                                    {
                                        mesh->SetBuffer(new byte [mesh->GetBufferSize()]);
    
                                        if(!ReadBytes(*mesh->GetBuffer(), mesh->GetBufferSize()))
                                        {
                                            delete mesh;
                                            return false;
                                        }						
                                    }
                                    else
                                    {	
                                        delete mesh; 
                                        fclose(m_pMeshFile);
                                        return false;	
                                    }
    
    
                                    // wczytanie semantyki
                                    if(!ReadInt(mesh->GetSemanticCount()))
                                    {
                                        delete mesh;
                                        return false;
                                    }
    
                                    if (mesh->GetSemanticCount() > 0)
                                    {
                                        mesh->SetSementicVertexFormat(new SVertexElement [mesh->GetSemanticCount()]);
    
                                        // do zrobienia potem - 
                                        if (fread(mesh->GetSementicVertexFormat(), mesh->GetSemanticCount() * TVERT_DATA_SIZE, 1, m_pMeshFile) != 1)
                                        {	
                                            delete mesh; 
                                            fclose(m_pMeshFile); 
                                            return false;	
                                        }	
                                    }
                                    else
                                    {	
                                        delete mesh; 
                                        fclose(m_pMeshFile); 
                                        return false;	
                                    }
                                }
                                break;	
    
                            case FMODEL_MESH_INDICES:
                                {
                                    if(!ReadInt(mesh->GetFaceCount()))
                                    {
                                        delete mesh; 
                                        return false;	
                                    }
                                    if (mesh->GetBufferSize() > 0)
                                    {
                                        mesh->SetFaces(new SFace [mesh->GetFaceCount()]);
    
                                        if(fread(mesh->GetFaces(), mesh->GetFaceCount() * sizeof(SFace), 1, m_pMeshFile) != 1)
                                        {	
                                            delete mesh;
                                            fclose(m_pMeshFile);
                                            return false;	
                                        }							
                                    }
                                    else
                                    {	
                                        delete mesh;
                                        fclose(m_pMeshFile); 
                                        return false;
                                    }
                                }
                                break;
    
                            case FMODEL_MODIFIER:
                                {
                                    if (length >= GUID_SIZE)
                                    {
                                        if(!ReadBytes(*mesh->GetClsid(), GUID_SIZE))
                                        {
                                            delete mesh; 
                                            return false;
                                        }
    
                                        int length = 0;
    
                                        ReadNextChunk(chunk_header, length);
    
                                        switch (chunk_header)
                                        {
                                        case FMODEL_SKIN:
                                            {
                                                mesh->SetSkin(new SSkin());
    
                                                if(!ReadInt(mesh->GetSkin()->n))
                                                {
                                                    delete mesh; 
                                                    return false;
                                                }
    
    
                                                mesh->GetSkin()->skinned_vertices = new SSkinnedVertice [mesh->GetSkin()->n];
    
                                                for (int i = 0; i < mesh->GetSkin()->n; ++i)
                                                {
                                                    if(!ReadInt(mesh->GetSkin()->skinned_vertices[i].vert_id))
                                                    {
                                                        delete mesh; 
                                                        return false;
                                                    }
    
                                                    if(!ReadInt(mesh->GetSkin()->skinned_vertices[i].n))
                                                    {
                                                        delete mesh; 
                                                        return false;
                                                    }
    
                                                    if (mesh->GetSkin()->skinned_vertices[i].n > 0)
                                                    {
                                                        mesh->GetSkin()->skinned_vertices[i].bones = 
                                                            new SAffectingBone [mesh->GetSkin()->skinned_vertices[i].n];
    
                                                        float sum = 0.0f;
                                                        for (int b = 0; b < mesh->GetSkin()->skinned_vertices[i].n; ++b)
                                                        {
                                                            if(!ReadInt(mesh->GetSkin()->skinned_vertices[i].bones[b].boneID))
                                                            {
                                                                delete mesh; 
                                                                return false;
                                                            }
    
                                                            if(!ReadFloat(mesh->GetSkin()->skinned_vertices[i].bones[b].weight))
                                                            {
                                                                delete mesh; 
                                                                return false;
                                                            }    
    
                                                            sum += mesh->GetSkin()->skinned_vertices[i].bones[b].weight;
                                                        }
    
                                                        for (int b = 0; b < mesh->GetSkin()->skinned_vertices[i].n; ++b)
                                                            mesh->GetSkin()->skinned_vertices[i].bones[b].weight /= sum;
                                                    }
                                                }
                                            }
                                            break;
                                        }
                                    }
                                    else
                                    {	
                                        delete mesh; 
                                        fclose(m_pMeshFile); 
                                        return false;	
                                    }
                                }
                                break;
                            case FMODEL_MESH:	// they are allowed...
                            case FMODEL_PLUGIN:
                                break;
    //                         default:
    // 
    //                             // skip
    //                             delete mesh; 
    //                             fclose(m_pMeshFile); 
    //                             return false;	
                            }
                        } 
                        while (chunk_header != FMODEL_MESH && chunk_header != FMODEL_PLUGIN);
    
                        header = chunk_header;
    
                        // push back gathered data				
                        model->AddMesh(mesh);
                    }
                    break;
    
                    case FMODEL_PLUGIN:
                        {
                            byte clsid [16];
    
                            if(!ReadBytes(*clsid, GUID_SIZE))
                            {
                                fclose(m_pMeshFile); 
                                return false;	
                            }
    
                            short chunk_header = 0;
                            int length = 0;
                            
                            // read header of chunk
                            ReadNextChunk(chunk_header, length);
    
                            if (length > 6)
                            {
                                switch (chunk_header)
                                {
                                case FMODEL_BONES:
                                    {
                                        SSkeleton* skeleton = new SSkeleton();
    
                                        if (!LoadSkeleton(skeleton))
                                        {	
                                            delete skeleton; 
                                            fclose(m_pMeshFile); 
                                            return false;
                                        }
    
                                       // assert(!fmesh->skeleton);
                                        model->SetSkeleton(skeleton);
    
                                        // EOF - if there is nothing more or file terminated by 0
                                        if (feof(m_pMeshFile) || (fileSize - ftell(m_pMeshFile) < 2))
                                        {		
                                            fclose(m_pMeshFile);			
                                            return true;
                                        }
    
    
                                        ReadNextChunk(chunk_header, length);
    
                                        header = chunk_header;
                                        chunkSize = length;									
                                    }
                                    break;
                                }
                            }
                        }
                        break;
    
                default:
                    fclose(m_pMeshFile);
                    return false;
                }		
            } 
            while (!feof(m_pMeshFile) && (fileSize - ftell(m_pMeshFile) > 1));
    
            fclose(m_pMeshFile);			
            return true;
        }

    return false;
}

//--------------------------------------------------------------------------------------------------
bool FileChunkReader::BeginChunk(int &fileSize )
{
    short header = 0;
    if (fread(&header, SHORT_INT_SIZE, 1, m_pMeshFile) != 1)
    {
        fclose(m_pMeshFile); 
        return false;	
    }

    if (fread(&fileSize, SIZES_INT_SIZE, 1, m_pMeshFile) != 1)
    {	
        fclose(m_pMeshFile); 
        return false;	
    }
    return true;
}

//--------------------------------------------------------------------------------------------------
bool FileChunkReader::ReadNextChunk( short &header, int &chunkSize )
{
    // read mesh header
    if (fread(&header, SHORT_INT_SIZE, 1, m_pMeshFile) != 1)
    {	
        fclose(m_pMeshFile);
        return false;	
    }

    // chunk size?
    if (fread(&chunkSize, SIZES_INT_SIZE, 1, m_pMeshFile) != 1)
    {	
        fclose(m_pMeshFile);
        return false;	
    }

    return true;
}


//--------------------------------------------------------------------------------------------------
void FileChunkReader::ChangeFile(const std::string name)
{
    fclose(m_pFile);
    m_pFile = fopen(name.c_str(),"rb"); // otwieramy plik do czytania
    if(!m_pFile)
    {
	/* Handle error properly here */
        return;
    }
}


//--------------------------------------------------------------------------------------------------
bool FileChunkReader::ReadInt(int &data )
{
    if (fread(&data, SIZES_INT_SIZE, 1, m_pMeshFile) != 1)
    {	
        fclose(m_pMeshFile); 
        return false;	
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool FileChunkReader::ReadString(char *buff , int size)
{
    if (fread(buff, size, 1, m_pMeshFile) != 1)
    {	
        fclose(m_pMeshFile);
        return false;	
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool FileChunkReader::ReadBytes( byte &data, int size )
{
    if (fread(&data, size, 1, m_pMeshFile) != 1)
    {	
        fclose(m_pMeshFile); 
        return false;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool FileChunkReader::ReadFloat( float &data )
{
    if (fread(&data, SIZES_FLOAT_SIZE, 1, m_pMeshFile) != 1)
    {	
        fclose(m_pMeshFile); 
        return false;	
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool FileChunkReader::ReadShortInt( short int &data )
{
    if (fread(&data, SHORT_INT_SIZE, 1, m_pMeshFile) != 1)
    {	
        fclose(m_pMeshFile); 
        return false;	
    }

    return true;
}

//--------------------------------------------------------------------------------------------------
bool FileChunkReader::LoadSkeleton( SSkeleton* skeleton )
{

    if(!ReadInt(skeleton->bones_count))
    {
        return false;
    }

    if (skeleton->bones_count > 0)
        skeleton->bones = new SBone [skeleton->bones_count];

    for (int b = 0; b < skeleton->bones_count; ++b)
    {	
        short chunk_header = 0;
        int length = 0;

        ReadNextChunk(chunk_header, length);
        if (chunk_header != FMODEL_STRING)
            return false;

        if (length > 6)
        {
            // get name of bone
            length -= HEADER_SIZE;
            char* buff = new char [length];
           
            if(!ReadString(buff, (SIZES_CHAR_SIZE * length)))
            {
                delete [] buff; 
                return false;
            }

            skeleton->bones[b].name = buff;
            delete [] buff;

            // parent id
            if(!ReadInt(skeleton->bones[b].parent_id))
                return false;

            // get transformations
            if (fread(&skeleton->bones[b].translation, SIZES_FLOAT_SIZE * 3, 1, m_pMeshFile) != 1)
                return false;
            if (fread(&skeleton->bones[b].quaternion, SIZES_FLOAT_SIZE * 4, 1, m_pMeshFile) != 1)
                return false;
            if (fread(&skeleton->bones[b].bone_space_trans, SIZES_FLOAT_SIZE * 3, 1, m_pMeshFile) != 1)
                return false;
            if (fread(&skeleton->bones[b].bone_space_quate, SIZES_FLOAT_SIZE * 4, 1, m_pMeshFile) != 1)
                return false;

            // child bones
            if(!ReadInt(skeleton->bones[b].n))
                return false;

            if (skeleton->bones[b].n > 0)
                skeleton->bones[b].child_bone_id = new int [skeleton->bones[b].n];

            for (int cbi = 0; cbi < skeleton->bones[b].n; ++cbi)
                if (fread(&skeleton->bones[b].child_bone_id [cbi], SIZES_INT_SIZE, 1, m_pMeshFile) != 1)
                    return false;

            // another chunk
            if (fread(&chunk_header, SHORT_INT_SIZE, 1, m_pMeshFile) != 1)
                return false;
            if (chunk_header != FMODEL_DOF)
                return false;
            if (fread(&length, SIZES_INT_SIZE, 1, m_pMeshFile) != 1)
                return false;

            // dof
            if (length > 6)
                if (fread(&skeleton->bones[b].dof, sizeof(SDof), 1, m_pMeshFile) != 1)
                    return false;
        }
    }

    return true;
}

bool FileChunkReader::LoadMeshVersion2( SModelData* fmesh )
{
    m_pMeshFile = NULL;

    if (m_pMeshFile = fopen(m_pFileName.c_str(), "rb"))
    {
        // read file header
        short header = 0;
        int fileSize = 0;
        int chunkSize = 0;

        BeginChunk(fileSize); 
        ReadNextChunk(header, chunkSize);

        do 
        {
            switch (header)
            {
            case FMODEL_MESH:
                {
                    SMesh* mesh = new SMesh();

                    // read material index in material set library
                    ReadInt(mesh->i);
                    short chunk_header = 0;

                    do 
                    {
                        // EOF - if there isn't any plugin
                        // EOF - if there is nothing more or file terminated by 0
                        if (feof(m_pMeshFile) || (fileSize - ftell(m_pMeshFile) < 2))
                        {			
                            fmesh->mesh.push_back(mesh);
                            fclose(m_pMeshFile);			
                            return true;
                        }

                        int length = 0;

                        // read header of chunk
                        ReadNextChunk(chunk_header, length);

                        switch (chunk_header)
                        {
                        case FMODEL_STRING:
                            {
                                length -= HEADER_SIZE;
                                char* buff = new char [length];

                                if(!ReadString(buff, (SIZES_CHAR_SIZE * length)))
                                {
                                    delete [] buff; 
                                    delete mesh; 
                                    return false;
                                }
                                mesh->name = buff;

                                delete [] buff;
                            }
                            break;

                        case FMODEL_MESH_BUFFER:
                            {
                                ReadInt(mesh->mesh_buffer->n);

                                if (mesh->mesh_buffer->n > 0)
                                {
                                    mesh->mesh_buffer->buffer = new byte [mesh->mesh_buffer->n];

                                    if(!ReadBytes(*mesh->mesh_buffer->buffer, mesh->mesh_buffer->n))
                                    {
                                        delete mesh;
                                        return false;
                                    }						
                                }
                                else
                                {	
                                    delete mesh; 
                                    fclose(m_pMeshFile);
                                    return false;	
                                }


                                // wczytanie semantyki
                                if(!ReadInt(mesh->mesh_buffer->element_count))
                                {
                                    delete mesh;
                                    return false;
                                }

                                if (mesh->mesh_buffer->element_count > 0)
                                {
                                    mesh->mesh_buffer->vertex_format = new SVertexElement [mesh->mesh_buffer->element_count];

                                    // wczytuje wszystkie wartoœci semantic.
                                    if (fread(mesh->mesh_buffer->vertex_format, mesh->mesh_buffer->element_count * TVERT_DATA_SIZE, 1, m_pMeshFile) != 1)
                                    {	
                                        delete mesh; 
                                        fclose(m_pMeshFile); 
                                        return false;	
                                    }	
                                }
                                else
                                {	
                                    delete mesh; 
                                    fclose(m_pMeshFile); 
                                    return false;	
                                }
                            }
                            break;	

                        case FMODEL_MESH_INDICES:
                            {
                                if(!ReadInt(mesh->mesh_faces->face_count))
                                {
                                    delete mesh; 
                                    return false;	
                                }
                                if (mesh->mesh_buffer->n > 0)
                                {
                                    mesh->mesh_faces->faces = new SFace [mesh->mesh_faces->face_count];

                                    if(fread(mesh->mesh_faces->faces, mesh->mesh_faces->face_count * sizeof(SFace), 1, m_pMeshFile) != 1)
                                    {	
                                        delete mesh;
                                        fclose(m_pMeshFile);
                                        return false;	
                                    }							
                                }
                                else
                                {	
                                    delete mesh;
                                    fclose(m_pMeshFile); 
                                    return false;
                                }
                            }
                            break;

                        case FMODEL_MODIFIER:
                            {
                               // if (length >= GUID_SIZE)
                              //  {
                                    if(!ReadBytes(*mesh->modifier->clsid, GUID_SIZE))
                                    {
                                        delete mesh; 
                                        return false;
                                    }

                                    int length = 0;

                                    ReadNextChunk(chunk_header, length);

                                    switch (chunk_header)
                                    {
                                    case FMODEL_SKIN:
                                        {
                                            mesh->modifier->skin = new SSkin();

                                            if(!ReadInt(mesh->modifier->skin->n))
                                            {
                                                delete mesh; 
                                                return false;
                                            }


                                            mesh->modifier->skin->skinned_vertices = new SSkinnedVertice [mesh->modifier->skin->n];

                                            for (int i = 0; i < mesh->modifier->skin->n; ++i)
                                            {
                                                if(!ReadInt(mesh->modifier->skin->skinned_vertices[i].vert_id))
                                                {
                                                    delete mesh; 
                                                    return false;
                                                }

                                                if(!ReadInt(mesh->modifier->skin->skinned_vertices[i].n))
                                                {
                                                    delete mesh; 
                                                    return false;
                                                }

                                                if (mesh->modifier->skin->skinned_vertices[i].n > 0)
                                                {
                                                    mesh->modifier->skin->skinned_vertices[i].bones = 
                                                        new SAffectingBone [mesh->modifier->skin->skinned_vertices[i].n];


                                                    float sum = 0.0f;
                                                    for (int b = 0; b < mesh->modifier->skin->skinned_vertices[i].n; ++b)
                                                    {
                                                        if(!ReadInt(mesh->modifier->skin->skinned_vertices[i].bones[b].boneID))
                                                        {
                                                            delete mesh; 
                                                            return false;
                                                        }

                                                        if(!ReadFloat(mesh->modifier->skin->skinned_vertices[i].bones[b].weight))
                                                        {
                                                            delete mesh; 
                                                            return false;
                                                        }    

                                                        sum += mesh->modifier->skin->skinned_vertices[i].bones[b].weight;
                                                    }
                                                    if(sum != 1.0f)
                                                    {
                                                        int tmp = 1;
                                                    }

                                                    for (int b = 0; b < mesh->modifier->skin->skinned_vertices[i].n; ++b)
                                                        mesh->modifier->skin->skinned_vertices[i].bones[b].weight /= sum;
                                                }
                                            }
                                        }
                                        break;
                                    }
                              //  }
                               // else
                               // {	
                               //     delete mesh; 
                               //     fclose(m_pMeshFile); 
                               //     return false;	
                               // }
                            }
                            break;
//                         case FMODEL_MESH:	// they are allowed...
//                         case FMODEL_PLUGIN:
//                             break;
//                             //                         default:
//                             // 
//                             //                             // skip
//                             //                             delete mesh; 
//                             //                             fclose(m_pMeshFile); 
//                             //                             return false;	
                        }
                    } 
                    while (chunk_header != FMODEL_MESH && chunk_header != FMODEL_PLUGIN);

                    header = chunk_header;

                    // push back gathered data		
                    
                    fmesh->mesh.push_back(mesh);
                }
                break;

            case FMODEL_PLUGIN:
                {
                    byte clsid [16];

                    if(!ReadBytes(*clsid, GUID_SIZE))
                    {
                        fclose(m_pMeshFile); 
                        return false;	
                    }

                    short chunk_header = 0;
                    int length = 0;

                    // read header of chunk
                    ReadNextChunk(chunk_header, length);

                    if (length > 6)
                    {
                        switch (chunk_header)
                        {
                        case FMODEL_BONES:
                            {
                                SSkeleton* skeleton = new SSkeleton();

                                if (!LoadSkeleton(skeleton))
                                {	
                                    delete skeleton; 
                                    fclose(m_pMeshFile); 
                                    return false;
                                }

                                // assert(!fmesh->skeleton);
                                fmesh->skeleton = skeleton;

                                // EOF - if there is nothing more or file terminated by 0
                                if (feof(m_pMeshFile) || (fileSize - ftell(m_pMeshFile) < 2))
                                {		
                                    fclose(m_pMeshFile);			
                                    return true;
                                }


                                ReadNextChunk(chunk_header, length);

                                header = chunk_header;
                                chunkSize = length;									
                            }
                            break;
                        }
                    }
                }
                break;

            default:
                fclose(m_pMeshFile);


                return false;
            }		
        } 
        while (!feof(m_pMeshFile) && (fileSize - ftell(m_pMeshFile) > 1));

        fclose(m_pMeshFile);			
        return true;
    }

    return false;
}




//TODO: NOWA Wersja Chunk Readera - nied³ugo do poprawy - narazie ma³o istotne.
/*
ChunkFile::ChunkFile()
: m_DataBuffer(NULL),
m_DataBufferSize(0)
{
    ClearHeader();
}

ChunkFile::~ChunkFile()
{
    RemoveAllSubChunks();

    if(m_DataBuffer)
    {
        delete[] m_DataBuffer;
    }
}

EChunkID::TYPE ChunkFile::GetChunkType()
{
    return (EChunkID::TYPE)m_Header.chunkID;
}

EChunkVersion::TYPE ChunkFile::GetChunkVersion()
{
    return (EChunkVersion::TYPE)m_Header.chunkVersion;
}

void ChunkFile::SetChunkType( EChunkID::TYPE type )
{
    m_Header.chunkID = type;
}

void ChunkFile::SetChunkVersion( EChunkVersion::TYPE version )
{
    m_Header.chunkVersion = version;
}

void ChunkFile::Read( IModelStream* stream )
{
    RemoveAllSubChunks();

    ReadHeader(stream);
    ReadData(stream);

    for(UINT i = 0; i < m_Header.subChunkCount; ++i)
    {
        ChunkFile* chunk = new ChunkFile();
        m_SubChunks.push_back(chunk);

        chunk->Read(stream);
    }
}

void ChunkFile::Write( IModelStream* stream )
{
    m_Header.subChunkCount = m_SubChunks.size();
    m_Header.dataSize = m_DataBufferSize;

    WriteHeader(stream);
    WriteData(stream);

    for(UINT i = 0; i < m_SubChunks.size(); ++i)
    {
        m_SubChunks[i]->Write(stream);
    }
}

void ChunkFile::AllocateDataBuffer( UINT size )
{
    if(m_DataBuffer)
    {
        delete[] m_DataBuffer;
        m_DataBuffer = NULL;
    }

    if(size > 0)
    {
        m_DataBuffer = new char[size];
    }

    m_DataBufferSize = size;
}

uint ChunkFile::GetDataBufferSize()
{
    return m_DataBufferSize;
}

void* ChunkFile::GetDataBuffer()
{
    return m_DataBuffer;
}

ChunkFile* ChunkFile::GetSubChunk( UINT index )
{
    return m_SubChunks[index];
}

uint ChunkFile::GetSubChunkCount()
{
    return m_SubChunks.size();
}

void ChunkFile::AddSubChunk( ChunkFile* chunk )
{
    m_SubChunks.push_back(chunk);
}

void ChunkFile::RemoveSubChunk( UINT index )
{
    delete m_SubChunks[index];
    m_SubChunks.erase(m_SubChunks.begin() + index);
}

void ChunkFile::RemoveAllSubChunks()
{
    for(size_t i = 0; i < m_SubChunks.size(); ++i)
    {
        delete m_SubChunks[i];
    }

    m_SubChunks.clear();
}

void ChunkFile::ReadHeader( IModelStream* stream )
{
    // Na wszelki wypadek zerujemy aktulany nag³owek
    ClearHeader();

    // Próbujemy odczytaæ nag³ówek
    if ( stream->Read((VOID*)&m_Header,sizeof(m_Header)) != sizeof(m_Header) )
    {
        // Nie uda³o siê za³adowaæ nag³owka, logujemy info 
        // i zerujemy to co siê odczyta³o
        //LOG_ERROR(3,"B³¹d podczas odczytywania nag³ówka");
        ClearHeader();
        return;
    }

    // Nag³ówek odczytany poprawnie, sprawdzamy czy rodzaj chunka siê zgadza

    switch(m_Header.chunkID)
    {
    case EChunkID::ModelFile:
    case EChunkID::Mesh:
    case EChunkID::VertexFormat:
    case EChunkID::VertexBuffer:
    case EChunkID::IndexBuffer:
    case EChunkID::AABB:
        // Znany rodzaj chunka - kontynuujemy
        break;
    default:
        // B³êdny ID chunka, logujemy info 
        // i zerujemy to co siê odczyta³o
        //LOG_ERROR(3,"B³¹d podczas odczytywania nag³ówka (chunkID)");
        ClearHeader();
    }
}

void ChunkFile::ReadData( IModelStream* stream )
{
    AllocateDataBuffer(m_Header.dataSize);

    if(m_Header.dataSize > 0)
    {
        // Próbujemy odczytaæ dane
        if ( stream->Read(m_DataBuffer, m_Header.dataSize) != m_Header.dataSize )
        {
            // Nie uda³o siê za³adowaæ danych, logujemy info 
            // i zerujemy to co siê odczyta³o
            //LOG_ERROR(3,"B³¹d podczas odczytywania danych");

            AllocateDataBuffer(0);
            return;
        }
    }
}

void ChunkFile::WriteHeader( IModelStream* stream )
{
    if( stream->Write((VOID*)&m_Header, sizeof(m_Header)) != sizeof(m_Header) )
    {
        // Nie uda³o siê zapisaæ nag³owka, logujemy info 
        //LOG_ERROR(3,"B³¹d podczas zapisywania nag³ówka");
        return;
    }
}

void ChunkFile::WriteData( IModelStream* stream )
{
    if(m_DataBufferSize > 0)
    {
        if( stream->Write(m_DataBuffer, m_DataBufferSize) != m_DataBufferSize )
        {
            // Nie uda³o siê zapisaæ danych, logujemy info 
            //LOG_ERROR(3,"B³¹d podczas zapisywania danych");
            return;
        }
    }
}

void ChunkFile::ClearHeader()
{
    m_Header.chunkID = EChunkID::Unknown;
    m_Header.chunkVersion = EChunkVersion::Unknown;
    m_Header.dataSize = 0;
    m_Header.subChunkCount = 0;
}
*/
