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
*	This class also contains our data readers and writers:
*		1.)  Read BVH
*		2.)  Write BVH
*		3.)  Read Acclaim format
*		4.)	 Write Marker data
*		5.)	 Write MEL script
*		6.)	 Write RIB file
*		7.)	 Hardware Input/Output
*
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

// Variables for hardwaredata input
static int portvalue = 0;
static int xpos, xrot;
static int lshoz, lshoy, larm, zpos, lthigh, lknee, ypos;
static int rshoz, rshoy, rarm, zrot, rthigh, rknee, yrot;

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

// The following function, through the use of a recursive helper function, reads the BioVision
// motion data format and initializes all data structures accordingly.
int ASFAMCParser::readBVHfile( char* BVHFileName )
{

    if ((tempFilePointer  = fopen(BVHFileName, "rb")) == NULL)
    {
        printf ("The file %s was not opened\n", BVHFileName) ;
        return 0;
    }
    else
        printf ("The file %s was opened\n", BVHFileName) ;

    //Reset the pointer to the beginning of the file...
    fseek (tempFilePointer, 0L, SEEK_SET) ;

    //While not at the end of file...
    while (!feof(tempFilePointer))
    {
        char buffer[2000] ;

        fscanf (tempFilePointer, "%s", buffer) ;

        //First look to see which header we're dealing with...
        if (strcmp (buffer, "HIERARCHY") == 0)	
        {
            readBVHjoint() ;
        }
        else if (strcmp (buffer, "MOTION") == 0)
        {
            float frameBuffer;			
            fscanf (tempFilePointer, "%s", buffer) ;
            fscanf (tempFilePointer, "%f", &frameBuffer) ;
            numberOfFrames = frameBuffer;

            fscanf (tempFilePointer, "%s", buffer) ;
            fscanf (tempFilePointer, "%s", buffer) ;
            fscanf (tempFilePointer, "%f", &frameBuffer) ;
            frameTime = frameBuffer;

            //Now begins the actual data reading...

            frameBuffer = 0.0f ;

            for(int Counter=0; Counter<numberOfFrames; Counter++)
            {
                for(int Counter2=0; Counter2<joints.size(); Counter2++)
                {
                    frameData frameDataVector;
                    if( joints[Counter2].numOfChannels() > 0 )
                    {
                        for(int Counter3=0; Counter3<joints[Counter2].numOfChannels(); Counter3++)
                        {
                            fscanf (tempFilePointer, "%f", &frameBuffer) ;	//value 1
                            frameDataVector.push_back( frameBuffer );
                        }
                        joints[Counter2].addFrame( frameDataVector );
                    }
                }
            }
        }
    }

    vectorOfJoints::iterator jointPointer = joints.begin();
    for(int Counter4=0; Counter4<joints.size(); Counter4++)
    {
        int limit=0;
        char order[4];
        for(int Counter=0; Counter<jointPointer->numOfChannels(); Counter++)
        {
            if( _stricmp(jointPointer->getChannelName(Counter),"Xrotation")==0 || _stricmp(jointPointer->getChannelName(Counter),"rx")==0 )
            {
                order[limit] = 'X';
                limit++;
            }
            else if( _stricmp(jointPointer->getChannelName(Counter),"Yrotation")==0 || _stricmp(jointPointer->getChannelName(Counter),"ry")==0 )
            {
                order[limit] = 'Y';
                limit++;
            }
            else if( _stricmp(jointPointer->getChannelName(Counter),"Zrotation")==0 || _stricmp(jointPointer->getChannelName(Counter),"rz")==0 )
            {
                order[limit] = 'Z';
                limit++;
            }
        }
        order[3] = '\0';
        jointPointer->setRotationOrder( order );
        jointPointer++;
    }

    // Coimpute Euler markers
    jointPointer = joints.begin();
    glPushMatrix();

    for(int Counter5=0; Counter5<joints.size(); Counter5++)
    {
        for(int frameNumber=1; frameNumber<=numberOfFrames; frameNumber++)
            computeEulerMarker( &(*jointPointer), frameNumber, 1 );
        jointPointer->copyEulerMarkersToOriginal();
        jointPointer++;
    }
    glPopMatrix();

    // Compute Quaternions
    jointPointer = joints.begin();
    for(int Counter6=0; Counter6<joints.size(); Counter6++)
    {
        if( jointPointer->getType() == ROOT || jointPointer->getType() == JOINT)
            jointPointer->computeQuaternions();
        jointPointer++;
    }

    // Compute Quaternion markers
    glPushMatrix();
    jointPointer = joints.begin();

    for(int Counter7=0; Counter7<joints.size(); Counter7++)
    {
        for(int frameNumber=1; frameNumber<=numberOfFrames; frameNumber++)
            computeQuaternionMarker( &(*jointPointer), frameNumber, 1 );
        jointPointer++;
    }
    glPopMatrix();

    fclose (tempFilePointer) ;

    // Compute floor height
    floorHeight = 100000000;
    jointPointer = joints.begin();

    for(int Counter8=0; Counter8<joints.size(); Counter8++)
    {
        float returnValue = jointPointer->smallestYvalueInMarkers();
        jointPointer++;	
        if( floorHeight > returnValue )
            floorHeight = returnValue;
    }			

    exists = 1;

    return 1;
}

void ASFAMCParser::recomputeEulerMarkers()
{
    vectorOfJoints::iterator jointPointer = joints.begin();

    glPushMatrix();
    for(int Counter=0; Counter<joints.size(); Counter++)
    {
        jointPointer->clearEulerMarkers();
        for(int frameNumber=1; frameNumber<=numberOfFrames; frameNumber++)
            computeEulerMarker( &(*jointPointer), frameNumber, 1 );
        jointPointer++;
    }
    glPopMatrix();
}

void ASFAMCParser::recomputeQuaternionMarkers()
{
    vectorOfJoints::iterator jointPointer = joints.begin();

    glPushMatrix();
    for(int Counter=0; Counter<joints.size(); Counter++)
    {
        jointPointer->clearQuaternionMarkers();
        for(int frameNumber=1; frameNumber<=numberOfFrames; frameNumber++)
            computeQuaternionMarker( &(*jointPointer), frameNumber, 1 );
        jointPointer++;
    }
    glPopMatrix();
}

// recursive joint reading function 
void ASFAMCParser::readBVHjoint ()
{
    char  buffer[10000] ;
    float tempBufferf, tempBufferf2, tempBufferf3  ;
    int   tempBufferi    ;

    //Thus begins the loop for nodes...
    fscanf (tempFilePointer, "%s", buffer) ;
    //printf ("The first printf : %s\n", buffer) ;

    //We're breaking recursion now...
    if (strcmp (buffer, "}") == 0)
    {	
        tempParentVector.pop_back();
        //printf ("RETURNING 1\n") ;
        return ;
    }

    Channel tempJoint;

    //If we're at the end site...
    if (_stricmp (buffer, "End") == 0)	
    {
        fscanf (tempFilePointer, "%s", buffer) ;
        //printf ("ONCE : %s\n", buffer) ;

        tempJoint.setType("EFFECTOR");
        tempJoint.setParent( tempParentVector.back() );

        // Create my name with the word Effector before the name of my parents name
        char* newBuffer = new char[ 9 + (strlen(tempJoint.getParent()))];
        strcpy(newBuffer, "Effector");
        strcat(newBuffer, tempJoint.getParent() );
        tempJoint.setName(newBuffer);
        tempParentVector.push_back( strdup(newBuffer) );
        delete[] newBuffer ;

        // Call my parent and set his child to me
        int jointPosition = getJointPosition( tempJoint.getParent() );
        joints[ jointPosition ].addChild( tempJoint.getName() );

        fscanf (tempFilePointer, "%s", buffer) ;
        //printf ("TWICE : %s\n", buffer) ;
        fscanf (tempFilePointer, "%s", buffer) ;
        //printf ("THRICE : %s\n", buffer) ;
        fscanf (tempFilePointer, "%f", &tempBufferf) ;
        //printf ("4 : %f\n", floatBuffer) ;
        fscanf (tempFilePointer, "%f", &tempBufferf2) ;
        //printf ("4 : %f\n", floatBuffer2) ;
        fscanf (tempFilePointer, "%f", &tempBufferf3) ;
        //printf ("4 : %f\n", floatBuffer3) ;
        tempJoint.setOffset( tempBufferf, tempBufferf2, tempBufferf3 );

        if( tempJoint.getType() == ROOT )
            numOfRoots++;
        joints.push_back( tempJoint );
        mappedJointNames[ strdup( tempJoint.getName() ) ] = joints.size() - 1;
        mappedEffectorNames[ strdup(tempJoint.getName()) ] = joints.size() - 1;
    }
    else
    {
        if(strcmp("ROOT", buffer) != 0)
            tempJoint.setParent( tempParentVector.back() );

        tempJoint.setType( buffer );

        fscanf (tempFilePointer, "%s", buffer) ;
        tempJoint.setName( buffer );				//This one is the Joint Name
        //printf ("Joint name : %s\n", buffer) ;

        tempParentVector.push_back( strdup(buffer) );

        if( tempJoint.getType() != ROOT )
        {
            // Call my parent and set his child to me
            int jointPosition = getJointPosition( tempJoint.getParent() );
            joints[ jointPosition ].addChild( tempJoint.getName() );
        }

        fscanf (tempFilePointer, "%s", buffer) ;		//This one is the bracket - Throw away...
        //printf ("Bracket : %s\n", buffer) ;

        fscanf (tempFilePointer, "%s", buffer) ;		//This one is OFFSET word

        fscanf (tempFilePointer, "%f", &tempBufferf) ;//This one is OFFSET x
        fscanf (tempFilePointer, "%f", &tempBufferf2) ;//This one is OFFSET y
        fscanf (tempFilePointer, "%f", &tempBufferf3) ;//This one is OFFSET z
        tempJoint.setOffset( tempBufferf,tempBufferf2, tempBufferf3 );

        fscanf (tempFilePointer, "%s", buffer) ;		//This one is CHANNEL word

        fscanf (tempFilePointer, "%d", &tempBufferi) ; //This one is CHANNEL number

        for (int Counter = 0; Counter < tempBufferi; Counter++)
        {
            fscanf (tempFilePointer, "%s", buffer) ;
            tempJoint.addChannelName( buffer );			//This is one of the CHANNELS

        }

        if( tempJoint.getType() == ROOT )
            numOfRoots++;
        joints.push_back( tempJoint );
        mappedJointNames[ strdup( tempJoint.getName() ) ] = joints.size() - 1;
    }

    //	tempJoint.dump() ;

    //Now we go back up to the top...
    readBVHjoint () ;

    fscanf (tempFilePointer, "%s", buffer) ;

    if (strcmp (buffer, "MOTION") != 0)
    {
        int length = -1 * (strlen(buffer)) ;
        fseek (tempFilePointer, length, SEEK_CUR)   ;
        readBVHjoint () ;
        //printf ("HERE IN OTHER REWIND %d %s\n", len, buffer) ;
    }
    else
    {
        int length = -1 * (strlen(buffer)) ;
        fseek (tempFilePointer, length, SEEK_CUR)   ;
        //printf ("HERE IN MOTION REWIND %d %s\n", len, buffer) ;
        return ;
    }
}


// The following function, through the use of a helper function, loads the Acclaim motion capture format
int ASFAMCParser::readAcclaimFiles(std::string ASFFileName, std::string AMCFileName ) 
{
    FILE *tempFilePointerASF ;
    FILE *tempFilePointerAMC ;

    //First argument, the ASF file, is opened
    if ((tempFilePointerASF  = fopen(ASFFileName.c_str(), "r")) == NULL)
    {
        printf ("The file %s was not opened\n", ASFFileName) ;
        return 0;
    }
    else
        printf ("The file %s is being processed.\n", ASFFileName) ;

    //First argument, the AMC file, is opened
    if ((tempFilePointerAMC  = fopen(AMCFileName.c_str(), "r")) == NULL)
    {
        printf ("The file %s was not opened\n", AMCFileName) ;
        return 0;
    }
    else
        printf ("The file %s is being processed.\n", AMCFileName) ;

    //Reset the pointer to the beginning of the file...
    fseek (tempFilePointerASF, 0L, SEEK_SET) ;
    fseek (tempFilePointerAMC, 0L, SEEK_SET) ;

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
                        if (Counter == 0) tempOX = atof(token) ;
                        if (Counter == 1) tempOY = atof(token) ;
                        if (Counter == 2) tempOZ = atof(token) ;						//HERE IS WHERE I TAKE
                        token = strtok (NULL, " (),\t\n") ;						// ORDER INTO CONSID.
                    }
                    tempJoint.setOffset(tempOX, tempOY, tempOZ) ;						//CHANGE OFFSET FN VARS!

                    readFlag = 1 ;
                }
                else if (strcmp (token, "orientation") == 0)
                {
                    token = strtok (NULL, " (),\t\n") ;
                    for (int Counter = 0; Counter < 3; Counter++)
                    {
                        //						printf ("The ORIENTATION ORDER is: %s\n", token) ;
                        token = strtok (NULL, " (),\t\n") ;	
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
                            //							printf ("dir X : %f\n", dirX) ;
                            token = strtok (NULL, " (),\t\n") ;
                            dirY = atof(token) ;
                            //							printf ("dir Y : %f\n", dirY) ;
                            token = strtok (NULL, " (),\t\n") ;
                            dirZ = atof(token) ;
                            //							printf ("dir Z : %f\n", dirZ) ;
                        }
                        else if (strcmp (token, "length") == 0)
                        {
                            token = strtok (NULL, " (),\t\n") ;
                            length = atof(token) ;
                            //							printf ("LENGTH is : %f\n", len)  ;					//Now that we've got
                            dirX = dirX * length * globalScale ;					// the dirs and len's
                            dirY = dirY * length * globalScale ;					// we will compose and
                            dirZ = dirZ * length * globalScale ;					// add the offset vec.
                            tempJoint.setOffset(dirX, dirY, dirZ) ;
                        }
                        else if (strcmp (token, "axis") == 0)
                        {
                            token = strtok (NULL, " (),\t\n") ;					
                            //							printf ("axs X : %s\n", token) ;					
                            token = strtok (NULL, " (),\t\n") ;					
                            //							printf ("axs Y : %s\n", token) ;					
                            token = strtok (NULL, " (),\t\n") ;
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

                    if(isDummyFile)
                        tempJoint.setType("DUMMY");

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

            LocalFree (hSubBuffer) ;
            LocalFree (hSubCopy) ;
            LocalFree (hBuffer) ;
            LocalFree (hToken) ;
            LocalFree (parent) ;
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

  //  RemoveDummyJoint();

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

    jointPointer = joints.begin();

    // Compute Euler Markers 
    glPushMatrix();
    for(int Counter8=0; Counter8<joints.size(); Counter8++)
    {
        printf ("Computer effecotr for : %s\n", jointPointer->getName()) ;

        for(int frameNumber=1; frameNumber<=numberOfFrames; frameNumber++)
        {
            computeEulerMarker( &(*jointPointer), frameNumber, 1 );
            jointPointer->copyEulerMarkersToOriginal();
        }
        jointPointer++;
    }
    glPopMatrix();

    // ³adowanie rotacji(qwaternions)
    // Compute Quaternions
    jointPointer = joints.begin();

    for(int Counter7=0; Counter7<joints.size(); Counter7++)
    {
        if( jointPointer->getType() == ROOT || jointPointer->getType() == JOINT )
            jointPointer->computeQuaternions();
        jointPointer++;
    }

    // Compute Quaternion markers
    glPushMatrix();
    jointPointer = joints.begin();

    for(int Counter10=0; Counter10<joints.size(); Counter10++)
    {
        for(int frameNumber=1; frameNumber<=numberOfFrames; frameNumber++)
            computeQuaternionMarker( &(*jointPointer), frameNumber, 1 );
        jointPointer++;
    }
    glPopMatrix();

    // Compute floor height
    floorHeight = 100000000;
    jointPointer = joints.begin();

    for(int Counter11=0; Counter11<joints.size(); Counter11++)
    {
        float returnValue = jointPointer->smallestYvalueInMarkers();
        jointPointer++;	
        if( floorHeight > returnValue )
            floorHeight = returnValue;
    }			

    exists = 1 ;

    fclose (tempFilePointerASF) ;
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

// Though currently not working, the following function writes a renderman RIB file
void ASFAMCParser::writeRIBfile (const char * openFileName, const char * outputFileName, int resX, int resY, int startFrame, int endFrame)
{
    //char* openFileName   ;
    //char* outputFileName ;

    FILE *ribStream ;

    //For testing purposes only...
    //outputFileName = "finalRender" ;

    if ((ribStream  = fopen(openFileName, "w")) == NULL)
        printf ("The file %s was not opened\n", openFileName) ;
    else
        printf ("The file %s was opened\n", openFileName) ;


    //Used to buffer frame numbers...
    int bufferLength = 0        ;

    for (int j = 1 ; j < 1000000; j *= 10)
    {
        if (endFrame - startFrame >= (j))
            bufferLength++ ;
    }

    //Begin by writing the necessary comments on the top...
    fprintf (ribStream, "# Original file created by M.H.A.Q.S.\n") ;
    //Filename goes here...
    fprintf (ribStream, "# Output filename %s\n", outputFileName) ;
    //Resolution goes here...
    fprintf (ribStream, "# Output resolution %d x %d\n", resX, resY) ;

    //Here comes the camera stuff...
    fprintf (ribStream, "version 3.03\n") ;
    fprintf (ribStream, "Option \"searchpath\" \"shader\" [\".:../shaders:&\"]\n") ;
    fprintf (ribStream, "\n") ;

    char zeroBuffer [100] ;
    char newBuff [100] ;

    printf ("Writing RIB file...\n") ;

    float tx, ty, tz, tx2, ty2, tz2;  // These are used for getting the translate for each joint

    for (int i = startFrame; i <= endFrame; i++)
    {
        fprintf (ribStream, "FrameBegin %d\n", (i-1)) ;

        //Assemble the proper line to feed to the ribStream - works for 0 buffering...
        zeroBuffer[0] = '\0' ;
        strcat (zeroBuffer, "  Display \"%s.%0") ;
        newBuff[0] = '\0' ;

        _itoa( bufferLength, newBuff, 10) ;

        strcat (zeroBuffer, newBuff) ;
        strcat (zeroBuffer, "d.tif\" \"tiff\" \"rgba\"\n") ;

        //fprintf (ribStream, "  Display \"%s.%03d\" \"file\" \"rgba\"\n", outputFileName, i) ;
        fprintf (ribStream, zeroBuffer, outputFileName, i) ;
        fprintf (ribStream, "  Format %d %d 1\n", resX, resY) ;
        fprintf (ribStream, "  Projection \"perspective\" \"fov\" 40\n") ;		
        //fprintf (ribStream, "  Translate 0 0 %f\n", camTranslate ) ;
        //fprintf (ribStream, "  Rotate %f 1 0 0\n", camRotateX) ;
        //fprintf (ribStream, "  Rotate %f 0 1 0\n", camRotateY) ;
        fprintf (ribStream, "  ConcatTransform [ ") ;
        for (int xx = 0; xx < 16; xx++)
        {
            fprintf (ribStream, "%f ", rMatrix[xx]) ;
        }

        fprintf (ribStream, "]\n") ;

        fprintf (ribStream, "  WorldBegin\n") ;

        // Set up the lights.

        fprintf (ribStream, "    LightSource \"distantlight\" 1  \"intensity\" 0.75  \"from\"  [ 1 1 1 ]  \"to\"  [ 0 0 0 ]  \"lightcolor\"  [ 1 1 1 ]\n") ; 
        fprintf (ribStream, "    LightSource \"ambientlight\" 2  \"intensity\" 0.3\n") ;


        vectorOfJoints::iterator j2 = joints.begin();
        Channel *j;
        for(int m=0; m < getNumberOfJoints(); m++)
        {
            if( j2->getType() != ROOT )
            {
                fprintf (ribStream, "    TransformBegin\n") ;
                //fprintf (ribStream, "    Identity\n") ;
                fprintf (ribStream, "      Color 1 0 0\n")          ; //Hardcode color for now...
                fprintf (ribStream, "      Surface \"matte\"\n") ;

                j2->getEulerMarkerPosition( i, tx2, ty2, tz2 );
                char *parentName = j2->getParent();
                j = getJoint( parentName );
                j->getEulerMarkerPosition( i, tx, ty, tz );
                //				delete[] parentName;

                fprintf (ribStream, "      Translate %f %f %f\n", tx, ty, tz);

                float rotY = (float)(atan((tz2-tz)/(tx2-tx))*180/PI);
                if( (tx2-tx)<0 )
                    rotY = -180.0f - rotY;
                else if( (tx2-tx)>0 )
                    rotY *= -1;
                else if( (tz2-tz) > 0 )
                    rotY = -90;
                else
                    rotY = 90;

                float distanceXZ = (float)sqrt ( (tx2-tx)*(tx2-tx) + (tz2-tz)*(tz2-tz) );
                float rotZ = (float)(atan( (ty2-ty) / distanceXZ )*180/PI);
                if( distanceXZ == 0 )
                    rotZ = 90;

                fprintf (ribStream, "      Rotate %f 0 1 0\n", rotY)      ;
                fprintf (ribStream, "      Rotate %f 0 0 1\n", rotZ)      ;
                fprintf (ribStream, "      Scale 1 0.35 0.35\n")      ;

                float distance = (float)sqrt ( (tx2-tx)*(tx2-tx) + (ty2-ty)*(ty2-ty) + (tz2-tz)*(tz2-tz) );
                fprintf (ribStream, "      Translate %f 0 0\n", distance/2.0f );
                fprintf (ribStream, "      Sphere %f 0 1 360\n", distance/2.0f )   ; //Again, explicit for now...
                fprintf (ribStream, "    TransformEnd\n") ;
            }	
            j2++;
        }
        fprintf (ribStream, "  WorldEnd\n") ;
        fprintf (ribStream, "FrameEnd\n") ;
        fprintf (ribStream, "\n\n") ;
    }

    //delete[] zeroBuffer ;
    //delete[] newBuff    ;

    printf ("Done!\n") ;

    fclose (ribStream) ;
}


// This function writes a marker file according to a marker input file...
void ASFAMCParser::writeMarkerFile(const char* openFileName)
{

    FILE *markerStream ;

    if ((markerStream  = fopen(openFileName, "w")) == NULL)
        printf ("The file %s was not opened\n", openFileName) ;
    else
        printf ("The file %s was opened\n", openFileName) ;


    printf("Writing Marker File...\n") ;

    //Start out with the two requisite tags: KSample and KTime
    fprintf (markerStream, "KSample,") ;
    fprintf (markerStream, "KTime,")   ;

    //Dig through the joint names and print their X, Y, and Z -names- only...
    float timeIncrementer = frameTime  ;
    float timeKeeper      = 0          ;

    vectorOfJoints::iterator jointPtr  ;
    jointPtr = joints.begin() ;	

    for (int i=0; i<joints.size(); i++)
    {
        fprintf(markerStream, "%s-X,",jointPtr->getName());
        fprintf(markerStream, "%s-Y,",jointPtr->getName());
        //Check for EndOfLine Printing...
        if (i + 1 == mappedJointNames.size())
            fprintf(markerStream, "%s-Z\n",jointPtr->getName());
        else
            fprintf(markerStream, "%s-Z,",jointPtr->getName());

        jointPtr++ ;
    }

    //Frame Numbering starts at ONE!
    for (int j = 1; j <= getNumberOfFrames(); j++)
    {
        //Print the frame number first...
        fprintf(markerStream, "%d,", j) ;
        fprintf(markerStream, "%f,", timeKeeper) ;
        timeKeeper += timeIncrementer   ;

        jointPtr = joints.begin() ;
        for (int k=0; k<joints.size(); k++)
        {
            //Gather pertient information...
            float mx, my, mz ;

            jointPtr->getOriginalMarkerPosition(j, mx, my, mz) ;

            //Check for EndOfLine Printing...
            if (k + 1 == mappedJointNames.size())
                fprintf (markerStream, "%f,%f,%f", mx, my, mz) ;
            else
                fprintf (markerStream, "%f,%f,%f,", mx, my, mz) ;

            jointPtr++ ;		
        }

        fprintf(markerStream, "\n") ;
    }

    printf ("Done!\n") ;

    fclose (markerStream) ;
}

// This function writes a MEL script, effectively exporting BVH and Acclaim files to Maya
void ASFAMCParser::writeMelScript(const char * openFileName)
{
    FILE *melStream ;

    //For testing purposes only...
    //outputFileName = "finalRender" ;

    if ((melStream  = fopen(openFileName, "w")) == NULL)
        printf ("The file %s was not opened\n", openFileName) ;
    else
        printf ("The file %s was opened\n", openFileName) ;

    fprintf (melStream, "//Original file created by M.H.A.Q.S.\n") ;
    fprintf (melStream, "//This file will create a maya skeleton heirarchy and animation out of the data\n") ;
    fprintf (melStream, "//  last seen in the motion editor.\n") ;
    fprintf (melStream, "\n") ;

    fprintf (melStream, "//First we set the timerange to insure a proper fit.\n") ;
    fprintf (melStream, "playbackOptions -min 1 -max %d ;\n", (int)numberOfFrames)  ;  //This always exports all of the animation...
    fprintf (melStream, "\n") ;

    for(int i=0; i < getNumberOfRoots(); i++)
    {
        writeMelJoints( getRootName( i ), melStream, NULL );
    }

    printf ("Done writing MEL file\n") ;

    fclose (melStream) ;
}

// This is a helper function used by the write MEL function noted earlier.
// It recursively calls itself to account for every joint.
void ASFAMCParser::writeMelJoints(char *name, FILE *melStream, char *oldName)
{
    Channel *j = getJoint( name );

    printf ("%s\n", name) ;

    fprintf (melStream, "\n\n//Currently writing information for joint %s\n", name) ;
    fprintf (melStream, "\n") ;

    float ox, oy, oz, rx, ry, rz, tx, ty, tz ;
    j->getOffset(ox, oy, oz) ;
    fprintf (melStream, "joint -relative -name %s -position %f %f %f -rotationOrder yxz ;\n", name, ox, oy, oz) ;
    //fprintf (melStream, "sphere -name %s ;\n", name) ;

    for (int fnum=1; fnum <= numberOfFrames; fnum++)
    {
        fprintf (melStream, "currentTime %d ;\n", fnum) ;

        if (j->getType() == ROOT)
        {
            j->getTranslation(fnum, tx, ty, tz) ;
            fprintf (melStream, "setKeyframe -value %f -attribute \"translateX\" %s ;\n", tx, name) ;
            fprintf (melStream, "setKeyframe -value %f -attribute \"translateY\" %s ;\n", ty, name) ;
            fprintf (melStream, "setKeyframe -value %f -attribute \"translateZ\" %s ;\n", tz, name) ;
        }
        if (j->getType() != EFFECTOR)
        {
            j->getRotation(fnum, rx, ry, rz) ;

            fprintf (melStream, "setKeyframe -value %f -attribute \"rotateZ\" %s ;\n", rz, name) ;
            fprintf (melStream, "setKeyframe -value %f -attribute \"rotateX\" %s ;\n", rx, name) ;
            fprintf (melStream, "setKeyframe -value %f -attribute \"rotateY\" %s ;\n", ry, name) ;

            //Works with spheres...
            //j->getMarkerPosition(fnum, rx, ry, rz) ;
            //fprintf (melStream, "setKeyframe -value %f -attribute \"translateX\" %s ;\n", rx, name) ;
            //fprintf (melStream, "setKeyframe -value %f -attribute \"translateY\" %s ;\n", ry, name) ;
            //fprintf (melStream, "setKeyframe -value %f -attribute \"translateZ\" %s ;\n", rz, name) ;
        }
    }

    oldName = strdup(name) ;

    for(int i=0; i < j->numOfChildren(); i++)
    {	
        if( j->getType() != EFFECTOR )
            writeMelJoints( j->getChild( i ), melStream, oldName );
    }

    fprintf (melStream, "pickWalk -d up;\n") ;
}

// This is a function that saves bvh files to the disk.
void ASFAMCParser::writeBVHfile(char * openFileName)
{
    FILE *bvhStream ;

    //For testing purposes only...
    //outputFileName = "finalRender" ;

    if ((bvhStream  = fopen(openFileName, "w")) == NULL)
        printf ("The file %s was not opened\n", openFileName) ;
    else
        printf ("The file %s was opened\n", openFileName) ;

    fprintf (bvhStream, "HIERARCHY\n") ;

    for(int Counter=0; Counter < getNumberOfRoots(); Counter++)
    {
        writeBvhJoints( getRootName( Counter ), bvhStream, 0 );
    }

    fprintf (bvhStream, "MOTION\n") ;
    fprintf (bvhStream, "Frames: %d\n", (int)numberOfFrames) ;
    fprintf (bvhStream, "Frame Time: %f\n", frameTime)  ;

    for(int Counter2=0; Counter2 < getNumberOfRoots(); Counter2++)
    {
        writeBvhData( getRootName( Counter2 ), bvhStream);
    }

    printf ("Done writing BVH file!\n") ;

    fclose (bvhStream) ;
}

// This is a helper (recursive) function used by the write BVH file function.
void ASFAMCParser::writeBvhJoints(char *name, FILE *bvhStream, int tabBuffer)
{
    float ox, oy, oz ;

    Channel *j = getJoint( name );

    writeTabs (bvhStream, tabBuffer) ;
    if (j->getType() == ROOT)
    {
        fprintf (bvhStream, "ROOT ") ;
        fprintf (bvhStream, "%s\n", name) ;
    }
    else if (j->getType() == JOINT)
    {
        fprintf (bvhStream, "JOINT ") ;
        fprintf (bvhStream, "%s\n", name) ;
    }
    else if (j->getType() == EFFECTOR)
    {
        fprintf (bvhStream, "End Site\n") ;
    }

    writeTabs (bvhStream, tabBuffer) ;  fprintf (bvhStream, "{\n") ;
    j->getOffset(ox, oy, oz) ;
    writeTabs (bvhStream, tabBuffer) ;	fprintf (bvhStream, "\tOFFSET\t%f\t%f\t%f\n", ox, oy, oz) ;

    if (j->getType() != EFFECTOR)
    {
        writeTabs (bvhStream, tabBuffer) ;  fprintf (bvhStream, "\tCHANNELS %d ", j->numOfChannels()) ;		//NEED THIS FN...

        for (int k = 0; k < j->numOfChannels(); k++)
        {
            fprintf (bvhStream, "%s ", j->getChannelName(k)) ;
        }

        fprintf (bvhStream, "\n") ;
    }


    for(int i=0; i < j->numOfChildren(); i++)
    {	
        if( j->getType() != EFFECTOR )
            writeBvhJoints( j->getChild( i ), bvhStream, tabBuffer+1);
    }

    writeTabs (bvhStream, tabBuffer) ;	fprintf(bvhStream, "}\n") ;

}

// This is a second (recursive) function that writes the frame data to the bvh file.
void ASFAMCParser::writeBvhData(char *name, FILE *bvhStream)
{
    float tx, ty, tz, rx, ry, rz ;

    for (int Counter2=1; Counter2 <= numberOfFrames; Counter2++)
    {
        Channel *tempJoint = getJoint( name );

        if (tempJoint->getType() == ROOT)
        {
            //printf ("Root stuff\n") ;
            tempJoint->getTranslation(Counter2, tx, ty, tz) ;
            tempJoint->getRotation(Counter2, rx, ry, rz) ;

            fprintf (bvhStream, "%f %f %f ", tx, ty, tz) ;
            fprintf (bvhStream, "%f %f %f ", rz, rx, ry) ;
        }
        else if (tempJoint->getType() == JOINT)
        {
            //printf ("Joint stuff\n") ;
            tempJoint->getRotation(Counter2, rx, ry, rz) ;

            fprintf (bvhStream, "%f %f %f ", rz, rx, ry) ;
        }

        for(int Counter=0; Counter < tempJoint->numOfChildren(); Counter++)
        {	
            if( tempJoint->getType() != EFFECTOR )
                writeBvhDataHelper( tempJoint->getChild( Counter ), bvhStream, Counter2);
        }

        fprintf (bvhStream, "\n") ;
    }
}

// More help is needed to write BVH files, and this provides it.
void ASFAMCParser::writeBvhDataHelper(char *name, FILE *bvhStream, int frameNum)
{
    float tx, ty, tz, rx, ry, rz ;

    Channel *tempJoint = getJoint( name );

    if (tempJoint->getType() == ROOT)
    {
        //printf ("Root stuff\n") ;
        tempJoint->getTranslation(frameNum, tx, ty, tz) ;
        tempJoint->getRotation(frameNum, rx, ry, rz) ;

        fprintf (bvhStream, "%f %f %f ", tx, ty, tz) ;
        fprintf (bvhStream, "%f %f %f ", rz, rx, ry) ;
    }
    else if (tempJoint->getType() == JOINT)
    {
        //printf ("Joint stuff\n") ;
        tempJoint->getRotation(frameNum, rx, ry, rz) ;

        fprintf (bvhStream, "%f %f %f ", rz, rx, ry) ;
    }

    for(int Counter=0; Counter < tempJoint->numOfChildren(); Counter++)
    {	
        if( tempJoint->getType() != EFFECTOR )
            writeBvhDataHelper( tempJoint->getChild( Counter ), bvhStream, frameNum);
    }

}

// This is a function that takes any filestream and sticks the requested number of tabs
// in it current location.
void ASFAMCParser::writeTabs(FILE *fileStream, int tabNum)
{
    for (int Counter = 0; Counter < tabNum; Counter++)
        fprintf (fileStream, "\t") ;
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

/**********************************************************************************
*
*	These next two function computeQuaternionMarker and computeEulerMarker will
*	compute their respective markers for a certain frameNum by calculating the
*	marker position by using the glMatrix stack.  I did this for two reasons,
*	first by using the glMatrix stack it made it alot easier.  Second by using
*	the glMatrix stuff we make good use of the graphics hardware which gives
*	increased performance
*
**********************************************************************************/
void ASFAMCParser::computeQuaternionMarker(Channel *j, int frameNum, int initialCall)
{
    if(j->getType() == EFFECTOR){
        float ox, oy, oz;

        if (initialCall)
            glLoadIdentity();
        int parentPosition = getJointPosition( j->getParent() );
        vectorOfJoints::iterator jointPointer = joints.begin();
        for(int k=0; k<parentPosition; k++)
            jointPointer++;
        computeQuaternionMarker( &(*jointPointer), frameNum );

        j->getOffset( ox, oy, oz );
        glTranslatef( ox, oy, oz );

        if (initialCall)
        {
            float M[16];
            glGetFloatv( GL_MODELVIEW_MATRIX, M );
            j->addQuaternionMarkerPosition( M[12], M[13], M[14] );
        }

    }else if(j->getType() == JOINT){
        float ox, oy, oz;

        if (initialCall)
            glLoadIdentity();
        int parentPosition = getJointPosition( j->getParent() );
        vectorOfJoints::iterator jointPointer = joints.begin();
        for(int k=0; k<parentPosition; k++)
            jointPointer++;
        computeQuaternionMarker( &(*jointPointer), frameNum );

        j->getOffset( ox, oy, oz );
        glTranslatef( ox, oy, oz );

        //		j->getRotation( frameNum, rx, ry, rz );
        //		glRotatef( rz, 0, 0, 1 );
        //		glRotatef( rx, 1, 0, 0 );
        //		glRotatef( ry, 0, 1, 0 );
        float rotM[16];
        int retVal = j->getQuaternionMatrix( frameNum, rotM );
        if( retVal == 1)
            glMultMatrixf( rotM );

        if (initialCall)
        {
            float M[16];
            glGetFloatv( GL_MODELVIEW_MATRIX, M );
            j->addQuaternionMarkerPosition( M[12], M[13], M[14] );
        }

    }else if(j->getType() == ROOT){
        float tx, ty, tz, ox, oy, oz;

        if (initialCall)
            glLoadIdentity();
        j->getTranslation( frameNum, tx, ty, tz );
        glTranslatef( tx, ty, tz );

        j->getOffset( ox, oy, oz );
        glTranslatef( ox, oy, oz );

        //		j->getRotation( frameNum, rx, ry, rz );
        //		glRotatef( rz, 0, 0, 1 );
        //		glRotatef( rx, 1, 0, 0 );
        //		glRotatef( ry, 0, 1, 0 );
        float rotM[16];
        int retVal = j->getQuaternionMatrix( frameNum, rotM );
        if( retVal == 1 )
            glMultMatrixf( rotM );

        if (initialCall)
        {
            float M[16];
            glGetFloatv( GL_MODELVIEW_MATRIX, M );
            j->addQuaternionMarkerPosition( M[12], M[13], M[14] );
        }
    }
}
void ASFAMCParser::computeEulerMarker(Channel *j, int frameNum, int initialCall)
{
    if(j->getType() == EFFECTOR){
        float ox, oy, oz;

        if (initialCall)
            glLoadIdentity();
        int parentPosition = getJointPosition( j->getParent() );
        vectorOfJoints::iterator jointPointer = joints.begin();
        for(int k=0; k<parentPosition; k++)
            jointPointer++;
        computeEulerMarker( &(*jointPointer), frameNum );

        j->getOffset( ox, oy, oz );
        glTranslatef( ox, oy, oz );

        if (initialCall)
        {
            float M[16];
            glGetFloatv( GL_MODELVIEW_MATRIX, M );
            j->addEulerMarkerPosition( M[12], M[13], M[14] );
        }

    }else if(j->getType() == JOINT){
        float rx, ry, rz, ox, oy, oz;

        if (initialCall)
            glLoadIdentity();
        int parentPosition = getJointPosition( j->getParent() );
        vectorOfJoints::iterator jointPointer = joints.begin();
        for(int k=0; k<parentPosition; k++)
            jointPointer++;
        computeEulerMarker( &(*jointPointer), frameNum );

        j->getOffset( ox, oy, oz );
        glTranslatef( ox, oy, oz );

        // Check the return to make sure we got a valid number
        int retVal = j->getRotation( frameNum, rx, ry, rz );
        if( retVal == 1 )
        {
            char *rotOrder = j->getRotationOrder();
            if( strcmp(rotOrder, "ZXY") == 0 ){
                glRotatef( rz, 0, 0, 1 );
                glRotatef( rx, 1, 0, 0 );
                glRotatef( ry, 0, 1, 0 );
            }else if( strcmp(rotOrder, "XYZ") == 0 ){
                glRotatef( rx, 1, 0, 0 );
                glRotatef( ry, 0, 1, 0 );
                glRotatef( rz, 0, 0, 1 );
            }
        }

        if (initialCall)
        {
            float M[16];
            glGetFloatv( GL_MODELVIEW_MATRIX, M );
            j->addEulerMarkerPosition( M[12], M[13], M[14] );
        }

    }else if(j->getType() == ROOT){
        float rx, ry, rz, tx, ty, tz, ox, oy, oz;

        if (initialCall)
            glLoadIdentity();

        j->getTranslation( frameNum, tx, ty, tz );
        glTranslatef( tx, ty, tz );

        j->getOffset( ox, oy, oz );
        glTranslatef( ox, oy, oz );

        // Check the return to make sure we got a valid number
        int retVal = j->getRotation( frameNum, rx, ry, rz );
        if( retVal == 1 )
        {
            char *rotOrder = j->getRotationOrder();
            if( strcmp(rotOrder, "ZXY") == 0 ){
                glRotatef( rz, 0, 0, 1 );
                glRotatef( rx, 1, 0, 0 );
                glRotatef( ry, 0, 1, 0 );
            }else if( strcmp(rotOrder, "XYZ") == 0 ){
                glRotatef( rx, 1, 0, 0 );
                glRotatef( ry, 0, 1, 0 );
                glRotatef( rz, 0, 0, 1 );
            }
        }

        if (initialCall)
        {
            float M[16];
            glGetFloatv( GL_MODELVIEW_MATRIX, M );
            j->addEulerMarkerPosition( M[12], M[13], M[14] );
        }
    }
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

/***************************************************************************************
* 
* These two functions writeMarkersToFile and writeMarkersToFileRecursion are used
* to compute a marker file from an input file
* Input file looks like:
*		name joint x y z
* The user should call the first function writeMarkersToFile with the input filename
* and the output filename and then this function will use the other function to write 
* out the markers
*
***************************************************************************************/
typedef std::vector<char*> markerNames;
typedef std::vector<float> intValues;

void ASFAMCParser::writeMarkersToFile(const char* readFile, const char* writeFile)
{
    int i;
    markerNames mNames;
    intValues xValue, yValue, zValue;

    FILE * fptr = fopen( writeFile, "w" );
    fprintf(fptr, "KSample,KTime,");

    FILE * rFile = fopen( readFile, "rb" );
    char buffer[1000];

    while( !feof( rFile ) )
    {
        int n = fscanf( rFile, "%s", buffer );
        if( n <= 0)
            break;

        fprintf(fptr, "%s-X,%s-Y,%s-Z,",buffer,buffer,buffer);
        fscanf( rFile, "%s", buffer );
        mNames.push_back( strdup( buffer ) );

        float x, y, z;
        fscanf( rFile, "%f", &x );
        xValue.push_back( x );
        fscanf( rFile, "%f", &y );
        yValue.push_back( y );
        fscanf( rFile, "%f", &z );
        zValue.push_back( z );
    }
    fprintf(fptr, "\n");

    glPushMatrix();
    for(i=1; i<=numberOfFrames; i++)
    {
        fprintf(fptr, "%d,%f,",i,i*frameTime);
        for(int k=0; k<mNames.size(); k++)
        {
            Channel *j = getJoint( mNames[k] );
            writeMarkersToFileRecursion(j, i, fptr, xValue[k], yValue[k], zValue[k], 1 );
        }
        fprintf(fptr, "\n");
    }
    glPopMatrix();

    printf("Marker file %s successfully written\n", writeFile);
    fclose( rFile );
    fclose( fptr );
}

void ASFAMCParser::writeMarkersToFileRecursion(Channel *j, int frameNum, FILE* fptr, float offx, float offy, float offz, int initialCall)
{
    if(j->getType() == EFFECTOR){
        float ox, oy, oz;

        if (initialCall)
            glLoadIdentity();
        int parentPosition = getJointPosition( j->getParent() );
        vectorOfJoints::iterator jointPointer = joints.begin();
        for(int k=0; k<parentPosition; k++)
            jointPointer++;
        writeMarkersToFileRecursion( &(*jointPointer), frameNum, fptr );

        j->getOffset( ox, oy, oz );
        glTranslatef( ox, oy, oz );

        if(initialCall)
            glTranslatef( offx, offy, offz );

        if (initialCall)
        {
            float M[16];
            glGetFloatv( GL_MODELVIEW_MATRIX, M );
            fprintf( fptr, "%f %f %f ", M[12], M[13], M[14] );
            //			j->addEulerMarkerPosition( M[12], M[13], M[14] );
        }

    }else if(j->getType() == JOINT){
        float rx, ry, rz, ox, oy, oz;

        if (initialCall)
            glLoadIdentity();
        int parentPosition = getJointPosition( j->getParent() );
        vectorOfJoints::iterator jointPointer = joints.begin();
        for(int k=0; k<parentPosition; k++)
            jointPointer++;

        writeMarkersToFileRecursion( &(*jointPointer), frameNum, fptr );

        j->getOffset( ox, oy, oz );
        glTranslatef( ox, oy, oz );

        // Check the return to make sure we got a valid number
        int retVal = j->getRotation( frameNum, rx, ry, rz );
        if( retVal == 1 )
        {
            char *rotOrder = j->getRotationOrder();

            if( strcmp(rotOrder, "ZXY") == 0 ){
                glRotatef( rz, 0, 0, 1 );
                glRotatef( rx, 1, 0, 0 );
                glRotatef( ry, 0, 1, 0 );
            }else if( strcmp(rotOrder, "XYZ") == 0 ){
                glRotatef( rx, 1, 0, 0 );
                glRotatef( ry, 0, 1, 0 );
                glRotatef( rz, 0, 0, 1 );
            }
        }

        if(initialCall)
            glTranslatef( offx, offy, offz );

        if (initialCall)
        {
            float M[16];
            glGetFloatv( GL_MODELVIEW_MATRIX, M );
            fprintf( fptr, "%f %f %f ", M[12], M[13], M[14] );
            //			j->addEulerMarkerPosition( M[12], M[13], M[14] );
        }

    }else if(j->getType() == ROOT){
        float rx, ry, rz, tx, ty, tz, ox, oy, oz;

        if (initialCall)
            glLoadIdentity();

        j->getTranslation( frameNum, tx, ty, tz );
        glTranslatef( tx, ty, tz );

        j->getOffset( ox, oy, oz );
        glTranslatef( ox, oy, oz );

        if(initialCall)
            glTranslatef( offx, offy, offz );

        // Check the return to make sure we got a valid number
        int retVal = j->getRotation( frameNum, rx, ry, rz );
        if( retVal == 1 )
        {
            char *rotOrder = j->getRotationOrder();

            if( strcmp(rotOrder, "ZXY") == 0 ){
                glRotatef( rz, 0, 0, 1 );
                glRotatef( rx, 1, 0, 0 );
                glRotatef( ry, 0, 1, 0 );
            }else if( strcmp(rotOrder, "XYZ") == 0 ){
                glRotatef( rx, 1, 0, 0 );
                glRotatef( ry, 0, 1, 0 );
                glRotatef( rz, 0, 0, 1 );
            }
        }

        if (initialCall)
        {
            float M[16];
            glGetFloatv( GL_MODELVIEW_MATRIX, M );
            fprintf( fptr, "%f %f %f ", M[12], M[13], M[14] );
            //			j->addEulerMarkerPosition( M[12], M[13], M[14] );
        }
    }
}

void ASFAMCParser::dump()
{
    printf("==========================Dumping motion data=================================\n");
    for(int Counter=0; Counter<joints.size(); Counter++)
        joints[Counter].dump();
    printf("==========================Done dumping motion data============================\n");
}

void ASFAMCParser::dumpMappedNames()
{
    printf("==========================Dumping mapped joint names===========================\n");
    mappedNames::iterator mapPointer = mappedJointNames.begin();
    printf("Map size = %d\n", mappedJointNames.size());

    for(int Counter=0; Counter<mappedJointNames.size(); Counter++)
    {
        printf("ID: %d\tName: %s\n", (*mapPointer).second, (*mapPointer).first);
        mapPointer++;
    }
    printf("==========================Done dumping mapped joint names======================\n");

    printf("==========================Dumping mapped effector names==========================\n");
    mappedNames::iterator mapEffectorPointer = mappedEffectorNames.begin();
    printf("Map size = %d\n", mappedEffectorNames.size());

    for(int Counter2=0; Counter2<mappedEffectorNames.size(); Counter2++)
    {
        printf("ID: %d\tName: %s\n", (*mapEffectorPointer).second, (*mapEffectorPointer).first);
        mapEffectorPointer++;
    }
    printf("==========================Done dumping mapped effector names=====================\n");
}

vectorOfJoints* ASFAMCParser::getJoints()
{
    return &joints;
}