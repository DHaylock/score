#pragma once
#include <Inspector/InspectorWidgetFactoryInterface.hpp>
#include <QList>

namespace Scenario
{
class ScenarioInspectorWidgetFactoryWrapper final
    : public Inspector::InspectorWidgetFactory
{
  SCORE_CONCRETE("066fffc1-c82c-4ffd-ad7c-55a65bfa067f")
public:
  ScenarioInspectorWidgetFactoryWrapper() : InspectorWidgetFactory{}
  {
  }

  QWidget* make(
      const QList<const QObject*>& sourceElements,
      const score::DocumentContext& doc,
      QWidget* parent) const override;

  bool matches(const QList<const QObject*>& objects) const override;
};
}
