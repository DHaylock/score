#include <Scenario/iscore_plugin_scenario.hpp>
#include <unordered_map>
#include <utility>

#include <iscore/plugins/customfactory/StringFactoryKeySerialization.hpp>
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <iscore_plugin_scenario_commands_files.hpp>
#include <iscore/command/CommandGeneratorMap.hpp>
#include <iscore/command/SerializableCommand.hpp>

std::pair<const CommandParentFactoryKey, CommandGeneratorMap> iscore_plugin_scenario::make_commands()
{
    using namespace Scenario;
    using namespace Scenario::Command;
    std::pair<const CommandParentFactoryKey, CommandGeneratorMap> cmds{ScenarioCommandFactoryName(), CommandGeneratorMap{}};

    using Types = TypeList<
#include <iscore_plugin_scenario_commands.hpp>
      >;
    for_each_type<Types>(iscore::commands::FactoryInserter{cmds.second});

    return cmds;
}
