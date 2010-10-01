#include "ASFAMCParser.h"


//--------------------------------------------------------------------------------------------------
int ASFAMCParser::readAcclaimFiles( char* ASFFileName, char* AMCFileName )
{
    /*
    FILE *tempFilePointerASF ;
    FILE *tempFilePointerAMC ;

    //First argument, the ASF file, is opened
    if ((tempFilePointerASF  = fopen(ASFFileName, "r")) == NULL)
    {
        printf ("The file %s was not opened\n", ASFFileName) ;
        return 0;
    }
    else
        printf ("The file %s is being processed.\n", ASFFileName) ;

    //First argument, the AMC file, is opened
    if ((tempFilePointerAMC  = fopen(AMCFileName, "r")) == NULL)
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
                int readFlag = 0 ;
                int dofNum   = 0 ;

                if (strcmp (token, "begin") == 0)
                {
                    Channel tempJoint ;

                    float dirX, dirY, dirZ, length ;

                    token = strtok (NULL, " (),\t\n") ;

                    while (token != NULL)
                    {
                        if (strcmp (token, "id") == 0)
                        {
                            token = strtok (NULL, " (),\t\n") ;
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

    vectorOfJoints::iterator jointPointer = joints.begin();

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
            computeEulerMarker( jointPointer, frameNumber, 1 );
            jointPointer->copyEulerMarkersToOriginal();
        }
        jointPointer++;
    }
    glPopMatrix();

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
            computeQuaternionMarker( jointPointer, frameNumber, 1 );
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

    */
    return 1;   
}