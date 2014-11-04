/********************************************************************
    created:  2012/12/17
    created:  17:12:2012   11:03
    filename: IPin.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IPIN_H__
#define HEADER_GUARD___IPIN_H__

#include <dflib/INode.h>
////////////////////////////////////////////////////////////////////////////////
namespace df {
////////////////////////////////////////////////////////////////////////////////

//! Forward declaration
class IConnection;
class ModelBase;

//! Interfejs bazowy pinu. Opisuje jego typ, po��czenia, w�ze� do kt�rego nale�y.
class IPin
{
	friend class ModelBase;

public:

	//! Typ pinu
	enum PinType {
		INPUT_PIN,	//! pin wejsciowy
		OUTPUT_PIN	//! pin wyj�ciowy
	};

	//! Typ ilo�ci po��cze�
	typedef unsigned int size_type;

public:
	//! \return Typ pinu
	virtual const PinType type() const = 0;

	//! \return W�ze� do kt�rego nalezy pin
	virtual INode * node() = 0;
	//! \return W�ze� do kt�rego nalezy pin
	virtual const INode * node() const = 0;

	//! \return Czy pin jest pod��czony
	virtual const bool connected() const = 0;
	//! \return Ilo�� po��cze� pinu
	virtual const size_type connectionsSize() const = 0;
	//! \param idx Indeks po��czenia kt�re chcemy pobra�
	//! \return Po��czenie o zadanym indeksie
	virtual const IConnection * connection(size_type idx) const = 0;
	//! \param idx Indeks po��czenia kt�re chcemy pobra�
	//! \return Po��czenie o zadanym indeksie
	virtual IConnection * connection(size_type idx) = 0;

private:
	//! \param connection Nowe po��czenie kt�rego cz�sci� jest dany pin
	virtual void addConnection(IConnection * connection) = 0;
	//! \param connection Po��czenie kt�rego cz�sci� by� dany pin - usuwamy
	virtual void removeConnection(IConnection * connection) = 0;
};

//! Forward declaration
class IOutputPin;

//! Interfejs pinu wejsciowego. Dodatkowo opisuje mo�liwo�ci ��czenia tego pinu
//! z pinami wyj�ciowymi
class IInputPin : public IPin
{
public:	
	//! \return Typ pinu
	virtual const PinType type() const { return IPin::INPUT_PIN; }

	//! \return W�ze� do kt�rego nalezy pin
	virtual ISinkNode * node() = 0;
	//! \return W�ze� do kt�rego nalezy pin
	virtual const ISinkNode * node() const = 0;

	//! \param pin Pin wyj�ciowy, kt�ry testujemy pod k�tem kompatybilno�ci ��czenia
	//! \return Czy mo�na tego typu  piny po��czy�
	virtual const bool pinCompatible(const IOutputPin * pin) const = 0;
	//! \param pin Pin wyj�ciowy, kt�ry testujemy pod k�tem mozliwo�ci ��czenia
	//! \return Czy mo�na te piny po��czy�
	virtual const bool canConnect(const IOutputPin * pin) const = 0;
};

//! Interfejs pinu wyj�ciowego. 
class IOutputPin : public IPin
{
public:
	//! \return Typ pinu
	virtual const PinType type() const { return IPin::OUTPUT_PIN; }
	//! \return W�ze� do kt�rego nalezy pin
	virtual ISourceNode * node() = 0;
	//! \return W�ze� do kt�rego nalezy pin
	virtual const ISourceNode * node() const = 0;
};

//! Interfejs rozszerzonego piny wejsciowego. Dodaje informacje o konieczno�ci
//! pod��czenia danego pinu oraz ewentualnych pinach uzale�nionych od tego pinu
class IExtendedInputPin : public IInputPin
{
public:
	//! \return Czy pin musi by� pod��czony
	virtual const bool required() const = 0;

	//! \return Ilo�� pin�w zale�nych od pinu
	virtual const size_type dependentSize() const = 0;
	//! \return Czy s� jakie� piny zale�ne
	virtual const bool dependentEmpty() const = 0;
	//! \param idx Indeks pinu wyj�ciowego zale�nego od pinu
	//! \return Pin wyj�ciowy zale�ny od pinu
	virtual IOutputPin * dependent(size_type idx) = 0;
	//! \param idx Indeks pinu wyj�ciowego zale�nego od pinu
	//! \return Pin wyj�ciowy zale�ny od pinu
	virtual const IOutputPin * dependent(size_type idx) const = 0;

	//! \param pin Pin wyj�ciowy kt�ry testujemy pod k�tem zale�no�ci od pinu
	//! \return Czy zadany pin wyj�ciowy jest zalezny od pinu
	virtual const bool isDependcyFor(const IOutputPin * pin) const = 0;
};

//! Interfejs rozszerzonego pinu wyj�ciowego. Dodaje informacj� na temat
//! zale�no�ci tego pinu od pin�w wej�ciowych.
class IExtendedOutputPin : public IOutputPin
{
public:
	//! \return Ilo�� pin�w od k�rych zale�ny jest pin
	virtual const size_type dependencySize() const = 0;
	//! \return Czy pin jest uzale�niony od jakich� pin�w wej�ciowych
	virtual const bool dependencyEmpty() const = 0;

	//! \param idx Indeks pinu od kt�rego jest uzale�niony ten pin
	//! \return Pin wej�ciowy od kt�rego jest uzalezniony ten pin
	virtual IInputPin * dependency(size_type idx) = 0;
	//! \param idx Indeks pinu od kt�rego jest uzale�niony ten pin
	//! \return Pin wej�ciowy od kt�rego jest uzalezniony ten pin
	virtual const IInputPin * dependency(size_type idx) const = 0;
	//! \param pin Pin wej�ciowy testowany pod k�tem zale�no�ci
	//! \return Czy pin jest uzale�niony od danego pinu
	virtual const bool dependsOn(const IInputPin * pin) const = 0;
	//! \return Czy zale�no�ci pinu s� spe�nione
	virtual const bool dependenciesSatisfied() const = 0;
};

}

#endif	//	HEADER_GUARD___IPIN_H__
