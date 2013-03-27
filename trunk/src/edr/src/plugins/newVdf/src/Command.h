/********************************************************************
	created:	2012/12/26
	created:	26:12:2012   22:37
	filename: 	Command.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__COMMAND_H__
#define HEADER_GUARD_NEWVDF__COMMAND_H__

#include <utils/SmartPtr.h>
#include <QtCore/QObject>
#include <QtCore/QPointF>
#include <plugins/newVdf/ICommand.h>

#include "SceneBuilder.h"
#include "SceneModel.h"

namespace vdf {


class IVisualItem;

//! Dodaje element (node) na scen�.
class AddToSceneCommand : public ICommand
{
public:
	//! Konstruktor
	//! \param scene  Model sceny vdf, do kt�rego dodajemy element
	//! \param node Dodawany element
	//! \param scenePos Pozycja na scenie, gdzie zostanie wstawiony element
	AddToSceneCommand(SceneModelPtr scene, const SceneBuilder::VisualNodeWithPins& node, const QPointF& scenePos);
public:
	//! Wstawia elment na scen� wraz z pinami, kt�re przechowuje.
	virtual void doIt();
	//! Usuwa element wraz z pinami ze sceny
	virtual void undoIt();
	//! Nazwa polecenia
	virtual QString name();
private:
	//! Model sceny vdf, do kt�rego dodajemy element
	SceneModelPtr sceneModel;
	//! Dodawany element
	SceneBuilder::VisualNodeWithPins nodeWithPins;
	//! Pozycja na scenie, gdzie zostanie wstawiony element
	QPointF position;
};

//! Polecenie obs�uguje dodanie po��czenia pomi�dzy pinami.
class AddConnectionCommand : public ICommand
{
public:
	//! Konstruktor
	//! \param scene Model sceny vdf, do kt�rego dodajemy element
	//! \param p1 Pin wyj�ciowy
	//! \param p2 Pin wej�ciowy
	AddConnectionCommand(SceneModelPtr scene, IVisualOutputPinPtr p1, IVisualInputPinPtr p2);
public:
	//! Dodaje po��czenie pomi�dzy pinami
	virtual void doIt();
	//! Usuwa po��czenie
	virtual void undoIt();
	//! Nazwa polecenia
	virtual QString name() { return QString(typeid(this).name()); }
private:
	//! Model sceny vdf, do kt�rego dodajemy element
	SceneModelPtr sceneModel;
	//! Pin wyj�ciowy
	IVisualOutputPinPtr outputPin;
	//! Pin wej�ciowy
	IVisualInputPinPtr inputPin;
	//! Utworzone po��czenie
    IVisualConnectionPtr connection;
};

//! Polecenie przesuwa elementy na scenie
class MoveCommand : public ICommand
{
public:
	//! Konstruktor
	//! \param item Element graficzny sceny, kt�re zostanie przesuni�ty 
	//! \param newP Nowa pozycja elementu 
	//! \param oldP Stara pozycja elementu
	MoveCommand(QGraphicsItem* item, const QPointF& newP, const QPointF& oldP);

public:
	//! Przesuwa element
	virtual void doIt();
	//! Cofa przesuni�cie
	virtual void undoIt();
	//! Nazwa polecenia 
	virtual QString name() { return QString(typeid(this).name()); }

private:
	//! Element graficzny sceny, kt�re zostanie przesuni�ty 
	QGraphicsItem* item;
	//! Nowa pozycja elementu graficznego
	QPointF newPosition;
	//! Stara pozycja elementu graficznego
	QPointF oldPosition;
};

//! Polecenie agreguje inne polecenia
class MultiCommand : public ICommand
{
public:
	//! Konstruktor
	//! \param c Wektor z poleceniami do wykonania
	MultiCommand(const std::vector<ICommandPtr>& c);
public:
	//! Kolejno wykonuje dostarczone polecenia
	virtual void doIt();
	//! Cofa polecenia w odwrotnej kolejno�ci do wykonania
	virtual void undoIt();
	//! Nazwa polecenia 
	virtual QString name() { return QString(typeid(this).name()); }

private:
	//! Dostarczone polecenia do wykonania
	std::vector<ICommandPtr> commands;
};

//! Polecenie usuwa po��czenie pomi�dzy pinami
class RemoveConnectionCommand : public ICommand
{
public:
    //! Konstruktor
    //! \param scene Model sceny vdf 
    //! \param toRemove Po��czenie do usuni�cia
    RemoveConnectionCommand(SceneModelPtr scene, IVisualConnectionPtr toRemove);

public:
	//! Usuwa po��czenie
    virtual void doIt();
	//! Ponownie wstawia po��czenie na scen�
    virtual void undoIt();
	//! Nazwa polecenia
    virtual QString name() { return QString(typeid(this).name()); }

private:
	//! Po��czenie do usuni�cia
    IVisualConnectionPtr item;
	//! Model sceny vdf
    SceneModelPtr sceneModel;
};

//! Polecenie usuwa element (node) ze sceny vdf
class RemoveNodeCommand : public ICommand
{
public:
	//! Konstruktor
	//! \param scene Model sceny vdf 
	//! \param toRemove Element do usuni�cia
	RemoveNodeCommand(SceneModelPtr scene, IVisualNodePtr toRemove);
public:
	//! Usuwa w�ze� ze sceny, razem z pinami i dowi�zanymi do nich po��czeniami
	virtual void doIt();
	//! Wstawia z powrotem usuni�ty w�ze�, przywraca po��czenia
	virtual void undoIt();
	//! Nazwa polecenia
	virtual QString name() { return QString(typeid(this).name()); }

private:
	//! Usuwa po��czenie dla pinu wej�ciowego (mo�liwe jedno)
	//! \param pin Pin, z kt�rego usuwamy
	void removeConnectionFromPin( IVisualInputPinPtr pin );
    //! Usuwa po��czenia dl pinu wyj�ciowego (mo�liwe wiele)
    //! \param pin Pin, z kt�rego usuwamy
    void removeConnectionFromPin( IVisualOutputPinPtr pin );
    //! Usuwa po��czenie ze sceny
    //! \param connection po��czenie do usuni�cia
    void removeConnection( IVisualConnectionPtr connection );

private:
	//! Usuwany element
	IVisualNodePtr item;
	//! Model sceny vdf
	SceneModelPtr sceneModel;
	//! Wszystkie usuni�te przy okazji po��czenia
	std::list<IVisualConnectionPtr> removedConnections;
	//! Wszystkie usuni�te przy okazji piny
	std::list<IVisualPinPtr> removedPins;
};

//! Usuwa ca�� grup� element�w graficznych, wykorzustuje inne polecenia do realizacji tego celu
class RemoveSelectedCommand : public ICommand
{
public:
	//! Konstruktor
	//! \param scene Model sceny vdf 
	//! \param selectedItems Elementy graficzne, kt�rych reprezentacja logiczna ma zosta� usuni�ta z modelu
	RemoveSelectedCommand(SceneModelPtr scene, const QList<QGraphicsItem*> selectedItems);
public:
	//! Usuwa zaznaczone elementy
	virtual void doIt();
	//! Przywraza usuni�te elementy
	virtual void undoIt();
	//! Nazwa polecenia
	virtual QString name() { return QString(typeid(this).name()); }
private:
	//! Elementy graficzne, kt�rych reprezentacja logiczna ma zosta� usuni�ta z modelu
    QList<QGraphicsItem*> items;
	//! Model sceny vdf
    SceneModelPtr sceneModel;
	//! Polecenia usuwaj�ce elementy ze sceny
    std::vector<ICommandPtr> commands;
};
}

#endif
