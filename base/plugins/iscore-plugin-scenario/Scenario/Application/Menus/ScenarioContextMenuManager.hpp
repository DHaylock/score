#pragma once
#include <QObject>

namespace Process { class LayerPresenter; }
class QMenu;
class QPoint;
class QPointF;
class SlotPresenter;
class TemporalScenarioPresenter;
class QAction;

namespace iscore
{
struct DocumentContext;
}

class ScenarioContextMenuManager final : public QObject
{
    public:
        static void createSlotContextMenu(
                const iscore::DocumentContext& docContext,
                QMenu& menu,
                const SlotPresenter& slotp);

        static void createScenarioContextMenu(
                const iscore::DocumentContext& ctx,
                QMenu& menu,
                const QPoint& pos,
                const QPointF& scenepos,
                const TemporalScenarioPresenter& pres);

        static void createLayerContextMenu(
                QMenu& menu,
                const QPoint& pos,
                const QPointF& scenepos,
                const Process::LayerPresenter& pres);

};
