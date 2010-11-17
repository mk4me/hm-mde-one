/********************************************************************************************
*	Files: motionData.cpp
*
*	Some key things to note:
*	There are two major structures in this class
*		1.) a vector of joints 
*		2.) a map of joint names
*	The map can be used to look up a the position of a joint in the joint vector
*   You do this by calling getJointPosition (this is a private function though)
*	
*	This class is the base of the animation window and graph editor.  The animation window
*	uses this class to for getting values of joints for drawing.  The graph editor then
*	uses this class to change values and interpolate values.
*
*	One thing to note, after the graph editor changes points it will call recompute on the
*   quaternions and euler data so that it can be recomputed.
*
*	This class contains our data readers and writers:
*		1.)  Read Acclaim format
*********************************************************************************************/

#include <mbstring.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <windows.h>

#include "ASFAMCParser.h"

// Defines for Hardware Interaction
#define PORTADDRESS 0x378			/* Enter Your Port Address Here */
#define IRQ 7						/* IRQ Here */
#define DATA_ADDRESS PORTADDRESS+0
#define STATUS_ADDRESS PORTADDRESS+1
#define CONTROL_ADDRESS PORTADDRESS+2


#ifndef PI
#define PI 3.1415926536
#endif

ASFAMCParser::ASFAMCParser()
{
    numOfRoots = 0;
    exists = 0;
    floorHeight = 0;
}

ASFAMCParser::~ASFAMCParser()
{
}

int ASFAMCParser::getJointPosition( const char* jointName )
{
    return mappedJointNames[jointName];
}

float ASFAMCParser::getFloorHeight()
{
    return floorHeight;
}


FILE *tempFilePointer ;
std::vector<char*> tempParentVector;



// The following function, through the use of a helper function, loads the Acclaim motion capture format
int ASFAMCParser::readAcclaimFiles(std::string ASFFileName, std::string AMCFileName ) 
{
   

    ReadASFFile(ASFFileName);

    //RemoveDummyJoint();

    ReadAMCFile(AMCFileName);

    exists = 1 ;

    return 1;
}

int ASFAMCParser::ReadASFFile(std::string ASFFileName)
{
    FILE *tempFilePointerASF ;

    //First argument, the ASF file, is opened
    if ((tempFilePointerASF  = fopen(ASFFileName.c_str(), "r")) == NULL)
    {
        printf ("The file %s was not opened\n", ASFFileName) ;
        return 0;
    }
    else
        printf ("The file %s is being processed.\n", ASFFileName) ;

    //Reset the pointer to the beginning of the file...
    fseek (tempFilePointerASF, 0L, SEEK_SET) ;

    //Buffers used for file reading...
    char buffer[10000]    ;

    //Definables used for skeleton construction...
    float globalScale = 0.0f ;
    bool  isDegrees   = true ;


    //Let us first dig through the ASF file and get all the goods...
    //While not at the end of file...
    while (!feof(tempFilePointerASF))
    {
        fscanf (tempFilePointerASF, "%[^:\0]", buffer) ;  //Get the whole chunka' data to work with...
        //printf ("NEW BUFFER\n") ;

        char * token ;
        token = strtok (buffer, " (),\t\n") ;

        // We have now discovered the version section of the asf file
        if (strcmp (token, "version") == 0)
        {
            token = strtok (NULL, " (),\t\n") ;
            if (strcmp (token, "1.1") == 0)
                printf ("Current file version (%s) is of questionable nature...", token) ;
        }
        // Here is the name section of the asf file
        else if (strcmp (token, "name") == 0)
        {
            token = strtok (NULL, " (),\t\n") ;
            printf ("Currently reading ASF skeleton : %s\n", token) ;
        }
        // Here is the units portion of the file
        else if (strcmp (token, "units") == 0)
        {
            token = strtok (NULL, " (),\t\n") ;

            while (token != NULL)
            {
                if (strcmp (token, "mass") == 0)
                {
                    token = strtok (NULL, " (),\t\n") ;
                    //printf ("The MASS is: %s\n", token) ;
                }
                else if (strcmp (token, "length") == 0)
                {
                    token = strtok (NULL, " (),\t\n") ;
                    globalScale = (float)atof(token)  ;
                    //					printf ("The LENGTH is: %f\n", globalScale) ;
                }
                else if (strcmp (token, "angle") == 0)
                {
                    token = strtok (NULL, " (),\t\n") ;
                    if (strcmp (token, "deg") == 0)
                    {
                        isDegrees = true ;
                    }
                    else
                    {
                        isDegrees = false ;
                    }
                    //					printf ("The ANGLES are in degrees : %d\n", isDegrees) ;
                }

                token = strtok (NULL, " (),\t\n") ;
            }			
        }
        // It is safe to ignore everything in the documentation part.
        else if (strcmp (token, "documentation") == 0)
        {
            token = strtok (NULL, " (),\t\n") ;

            while (token != NULL)
            {
                token = strtok (NULL, " (),\t\n") ;
            }
        }
        // We have now entered the root portion of the asf file.
        else if (strcmp (token, "root") == 0)
        {
            int readFlag = 0    ;
            float tempOX, tempOY, tempOZ ; 
            Channel tempJoint             ;

            //Init the necessary root var's
            numOfRoots++      ;
            tempJoint.setType("ROOT") ;													//set the type
            tempJoint.setName("root") ;													//set the name
            tempJoint.setID(0);
            tempJoint.setLength(0.0f);
            //No need to set parent for ROOT...

            token = strtok (NULL, " (),\t\n") ;

            while (token != NULL)
            {
                if (strcmp (token, "axis") == 0)
                {
                    token = strtok (NULL, " (),\t\n") ;
                    //					printf ("The AXES ORDER is: %s\n", token) ;					
                    tempJoint.setRotationOrder( token );
                }
                else if (strcmp (token, "order") == 0)
                {
                    token = strtok (NULL, " (),\t\n") ;
                    while (strlen (token) == 2)									
                    {
                        //						printf ("Root channel names in order : %s\n", token) ;
                        tempJoint.addChannelName(token) ;								//add the channel names
                        token = strtok (NULL, " (),\t\n") ;
                    }

                    readFlag = 1 ;
                }
                else if (strcmp (token, "position") == 0)
                {
                    token = strtok (NULL, " (),\t\n") ;
                    for (int Counter = 0; Counter < 3; Counter++)
                    {
                        //						printf ("An OFFSET is: %s\n", token) ;
                        if (Counter == 0)
                        {
                            tempOX = atof(token);
                            tempJoint.setDirX(tempOX);
                            //      tempJoint.setRootPosition(tempOX);
                        }
                        if (Counter == 1) 
                        {
                            tempOY = atof(token);
                            tempJoint.setDirY(tempOY);
                            //      tempJoint.setRootPosition(tempOY);
                        }
                        if (Counter == 2)
                        {
                            tempOZ = atof(token);
                            tempJoint.setDirZ(tempOZ);
                            //      tempJoint.setRootPosition(tempOZ);
                        }                                                        //HERE IS WHERE I TAKE
                        token = strtok (NULL, " (),\t\n") ;						// ORDER INTO CONSID.
                    }
                    tempJoint.setOffset(tempOX, tempOY, tempOZ) ;						//CHANGE OFFSET FN VARS!

                    readFlag = 1 ;
                }
                else if (strcmp (token, "orientation") == 0)
                {
                    for (int Counter = 0; Counter < 3; Counter++)
                    {
                        //						printf ("The ORIENTATION ORDER is: %s\n", token) ;
                        token = strtok (NULL, " (),\t\n") ;	
                        tempJoint.setASFAxis(atof(token));
                    }
                    readFlag = 1 ;
                }

                if (!readFlag)
                    token = strtok (NULL, " (),\t\n") ;
                readFlag = 0 ;
            }

            joints.push_back(tempJoint) ;
            mappedJointNames[ strdup(tempJoint.getName()) ] = joints.size() - 1 ;			//ADD THE JOINT HERE
        }
        // We have entered the bonedata section of the asf file
        else if (strcmp (token, "bonedata") == 0)
        {
            token = strtok (NULL, " (),\t\n") ;

            while (token != NULL)
            {
                bool isDummyFile = true;

                int readFlag = 0 ;
                int dofNum   = 0 ;
                int id = 0;

                if (strcmp (token, "begin") == 0)
                {
                    Channel tempJoint ;

                    float dirX, dirY, dirZ, length ;

                    token = strtok (NULL, " (),\t\n") ;

                    while (token != NULL)
                    {
                        isDummyFile = true;

                        if (strcmp (token, "id") == 0)
                        {
                            token = strtok (NULL, " (),\t\n") ;
                            id = atoi(token);
                            tempJoint.setID(id);
                            //							printf ("ID is : %s\n", token) ;
                        }
                        else if (strcmp (token, "name") == 0)
                        {
                            token = strtok (NULL, " (),\t\n") ;					
                            //							printf ("NAME is : %s\n", token) ;
                            tempJoint.setName(token) ;									//set the name...
                            tempJoint.setType("JOINT") ;
                        }
                        else if (strcmp (token, "direction") == 0)
                        {
                            token = strtok (NULL, " (),\t\n") ;
                            dirX = atof(token) ;
                            tempJoint.setDirX(dirX);

                            //							printf ("dir X : %f\n", dirX) ;
                            token = strtok (NULL, " (),\t\n") ;
                            dirY = atof(token) ;
                            tempJoint.setDirY(dirY);

                            //							printf ("dir Y : %f\n", dirY) ;
                            token = strtok (NULL, " (),\t\n") ;
                            dirZ = atof(token) ;
                            tempJoint.setDirZ(dirZ);
                            //							printf ("dir Z : %f\n", dirZ) ;
                        }
                        else if (strcmp (token, "length") == 0)
                        {
                            token = strtok (NULL, " (),\t\n") ;
                            length = atof(token) ;
                            tempJoint.setLength(length);
                            //							printf ("LENGTH is : %f\n", len)  ;					//Now that we've got
                            dirX = dirX * length * globalScale ;					// the dirs and len's
                            dirY = dirY * length * globalScale ;					// we will compose and
                            dirZ = dirZ * length * globalScale ;					// add the offset vec.
                            tempJoint.setOffset(dirX, dirY, dirZ) ;
                        }
                        else if (strcmp (token, "axis") == 0)
                        {
                            token = strtok (NULL, " (),\t\n") ;	
                            tempJoint.setASFAxis(atof(token));
                            //							printf ("axs X : %s\n", token) ;					
                            token = strtok (NULL, " (),\t\n") ;		
                            tempJoint.setASFAxis(atof(token));
                            //							printf ("axs Y : %s\n", token) ;					
                            token = strtok (NULL, " (),\t\n") ;
                            tempJoint.setASFAxis(atof(token));
                            //							printf ("axs Z : %s\n", token) ;
                            token = strtok (NULL, " (),\t\n") ;
                            //							printf ("axs order : %s\n", token);
                            tempJoint.setRotationOrder( token );
                        }
                        else if (strcmp (token, "dof") == 0)
                        {
                            token = strtok (NULL, " (),\t\n") ;
                            while (strlen (token) == 2)
                            {
                                tempJoint.addChannelName(token) ;
                                //								printf ("dofVal : %s\n", token) ;
                                dofNum++ ;
                                token = strtok (NULL, " (),\t\n") ;
                            }
                            readFlag = 1 ;

                            isDummyFile = false;
                        }
                        else if (strcmp (token, "limits") == 0)
                        {
                            for (int Counter = 0; Counter < dofNum; Counter++)
                            {
                                token = strtok (NULL, " (),\t\n") ;					//What on earth to
                                //								printf ("lim : %s\n", token) ;						//  do with limits?
                                token = strtok (NULL, " (),\t\n") ;					//What on earth to
                                //								printf ("lim : %s\n", token) ;						//  do with limits?
                            }
                            readFlag = 1 ;
                        }

                        if (!readFlag)
                            token = strtok (NULL, " (),\t\n") ;
                        readFlag = 0 ;
                        if (strcmp (token, "end") == 0)
                            break ;
                    }

                    //  if(isDummyFile)
                    //      tempJoint.setType("DUMMY");

                    joints.push_back(tempJoint) ;
                    mappedJointNames[ strdup(tempJoint.getName()) ] = joints.size() - 1 ;			//ADD THE JOINT HERE
                }

                token = strtok (NULL, " (),\t\n") ;
            }
            delete[] token ;
        }

        // We have entered the hierarchy section of the asf file!
        else if (strcmp (token, "hierarchy") == 0)
        {
            char * hBuffer     ;
            char * hToken      ;
            char * hSubBuffer  ;
            char * hSubCopy    ;
            char * parent      ;
            int readFlag = 0   ;

            //I now have the whole chunk of data...
            //Time to copy it and dig with a new tokenizer...
            token = strtok (NULL, ":\0")   ;
            hBuffer = strdup(token) ;

            //			printf ("t0ken : %s\n", hBuffer) ;

            //			printf ("xxx %s", token) ;

            hToken = strtok (hBuffer, "\n") ;

            while (hToken != NULL)
            {
                int parentFlag = 1 ;
                int readFlag   = 0 ;
                char * nodeName = new char[1000] ;

                hSubBuffer = strdup (hToken)     ;
                hSubCopy   = strdup (hSubBuffer) ;

                //				printf ("%d\n",strlen(hSubBuffer)) ;
                //				printf ("LINE: %s\n", hSubBuffer)  ;

                int length = strlen(hSubBuffer) ;

                for (int Counter = 0; Counter < length; Counter++)
                {
                    //					printf ("%d", i) ;

                    if (hSubBuffer[Counter] == ' ' || hSubBuffer[Counter] == '\t'  
                        || hSubBuffer[Counter] == '(' || hSubBuffer[Counter] == ')' 
                        || hSubBuffer[Counter] == '\n' || hSubBuffer[Counter] == '\0')
                    {
                        if (readFlag != 0)
                        {
                            hSubCopy = hSubCopy + strlen (nodeName) ;
                            readFlag = 0 ;
                        }
                        else
                        {
                            hSubCopy = hSubCopy + 1 ;
                            readFlag = 0 ;
                        }
                    }
                    else if (readFlag == 0)
                    {
                        sscanf (hSubCopy, "%s", nodeName) ;
                        hSubCopy = hSubCopy + 1 ;
                        if (parentFlag == 1 && strcmp(nodeName, "begin") != 0
                            && strcmp(nodeName, "end") != 0)
                        {
                            parent = strdup(nodeName) ;
                            parentFlag = 0 ;
                            //							printf ("The parent node is :%s\n", parent) ;
                        }
                        else if (strcmp(nodeName, "begin") != 0 && strcmp(nodeName, "end") != 0)
                        {
                            //SET UP CHILDREN OF PARENT HERE
                            int jointPosition = getJointPosition( parent ) ;
                            joints[ jointPosition ].addChild(nodeName)     ;

                            jointPosition = getJointPosition( nodeName ) ;
                            joints[ jointPosition ].setParent( parent )      ;

                            //							printf ("Child : %s\n", nodeName) ;
                        }
                        readFlag = 1 ;
                    }
                }

                delete[] nodeName ;
                hToken = strtok (NULL, "\n")         ;
            }

            //          LocalFree (hSubBuffer) ;
            //            LocalFree (hSubCopy) ;
            //      LocalFree (hBuffer) ;
            //        LocalFree (hToken) ;
            //    LocalFree (parent) ;
        }

        else if (strcmp (token, "skin") == 0)
        {
            token = strtok (NULL, " (),\t\n") ;

            while (token != NULL)
            {
                token = strtok (NULL, " (),\t\n") ;
            }
        }


        while (token != NULL)
        {
            token = strtok (NULL, " \t\n") ;
        }

        fscanf (tempFilePointerASF, "%c", buffer)     ;  //Get rid of that \n at the end of the line...

        delete[] token ;
    }

    fclose (tempFilePointerASF) ;

    return 1;
}

int ASFAMCParser::ReadAMCFile(std::string AMCFileName)
{
    FILE *tempFilePointerAMC ;

    //First argument, the AMC file, is opened
    if ((tempFilePointerAMC  = fopen(AMCFileName.c_str(), "r")) == NULL)
    {
        printf ("The file %s was not opened\n", AMCFileName) ;
        return 0;
    }
    else
        printf ("The file %s is being processed.\n", AMCFileName) ;

    fseek (tempFilePointerAMC, 0L, SEEK_SET) ;

    //Buffers used for file reading...
    char buffer[10000]    ;

    //Definables used for skeleton construction...
    float globalScale = 0.0f ;
    bool  isDegrees   = true ;

    //Now it's time to dig through that .AMC file...
    while (!feof(tempFilePointerAMC))
    {
        //int looper = 0 ;
        //int jointPosition ;
        //frameData fd ;

        fscanf (tempFilePointerAMC, "%s", buffer) ;
        //		printf ("%s\n", buffer) ;
        if (feof(tempFilePointerAMC))
        {}
        else if (buffer[0] == '#')
        {
            fscanf (tempFilePointerAMC, "%[^\n]", buffer) ;
            fscanf (tempFilePointerAMC, "%c", buffer) ;
            //			printf ("Got COMMENT\n") ;
        }
        else if (buffer[0] == ':')
        {
            //			printf ("Got Header\n") ;
            fscanf (tempFilePointerAMC, "%[^\n]", buffer) ;
            fscanf (tempFilePointerAMC, "%c", buffer) ;
        }
        else if (buffer[0] >= 48 && buffer[0] <= 57)
        {
            numberOfFrames = atoi(buffer) ;
        }
        else
        {
            //			printf ("Joint Name : %s\n", buffer) ;

            //Get the number of elements for this name...

            int jointPosition = getJointPosition(buffer) ;
            int looper = joints[ jointPosition ].numOfChannels() ;

            float rx, ry, rz, tx, ty, tz;

            if( joints[ jointPosition ].getType() == ROOT )
            {
                //				frameData fd ;
                rx = ry = rz = tx = ty = tz = 0;

                for (int Counter = 0; Counter < looper; Counter++)
                {
                    fscanf (tempFilePointerAMC, "%s", buffer)   ;
                    char *chName = joints[ jointPosition ].getChannelName(Counter);
                    if( _stricmp( chName, "rx" ) == 0 )
                        rx = atof(buffer);
                    else if( _stricmp( chName, "ry" ) == 0 )
                        ry = atof(buffer);
                    else if( _stricmp( chName, "rz" ) == 0 )
                        rz = atof(buffer);
                    else if( _stricmp( chName, "rz" ) == 0 )
                        rz = atof(buffer);
                    else if( _stricmp( chName, "tx" ) == 0 )
                        tx = atof(buffer);
                    else if( _stricmp( chName, "ty" ) == 0 )
                        ty = atof(buffer);
                    else if( _stricmp( chName, "tz" ) == 0 )
                        tz = atof(buffer);
                } 
                joints[ jointPosition ].addFrame( tx, ty, tz, rx, ry, rz ) ;
            }
            else
            {
                rx = ry = rz = 0;

                for (int Counter = 0; Counter< looper; Counter++)
                {
                    fscanf (tempFilePointerAMC, "%s", buffer)   ;
                    char *chName = joints[ jointPosition ].getChannelName(Counter);

                    if( _stricmp( chName, "rx" ) == 0 )
                        rx = atof(buffer);
                    else if( _stricmp( chName, "ry" ) == 0 )
                        ry = atof(buffer);
                    else if( _stricmp( chName, "rz" ) == 0 )
                        rz = atof(buffer);
                } 
                joints[ jointPosition ].addFrame( rx, ry, rz ) ;
            }
        } 
    }

    /*vectorOfJoints::iterator */jointPointer = joints.begin();

    // This adds and buffers the proper data in the joint objects.

    int rx, ry, rz;

    for(int Counter9=0; Counter9<joints.size(); Counter9++)
    {
        if( jointPointer->numOfChannels()<3 )
        {
            rx = ry = rz = 0;
            for(int Counter=0; Counter<jointPointer->numOfChannels(); Counter++)
            {
                char *chName = jointPointer->getChannelName(Counter);

                if( _stricmp( chName, "rx" ) == 0)
                    rx = 1;
                else if( _stricmp( chName, "ry" ) == 0 )
                    ry = 1;
                else if( _stricmp( chName, "rz" ) == 0 )
                    rz = 1;

                //				delete[] chName;
            }
            if( rx == 0 )
                jointPointer->addChannelName( "rx" );
            if( ry == 0 )
                jointPointer->addChannelName( "ry" );
            if( rz == 0 )
                jointPointer->addChannelName( "rz" );
        }			
        jointPointer++;
    }

    fclose (tempFilePointerAMC) ;

    return 1;
}

void ASFAMCParser::RemoveDummyJoint()
{
    vectorOfJoints tempJoints;
    mappedNames tempMappedJointNames;

    Channel* tempParent;
    Channel* tempChild;

    //mappedJointNames[ strdup(tempJoint.getName()) ] = joints.size() - 1 ;	


    // TODO: zoptymalizowac w póŸniejszym czasie. Narazie nie jest to zbyt optymalne bo dwa razy przelatuemy po tablicy
    // no ale niestety obecna architektóra ³adowania z plików acclaime opiera sie na mapie która daje indeks tablicy z której
    // pobieramy odpowiedni joint  - wiêc przypiswanie odpowiednich indeksów do mapy jest szcezgólnie wa¿ne.
    // na potrzeby tego rozw¹zania czyli wywalenia jointów które sa dummy - mamy takie rozwi¹zanie

    // po pierwsze ³adujemy childy z jointow(dummy) do ich rodzicow jako childy
    for(vectorOfJoints::iterator it = joints.begin(); it != joints.end(); it++)
    {
        if(it->getType() == DUMMY)
        {
            std::string parent = it->getParent();

            tempParent = getJoint(parent.c_str());
            tempParent->removeChild(it->getName());

            for(int i = 0; i < it->numOfChildren(); i ++)
            {
                tempChild = getJoint(it->getChild(i));
                tempChild->setParent(parent.c_str());

                tempParent->addChild(tempChild->getName());
            }
        }
    }

    int counter = 0;
    // kopiujemy na nowo tablice
    for(vectorOfJoints::iterator it = joints.begin(); it != joints.end(); it++)
    {
        if(it->getType() != DUMMY)
        {
            it->setID(counter++);

            tempJoints.push_back(*it);
            tempMappedJointNames[strdup((it->getName())) ] = tempJoints.size() - 1;
        }
    }

    // update danych

    joints.clear();
    joints = tempJoints;

    mappedJointNames.clear();
    mappedJointNames = tempMappedJointNames;
}

// This function gives you a joint from a name you should not delete
// the pointer you get to this joint
Channel* ASFAMCParser::getJoint( const char* jointName )
{
    int jointPosition = getJointPosition( jointName );

    vectorOfJoints::iterator _joint_pointer = joints.begin();

    for(int Counter=0; Counter<jointPosition; Counter++)
        _joint_pointer++;

    return &(*_joint_pointer);
}

int ASFAMCParser::doesExist()
{
    return exists;
}

void ASFAMCParser::clearMotionData()
{
    exists = 0;
    mappedJointNames.clear();
    joints.clear();
    numOfRoots = 0;
    numberOfFrames = 0;
    frameTime = 0;
}

int ASFAMCParser::getNumberOfFrames()
{
    return (int)numberOfFrames;
}

int ASFAMCParser::getNumberOfRoots()
{
    return numOfRoots;
}

// This function is used to get a root in case there are multiple roots
char* ASFAMCParser::getRootName(int rootNumber)
{
    for(int Counter=0; Counter<joints.size(); Counter++)
    {
        if( joints[Counter].getType() == ROOT )
        {
            if( rootNumber > 0)
                rootNumber--;
            else if(rootNumber == 0)
                return joints[Counter].getName();
        }
    }
    return NULL;
}


void ASFAMCParser::resetJointPointer()
{
    jointPointer = joints.begin();
}

Channel* ASFAMCParser::getCurrentJointPointer()
{
    jointPointer = joints.begin();

    if(joints.size() > 0)
        return &(*jointPointer);

    return NULL;
}

void ASFAMCParser::advanceJointPointer()
{
    jointPointer++;
}

int ASFAMCParser::getNumberOfJoints()
{
    return joints.size();
}

void ASFAMCParser::setCameraPosition(float* rT)
{
    for (int Counter = 0 ; Counter < 16; Counter++)
    {
        rMatrix[Counter] = rT[Counter] ;
    }
}

float ASFAMCParser::getFrameTime()
{
    return frameTime;
}

void ASFAMCParser::setFrameTime(float ft)
{
    frameTime = ft;
}

vectorOfJoints* ASFAMCParser::getJoints()
{
    return &joints;
}
