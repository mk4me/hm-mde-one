#include "NewVdfPCH.h"
#include "CommandStackDebug.h"

using namespace vdf;

CommandStackDebug::CommandStackDebug( CommandStackPtr stack, QWidget* parent ) :
	QListWidget(parent),
	commandStack(stack)
{

}

void CommandStackDebug::refresh()
{
	this->clear();
	for (auto it = commandStack->commands.begin(); it != commandStack->commands.end(); ++it) {
		QListWidgetItem* item = new QListWidgetItem();
		if (it == commandStack->currentCommand) {
			item->setText(QString("* %1").arg((*it)->name()));
		} else {
			item->setText((*it)->name());
		}
		
		this->addItem(item);
	}
}
