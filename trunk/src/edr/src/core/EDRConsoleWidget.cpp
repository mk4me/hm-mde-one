#include "CorePCH.h"
#include "EDRConsoleWidget.h"
#include "EDRTitleBar.h"

EDRConsoleWidget::EDRConsoleWidget(const QString &title, QWidget *parent, Qt::WindowFlags flags)
    : EDRDockWidget(title, parent, flags), consoleWidget(new EDRConsoleInnerWidget())
{
    init();
}

EDRConsoleWidget::EDRConsoleWidget(QWidget *parent, Qt::WindowFlags flags)
    : EDRDockWidget(parent, flags), consoleWidget(new EDRConsoleInnerWidget())
{
    init();
}

EDRConsoleWidget::~EDRConsoleWidget()
{

}

void EDRConsoleWidget::init() 
{
    qRegisterMetaType<EDRConsoleWidgetEntry>("EDRConsoleWidgetEntry");
    qRegisterMetaType<EDRConsoleWidgetEntryPtr>("EDRConsoleWidgetEntryPtr");

    EDRTitleBar * titleBar = supplyWithEDRTitleBar(this);

    //EDRTitleBar * titleBar = new EDRTitleBar();
    //setTitleBarWidget(titleBar);

    //connect(this, SIGNAL(windowTitleChanged(const QString &)), titleBar, SLOT(setTitle(const QString &)));

    setWidget(consoleWidget);
    setWindowTitle(consoleWidget->windowTitle());


    //QLabel * label;
    QToolButton *clearButton;
    QToolButton *toggleWordWrapButton;

    clearButton = new QToolButton(titleBar);
    clearButton->setObjectName(QString::fromUtf8("clearButton"));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/resources/icons/clear_console.png"), QSize(), QIcon::Normal, QIcon::Off);
    clearButton->setIcon(icon);
    clearButton->setIconSize(QSize(16, 16));

    toggleWordWrapButton = new QToolButton(titleBar);
    toggleWordWrapButton->setObjectName(QString::fromUtf8("toggleWordWrapButton"));
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/resources/icons/word_wrap_16x16.png"), QSize(), QIcon::Normal, QIcon::Off);
    toggleWordWrapButton->setIcon(icon1);
    toggleWordWrapButton->setCheckable(true);

    clearButton->setText(QApplication::translate("ConsoleWidget", "clear", 0, QApplication::UnicodeUTF8));
    toggleWordWrapButton->setText(QApplication::translate("ConsoleWidget", "toggle word wrap", 0, QApplication::UnicodeUTF8));

    QObject::connect(clearButton, SIGNAL(clicked()), consoleWidget->textEdit, SLOT(clear()));
    QObject::connect(toggleWordWrapButton, SIGNAL(toggled(bool)), this, SLOT(setWordWrap(bool)));

    QMetaObject::connectSlotsByName(this);

    toggleWordWrapButton->setChecked( consoleWidget->textEdit->lineWrapMode() != QTextOption::NoWrap );

    titleBar->addObject(clearButton, IEDRTitleBar::Left);
    titleBar->addObject(toggleWordWrapButton, IEDRTitleBar::Left);
}

void EDRConsoleWidget::logEntry( const EDRConsoleWidgetEntry& entry )
{
    QTextCharFormat fmt;

    if ( entry.severity == core::LogSeverityInfo ) {
        fmt.setForeground( QColor(0, 0, 0, 255) );
    } else if ( entry.severity == core::LogSeverityWarning ) {
        fmt.setForeground( QColor(255, 130, 0, 255) );
    } else if ( entry.severity == core::LogSeverityError ) {
        fmt.setForeground( QColor(255, 0, 0, 255) );
    } else {
        fmt.setForeground( QColor(100, 150, 50, 255) );
    }

    consoleWidget->textEdit->setCurrentCharFormat(fmt);
    consoleWidget->textEdit->append( entry.message ); 
}

void EDRConsoleWidget::logEntry( EDRConsoleWidgetEntryPtr entry )
{
    logEntry(*entry);
}

void EDRConsoleWidget::flushQueue()
{
    QMutexLocker locker(&queueMutex);
    while ( !queuedEntries.empty() ) {
        logEntry( *queuedEntries.front() );
        queuedEntries.pop();
    }
}

void EDRConsoleWidget::queueEntry( EDRConsoleWidgetEntryPtr entry )
{
    QMutexLocker locker(&queueMutex);
    queuedEntries.push(entry);
}

void EDRConsoleWidget::logOrQueueEntry( EDRConsoleWidgetEntryPtr entry )
{
	QThread* widgetThread = this->thread();
	QThread* currentThred = QThread::currentThread();
    if ( currentThred == widgetThread ) {
        logEntry(*entry);
    } else {
        queueEntry(entry);
    }
}

void EDRConsoleWidget::setWordWrap( bool wrap )
{
    consoleWidget->textEdit->setWordWrapMode( wrap ? QTextOption::WrapAtWordBoundaryOrAnywhere : QTextOption::NoWrap );
}