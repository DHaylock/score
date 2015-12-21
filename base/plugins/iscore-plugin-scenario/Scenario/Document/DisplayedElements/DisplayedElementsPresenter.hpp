#pragma once
#include <Process/TimeValue.hpp>
#include <Process/ZoomHelper.hpp>
#include <Scenario/Document/BaseScenario/BaseScenarioPresenter.hpp>
#include <QObject>

#include <vector>

class BaseGraphicsObject;
class ConstraintModel;
class DisplayedElementsModel;
class FullViewConstraintPresenter;
class LayerPresenter;
class ScenarioDocumentPresenter;

// Contains the elements that are shown (not necessarily the ones in
// BaseScenarioModel)
class DisplayedElementsPresenter final :
        public QObject,
        public BaseScenarioPresenter<DisplayedElementsModel, FullViewConstraintPresenter>
{
        Q_OBJECT
    public:
        DisplayedElementsPresenter(ScenarioDocumentPresenter* parent);
        using QObject::event;
        using BaseScenarioPresenter<DisplayedElementsModel, FullViewConstraintPresenter>::event;

        BaseGraphicsObject& view() const;

        void on_displayedConstraintChanged(const ConstraintModel &m);
        void showConstraint();

        void on_zoomRatioChanged(ZoomRatio r);

        void on_displayedConstraintDurationChanged(TimeValue);
        void on_displayedConstraintHeightChanged(double);

    signals:
        void requestFocusedPresenterChange(LayerPresenter*);

    private:
        void updateLength(double);

        ScenarioDocumentPresenter* m_model{};

        std::vector<QMetaObject::Connection> m_connections;
};
