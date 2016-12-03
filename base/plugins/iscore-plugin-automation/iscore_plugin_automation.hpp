#pragma once
#include <QObject>
#include <iscore/plugins/qt_interfaces/CommandFactory_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/GUIApplicationContextPlugin_QtInterface.hpp>
#include <iscore/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>
#include <utility>
#include <vector>

#include <iscore/application/ApplicationContext.hpp>
#include <iscore/command/CommandGeneratorMap.hpp>
#include <iscore/command/SerializableCommand.hpp>
#include <iscore/plugins/customfactory/FactoryInterface.hpp>
/**
 * @namespace Automation
 * @brief Namespace related to the Automation process
 *
 * This namespace contains the Automation process, layer, and related edition
 * commands.
 *
 * The Automation is a process that sets the value of a parameter across time.
 * It is related to the \ref ossia::automation process.
 *
 *
 */
class iscore_plugin_automation final
    : public QObject,
      public iscore::Plugin_QtInterface,
      public iscore::FactoryInterface_QtInterface,
      public iscore::CommandFactory_QtInterface
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID FactoryInterface_QtInterface_iid)
  Q_INTERFACES(iscore::Plugin_QtInterface iscore::FactoryInterface_QtInterface
                   iscore::CommandFactory_QtInterface)

public:
  iscore_plugin_automation();
  virtual ~iscore_plugin_automation();

private:
  std::vector<std::unique_ptr<iscore::FactoryInterfaceBase>> factories(
      const iscore::ApplicationContext& ctx,
      const iscore::AbstractFactoryKey& factoryName) const override;
  std::pair<const CommandParentFactoryKey, CommandGeneratorMap>
  make_commands() override;

  iscore::Version version() const override;
  UuidKey<iscore::Plugin> key() const override;
};
