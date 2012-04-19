/********************************************************************
	created:	2011/12/06
	created:	6:12:2011   10:34
	filename: 	StatsTable.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEW_CHART__STATSTABLE_H__
#define HEADER_GUARD_NEW_CHART__STATSTABLE_H__

#include "ui_statsTable.h"
#include <plugins/c3d/C3DChannels.h>

//! Wyswietla statystyki wykresow w postaci pogrupowanego drzewa wpisow.
//! Widget normalnie jest zwiniety, rozwijanie nastepuje po nacisnieciu przycisku.
class StatsTable : public QWidget, private Ui::StatsTable
{
    Q_OBJECT;
public:
    typedef std::multimap<ScalarChannelStatsConstPtr, QTreeWidgetItem*> statsMultimap;
    typedef boost::iterator_range<statsMultimap::iterator> range;
    typedef boost::iterator_range<statsMultimap::const_iterator> const_range;
    typedef ScalarChannelStatsConstPtr::element_type::ChannelConstPtr channelConstPtr;

public:
    StatsTable(QWidget* parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~StatsTable() {}

public slots:
    //! Dodanie wpisu ze statystykami
    //! \param group nazwa grupy (np. strona lewa, prawa ...), jesli grupa nie istnieje, to zostanie stworzona, jesli "" to grupa jest pominieta.
    //! \param name nazwa wpisu 
    //! \param stats obiekt ze statystykami
    //! \param backgroundColor kolor tla dla dodawanego elementu
    //! \return utworzony element drzwa
    QTreeWidgetItem* addEntry(const QString& group, const QString& name, ScalarChannelStatsConstPtr stats, const QColor& backgroundColor = Qt::white);

    QTreeWidgetItem* tryGetEntry(const QString& group, const QString& name);

    range getEntries(ScalarChannelStatsConstPtr stats);
    std::list<QTreeWidgetItem*> getEntriesByChannel(channelConstPtr channel);
    QStringList getGroups() const;
    void setActive(QTreeWidgetItem* treeItem);
    void setActive(const QString& group, const QString& name);
    void clear();
    void recalculateHeight();
    
private:
    int rowHeight;
    std::multimap<ScalarChannelStatsConstPtr, QTreeWidgetItem*> stats2TreeItems;
};
typedef core::shared_ptr<StatsTable> StatsTablePtr;
typedef core::shared_ptr<const StatsTable> StatsTableConstPtr;


#endif
