#include "LibraryPanelView.hpp"
#include <core/view/View.hpp>
#include <Library/JSONLibrary/LibraryWidget.hpp>
// Library shall have :
// - A global pane (system-wide)
// - A local pane (document-wide, saved in the panel)

namespace Library
{
static const iscore::DefaultPanelStatus status{false, Qt::RightDockWidgetArea, 0, QObject::tr("Library")};

const iscore::DefaultPanelStatus &LibraryPanelView::defaultPanelStatus() const
{ return status; }

LibraryPanelView::LibraryPanelView(QObject* parent):
    iscore::PanelView {parent},
    m_widget{new QTabWidget}
{
    auto projectLib = new LibraryWidget{m_widget};
    m_widget->addTab(projectLib, tr("Project"));

    auto systemLib = new LibraryWidget{m_widget};
    m_widget->addTab(systemLib, tr("System"));

    m_widget->setObjectName("LibraryExplorer");
}

QWidget* LibraryPanelView::getWidget()
{
    return m_widget;
}
}
