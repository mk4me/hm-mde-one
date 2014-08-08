#include "PythonPCH.h"
#include "PythonLogic.h"
#include "PythonService.h"
#include "PythonHighlighter.h"
#include <QtWidgets/QLabel>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLabel>
#include <QtGui/QFont>
#include <utils/ObjectWrapper.h>
#include "PythonConsole.h"
#include "QtWidgets/QAction"
#include "PythonEditor.h"

using namespace python;

PythonService::PythonService() :
    editor(nullptr)
{

}

PythonService::~PythonService()
{

}


void PythonService::init( core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager, 
	core::IMemoryDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager )
{
	bridge = utils::make_shared<MdeBridge>();
	bridge->setManagers(sourceManager, visualizerManager, memoryDataManager, streamDataManager, fileDataManager);
	logic = utils::make_shared<PythonLogic>(bridge);
	console = new python::PythonConsole(logic);

	editor = PythonEditor::createPythonEditor();
	QList<QAction*> actions;

	QAction* run = new QAction("Run", editor);
	connect(run, SIGNAL(triggered()), this, SLOT(runScript()));
	actions.push_back(run);

	editor->addActions(actions);
	QString text("print \"Hello world\"\nresult = 5 * 4");
	editor->setPlainText(text);

}

QWidget* PythonService::getWidget( )
{
    return editor;
}

void PythonService::runScript()
{
    int input = 10;
    logic->getDict()["input"] = input;
    QString out = QString::fromStdString(logic->run(editor->toPlainText().toStdString()));
	//output->setText(out);
	console->append(out);
}

const bool PythonService::lateInit()
{
    return true;
}

void PythonService::finalize()
{
 
}

void PythonService::update( double deltaTime )
{
 
}

QWidgetList PythonService::getPropertiesWidgets()
{
    QWidgetList l;
    l.push_back(console);
	return l;
}

