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
#include <boost/range.hpp>
#include <plugins/c3d/C3DChannels.h>

#include <QtGui/QItemDelegate>

//! klasa powstala, aby wymusic wielko�� wpisow, dzi�ki temu mo�na �atwo obliczyc potrzebna wysoko��
class ItemDelegate : public QItemDelegate
{
public:
    //! Konstruktor wymagajacy podania rozmiaru
    //! \param width szeroko�� 
    //! \param height wysoko��
    ItemDelegate(int width, int height) : width(width), height(height) {}
    //! metoda wykorzystywana przez Qt przy ustalaniu wielko�ci elementow
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const
    {
        return QSize(width, height);
    }

    //! 
    int getWidth() const { return width; }
    //! 
    //! \param val 
    void setWidth(int val) { width = val; }
    //! 
    int getHeight() const { return height; }
    //! 
    //! \param val 
    void setHeight(int val) { height = val; }

private:
    //! szeroko��
    int width;
    //! wysoko��
    int height;
};

//! wy�wietla statystyki wykresow w postaci pogrupowanego drzewa wpisow.
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
    //! \param group nazwa grupy (np. strona lewa, prawa ...), je�li grupa nie istnieje, to zostanie stworzona, je�li "" to grupa jest pominieta.
    //! \param name nazwa wpisu 
    //! \param stats obiekt ze statystykami
    //! \param backgroundColor kolor t�a dla dodawanego elementu
    //! \return utworzony element drzwa
    QTreeWidgetItem* addEntry(const QString& group, const QString& name, ScalarChannelStatsConstPtr stats, const QColor& backgroundColor = Qt::white);
    //! pr�buje pobra� wpis ze statystykami
    //! \param group nazwa grupy (np. strona lewa, prawa ...)
    //! \param name nazwa wpisu 
    //! \return element drzewa z wpisem lub nullptr
    QTreeWidgetItem* tryGetEntry(const QString& group, const QString& name);
    //! Pobiera wszystkie wpisy zwi�zane z konkretynymi statystykami
    //! \param stats statystyki, dla kt�rych maja by� pobrane wpisy
    //! \return obiekt boost::range z wpisami
    range getEntries(ScalarChannelStatsConstPtr stats);
    //! Pobiera wszystkie wpisy zwi�zane z konkretynym kana�em
    //! \param channel kana� dla kt�rego maja by� pobrane wpisy
    //! \return lista z elementami drzewa
    std::list<QTreeWidgetItem*> getEntriesByChannel(channelConstPtr channel);
    //! \return wszytskie grupy wpisow (np. strona lewa, prawa ...)
    QStringList getGroups() const;
    //! dany element staje si� aktywny (rozwiniety, pozostale na jego poziome s� zwijane)
    //! \param treeItem obiekt, kt�ry ma stac si� aktywny
    void setActive(QTreeWidgetItem* treeItem);
    //! dany element staje si� aktywny (rozwiniety, pozostale na jego poziome s� zwijane)
    //! \param group nazwa grupy (np. strona lewa, prawa ...)
    //! \param name nazwa wpisu 
    void setActive(const QString& group, const QString& name);
    //! czysci / resetuje obiekt
    void clear();
    //! oblicza na nowo wymagana wysoko��
    void recalculateHeight();
    
private:
    //! wysoko�� wiersza
    int rowHeight;
    //! mapa statystyki -> wpis z drzewie ze statystykami
    std::multimap<ScalarChannelStatsConstPtr, QTreeWidgetItem*> stats2TreeItems;
    ItemDelegate treeItemDelegate;
};
typedef core::shared_ptr<StatsTable> StatsTablePtr;
typedef core::shared_ptr<const StatsTable> StatsTableConstPtr;


#endif
