// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <JS/Executor/Component.hpp>
#include <JS/Executor/StateComponent.hpp>
#include <JS/Inspector/JSInspectorFactory.hpp>
#include <JS/JSProcessFactory.hpp>
#include <JS/JSStateProcess.hpp>
#include <score/tools/std/HashMap.hpp>

#include "JS/Commands/JSCommandFactory.hpp"
#include "score_plugin_js.hpp"
#include <Engine/Executor/DocumentPlugin.hpp>
#include <Inspector/InspectorWidgetFactoryInterface.hpp>
#include <Process/ProcessFactory.hpp>
#include <score/plugins/customfactory/FactoryFamily.hpp>
#include <score/plugins/customfactory/FactorySetup.hpp>
#include <score/plugins/customfactory/StringFactoryKey.hpp>
#include <score_plugin_js_commands_files.hpp>

score_plugin_js::score_plugin_js() : QObject{}
{
}

score_plugin_js::~score_plugin_js()
{
}

std::vector<std::unique_ptr<score::InterfaceBase>>
score_plugin_js::factories(
    const score::ApplicationContext& ctx,
    const score::InterfaceKey& key) const
{
  return instantiate_factories<score::ApplicationContext,
      FW<Process::ProcessModelFactory, JS::ProcessFactory>,
      FW<Process::LayerFactory, JS::LayerFactory>,
      FW<Process::StateProcessFactory, JS::StateProcessFactory>,
      FW<Inspector::InspectorWidgetFactory, JS::InspectorFactory, JS::StateInspectorFactory>,
      FW<Engine::Execution::ProcessComponentFactory, JS::Executor::ComponentFactory>,
      FW<Engine::Execution::StateProcessComponentFactory, JS::Executor::StateProcessComponentFactory>>(
      ctx, key);
}

std::pair<const CommandGroupKey, CommandGeneratorMap>
score_plugin_js::make_commands()
{
  using namespace JS;
  std::pair<const CommandGroupKey, CommandGeneratorMap> cmds{
      JS::CommandFactoryName(), CommandGeneratorMap{}};

  using Types = TypeList<
#include <score_plugin_js_commands.hpp>
      >;
  for_each_type<Types>(score::commands::FactoryInserter{cmds.second});

  return cmds;
}
