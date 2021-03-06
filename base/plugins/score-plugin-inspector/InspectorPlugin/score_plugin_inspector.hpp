#pragma once
#include <QObject>
#include <score/plugins/qt_interfaces/FactoryFamily_QtInterface.hpp>
#include <score/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <score/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>
#include <vector>

namespace score
{
class InterfaceListBase;
class PanelFactory;
} // namespace score

// RENAMEME
class score_plugin_inspector : public QObject,
                                public score::Plugin_QtInterface,
                                public score::FactoryInterface_QtInterface,
                                public score::FactoryList_QtInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID FactoryInterface_QtInterface_iid)
  Q_INTERFACES(score::Plugin_QtInterface score::FactoryInterface_QtInterface
                   score::FactoryList_QtInterface)

  SCORE_PLUGIN_METADATA(1, "0ed1520f-e120-458e-a5a8-b3f05f3b6b6c")
public:
  score_plugin_inspector();
  ~score_plugin_inspector();

  // Panel interface
  std::vector<std::unique_ptr<score::InterfaceBase>> factories(
      const score::ApplicationContext&,
      const score::InterfaceKey& factoryName) const override;

  // Factory for inspector widgets
  std::vector<std::unique_ptr<score::InterfaceListBase>>
  factoryFamilies() override;
};
