#include "SpaceLayerPresenter.hpp"
#include "SpaceLayerModel.hpp"
#include "SpaceLayerView.hpp"
#include "SpaceProcess.hpp"
#include <iscore/widgets/GraphicsItem.hpp>

#include <QGraphicsScene>
#include "Widgets/SpaceGuiWindow.hpp"
#include <iscore/document/DocumentInterface.hpp>
#include <core/document/Document.hpp>
#include <QMainWindow>

#include "src/Area/AreaFactory.hpp"
#include "src/Area/SingletonAreaFactoryList.hpp"

namespace Space
{
SpaceLayerPresenter::SpaceLayerPresenter(
        const Process::LayerModel& model,
        Process::LayerView* view,
        QObject* parent):
    LayerPresenter{"LayerPresenter", parent},
    m_model{static_cast<const Space::LayerModel&>(model)},
    m_view{static_cast<SpaceLayerView*>(view)},
    m_ctx{iscore::IDocument::documentContext(m_model.processModel())},
    m_focusDispatcher{m_ctx.document}
{
    const auto& procmodel = static_cast<Space::ProcessModel&>(m_model.processModel());
    m_spaceWindowView = new QMainWindow;
    m_spaceWindowView->setCentralWidget(
                new SpaceGuiWindow{
                    m_ctx,
                    procmodel,
                    m_spaceWindowView});

    connect(m_view, &SpaceLayerView::guiRequested,
            m_spaceWindowView, &QWidget::show);

    connect(m_view, &SpaceLayerView::contextMenuRequested,
            this, &LayerPresenter::contextMenuRequested);
    for(const auto& area : procmodel.areas)
    {
        on_areaAdded(area);
    }

    procmodel.areas.added.connect<SpaceLayerPresenter, &SpaceLayerPresenter::on_areaAdded>(this);
    procmodel.areas.removing.connect<SpaceLayerPresenter, &SpaceLayerPresenter::on_areaRemoved>(this);
    m_view->setEnabled(true);

    parentGeometryChanged();
}

SpaceLayerPresenter::~SpaceLayerPresenter()
{
    deleteGraphicsObject(m_view);
}

void SpaceLayerPresenter::setWidth(qreal width)
{
    m_view->setWidth(width);
    update();
}

void SpaceLayerPresenter::setHeight(qreal height)
{
    m_view->setHeight(height);
    update();
}
void SpaceLayerPresenter::on_focusChanged()
{

}

void SpaceLayerPresenter::putToFront()
{
    m_view->setFlag(QGraphicsItem::ItemStacksBehindParent, false);
    update();
}

void SpaceLayerPresenter::putBehind()
{
    m_view->setFlag(QGraphicsItem::ItemStacksBehindParent, true);
    update();
}

void SpaceLayerPresenter::on_zoomRatioChanged(ZoomRatio)
{
    //ISCORE_TODO;
    update();
}

void SpaceLayerPresenter::parentGeometryChanged()
{
    //ISCORE_TODO;
    update();
}

const Process::LayerModel &SpaceLayerPresenter::layerModel() const
{
    return m_model;
}

const Id<Process::ProcessModel> &SpaceLayerPresenter::modelId() const
{
    return m_model.processModel().id();
}

void SpaceLayerPresenter::update()
{
    m_view->update();
    for(auto& pres : m_areas)
    {
        pres.update();
    }
}

void SpaceLayerPresenter::on_areaAdded(const AreaModel & a)
{
    auto fact = m_ctx.app.components.factory<SingletonAreaFactoryList>().get(a.factoryKey());

    auto v = fact->makeView(m_view);
    // TODO call the factory list
    auto pres = fact->makePresenter(v, a, this);

    con(a, &AreaModel::areaChanged,
        pres, &AreaPresenter::on_areaChanged,
        Qt::QueuedConnection);
    m_areas.insert(pres);
    pres->on_areaChanged(a.currentMapping());
    update();
}

void SpaceLayerPresenter::on_areaRemoved(
        const AreaModel & a)
{
    auto& map = m_areas.get();
    auto it = map.find(a.id());
    if(it != map.end())
    {
        delete *it;
        map.erase(it);
    }

    update();
}


void SpaceLayerPresenter::fillContextMenu(
        QMenu* menu,
        const QPoint& pos,
        const QPointF& scenepos) const
{
    auto act = menu->addAction(tr("Show"));
    connect(act, &QAction::triggered, this, [&] () {
       m_spaceWindowView->show();
    });
}
}
