#include "PCH.h"
#include <osgui/StyleEx.h>
#include <osgui/Buttonized.h>

////////////////////////////////////////////////////////////////////////////////
namespace osgui {
////////////////////////////////////////////////////////////////////////////////

bool StyleEx::applyStyle(osgWidget::Widget* w, osgWidget::Reader r) {

	if(Style::applyStyle(w,r) == true){
		return true;
	}

	return handleExWidgets(w,r);
}

bool StyleEx::applyStyle(osgWidget::Label* label, osgWidget::Reader r) {
	if(Style::applyStyle(label,r) == true){
		return true;
	}

	if(applyStyle((osgWidget::Widget*)label,r) == true){
		return true;
	}

	std::string str;
	osg::Vec3   vec3;
	osg::Vec4   vec4;
	float       f;
	bool ret = true;

	// Font color using 4x 0-255 integers.
	if(_match("font_color %i %i %i %i", r)) {
		r.readSequence(vec4);
		label->setFontColor(vec4 / 255.0f);
	}

	// Font color using 3x 0-255 integers with a default alpha of 255.
	else if(_match("font_color %i %i %i", r)) {
		r.readSequence(vec3);
		label->setFontColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
	}

	// Font color using 4x 0.0f-1.0f floats.
	else if(_match("font_color %f %f %f %f", r)) {
		r.readSequence(vec4);
		label->setFontColor(vec4);
	}

	// Font color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
	else if(_match("font_color %f %f %f", r)) {
		r.readSequence(vec3);
		label->setFontColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
	}

	// Font size using int or float.
	else if(_match("font_size %i", r) || _match("font_size %f", r)) {
		r.readSequence(f);
		//label->setSize(0,0);
		label->setFontSize(f);
	}

	// Font type using string.
	else if(_match("font_type %s", r)) {
		r.readSequence(str);
		//label->setSize(0,0);
		label->setFont(str);
	}

	//fit to font settings
	else if(_match("font_fit %w", r)) {
		r.readSequence(str);
		
		LABEL_FONT_FIT_MODE ffmode = strToFontFit(str);

		osgWidget::XYCoord textSize = label->getTextSize();

		switch(ffmode){
		case HORIZONTAL_FIT:
			label->setHeight(textSize.y());
			label->addOrigin(0,0);
			if(label->getParent() != 0){
				label->getParent()->resize();
			}
			break;
		case VERTICAL_FIT:
			label->setWidth(textSize.x());
			label->addOrigin(0,0);
			if(label->getParent() != 0){
				label->getParent()->resize();
			}
			break;
		case ALL_FIT:
			label->setSize(textSize);
			label->addOrigin(0,0);
			if(label->getParent() != 0){
				label->getParent()->resize();
			}
			break;
		}
	}

	//text alligment based on osgText::Text capabilities
	else if(label->getText() != 0 && _match("font_align %w", r)) {
		r.readSequence(str);

		//TODO FIX
		//label->getText()->setAlignment(strToFontAligment(str));
		//label->setLabel(label->getLabel());
		//label->setSize(label->getTextSize());
	}

	// Font handled - continue
	else{
		ret = false;
	}

	if(ret == false){
		return handleExWidgets(label,r);
	}else{
		return true;
	}

}

bool StyleEx::applyStyle(osgWidget::Input* input, osgWidget::Reader r) {
	if(Style::applyStyle(input,r) == true){
		return true;
	}

	return handleExWidgets(input,r);
}

bool StyleEx::applyStyle(osgWidget::Window* window, osgWidget::Reader r){
	if(osgWidget::Style::applyStyle(window, r) == true){
		return true;
	}

	return handleExWindows(window, r);
}

StyleEx::LABEL_FONT_FIT_MODE StyleEx::strToFontFit(const std::string & fontFit){
	std::string ff = osgWidget::lowerCase(fontFit);

	if(ff == "vertical") return VERTICAL_FIT;

	else if(ff == "horizontal") return HORIZONTAL_FIT;

	else if(ff == "all") return ALL_FIT;

	else {
		osgWidget::warn()
			<< "Unkown FontFit mode [" << ff
			<< "]; using none." << std::endl
			;

		return NONE_FIT;
	}
}

osgText::Text::AlignmentType StyleEx::strToFontAligment(const std::string & fontAlign){
	std::string fa = osgWidget::lowerCase(fontAlign);

	if(fa == "left_top"){
		return osgText::Text::LEFT_TOP;
	}else if(fa == "left_center"){
		return osgText::Text::LEFT_CENTER;
	}else if(fa == "left_bottom"){
		return osgText::Text::LEFT_BOTTOM;
	}else if(fa == "center_top"){
		return osgText::Text::CENTER_TOP;
	}else if(fa == "center_center"){
		return osgText::Text::CENTER_CENTER;
	}else if(fa == "center_bottom"){
		return osgText::Text::CENTER_BOTTOM;
	}else if(fa == "right_top"){
		return osgText::Text::RIGHT_TOP;
	}else if(fa == "right_center"){
		return osgText::Text::RIGHT_CENTER;
	}else if(fa == "right_bottom"){
		return osgText::Text::RIGHT_BOTTOM;
	}else if(fa == "left_base_line"){
		return osgText::Text::LEFT_BASE_LINE;
	}else if(fa == "center_base_line"){
		return osgText::Text::CENTER_BASE_LINE;
	}else if(fa == "right_base_line"){
		return osgText::Text::RIGHT_BASE_LINE;
	}else if(fa == "left_bottom_base_line"){
		return osgText::Text::LEFT_BOTTOM_BASE_LINE;
	}else if(fa == "center_bottom_base_line"){
		return osgText::Text::CENTER_BOTTOM_BASE_LINE;
	}else if(fa == "right_bottom_base_line"){
		return osgText::Text::RIGHT_BOTTOM_BASE_LINE;
	}else if(fa == "base_line"){
		return osgText::Text::BASE_LINE;
	}else{
		osgWidget::warn()
			<< "Unkown font_align mode [" << fa
			<< "]; using base_line." << std::endl
			;

		return osgText::Text::BASE_LINE;
	}
}

bool StyleEx::handleExWindows(osgWidget::Window* window, osgWidget::Reader r){
	if(window->getBackground() != 0){
		return applyStyle(window->getBackground(), r);
	}

	return false;
}


bool StyleEx::handleExWidgets(osgWidget::Widget* w, osgWidget::Reader r){
	std::string str;
	bool ret = true;

	//use optimized image setting method
	if(_match("rect_image %s", r)) {
		r.readSequence(str);
		w->setImage(str, true, true);
	}else{
		osg::Vec2   vec2;
		osg::Vec3   vec3;
		osg::Vec4   vec4;
		float       f;

		if(dynamic_cast<IBorderized*>(w) != 0){
			IBorderized* borderized = dynamic_cast<IBorderized*>(w);
			// Border color using 4x 0-255 integers.
			if(_match("border_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				borderized->setBorderColor(vec4 / 255.0f);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("border_color %i %i %i", r)) {
				r.readSequence(vec3);

				borderized->setBorderColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("border_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				borderized->setBorderColor(vec4);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("border_color %f %f %f", r)) {
				r.readSequence(vec3);
				borderized->setBorderColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			// Border thickness using int or float
			else if(_match("border_width %i", r) || _match("border_width %f", r)) {
				r.readSequence(f);
				borderized->setBorderWidth(f);
			}

			//did not find anything - continue
			else{
				ret = false;
			}

			//was not an extended window - continue
		}
		
		if(ret == false && dynamic_cast<osgui::DefaultStylePolicy*>(w) != 0){
			osgui::DefaultStylePolicy* def = dynamic_cast<osgui::DefaultStylePolicy*>(w);
			ret = true;

			// Border color using 4x 0-255 integers.
			if(_match("hover_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				def->setHoverColor(vec4 / 255.0f);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("hover_color %i %i %i", r)) {
				r.readSequence(vec3);

				def->setHoverColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("hover_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				def->setHoverColor(vec4);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("hover_color %f %f %f", r)) {
				r.readSequence(vec3);
				def->setHoverColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			else if(_match("toggle_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				def->setToggleColor(vec4 / 255.0f);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("toggle_color %i %i %i", r)) {
				r.readSequence(vec3);

				def->setToggleColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("toggle_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				def->setToggleColor(vec4);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("toggle_color %f %f %f", r)) {
				r.readSequence(vec3);
				def->setToggleColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			else if(_match("pushed_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				def->setPushedColor(vec4 / 255.0f);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("pushed_color %i %i %i", r)) {
				r.readSequence(vec3);

				def->setPushedColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("pushed_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				def->setPushedColor(vec4);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("pushed_color %f %f %f", r)) {
				r.readSequence(vec3);
				def->setPushedColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			else if(_match("normal_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				def->setNormalColor(vec4 / 255.0f);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("normal_color %i %i %i", r)) {
				r.readSequence(vec3);
				def->setNormalColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("normal_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				def->setNormalColor(vec4);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("normal_color %f %f %f", r)) {
				r.readSequence(vec3);
				def->setNormalColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			//did not find anything - continue
			else{
				ret = false;
			}
			
		}
		
		if(ret == false && dynamic_cast<osgui::LabelStylePolicy*>(w) != 0){
			osgui::LabelStylePolicy* lab = dynamic_cast<osgui::LabelStylePolicy*>(w);

			ret = true;

			if(_match("label_hover_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				lab->setLabelHoverColor(vec4 / 255.0f);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("label_hover_color %i %i %i", r)) {
				r.readSequence(vec3);
				lab->setLabelHoverColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("label_hover_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				lab->setLabelHoverColor(vec4);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("label_hover_color %f %f %f", r)) {
				r.readSequence(vec3);
				lab->setLabelHoverColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			else if(_match("label_toggle_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				lab->setLabelToggleColor(vec4 / 255.0f);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("label_toggle_color %i %i %i", r)) {
				r.readSequence(vec3);
				lab->setLabelToggleColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("label_toggle_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				lab->setLabelToggleColor(vec4);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("label_toggle_color %f %f %f", r)) {
				r.readSequence(vec3);
				lab->setLabelToggleColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			else if(_match("label_pushed_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				lab->setLabelPushedColor(vec4 / 255.0f);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("label_pushed_color %i %i %i", r)) {
				r.readSequence(vec3);
				lab->setLabelPushedColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("label_pushed_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				lab->setLabelPushedColor(vec4);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("label_pushed_color %f %f %f", r)) {
				r.readSequence(vec3);
				lab->setLabelPushedColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			else if(_match("label_normal_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				lab->setLabelNormalColor(vec4 / 255.0f);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("label_normal_color %i %i %i", r)) {
				r.readSequence(vec3);
				lab->setLabelNormalColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("label_normal_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				lab->setLabelNormalColor(vec4);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("label_normal_color %f %f %f", r)) {
				r.readSequence(vec3);
				lab->setLabelNormalColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			//did not find anything - continue
			else{
				ret = false;
			}
		}

		if(ret == false && dynamic_cast<osgui::IBorderStylePolicy*>(w) != 0){
			osgui::IBorderStylePolicy* bord = dynamic_cast<osgui::IBorderStylePolicy*>(w);

			ret = true;

			if(_match("border_hover_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				bord->setBorderHoverColor(vec4 / 255.0f);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("border_hover_color %i %i %i", r)) {
				r.readSequence(vec3);
				bord->setBorderHoverColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("border_hover_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				bord->setBorderHoverColor(vec4);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("border_hover_color %f %f %f", r)) {
				r.readSequence(vec3);
				bord->setBorderHoverColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			else if(_match("border_toggle_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				bord->setBorderToggleColor(vec4 / 255.0f);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("border_toggle_color %i %i %i", r)) {
				r.readSequence(vec3);
				bord->setBorderToggleColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("border_toggle_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				bord->setBorderToggleColor(vec4);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("border_toggle_color %f %f %f", r)) {
				r.readSequence(vec3);
				bord->setBorderToggleColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			else if(_match("border_pushed_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				bord->setBorderPushedColor(vec4 / 255.0f);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("border_pushed_color %i %i %i", r)) {
				r.readSequence(vec3);
				bord->setBorderPushedColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("border_pushed_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				bord->setBorderPushedColor(vec4);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("border_pushed_color %f %f %f", r)) {
				r.readSequence(vec3);
				bord->setBorderPushedColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			else if(_match("border_normal_color %i %i %i %i", r)) {
				r.readSequence(vec4);
				bord->setBorderNormalColor(vec4 / 255.0f);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("border_normal_color %i %i %i", r)) {
				r.readSequence(vec3);
				bord->setBorderNormalColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 255.0f) / 255.0f);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("border_normal_color %f %f %f %f", r)) {
				r.readSequence(vec4);
				bord->setBorderNormalColor(vec4);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("border_normal_color %f %f %f", r)) {
				r.readSequence(vec3);
				bord->setBorderNormalColor(osg::Vec4(vec3[0], vec3[1], vec3[2], 1.0f));
			}

			//did not find anything - continue
			else{
				ret = false;
			}
		}
		
		if(ret == false && dynamic_cast<osgui::TextStylePolicy*>(w) != 0){
			osgui::TextStylePolicy* txt = dynamic_cast<osgui::TextStylePolicy*>(w);

			ret = true;

			if(_match("hover_style %s", r)) {
				r.readSequence(str);
				txt->setHoverStyle(str);
			}

			// Border color using 3x 0-255 integers with a default alpha of 255.
			else if(_match("toggle_style %s", r)) {
				r.readSequence(str);
				txt->setToggleStyle(str);
			}

			// Border color using 4x 0.0f-1.0f floats.
			else if(_match("pushed_style %s", r)) {
				r.readSequence(str);
				txt->setPushedStyle(str);
			}

			// Border color using 3x 0.0f-1.0f floats with a default alpha of 1.0f.
			else if(_match("normal_style %s", r)) {
				r.readSequence(str);
				txt->setNormalStyle(str);
			}

			else{
				ret = false;
			}
		}else{
			ret = false;
		}
	}

	return ret;
}

}