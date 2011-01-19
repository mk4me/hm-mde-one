#ifndef CONSOLE_WIDGET_H
#define CONSOLE_WIDGET_H

#include <QtGui/QWidget>

#include "ui_ConsoleWidget.h"
//#include "Logger.h"

class ConsoleWidget: public QWidget, Ui::ConsoleWidget
{
  Q_OBJECT
public:
  ConsoleWidget(void);
  ~ConsoleWidget(void);

  void print(const std::string& str); 

public slots: 
  void onEcho();
};

#endif