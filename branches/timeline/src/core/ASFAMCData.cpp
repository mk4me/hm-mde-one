#include "ASFAMCData.h"
/****************************************************************************************
*	Channel.cpp
*
*	Each joint holds information like:
*			name
*			parent's name
*			all its children's name
*			rotation order
*			lenght of joint (this is the offset)
*			channel names (rotateX, rotateY, rotateZ, translateX, ......)
*			frame data for those channels (these are the euler angles)
*			quaternion data for each channel
*			marker data at every frame for original, euler and quaternions
*
*	Any time you are required to give the values frameNum to a function in this class
*   you will think of 1 as the first frame and you can go to <= total number of frames
*
****************************************************************************************/

#include <stdio.h>
#include <string.h>


#ifndef PI
#define PI (3.1415926535)
#endif

enum QUATERNION_VECTOR { XYZ, JUSTX, JUSTY, JUSTZ };

// Main constructor initializes necessary strings to ""
Channel::Channel()
{
    name = strdup( "" );
    parent = strdup( "" );
    rotationOrder = strdup( "" );
    m_id = 0;
}

// Destructor clears all necessary memory.
Channel::~Channel()
{
    if( name )
        free( name );
    if( parent )
        free( parent );
    if( rotationOrder )
        free( rotationOrder );
    children.clear();
    offset.clear();
    channelNames.clear();
    frames.clear();
    originalMarkers.clear();
    eulerMarkers.clear();
    quaternionMarkers.clear();
    quatV.clear();
}

// Copy constructor does everything necessary.
Channel::Channel(const Channel& tempJoint)
{
    type = tempJoint.type;
    name = strdup( tempJoint.name );
    parent = strdup( tempJoint.parent );
    rotationOrder = strdup( tempJoint.rotationOrder );
    children = tempJoint.children;
    offset = tempJoint.offset;
    channelNames = tempJoint.channelNames;
    frames = tempJoint.frames;
    originalMarkers = tempJoint.originalMarkers;
    eulerMarkers = tempJoint.eulerMarkers;
    quaternionMarkers = tempJoint.quaternionMarkers;
    quatV = tempJoint.quatV;
    m_id = tempJoint.m_id;
}

// This returns the smallest Y value the markers have - this is used to calculate where
// the ground plane should be located in the scene.
float Channel::smallestYvalueInMarkers()
{
    float temp = 1000000000;
    for(int Counter=0; Counter<eulerMarkers.size(); Counter++)
    {
        markerPosition tempMarkerPosition = eulerMarkers[Counter];
        if( temp > tempMarkerPosition[1] )
            temp = tempMarkerPosition[1];
    }
    return temp;
}

// Sets the current joint's rotation order.
void Channel::setRotationOrder(char* order)
{
    if( rotationOrder )
        free( rotationOrder );
    rotationOrder = strdup( order );
}

// Gets the current joint's rotation order.
char* Channel::getRotationOrder()
{
    return rotationOrder;
}

// Sets the Channel type
void Channel::setType(const char* channelType)
{
    if(strcmp(channelType, "JOINT") == 0)
        type = JOINT;
    else if(strcmp(channelType, "EFFECTOR") == 0)
        type = EFFECTOR;
    else if(strcmp(channelType, "ROOT") == 0)
        type = ROOT;
    else if(strcmp(channelType, "DUMMY") == 0)
        type = DUMMY;
    else
        type = NOTYPE;
}

// Simply returns what type the current joint is.
int Channel::getType()
{
    return (int)type;
}

// Gets the current joint's name.
char* Channel::getName()
{
    return name;
}

// Sets the current joint's name.
void Channel::setName(const char* myName)
{
    name = strdup( myName );
}

// Gets the current joint's only parent as a string (name).
char* Channel::getParent()
{
    return parent;
}

// Sets the sole parent of the joint as a string (name).
void Channel::setParent(const char* parentName)
{
    parent = strdup( parentName );
}

// Returns how many children the joint has.
int Channel::numOfChildren()
{	
    return children.size();
}

// Gets the requested child's name as a string.
// Often called after the programmer finds out how many children there are.
char* Channel::getChild(int childNum)
{
    if( childNum < numOfChildren() )
        return children[ childNum ];
    return NULL;
}

// Adds another child to the list of children (as a string).
void Channel::addChild(const char* childName)
{
    children.push_back( strdup( childName ) );
}

//remove child
void Channel::removeChild(const char* childName)
{
    for(childVector::iterator it = children.begin(); it != children.end(); it++)
    {
        if(strcmp(*it, childName) == 0)
        {
            children.erase(it);
            break;
        }
    }
}

// Sets the current joint's offset from its parent.
void Channel::setOffset(float v1, float v2, float v3)
{
    offset.push_back(v1);
    offset.push_back(v2);
    offset.push_back(v3);
}

// Names one of the animation channels - this is kept in proper order according to
// when things are read in.
void Channel::addChannelName(char* channelName)
{
    channelNames.push_back( strdup(channelName) );
}

// Gets the requested channel name as a string.
char* Channel::getChannelName(int index)
{
    return channelNames[ index ];
}

// Returns the number of channels the current joint has.
int Channel::numOfChannels()
{
    return channelNames.size();
}

// Adds a frameData object to the current list of animation frames.
void Channel::addFrame(frameData& frame_data)
{
    frames.push_back( frame_data );
}

// Adds explicit frame data.  This one is used for the ROOT joint.
void Channel::addFrame(float xTranslate, float yTranslate, float zTranslate, float xRotate, float yRotate, float zRotate)
{
    frameData tempFrameData;
    tempFrameData.push_back( xTranslate );
    tempFrameData.push_back( yTranslate );
    tempFrameData.push_back( zTranslate );
    tempFrameData.push_back( xRotate );
    tempFrameData.push_back( yRotate );
    tempFrameData.push_back( zRotate );
    frames.push_back( tempFrameData );
}

// Adds explicit frame data.  This one is used for joints other than the ROOT.
void Channel::addFrame(float xRotate, float yRotate, float zRotate)
{
    frameData tempFrameData;
    tempFrameData.push_back( xRotate );
    tempFrameData.push_back( yRotate );
    tempFrameData.push_back( zRotate );
    frames.push_back( tempFrameData );
}

// Gets the current joint's rotation for the requested frame by reference.
int Channel::getRotation(int frameNum, float &xRotate, float &yRotate, float &zRotate)
{
    if( frameNum-1 >= frames.size() )
    {
        xRotate = yRotate = zRotate = 0;
        return 0;
    }
    frameData tempFrameData = frames[ frameNum-1 ];
    if(tempFrameData.size() == 6)
    {
        xRotate = tempFrameData[4];
        yRotate = tempFrameData[5];
        zRotate = tempFrameData[3];
    }
    else if(tempFrameData.size() == 3)
    {
        xRotate = tempFrameData[1];
        yRotate = tempFrameData[2];
        zRotate = tempFrameData[0];	
    }
    else
    {
        printf("encountered an error, trying to read bad ROTATION frame data at frame %d\n", frameNum);
        return -1;
    }
    return 1;
}

// Sets the current joint's rotation for the assigned frame.
void Channel::setRotation(int frameNum, float xRotate, float yRotate, float zRotate)
{
    frameVectors::iterator framesPointer = frames.begin();

    for(int Counter=1; Counter<frameNum; Counter++)
        framesPointer++;

    frameData tempFrameData = frames[ frameNum-1 ];
    if(tempFrameData.size() == 6)
    {
        tempFrameData[4] = xRotate;
        tempFrameData[5] = yRotate;
        tempFrameData[3] = zRotate;
    }
    else if(tempFrameData.size() == 3)
    {
        tempFrameData[1] = xRotate;
        tempFrameData[2] = yRotate;
        tempFrameData[0] = zRotate;	
    }
    else
    {
        printf("Encountered an error, trying to write bad ROTATION frame data at frame %d\n", frameNum);
    }

    framesPointer = frames.erase( framesPointer );
    frames.insert( framesPointer, tempFrameData );
}

// Gets the requested frame's translation values
void Channel::getTranslation(int frameNum, float &xTranslate, float &yTranslate, float &zTranslate)
{
// TODO: R.Zowal
//     if(frames.size() < 1)
//     {
//         xTranslate = offset[0];
//         yTranslate = offset[1];
//         zTranslate = offset[2];
//         return;
//     }

    frameData tempFrameData = frames[ frameNum-1 ];
 //   if(tempFrameData.size() == 6){
        xTranslate = tempFrameData[0];
        yTranslate = tempFrameData[1];
        zTranslate = tempFrameData[2];
 //   }
 //   else
//    {
 //       printf("encountered an error, trying to read bad TRANSLATION frame data at frame %d\n", frameNum);
 //   }
}

// Gets the joint's offset from its parent.
// Data is returned by reference floats.
void Channel::getOffset(float &xOffset, float &yOffset, float &zOffset)
{
    xOffset = offset[0];
    yOffset = offset[1];
    zOffset = offset[2];
}

// Gets the joint's offset from its parent.
// Data is returned by an offset vector.
offsetVector Channel::getOffset()
{
    return offset;
}

// This is used for the origional set of data and is executed once at startup.
void Channel::copyEulerMarkersToOriginal()
{
    originalMarkers = eulerMarkers;
}

// This is used to retrieve the origional data's marker positions.
void Channel::getOriginalMarkerPosition( int frameNum, float &xMarker, float &yMarker, float &zMarker )
{
    markerPosition markerPos = originalMarkers[frameNum - 1];
    xMarker = markerPos[0];
    yMarker = markerPos[1];
    zMarker = markerPos[2];
}

// This is used to add euler angle marker positions to a joint.
void Channel::addEulerMarkerPosition( float xMarker, float yMarker, float zMarker )
{
    markerPosition markerPos;
    markerPos.push_back(xMarker);
    markerPos.push_back(yMarker);
    markerPos.push_back(zMarker);
    eulerMarkers.push_back( markerPos );
}

// This is used to gather euler angle marker positions from a joint.
void Channel::getEulerMarkerPosition( int frameNum, float &xMarker, float &yMarker, float &zMarker )
{
    markerPosition markerPos = eulerMarkers[frameNum - 1];
    xMarker = markerPos[0];
    yMarker = markerPos[1];
    zMarker = markerPos[2];
}

// This is used to add quaternion angle marker positions to a joint.
void Channel::addQuaternionMarkerPosition( float xMarker, float yMarker, float zMarker )
{
    markerPosition markerPos;
    markerPos.push_back(xMarker);
    markerPos.push_back(yMarker);
    markerPos.push_back(zMarker);
    quaternionMarkers.push_back( markerPos );
}

// This is used to add quaternion angle marker positions to a joint.
void Channel::getQuaternionMarkerPosition( int frameNum, float &xMarker, float &yMarker, float &zMarker )
{
    markerPosition markerPos = quaternionMarkers[frameNum - 1];
    xMarker = markerPos[0];
    yMarker = markerPos[1];
    zMarker = markerPos[2];
}

// This function computes the Quaternions for the entire set of Euler angle rotations.
void Channel::computeQuaternions()
{
    Quaternion tempQuaternion; 
    float xRotate, yRotate, zRotate;

    for(int Counter=1; Counter<=(int)frames.size(); Counter++)
    {
        getRotation( Counter, xRotate, yRotate, zRotate );

        xRotate = xRotate * PI / 180;
        yRotate = yRotate * PI / 180;
        zRotate = zRotate * PI / 180;

        tempQuaternion.fromEulerAngles( xRotate, yRotate, zRotate );
        quatV.push_back( tempQuaternion);
    }

    //TODO: R.Zowal zminic to - acclaim nie 
    if((int)frames.size() < 1)
    {
        quatV.push_back(Quaternion(1.0f, 0.0f, 0.0f, 0.0f));
    }
}

// This function returns a 4x4 matrix of quaternion rotations.
int Channel::getQuaternionMatrix( int frameNum, float *tempMatrix )
{
    if( frameNum-1 >= frames.size() )
        return 0;
    quatV[ frameNum-1 ].getRotationMatrix( tempMatrix );
    return 1;
}

// This function returns the angle and axis of a quaternion representation by reference.
void Channel::getAngleAndAxis( int frameNum, float &angle, float &x, float &y, float &z )
{
    //TODO: pprawic jakby co
    if(quatV.size() > frameNum)
        quatV[ frameNum-1 ].getAngleAndAxis( angle, x, y, z );
    else
        quatV[0].getAngleAndAxis(angle, x, y, z);
}

// This interpolates the rotateX values of the Euler data.
void Channel::interpolateRotateX( int start, int stop )
{
    interpolate( start, stop, 1 );
}

// This interpolates the rotateY values of the Euler data.
void Channel::interpolateRotateY( int start, int stop )
{
    interpolate( start, stop, 2 );
}

// This interpolates the rotateZ values of the Euler data.
void Channel::interpolateRotateZ( int start, int stop )
{
    interpolate( start, stop, 0 );
}

// This is the workhorse that actually re-assigns newly-interpolated values to the frames between
// the two requested interpolation points.
void Channel::interpolate( int start, int stop, int channelPos )
{
    // user thinks of frame 1 as start but I use frame 0 so 
    // subtract 1 from start and stop
    start--;
    stop--;
    int frameDataXPosition;
    if( numOfChannels() == 6)
        frameDataXPosition = 3 + channelPos;
    else if( numOfChannels() == 3 )
        frameDataXPosition = channelPos;
    else{
        printf("Bad number of channels for joint %s in trying to interpolate X-rotation\n",getName());
        return;
    }

    float startX, stopX;
    frameData tempFrameDataStart, tempFrameDataStop;

    // If user gave me start = -1 then use first frame, but it is actually just copying stop frame to all the frames in between
    if( start == -1 )
        tempFrameDataStart = frames[ 0 ];
    else	
        tempFrameDataStart = frames[ start ];

    startX = tempFrameDataStart[ frameDataXPosition ];
    tempFrameDataStop = frames[ stop ];
    stopX = tempFrameDataStop[ frameDataXPosition ];

    frameVectors::iterator frameIterator = frames.begin();
    for(int ICounter=0; ICounter<=start; ICounter++)
    {
        frameIterator++;
    }

    for(int Counter=start+1; Counter<stop; Counter++)
    {
        frameData tempFrameData = frames[Counter];
        if( start == -1 )
            tempFrameData[ frameDataXPosition ] = stopX;
        else
            tempFrameData[ frameDataXPosition ] = (stopX-startX) * ((Counter - start)/float(stop-start)) + startX;
        frames.insert( frameIterator, tempFrameData );
        frameIterator++;
        frameIterator = frames.erase( frameIterator );
    }
}	


// This is the workhorse that actually re-assigns newly-interpolated values to the frames between
// the two requested quaternion interpolation points.
void Channel::interpolateQuaternion( int start, int stop )
{
    // user thinks of frame 1 as start but I use frame 0 so 
    // subtract 1 from start and stop
    start--;
    stop--;

    Quaternion to, from, result;

    // If user gave me start==0 - 1 then use first frame, but it is actually just copying stop frame to all the frames in between
    if( start != -1 )
        from = quatV[ start ];
    to = quatV[ stop ];

    quaternionVector::iterator quaternionIterator = quatV.begin();
    for(int ICounter=0; ICounter<=start; ICounter++)
    {
        quaternionIterator++;
    }

    for(int Counter=start+1; Counter<stop; Counter++)
    {
        if( start == -1 )
            result = to;
        else
            result.slerp( from, to, ((Counter - start)/float(stop-start)) );
        quatV.insert( quaternionIterator, result );
        quaternionIterator++;
        quaternionIterator = quatV.erase( quaternionIterator );
    }
}

// Clears all of the markers for quaternion data.
void Channel::clearQuaternionMarkers()
{
    quaternionMarkers.clear();
}

// Clears all of the markers for euler data.
void Channel::clearEulerMarkers()
{
    eulerMarkers.clear();
}

// A true/false value as to whether or not the joint contains motion data.
int Channel::hasFrameData()
{
    if( frames.size() > 0 )
        return 1;
    return 0;
}

//Just dump all the pertient information for the joint at hand.
void Channel::dump()
{
    printf("**************** Dumping Joint *****************\n");
    printf("Joint Name = %s\n", name);
    printf("Joint Type = %d\n", type);
    printf("Joint Parent = %s\n", parent);

    if( (int)children.size() > 0 )
    {
        printf("Joint children = ");

        for(int Counter=0; Counter<(int)children.size(); Counter++)
            printf("%s ", children[Counter]);
        printf("\n");
    }

    printf("Rotation order = %s\n",rotationOrder);
    printf("OFFSET ");

    if(offset.size() > 0)
        printf("%f %f %f", offset[0], offset[1], offset[2]);
    printf("\nJoint channel names  ");

    for(int Counter1=0; Counter1<(int)channelNames.size(); Counter1++)
        printf("%s ", channelNames[Counter1]);

    printf("\n");
    printf("FRAMES %d\n", frames.size());

    for(int Counter2=0; Counter2<(int)frames.size(); Counter2++)
    {
        frameData tempFrameData2 = frames[Counter2];

        for(int Counter2i=0; Counter2i<(int)tempFrameData2.size(); Counter2i++)
            printf("%f ",tempFrameData2[Counter2i]);
        printf("\n");
    }

    printf("ORIGINAL MARKER data %d\n", originalMarkers.size());

    for(int Counter3=0; Counter3<(int)originalMarkers.size(); Counter3++)
    {
        markerPosition tempMarkerPosition = originalMarkers[Counter3];

        for(int Counter3i=0; Counter3i<(int)tempMarkerPosition.size(); Counter3i++)
            printf("%f ",tempMarkerPosition[Counter3i]);
        printf("\n");

    }
    printf("EULER MARKER data %d\n", eulerMarkers.size());

    for(int Counter4=0; Counter4<(int)eulerMarkers.size(); Counter4++)
    {
        markerPosition tempMarkerPosition2 = eulerMarkers[Counter4];

        for(int Counter4i=0; Counter4i<(int)tempMarkerPosition2.size(); Counter4i++)
            printf("%f ",tempMarkerPosition2[Counter4i]);
        printf("\n");

    }

    printf("QUATERNION MARKER data %d\n", quaternionMarkers.size());

    for(int Counter5=0; Counter5<(int)quaternionMarkers.size(); Counter5++)
    {
        markerPosition tempMarkerPosition3 = quaternionMarkers[Counter5];

        for(int Counter5i=0; Counter5i<(int)tempMarkerPosition3.size(); Counter5i++)
            printf("%f ",tempMarkerPosition3[Counter5i]);
        printf("\n");

    }

    printf("QUATERNION data %d\n", quatV.size());

    for(int Counter6=0; Counter6<(int)quatV.size(); Counter6++)
    {
        Quaternion tempQuaternion = quatV[Counter6];
        tempQuaternion.print();
    }
}

void Channel::setID( const int id )
{
    m_id = id;
}

int Channel::getID()
{
    return m_id;
}