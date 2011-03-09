#include "PCH.h"
#include <osgui/Tooltip.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

Tooltip::Tooltip(void) : osgWidget::Box("",osgWidget::Box::VERTICAL, true), tooltipLabel(new BLABEL())
{
	tooltipLabel->setBorderWidth(0);
	setStyle("osgui.tooltip.base");
	tooltipLabel->setStyle("osgui.tooltip.text");
	setStrata(osgWidget::Window::STRATA_FOREGROUND);

	onEnterWidgetCallback = new osgWidget::Callback(&Tooltip::onEnterWidget, this, osgWidget::EVENT_MOUSE_ENTER);
	onEnterWindowCallback = new osgWidget::Callback(&Tooltip::onEnterWindow, this, osgWidget::EVENT_MOUSE_ENTER);
	onLeaveWidgetCallback = new osgWidget::Callback(&Tooltip::onLeaveWidget, this, osgWidget::EVENT_MOUSE_LEAVE);
	onLeaveWindowCallback = new osgWidget::Callback(&Tooltip::onLeaveWindow, this, osgWidget::EVENT_MOUSE_LEAVE);

	onDragWidgetCallback = new osgWidget::Callback(&Tooltip::onDragWidget, this, osgWidget::EVENT_MOUSE_DRAG);
	onDragWindowCallback = new osgWidget::Callback(&Tooltip::onDragWindow, this, osgWidget::EVENT_MOUSE_DRAG);
}


Tooltip::~Tooltip(void)
{
}

void Tooltip::managed(osgWidget::WindowManager * wm){
	osgWidget::Box::managed(wm);
	if(tooltipLabel->getParent() != this){
		addWidget(tooltipLabel);
	}

	hide();
}

void Tooltip::setText(const std::string & text){
	tooltipLabel->setLabel(text);
}

std::string Tooltip::getText() const{
	return tooltipLabel->getLabel();
}

bool Tooltip::isEmpty() const{
	return tooltipLabel->getLabel().empty() == true;
}

void Tooltip::clear(){
	tooltipLabel->setLabel("");
}

void Tooltip::show(const osgWidget::XYCoord & pos){
	setOrigin(pos);
	//resize();
	tooltipLabel->setStyle("osgui.tooltip.text");
	if(getWindowManager() != 0 && getWindowManager()->getStyleManager() != 0){
		getWindowManager()->getStyleManager()->applyStyles(this);
		getWindowManager()->getStyleManager()->applyStyles(tooltipLabel);
	}
	osgWidget::Box::show();

}

void Tooltip::show(osgWidget::point_type x, osgWidget::point_type y){
	show(osgWidget::XYCoord(x,y));
}

void Tooltip::registerItem(osgWidget::Widget * widget){
	if(registeredWidgets.find(widget) == registeredWidgets.end()){
		registeredWidgets.insert(widget);
		widget->addEventMask(osgWidget::EVENT_MASK_MOUSE_MOVE | osgWidget::EVENT_MASK_MOUSE_DRAG);
		widget->addCallback(onEnterWidgetCallback);
		widget->addCallback(onLeaveWidgetCallback);
		widget->addCallback(onDragWidgetCallback);
	}
}

void Tooltip::registerItem(osgWidget::Window * window){
	if(registeredWindows.find(window) == registeredWindows.end()){
		registeredWindows.insert(window);
		window->addEventMask(osgWidget::EVENT_MASK_MOUSE_MOVE | osgWidget::EVENT_MASK_MOUSE_DRAG);
		window->addCallback(onEnterWindowCallback);
		window->addCallback(onLeaveWindowCallback);
		window->addCallback(onDragWindowCallback);
	}
}

void Tooltip::unregisterItem(osgWidget::Widget * widget){
	if(registeredWidgets.erase(widget) > 0){
		if(currentWidget == widget){
			hide();
			currentWidget = 0;
		}
	}
}

void Tooltip::unregisterItem(osgWidget::Window * window){
	if(registeredWindows.erase(window) > 0){
		if(currentWindow == window){
			hide();
			currentWindow = 0;
		}
	}
}

bool Tooltip::onEnterWidget(osgWidget::Event& ev){
	if(registeredWidgets.find(ev.getWidget()) != registeredWidgets.end()){
		currentWidget = ev.getWidget();
		//show tooltip
		osgWidget::XYCoord pos = currentWidget->getParent()->getAbsoluteOrigin() + currentWidget->getOrigin();
		pos.x() += currentWidget->getWidth();
		pos.y() += currentWidget->getHeight();
		show(pos);
	}

	return false;
}

bool Tooltip::onEnterWindow(osgWidget::Event& ev){
	if(registeredWindows.find(ev.getWindow()) != registeredWindows.end()){
		currentWindow = ev.getWindow();
		//show tooltip
		osgWidget::XYCoord pos = currentWindow->getAbsoluteOrigin();
		pos.x() += currentWindow->getWidth();
		pos.y() += currentWindow->getHeight();
		show(pos);
	}

	return false;
}

bool Tooltip::onLeaveWidget(osgWidget::Event& ev){
	if(registeredWidgets.find(ev.getWidget()) != registeredWidgets.end()){
		//hide tooltip
		hide();
		currentWidget = 0;
	}

	return false;
}

bool Tooltip::onLeaveWindow(osgWidget::Event& ev){
	if(registeredWindows.find(ev.getWindow()) != registeredWindows.end()){
		//hide tooltip
		hide();
		currentWindow = 0;
	}

	return false;
}

bool Tooltip::onDragWidget(osgWidget::Event& ev){
	if(isVisible() ==  true && registeredWidgets.find(ev.getWidget()) != registeredWidgets.end()){
		//update position
		osgWidget::XYCoord pos = currentWidget->getParent()->getAbsoluteOrigin() + currentWidget->getOrigin();
		pos.x() += currentWidget->getWidth();
		pos.y() += currentWidget->getHeight();
		setOrigin(pos);
		resize();
	}

	return false;
}

bool Tooltip::onDragWindow(osgWidget::Event& ev){
	if(isVisible() ==  true && registeredWindows.find(ev.getWindow()) != registeredWindows.end()){
		osgWidget::XYCoord pos = currentWindow->getAbsoluteOrigin();
		pos.x() += currentWindow->getWidth();
		pos.y() += currentWindow->getHeight();
		setOrigin(pos);
		resize();
	}

	return false;
}

}