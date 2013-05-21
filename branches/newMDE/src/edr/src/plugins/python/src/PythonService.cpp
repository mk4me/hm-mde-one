#include "PythonPCH.h"
#include "PythonLogic.h"
#include "PythonService.h"
#include "PythonHighlighter.h"
#include <QtGui/QLabel>
#include <QtGui/QFont>
#include <QtGui/QTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>
#include <core/ObjectWrapper.h>


PythonService::PythonService() :
    editor(nullptr)
{

}

PythonService::~PythonService()
{

}

void PythonService::init(core::IManagersAccessor * managersAccessor)
{

}

QWidget* PythonService::getSettingsWidget( core::IActionsGroupManager * actionsManager )
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    editor = new QTextEdit;
    editor->setFont(font);
    
    QString text("print \"Hello world\"\nresult = 5 * 4");
    editor->setPlainText(text);
    QSyntaxHighlighter* highlighter = new PythonHighlighter(editor->document());
    //editor->setMinimumSize(600, 600);
    editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    editor->setStyleSheet("border: 1px solid red");
    return editor;
}

QWidget* PythonService::getWidget( core::IActionsGroupManager * actionsManager )
{
    QVBoxLayout* layout = new QVBoxLayout();

    QLabel* label = new QLabel("Widget TEST");
    layout->addWidget(label);

    QPushButton* buttonRun = new QPushButton();
    buttonRun->setText("RUN");
    connect(buttonRun, SIGNAL(clicked()), this, SLOT(runScript()));
    layout->addWidget(buttonRun);

    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    layout->addWidget(spacer);

    QWidget* widget = new QWidget();
    widget->setLayout(layout);
    return widget;
	
}

const std::string& PythonService::getName() const
{
	static std::string s = "PythonService";
	return s;
}

QWidget* PythonService::getControlWidget( core::IActionsGroupManager * actionsManager )
{
    QLabel* label = new QLabel("control TEST");
    label->setStyleSheet("background:red");
    return label;
}

void PythonService::runScript()
{
    int input = 10;
    logic.getDict()["input"] = input;
    logic.run(editor->toPlainText().toStdString());
}
