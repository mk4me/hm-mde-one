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

#define signals Q_SIGNALS
#define slots Q_SLOTS

#include "coreui/CoreAction.h"

using namespace python;

PythonService::PythonService() :
	editor(nullptr), 
	hierarchyManager(nullptr)
{

}

PythonService::~PythonService()
{

}


void PythonService::init(core::ISourceManager * sourceManager,
						 core::IVisualizerManager * visualizerManager,
						 core::IDataManager * memoryDataManager,
						 core::IStreamDataManager * streamDataManager,
						 core::IFileDataManager * fileDataManager,
						 core::IDataHierarchyManager * hierarchyDataManager)
{
	bridge = utils::make_shared<MdeBridge>();
	bridge->setManagers(sourceManager, visualizerManager, memoryDataManager, streamDataManager, fileDataManager, hierarchyDataManager);
	logic = utils::make_shared<PythonLogic>(bridge);
	hierarchyManager = hierarchyDataManager;
}

QWidget* PythonService::getWidget( )
{
	if (editor == nullptr){
		console = new python::PythonConsole(logic);

		editor = PythonEditor::createPythonEditor();
		QList<QAction*> actions;

		coreUI::CoreAction*  run = new coreUI::CoreAction(tr("Run"), QIcon(":/newVdf/icons/run.png"), tr("Run"), this, coreUI::CoreTitleBar::Left);
		connect(run, SIGNAL(triggered()), this, SLOT(runScript()));
		actions.push_back(run);

		editor->addActions(actions);
		QString text("print \"Hello world\"\nresult = 5 * 4");
		editor->setPlainText(text);
	}

    return editor;
}

void PythonService::runScript()
{
    int input = 10;
    logic->getDict()["input"] = input;
    QString out = QString::fromStdString(logic->run(editor->toPlainText().toStdString()));
	//output->setText(out);
	if (out.isEmpty()) {
		out = QString("Done running script (there was no output)\n");
	}
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
	console->setObjectName(QObject::tr("Console"));
    l.push_back(console);
	return l;
}

