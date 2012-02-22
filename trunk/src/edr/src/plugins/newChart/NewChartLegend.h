/********************************************************************
	created:	2012/02/22
	created:	22:2:2012   10:30
	filename: 	NewChartLegend.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__NEWCHARTLEGEND_H__
#define HEADER_GUARD_NEW_CHART__NEWCHARTLEGEND_H__

class QCheckBox;
class QPushButton;

class NewChartLegendItem : public QWidget
{
    Q_OBJECT;
public:
    NewChartLegendItem(const QwtLegendData & data, QWidget* parent = nullptr);

public:
    bool isItemVisible();
    void setItemVisible(bool active);
    void setItemVisibleEnabled(bool enabled);

    bool isItemActive() const;
    void setItemActive(bool checked);
    void setItemActiveEnabled(bool enabled);
    void setData( const QwtLegendData& data );
    void setTooltip( const QString& val );

signals:
    void buttonClicked(bool);
    void checkClicked(bool);

private slots:
    void onClick(bool);

private:
    QCheckBox* check;
    QPushButton* button;
};

class NewChartLegend : public QwtLegend
{
    Q_OBJECT;
public:
    explicit NewChartLegend( QWidget *parent = NULL ):
    QwtLegend(parent)
    {
    }

protected:
    virtual QWidget *createWidget( const QwtLegendData & ) const;

signals:
    void checkboxChanged(const QwtPlotItem* item, bool active);

public slots:
    virtual void updateLegend( const QwtPlotItem *, const QList<QwtLegendData> & );
    virtual void updateWidget( QWidget* w, const QwtLegendData& data );

private slots:
     void onCheck(bool checked);

private:
    std::map<const QWidget*, const QwtPlotItem *> widget2PlotItem;
};


#endif
