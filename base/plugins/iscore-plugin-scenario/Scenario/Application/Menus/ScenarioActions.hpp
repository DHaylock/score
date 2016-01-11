#pragma once
#include <iscore/menu/MenuInterface.hpp>
#include <iscore/selection/Selection.hpp>
#include <QList>
#include <QObject>
#include <QPoint>
#include <iscore_plugin_scenario_export.h>
class QAction;
class QMenu;
class QToolBar;
namespace iscore {
class MenubarManager;
}  // namespace iscore

namespace Scenario
{
class ScenarioApplicationPlugin;
class TemporalScenarioPresenter;
class ISCORE_PLUGIN_SCENARIO_EXPORT ScenarioActions : public QObject
{
        Q_OBJECT

    public:
        explicit ScenarioActions(iscore::ToplevelMenuElement, ScenarioApplicationPlugin *);
        virtual ~ScenarioActions();

        virtual void fillMenuBar(iscore::MenubarManager*) = 0;
        virtual void fillContextMenu(
                QMenu*,
                const Selection& s,
                const TemporalScenarioPresenter& pres,
                const QPoint&,
                const QPointF&) = 0;
        virtual bool populateToolBar(QToolBar* ) { return false; }
        virtual void setEnabled(bool) = 0;

        // TODO reimplement me for all the classes
        virtual QList<QAction*> actions() const;

    protected:
        ScenarioApplicationPlugin* m_parent{};
        iscore::ToplevelMenuElement m_menuElt;
};
}
