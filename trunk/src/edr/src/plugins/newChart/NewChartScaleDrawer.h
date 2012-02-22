/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:22
	filename: 	NewChartScaleDrawer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTSCALEDRAWER_H__

class PercentScaleDraw : public QwtScaleDraw
{
public:
    PercentScaleDraw(double left, double right, bool percentMode = false) :
      QwtScaleDraw(),
          left(left),
          right(right),
          percentMode(percentMode)
      {
      }

public:
    void setLeftRightValues(double left, double right)
    {
        this->left = left;
        this->right = right;
    }
    bool isPercentMode() const { return percentMode; }
    void setPercentMode(bool val) { percentMode = val; }
    QwtScaleDiv getScaleDiv();

protected:
    void drawLabel( QPainter *painter, double value ) const;

private:
    double left;
    double right;
    bool percentMode;
};

#endif
