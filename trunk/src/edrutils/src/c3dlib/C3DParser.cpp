#include <boost/smart_ptr.hpp>
#include <c3dlib/C3DParser.h>
#include <sstream>
#include <btkC3DFileIO.h>
//#include <Utilities/StringStreamBuf.h>
#include <btkAcquisitionFileReader.h>
#include <btkAcquisitionFileWriter.h>
#include <stdexcept>
#include <btkProcessObject.h>
#include <btkAcquisition.h>
#include <btkSeparateKnownVirtualMarkersFilter.h>
#include <btkForcePlatformsExtractor.h>
#include <btkGroundReactionWrenchFilter.h>
#include <btkDownsampleFilter.h>
#include <btkPointCollection.h>
#include <btkForcePlatformCollection.h>
#include <btkWrenchCollection.h>
#include <btkAcquisitionFileIO.h>
#include <btkMetaDataUtils.h>
#include <list>
#include <btkMergeAcquisitionFilter.h>
#include <utils/Debug.h>
#include <utils/DataChannelCollection.h>
namespace c3dlib {

class StringStreamBuf : public std::stringstream
{
public:
	StringStreamBuf(std::ostream & os) : old(nullptr), os(os)
	{
		old = os.rdbuf( rdbuf() );
	}

	~StringStreamBuf()
	{
		os.rdbuf(old);
	}

private:
	std::streambuf* old;
	std::ostream & os;
};

//! Kanal analogowy, implementacja przykrywa obiekt btk::Analog
class Analog : public C3DParser::IAnalog
{
private:
    //! przez ta wartosc zostana przemnozone probki
	double scale;
    //! wskaznik do obiektu z biblioteki btk
	btk::Analog::ConstPointer analog;

public:
	//! Domyslny konstruktor - pusty
	Analog() : scale(1.0) {}

	//! Destruktor wirtualny
	virtual ~Analog() {}

	//!
	//! \param val
	void setAnalog(btk::Analog::ConstPointer val)	  { analog = val; }
	//!
	virtual const std::string& getLabel() const		  { return analog->GetLabel(); }
	//!
	virtual const std::string& getDescription() const { return analog->GetDescription(); }
	//!
	virtual const std::string& getUnit() const	      { return analog->GetUnit(); }
	//!
	virtual double getScale() const					  { return analog->GetScale(); }
	//!
	//! \param index
	virtual double getValue(int index) const		  { return analog->GetValues()[index]; }
};
typedef Analog* AnalogPtr;
typedef const Analog* AnalogConstPtr;

// pojedynczy punkt c3d
class Point : public C3DParser::IPoint
{
private:
	btk::Point::ConstPointer point;
	int numOfFrames;
	Type type;
	//! przez ta wartosc zostana przemnozone pobierane wartosci
	float scale;
    std::string unit;

public:
	Point() :
	  //point(nullptr),
	  numOfFrames(-1),
	  scale(1.0f),
      unit("unknown")
	  {

	  }

public:
	btk::Point::ConstPointer getPoint() const { return point; }
	void setType(Point::Type type) { this->type = type; }
	void setPoint(btk::Point::ConstPointer val) { point = val; }
	void setNumberOfFrames(int val) { numOfFrames = val; }
	float getScale() const { return scale; }
	void setScale(float val) { scale = val; }
    void setUnit(const std::string& val) { unit = val; }

public:
	virtual const std::string& getLabel() const		  { return point->GetLabel(); }
	virtual const std::string& getDescription() const { return point->GetDescription(); }
    virtual const std::string& getUnit() const { return unit; }
	virtual Point::Type getType() const { return type; }

	virtual osg::Vec3 getValue(int index) const
	{
		const btk::Measure<3>::Values& value = point->GetValues();
		return osg::Vec3(value[index ], value[index + numOfFrames], value[index + 2 * numOfFrames]) * scale;
	}
};
typedef Point* PointPtr;
typedef const Point* PointConstPtr;


class Event : public C3DParser::IEvent
{
private:
	btk::Event::ConstPointer eventPtr;

public:
	virtual Context getContext() const
	{
        std::string ctx = eventPtr->GetContext();
        if (ctx == "Left") {
            return Left;
        } else if (ctx == "Right") {
            return Right;
        } else {
            return General;
        }
	}
	virtual std::string getSubject() const
	{
		return eventPtr->GetSubject();
	}
	virtual double getTime() const
	{
		return eventPtr->GetTime();
	}
	virtual int getFrame() const
	{
		return eventPtr->GetFrame();
	}
	virtual const std::string& getLabel() const
	{
		return eventPtr->GetLabel();
	}
	virtual const std::string& getDescription() const
	{
		return eventPtr->GetDescription();
	}

	virtual IEvent* clone() const
	{
        Event* newEvent = new Event(*this);
		return newEvent;
	}
public:
	btk::Event::ConstPointer getEventPtr() const { return eventPtr; }
	void setEventPtr(btk::Event::ConstPointer val) { eventPtr = val; }
};
typedef Event* EventPtr;
typedef const Event* EventConstPtr;

//! Pola tej klasy agreguja te dane wykorzystywane wewnatrz klasy C3DParser,
//! ktore nie moga byc widoczne w naglowku
class C3DParser::__data
{
public:
    //! wskaznik do danych akwizycji
    btk::Acquisition::Pointer aquisitionPointer;
    //! wskaznik do klasy reprezentujacej plik c3d
    btk::C3DFileIO::Pointer c3dPointer;
	btk::SeparateKnownVirtualMarkersFilter::Pointer virtualMarkersSeparator;
	btk::ForcePlatformsExtractor::Pointer forcePlatformsExtractor;
    int roi[2];
    int firstFrame;
    int lastFrame;

    __data() :
        firstFrame(0),
        lastFrame(0)
    {
        roi[0] = firstFrame;
        roi[1] = lastFrame;

        virtualMarkersSeparator = btk::SeparateKnownVirtualMarkersFilter::New();
        forcePlatformsExtractor = btk::ForcePlatformsExtractor::New();
    }
};

C3DParser::C3DParser() :
    data( new __data)
{

}

C3DParser::~C3DParser()
{
    delete data;
    for (auto it = points.begin(); it != points.end(); ++it) {
        delete(*it);
    }
    //! kolekcja kanalow analogowych
    for (auto it = analogs.begin(); it != analogs.end(); ++it) {
        delete(*it);
    }

    for (auto it =  events.begin(); it != events.end(); ++it) {
        delete(*it);
    }
    //! plyty GRF
    for (auto it = forcePlatforms.begin(); it != forcePlatforms.end(); ++it) {
        delete(*it);
    }
}


void C3DParser::save( const std::string& filename )
{
	UTILS_ASSERT(data->aquisitionPointer);
	btk::AcquisitionFileWriter::Pointer writer = btk::AcquisitionFileWriter::New();
	writer->SetFilename(filename);
	writer->SetInput(data->aquisitionPointer);
}

void C3DParser::load(const std::string& filename)
{
    std::string warnings;
	std::vector<std::string> filenames;
	filenames.push_back(filename);
	importFrom(filenames, warnings);
}

void C3DParser::importFrom( const std::vector<std::string>& filenames, std::string& importWarnings )
{
    // wczytywanie plikow (niekoniecznie c3d)
    std::vector<btk::AcquisitionFileReader::Pointer> readers;
    try
    {
        for (unsigned int i = 0 ; i < filenames.size() ; ++i)
        {
            btk::AcquisitionFileReader::Pointer reader = btk::AcquisitionFileReader::New();
            reader->SetFilename(filenames[i]);
            reader->Update();
            readers.push_back(reader);
        }
    }
    catch (btk::Exception& e)
    {
        throw std::runtime_error(e.what());
    }
    // Check if the original acquisition need to be cropped
    if (this->data->aquisitionPointer)
    {
        if ((this->data->roi[0] != this->data->firstFrame) || (this->data->roi[1] != this->data->lastFrame))
        {
            // Resize the data.
            this->data->aquisitionPointer->ResizeFrameNumber(this->data->aquisitionPointer->GetPointFrameNumber() - (this->data->lastFrame - this->data->roi[1]));
            this->data->aquisitionPointer->ResizeFrameNumberFromEnd(this->data->aquisitionPointer->GetPointFrameNumber() - (this->data->firstFrame - this->data->roi[0]));
            // Remove the events outside of the bounds
            btk::EventCollection::Iterator it = this->data->aquisitionPointer->BeginEvent();
            while (it != this->data->aquisitionPointer->EndEvent())
            {
                if (((*it)->GetFrame() < this->data->roi[0]) || ((*it)->GetFrame() > this->data->roi[1]))
                    it = this->data->aquisitionPointer->RemoveEvent(it);
                else
                    ++it;
            }
        }
    }

    // Laczenie danych z plikow
    int shift = !this->data->aquisitionPointer ? 0 : 1;
    StringStreamBuf err(std::cerr);
    btk::MergeAcquisitionFilter::Pointer merger = btk::MergeAcquisitionFilter::New();
    merger->SetInput(0, this->data->aquisitionPointer);
    for (unsigned int i = 0 ; i < readers.size() ; ++i) {
        merger->SetInput(i+shift, readers[i]->GetOutput());
    }
    merger->Update();
    this->data->aquisitionPointer = merger->GetOutput();
	// pobranie danych akwizycji
    this->loadAcquisition();
    importWarnings = err.str();
}

void C3DParser::loadAcquisition()
{
    std::string labelPrefix = "";
    btk::MetaData::ConstIterator itSubjects = this->data->aquisitionPointer->GetMetaData()->FindChild("SUBJECTS");
    if (itSubjects != this->data->aquisitionPointer->GetMetaData()->End())
    {
        btk::MetaDataInfo::Pointer labelPrefixesInfo = (*itSubjects)->ExtractChildInfo("LABEL_PREFIXES", btk::MetaDataInfo::Char, 2, false);
        if (labelPrefixesInfo)
        {
            labelPrefix = labelPrefixesInfo->ToString(0);
            labelPrefix = labelPrefix.erase(labelPrefix.find_last_not_of(' ') + 1);
            labelPrefix = labelPrefix.erase(0, labelPrefix.find_first_not_of(' '));
        }
    }

    this->data->virtualMarkersSeparator->SetInput(this->data->aquisitionPointer->GetPoints());
    this->data->forcePlatformsExtractor->SetInput(this->data->aquisitionPointer);

    this->data->virtualMarkersSeparator->SetLabelPrefix(labelPrefix);
    this->data->virtualMarkersSeparator->Update();

	this->data->forcePlatformsExtractor->Update();
	btk::ForcePlatformCollection::Pointer fp = this->data->forcePlatformsExtractor->GetOutput();
	float scale = getUnitScale("mm");
	for (btk::ForcePlatformCollection::ConstIterator it = fp->Begin(); it != fp->End(); ++it) {
		// system jest przygotowany dla 4 wierzcholkow
		// nie zaklada sie, ze platformy moga byc innym wielokatem.
		if ((*it)->GetCorners().size() == 12) {
			ForcePlatformPtr platform(new ForcePlatform());
			platform->origin[0] = (*it)->GetOrigin()[0];
			platform->origin[1] = (*it)->GetOrigin()[1];
			platform->origin[2] = (*it)->GetOrigin()[2];
			platform->origin *= scale;
			for (int i = 0; i < 4; ++i) {
				platform->corners[i].set(
					(*it)->GetCorner(i)(0,0),
					(*it)->GetCorner(i)(1,0),
					(*it)->GetCorner(i)(2,0)
					);
				platform->corners[i] *= scale;

			}
            platform->type = (*it)->GetType();
            int count = (*it)->GetChannelNumber();
            for (int i = 0; i < count; ++i) {
                auto channel = (*it)->GetChannel(i);
                platform->channelLabels.insert(channel->GetLabel());
            }
			forcePlatforms.push_back(platform);
		}
	}

    this->data->firstFrame = this->data->aquisitionPointer->GetFirstFrame();
    this->data->lastFrame = this->data->aquisitionPointer->GetLastFrame();
    this->data->roi[0] = this->data->firstFrame;
    this->data->roi[1] = this->data->lastFrame;

    btk::PointCollection::Pointer points = this->data->virtualMarkersSeparator->GetOutput(0);
    for (btk::PointCollection::ConstIterator it = points->Begin() ; it != points->End() ; ++it)
    {
        PointPtr p = new Point();
		p->setPoint(*it);
		p->setNumberOfFrames(this->getNumPointFrames());
		p->setType(Point::Marker);
		p->setScale(scale);
        p->setUnit(data->aquisitionPointer->GetPointUnit((*it)->GetType()));
		this->points.push_back(p);
    }
    // Virtual markers (CoM, CoG, ...)

    points = this->data->virtualMarkersSeparator->GetOutput(2);
    for (btk::PointCollection::ConstIterator it = points->Begin() ; it != points->End() ; ++it) {
        PointPtr p = new Point();
		p->setPoint(*it);
		p->setNumberOfFrames(this->getNumPointFrames());
		p->setType(Point::VirtualMarker);
		p->setScale(scale);
        p->setUnit(data->aquisitionPointer->GetPointUnit((*it)->GetType()));
        this->points.push_back(p);
    }
    // Virtual markers used to define frames
    points = this->data->virtualMarkersSeparator->GetOutput(1);
    for (btk::PointCollection::ConstIterator it = points->Begin() ; it != points->End() ; ++it) {
        PointPtr p = new Point();
		p->setPoint(*it);
		p->setNumberOfFrames(this->getNumPointFrames());
        p->setUnit(data->aquisitionPointer->GetPointUnit((*it)->GetType()));
		p->setType(Point::VirtualMarkerForFrame);
        this->points.push_back(p);
    }
    // Other points
    points = this->data->virtualMarkersSeparator->GetOutput(3);

    for (btk::PointCollection::ConstIterator it = points->Begin() ; it != points->End() ; ++it) {
        PointPtr p = new Point();
		p->setPoint(*it);
		p->setNumberOfFrames(this->getNumPointFrames());
		switch((*it)->GetType()) {
		case btk::Point::Angle: p->setType(Point::Angle); break;
		case btk::Point::Force: p->setType(Point::Force); break;
		case btk::Point::Moment: p->setType(Point::Moment); break;
		case btk::Point::Power: p->setType(Point::Power); break;
		case btk::Point::Scalar: p->setType(Point::Scalar); break;
		}
        p->setUnit(data->aquisitionPointer->GetPointUnit((*it)->GetType()));
        this->points.push_back(p);
    }


    for (btk::Acquisition::AnalogIterator it = this->data->aquisitionPointer->BeginAnalog() ; it != this->data->aquisitionPointer->EndAnalog() ; ++it) {
        AnalogPtr a = new Analog();
		a->setAnalog(*it);
		this->analogs.push_back(a);
    }

    // Event
    for (btk::Acquisition::EventIterator it = this->data->aquisitionPointer->BeginEvent() ; it != this->data->aquisitionPointer->EndEvent() ; ++it) {
        EventPtr e = new Event();
        e->setEventPtr(*it);
        this->events.push_back(e);
    }
}

double C3DParser::getAnalogFrequency() const
{
	double res = this->data->aquisitionPointer->GetAnalogFrequency();
	return res;
}

C3DParser::IAnalogConstPtr C3DParser::getAnalog( int index ) const
{
	UTILS_ASSERT(index >= 0 && index < getNumAnalogs() && this->analogs[index]);
	return this->analogs[index];
}

int C3DParser::getNumAnalogs() const
{
	return analogs.size();
}

int C3DParser::getNumAnalogFrames() const
{
	return getNumPointFrames() * getNumberAnalogSamplePerFrame();
}

int C3DParser::getNumberAnalogSamplePerFrame() const
{
	int spf = data->aquisitionPointer->GetNumberAnalogSamplePerFrame();
	return spf;
}


C3DParser::IPointConstPtr C3DParser::getPoint( int index ) const
{
	UTILS_ASSERT(index >= 0 && index < getNumPoints() && this->points[index]);
	return this->points[index];
}


double C3DParser::getPointFrequency() const
{
	double tmp = data->aquisitionPointer->GetPointFrequency();
	return tmp;
}


int C3DParser::getNumPoints() const
{
	return this->data->aquisitionPointer->GetPointNumber();
}

int C3DParser::getNumPointFrames() const
{
	int tmp = data->aquisitionPointer->GetPointFrameNumber();
	return tmp;
}

int C3DParser::getNumEvents() const
{
	return events.size();
}

C3DParser::IEventPtr C3DParser::getEvent( int index ) const
{
	UTILS_ASSERT(index >= 0 && index < getNumEvents());
	return events[index];
}

float C3DParser::getUnitScale( const std::string& unit ) const
{
	if (unit == "m") return 1.0f;
	if (unit == "mm") return 0.001f;
	throw std::runtime_error("Unknown unit");
}

}
