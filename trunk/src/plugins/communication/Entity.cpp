
#include "CommunicationPCH.h"
#include <plugins/communication/Entity.h>

using namespace communication;

Entity::Entity(int id) : id(id) { }

Entity::~Entity() { }

void Entity::setID(int id) {
	this->id = id;
}

const int Entity::getID() const {
	return id;
}
