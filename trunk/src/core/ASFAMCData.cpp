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
    staticAxis.clear();
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
    staticAxis = tempJoint.staticAxis;
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
    tempFrameData.push_back( zRotate );
    tempFrameData.push_back( xRotate );
    tempFrameData.push_back( yRotate );
    frames.push_back( tempFrameData );
}

// Adds explicit frame data.  This one is used for joints other than the ROOT.
void Channel::addFrame(float xRotate, float yRotate, float zRotate)
{
    frameData tempFrameData;
    tempFrameData.push_back( zRotate );
    tempFrameData.push_back( xRotate );
    tempFrameData.push_back( yRotate );
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
    frameData tempFrameData = frames[ frameNum-1 ];
    if(tempFrameData.size() == 6){
        xTranslate = tempFrameData[0];
        yTranslate = tempFrameData[1];
        zTranslate = tempFrameData[2];
    }
    else
    {
        printf("encountered an error, trying to read bad TRANSLATION frame data at frame %d\n", frameNum);
    }
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



void Channel::setID( const int id )
{
    m_id = id;
}

int Channel::getID()
{
    return m_id;
}


void Channel::invert(float *matrix)  
{
    int maxsize = 4;  // max number of rows (same as max number of columns)
    int actualsize = 4;  // actual size (rows, or columns) of the stored matrix

    for (int i=1; i < actualsize; i++) matrix[i] /= matrix[0]; // normalize row 0
    for (int i=1; i < actualsize; i++)  { 
        for (int j=i; j < actualsize; j++)  { // do a column of L
            float sum = 0.0;
            for (int k = 0; k < i; k++)  
                sum += matrix[j*maxsize+k] * matrix[k*maxsize+i];
            matrix[j*maxsize+i] -= sum;
        }
        if (i == actualsize-1) continue;
        for (int j=i+1; j < actualsize; j++)  {  // do a row of U
            float sum = 0.0;
            for (int k = 0; k < i; k++)
                sum += matrix[i*maxsize+k]*matrix[k*maxsize+j];
            matrix[i*maxsize+j] = 
                (matrix[i*maxsize+j]-sum) / matrix[i*maxsize+i];
        }
    }
    for ( int i = 0; i < actualsize; i++ )  // invert L
        for ( int j = i; j < actualsize; j++ )  {
            float x = 1.0;
            if ( i != j ) {
                x = 0.0;
                for ( int k = i; k < j; k++ ) 
                    x -= matrix[j*maxsize+k]*matrix[k*maxsize+i];
            }
            matrix[j*maxsize+i] = x / matrix[j*maxsize+j];
        }
        for ( int i = 0; i < actualsize; i++ )   // invert U
            for ( int j = i; j < actualsize; j++ )  {
                if ( i == j ) continue;
                float sum = 0.0;
                for ( int k = i; k < j; k++ )
                    sum += matrix[k*maxsize+j]*( (i==k) ? 1.0 : matrix[i*maxsize+k] );
                matrix[i*maxsize+j] = -sum;
            }
            for ( int i = 0; i < actualsize; i++ )   // final inversion
                for ( int j = 0; j < actualsize; j++ )  {
                    float sum = 0.0;
                    for ( int k = ((i>j)?i:j); k < actualsize; k++ )  
                        sum += ((j==k)?1.0:matrix[j*maxsize+k])*matrix[k*maxsize+i];
                    matrix[j*maxsize+i] = sum;
                }
};


void Channel::getQuaternionFromEuler( int frameNum, float &w, float &x, float &y, float &z )
{
    bool isOpenGLFormat = true;

    float Yaw, Pitch, Roll;
    getRotation(frameNum, Pitch, Yaw, Roll); // Yaw = x, Pitch = y, Roll = z

    // zamiana eulera na qwatrnion
    double qx,qy,qz,qw;

    // zamiana na radiany
     	Yaw = Yaw * PI / 180;
     	Pitch = Pitch * PI / 180;
     	Roll = Roll * PI / 180; 

    Yaw *= 0.5f; Pitch *= 0.5f; Roll *= 0.5f;

    float sy = sinf(Yaw), cy = cosf(Yaw);
    float sp = sinf(Pitch), cp = cosf(Pitch);
    float sr = sinf(Roll), cr = cosf(Roll);

    // Konwersja k¹tów Eulera na kwaternion,
    // który obraca ze wsp. obiektu do œwiata,
    qx = cy*sp*cr + sy*cp*sr;
    qy = sy*cp*cr - cy*sp*sr;
    qz = cy*cp*sr - sy*sp*cr;
    qw = cy*cp*cr + sy*sp*sr;

    //normalizacja
    float distance = (float)sqrt(qw*qw + qx*qx + qy*qy + qz*qz);
    if( distance == 0 )
    {
        qw = 1.0;
        qx = qy = qz = 0.0;
    }
    else
    {
        qw /= distance;
        qx /= distance;
        qy /= distance;
        qz /= distance;
    }


    // deklaracja danych macierzy
    float *Mm = new float[16]; //acm motiondata
    float *C = new float[16];

    getAsAxisMatrix(C);
    float *Cinv =  new float[16];

    float A[4][4]; // C - asf axic
    float B[4][4]; // Cinv
    float M[4][4]; // acm motion

    double L[4][4]; // wynik - macierz animacji


    // kwaternion na macierz
    float
        xx = qx * qx, yy = qy * qy, zz = qz * qz,
        xy = qx * qy, xz = qx * qz,
        yz = qy * qz, wx = qw * qx,
        wy = qw * qy, wz = qw * qz;

    Mm[0] = 1.0f - 2.0f * ( yy + zz ); //11
    Mm[1] = 2.0f * ( xy + wz ); //12
    Mm[2] = 2.0f * ( xz - wy ); //13
    Mm[3] = 0.0f; //14

    Mm[4] = 2.0f * ( xy - wz ); //21
    Mm[5] = 1.0f - 2.0f * ( xx + zz ); //22
    Mm[6] = 2.0f * ( yz + wx ); //23
    Mm[7] = 0.0f; //24

    Mm[8] = 2.0f * ( xz + wy ); //31
    Mm[9] = 2.0f * ( yz - wx ); //32
    Mm[10] = 1.0f - 2.0f * ( xx + yy ); //33
    Mm[11] = 0.0f; //34

    Mm[12] = 0.0f; //41
    Mm[13] = 0.0f; //42
    Mm[14] = 0.0f; //43
    Mm[15] = 1.0f; //44


    Cinv = C;

    // macierz odwrotna
    invert(Cinv);


    // Parsowanie danych
    if(isOpenGLFormat)
    {
        int counter = 0;
        for(int i = 0; i<4; i++)
            for(int j = 0; j<4; j++)
            {
                A[i][j] = C[counter];
                B[i][j] = Cinv[counter];
                M[i][j] = Mm[counter];

                counter++;
            }
    }

    /* Mnozenie macierzy Cinv*asmMotion */
        int i, j, k;
        for(i = 0; i < 4; i++)
            for(j = 0; j < 4; j++)
                L[i][j] = 0;
        for(i = 0; i < 4; i++) //ILWIERSZY
            for(j = 0; j < 4; j++) //ILWIERSZY
                for(k = 0; k < 4; k++) //ILKOLUMN
                    L[i][j] = L[i][j] + B[i][k] * M[k][j];


//         /* Mnozenie macierzy (B*M) * C */
//         for(i = 0; i < 4; i++)
//             for(j = 0; j < 4; j++)
//                 L[i][j] = 0;
//         for(i = 0; i < 4; i++) //ILWIERSZY
//             for(j = 0; j < 4; j++) //ILWIERSZY
//                 for(k = 0; k < 4; k++) //ILKOLUMN
//                     L[i][j] = L[i][j] + L[i][k] * A[k][j];



    // Konwercja z macierzy do kwaternionu
    float trace = L[0][0] + L[1][1] + L[2][2]; // I removed + 1.0f; see discussion with Ethan
    if( trace > 0 ) {// I changed M_EPSILON to 0
        float s = 0.5f / sqrtf(trace+ 1.0f);
        w = 0.25f / s;
        x = ( L[2][1] - L[1][2] ) * s;
        y = ( L[0][2] - L[2][0] ) * s;
        z = ( L[1][0] - L[0][1] ) * s;
    } else {
        if ( L[0][0] > L[1][1] && L[0][0] > L[2][2] ) {
            float s = 2.0f * sqrtf( 1.0f + L[0][0] - L[1][1] - L[2][2]);
            w = (L[2][1] - L[1][2] ) / s;
            x = 0.25f * s;
            y = (L[0][1] + L[1][0] ) / s;
            z = (L[0][2] + L[2][0] ) / s;
        } else if (L[1][1] > L[2][2]) {
            float s = 2.0f * sqrtf( 1.0f + L[1][1] - L[0][0] - L[2][2]);
            w = (L[0][2] - L[2][0] ) / s;
            x = (L[0][1] + L[1][0] ) / s;
            y = 0.25f * s;
            z = (L[1][2] + L[2][1] ) / s;
        } else {
            float s = 2.0f * sqrtf( 1.0f + L[2][2] - L[0][0] - L[1][1] );
            w = (L[1][0] - L[0][1] ) / s;
            x = (L[0][2] + L[2][0] ) / s;
            y = (L[1][2] + L[2][1] ) / s;
            z = 0.25f * s;
        }
    }



 //   Yaw *= 0.5f; Pitch *= 0.5f; Roll *= 0.5f;

//     if(Yaw < 0)
//     {
//         Yaw = (int)Yaw % 360;
//         Yaw = 360 + (int)Yaw;
//     }
// 
//     if(Pitch < 0)
//     {
//         Pitch = (int)Pitch % 360;
//         Pitch = 360 + (int)Pitch;
//     }
// 
//     if(Roll < 0)
//     {
//         Roll = (int)Roll % 360;
//         Roll = 360 + (int)Roll;
//     }

//       Yaw = abs(Yaw); Pitch = abs(Pitch); Roll = abs(Roll);
// // 
//      	Yaw = Yaw * PI / 180;
//      	Pitch = Pitch * PI / 180;
//      	Roll = Roll * PI / 180;
// 
//         	Quaternion* quat = new Quaternion(Pitch, Yaw, Roll);
//         
//         	quat->getQuaternion(w, x, y, z);

//     float sy = sinf(Yaw), cy = cosf(Yaw);
//     float sp = sinf(Pitch), cp = cosf(Pitch);
//     float sr = sinf(Roll), cr = cosf(Roll);
// 
//     x = cy*sp*cr + sy*cp*sr;
//     y = sy*cp*cr - cy*sp*sr;
//     z = cy*cp*sr - sy*sp*cr;
//     w = cy*cp*cr + sy*sp*sr;


// 	float rx, ry, rz;
// 	getRotation(frameNum, rx, ry, rz);
// 
//     // zamiana stopni na radiany
// 	rx = rx * PI / 180;
// 	ry = ry * PI / 180;
// 	rz = rz * PI / 180;
// 
// 
// 	Quaternion* quat = new Quaternion(rx, ry, rz);
// 
// 	quat->getQuaternion(w, x, y, z);
}

void Channel::setASFAxis( float angle )
{
    staticAxis.push_back(angle);
}

void Channel::getAsAxisMatrix( float* matrix )
{
    float Yaw, Pitch, Roll;
    Pitch = staticAxis[0];
    Yaw = staticAxis[1];
    Roll = staticAxis[2];

    // zamiana eulera na qwatrnion
    double qx,qy,qz,qw;

    // zamiana na radiany
    Yaw = Yaw * PI / 180;
    Pitch = Pitch * PI / 180;
    Roll = Roll * PI / 180; 

    Yaw *= 0.5f; Pitch *= 0.5f; Roll *= 0.5f;

    float sy = sinf(Yaw), cy = cosf(Yaw);
    float sp = sinf(Pitch), cp = cosf(Pitch);
    float sr = sinf(Roll), cr = cosf(Roll);

    // Konwersja k¹tów Eulera na kwaternion,
    // który obraca ze wsp. obiektu do œwiata,
    qx = cy*sp*cr + sy*cp*sr;
    qy = sy*cp*cr - cy*sp*sr;
    qz = cy*cp*sr - sy*sp*cr;
    qw = cy*cp*cr + sy*sp*sr;

    //normalizacja
    float distance = (float)sqrt(qw*qw + qx*qx + qy*qy + qz*qz);
    if( distance == 0 )
    {
        qw = 1.0;
        qx = qy = qz = 0.0;
    }
    else
    {
        qw /= distance;
        qx /= distance;
        qy /= distance;
        qz /= distance;
    }


    // kwaternion na macierz
    float
        xx = qx * qx, yy = qy * qy, zz = qz * qz,
        xy = qx * qy, xz = qx * qz,
        yz = qy * qz, wx = qw * qx,
        wy = qw * qy, wz = qw * qz;

    matrix[0] = 1.0f - 2.0f * ( yy + zz ); //11
    matrix[1] = 2.0f * ( xy + wz ); //12
    matrix[2] = 2.0f * ( xz - wy ); //13
    matrix[3] = 0.0f; //14

    matrix[4] = 2.0f * ( xy - wz ); //21
    matrix[5] = 1.0f - 2.0f * ( xx + zz ); //22
    matrix[6] = 2.0f * ( yz + wx ); //23
    matrix[7] = 0.0f; //24

    matrix[8] = 2.0f * ( xz + wy ); //31
    matrix[9] = 2.0f * ( yz - wx ); //32
    matrix[10] = 1.0f - 2.0f * ( xx + yy ); //33
    matrix[11] = 0.0f; //34

    matrix[12] = 0.0f; //41
    matrix[13] = 0.0f; //42
    matrix[14] = 0.0f; //43
    matrix[15] = 1.0f; //44
}