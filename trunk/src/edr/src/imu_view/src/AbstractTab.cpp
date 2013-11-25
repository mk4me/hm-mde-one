#include "MdePCH.h"
#include "AbstractTab.h"

AbstractTab::AbstractTab( const QIcon& icon, const QString& label ):
    icon(icon),
    label(label),
    active(false),
    enabled(true)
{

}


void AbstractTab::attachChildContext( IAppUsageContextPtr child )
{
    throw std::exception("The method or operation is not implemented.");
}

void AbstractTab::setEnabled( bool enabled )
{
    this->enabled = enabled;
}

bool AbstractTab::isEnabled() const
{
    return enabled;
}


QIcon AbstractTab::getIcon() const
{
    return icon;
}

QString AbstractTab::getLabel() const
{
    return label;
}

bool AbstractTab::isActive() const
{
    return active;
}

void AbstractTab::setActive( bool val )
{
    this->active = val;
}
