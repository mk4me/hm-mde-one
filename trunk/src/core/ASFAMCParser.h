#ifndef ASFAMCPARSER_H
#define ASFAMCPARSER_H 

#pragma warning(disable: 4786)
#pragma warning(disable: 4788)

/********************************************************************************************
*	Files: motionData.h
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
*********************************************************************************************/

#include <map>
#include <vector>
#include <stdio.h>
#include <string>

#include "ASFAMCData.h"


// This is the function used for comparing names in the stl map
struct namesCompare
{
    bool operator()(const char* s1, const char* s2) const
    {
        return (strcmp(s1, s2) < 0);
    }
};

typedef std::vector<Channel> vectorOfJoints;						// This structure will hold our motionData
typedef std::map<const char*, int, namesCompare> mappedNames;		// This structure gives us the position of the joint in the joints vector

class ASFAMCParser
{

private:

    mappedNames		mappedJointNames;		// This is a map of joint names
    mappedNames		mappedEffectorNames;	// This is a map of joint names that are effectors
    vectorOfJoints	joints;					// This is a vector of joints

    vectorOfJoints::iterator jointPointer;	// A pointer into the joint class, the user of this class
    // can use this in conjunction with the 3 jointPointer functions

    float			frameTime;				// how long each frame lasts for
    float			numberOfFrames;			// number of frames in animation

    int				numOfRoots;				// This tells me how many roots there are, this version
    // supports motion data files with multiple roots

    int				exists;					// This value is used to see if motion data exists so



    // Gets the global position of a requested joint.
    int getJointPosition( const char* jointName );

    void RemoveDummyJoint();

    // How high the floor should be - gathered by finding the lowest Y-value in the motion.
    float floorHeight;

    float rMatrix[16] ;
public:

    ASFAMCParser();
    ~ASFAMCParser();


    vectorOfJoints* getJoints();


    // this function is for skeleton animation
    int keepAnimating();

    // Read ASF and AMC files -> pass an asf filename then an amc one.
    int readAcclaimFiles(std::string ASFFileName, std::string AMCFileName ) ;

    // Compute the end effectors for all the joints.
    void computeEffectors();


    // Get a pointer to a joint given by jointName.  You should not delete the memory
    // for this joint I give you
    Channel* getJoint( const char* jointName );

    // Is their motion data to be displayed
    int doesExist();

    // clear motion data so that a new file can be read in.  This will set motion data 
    // to does not exist
    void clearMotionData();

    // Returns the nubmer of frames
    int getNumberOfFrames();

    // Use these two functions together so that you get the number of roots and then loop on that count
    // from 0 to numOfRoots-1 and call getRootName with that number
    int getNumberOfRoots();
    char* getRootName( int );


    // These 4 functions can be used for going through the joints sequentially
    // Here is an example use:
    //		resetJointPointer()
    //		for(int i=0; i<getNumberOfJoints; i++){
    //			joint *j = getCurrentJointPointer();   // then do what you want with it
    //			advanceJointPointer();
    //		}
    void resetJointPointer();				// reset the joint pointer
    Channel* getCurrentJointPointer();		// get the current pointer
    void advanceJointPointer();				// go to the next pointer
    int getNumberOfJoints();				// get the number of joints


    // set the camera position
    void setCameraPosition(float, float, float);

    // Returns the time that each frame should be shown
    float getFrameTime();

    // Sets the time that each frame should be shown
    void setFrameTime(float ft);

    // This is used for the RIB exporter and gathers a 4x4 transformation matrix for the camera.
    void setCameraPosition(float* rMatrix);

    float getFloorHeight();
};

#endif
