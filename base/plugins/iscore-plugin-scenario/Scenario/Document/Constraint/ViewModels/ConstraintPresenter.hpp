#pragma once
#include <Process/TimeValue.hpp>
#include <Process/ZoomHelper.hpp>

#include <QPoint>
#include <QString>
#include <QTimer>
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore_plugin_scenario_export.h>
#include <nano_signal_slot.hpp>

namespace Process
{
struct ProcessPresenterContext;
}

namespace Scenario
{
class ConstraintHeader;
class ConstraintModel;
class ConstraintView;
class ConstraintViewModel;
class RackModel;
class RackPresenter;

class ISCORE_PLUGIN_SCENARIO_EXPORT ConstraintPresenter : public QObject,
                                                          public Nano::Observer
{
  Q_OBJECT

public:
  ConstraintPresenter(
      const ConstraintViewModel& model,
      ConstraintView* view,
      ConstraintHeader* header,
      const Process::ProcessPresenterContext& ctx,
      QObject* parent);
  virtual ~ConstraintPresenter();
  virtual void updateScaling();

  bool isSelected() const;

  RackPresenter* rack() const;

  const ConstraintModel& model() const;
  const ConstraintViewModel& abstractConstraintViewModel() const
  {
    return m_viewModel;
  }

  ConstraintView* view() const;

  void on_zoomRatioChanged(ZoomRatio val);
  ZoomRatio zoomRatio() const
  {
    return m_zoomRatio;
  }

  const Id<ConstraintModel>& id() const;

  void on_defaultDurationChanged(const TimeValue&);
  void on_minDurationChanged(const TimeValue&);
  void on_maxDurationChanged(const TimeValue&);

  void on_playPercentageChanged(double t);

  void on_rackShown(const OptionalId<RackModel>&);
  void on_rackHidden();
  void on_noRacks();

  void on_racksChanged(const RackModel&);
  void on_racksChanged();

  void updateHeight();

signals:
  void pressed(QPointF) const;
  void moved(QPointF) const;
  void released(QPointF) const;

  void askUpdate();
  void heightChanged();           // The vertical size
  void heightPercentageChanged(); // The vertical position

protected:
  // Process presenters are in the slot presenters.
  const ConstraintViewModel& m_viewModel;
  ConstraintView* m_view{};
  ConstraintHeader* m_header{};

  const Process::ProcessPresenterContext& m_context;

private:
  void updateChildren();
  void createRackPresenter(const RackModel&);
  void clearRackPresenter();
  void on_rackRemoved(const RackModel&);

  void updateBraces();

  ZoomRatio m_zoomRatio{};
  RackPresenter* m_rack{};
};

// TODO concept: constraint view model.

template <typename T>
const typename T::viewmodel_type* viewModel(const T* obj)
{
  return static_cast<const typename T::viewmodel_type*>(
      &obj->abstractConstraintViewModel());
}

template <typename T>
const typename T::view_type* view(const T* obj)
{
  return static_cast<const typename T::view_type*>(obj->view());
}

template <typename T>
typename T::view_type* view(T* obj)
{
  return static_cast<typename T::view_type*>(obj->view());
}

template <typename T>
const typename T::viewmodel_type& viewModel(const T& obj)
{
  return static_cast<const typename T::viewmodel_type&>(
      obj.abstractConstraintViewModel());
}

template <typename T>
typename T::view_type& view(const T& obj)
{
  return static_cast<typename T::view_type&>(*obj.view());
}

template <typename T>
typename T::view_type& view(T& obj)
{
  return static_cast<typename T::view_type&>(*obj.view());
}
}
