#include <Automation/AutomationModel.hpp>
#include <Device/Address/AddressSettings.hpp>
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>
#include <Scenario/Commands/Cohesion/CreateCurveFromStates.hpp>
#include <Scenario/Commands/Cohesion/InterpolateMacro.hpp>
#include <Scenario/Document/Constraint/ConstraintModel.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include <boost/multi_index/detail/hash_index_iterator.hpp>
#include <iscore/document/DocumentInterface.hpp>
#include <iscore/tools/SettableIdentifierGeneration.hpp>
#include <QObject>
#include <QString>
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

#include <Device/Address/Domain.hpp>
#include <Device/Node/DeviceNode.hpp>
#include "InterpolateStates.hpp"
#include <Process/LayerModel.hpp>
#include <Process/Process.hpp>
#include <Process/State/MessageNode.hpp>
#include <Scenario/Document/Constraint/Rack/Slot/SlotModel.hpp>
#include <Scenario/Document/State/ItemModel/MessageItemModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <State/Address.hpp>
#include <State/Message.hpp>
#include <State/Value.hpp>
#include <State/ValueConversion.hpp>
#include <iscore/command/Dispatchers/CommandDispatcher.hpp>
#include <iscore/selection/SelectionStack.hpp>
#include <iscore/tools/ModelPath.hpp>
#include <iscore/tools/NotifyingMap.hpp>
#include <iscore/tools/SettableIdentifier.hpp>
#include <iscore/tools/TreeNode.hpp>

void InterpolateStates(const QList<const ConstraintModel*>& selected_constraints,
                       iscore::CommandStackFacade& stack)
{
    // For each constraint, interpolate between the states in its start event and end event.

    // They should all be in the same scenario so we can select the first.
    Scenario::ScenarioModel* scenar = dynamic_cast<Scenario::ScenarioModel*>(
                                selected_constraints.first()->parent());

    auto& devPlugin = iscore::IDocument::documentContext(*scenar).plugin<DeviceDocumentPlugin>();
    auto& rootNode = devPlugin.rootNode();

    auto big_macro = new AddMultipleProcessesToMultipleConstraintsMacro;
    for(auto& constraint : selected_constraints)
    {
        const auto& startState = scenar->state(constraint->startState());
        const auto& endState = scenar->state(constraint->endState());

        iscore::MessageList startMessages = flatten(startState.messages().rootNode());
        iscore::MessageList endMessages = flatten(endState.messages().rootNode());

        std::vector<std::pair<const iscore::Message*, const iscore::Message*>> matchingMessages;

        for(auto& message : startMessages)
        {
            if(!message.value.val.isNumeric())
                continue;

            auto it = std::find_if(std::begin(endMessages),
                                   std::end(endMessages),
                                   [&] (const iscore::Message& arg) {
                return message.address == arg.address
                        && arg.value.val.isNumeric()
                        // TODO see CreateSequence (and refactor this) && message.value.val.impl().which() == arg.value.val.impl().which()
                        && message.value != arg.value; });

            if(it != std::end(endMessages))
            {
                // TODO any_of
                auto has_existing_curve = std::find_if(
                            constraint->processes.begin(),
                            constraint->processes.end(),
                            [&] (const Process::ProcessModel& proc) {
                    auto ptr = dynamic_cast<const AutomationModel*>(&proc);
                    if(ptr && ptr->address() == message.address)
                        return true;
                    return false;
                });

                if(has_existing_curve != constraint->processes.end())
                    continue;

                matchingMessages.emplace_back(&message, &*it);
            }
        }

        if(!matchingMessages.empty())
        {
            // Generate brand new ids for the processes
            auto process_ids = getStrongIdRange<Process::ProcessModel>(matchingMessages.size(), constraint->processes);
            auto macro_tuple = makeAddProcessMacro(*constraint, matchingMessages.size());
            auto macro = std::get<0>(macro_tuple);
            auto& bigLayerVec = std::get<1>(macro_tuple);

            Path<ConstraintModel> constraintPath{*constraint};

            int i = 0;
            for(const auto& elt : matchingMessages)
            {
                double start = iscore::convert::value<double>(elt.first->value);
                double end = iscore::convert::value<double>(elt.second->value);

                double min = std::min(start, end);
                double max = std::max(start, end);
                if(auto node = iscore::try_getNodeFromAddress(rootNode, elt.first->address))
                {
                    const iscore::AddressSettings& as = node->get<iscore::AddressSettings>();
                    if(as.domain.min.val.isNumeric())
                        min = std::min(min, iscore::convert::value<double>(as.domain.min));

                    if(as.domain.max.val.isNumeric())
                        max = std::max(max, iscore::convert::value<double>(as.domain.max));
                }

                macro->addCommand(new CreateCurveFromStates{
                                      Path<ConstraintModel>{constraintPath},
                                      bigLayerVec[i],
                                      process_ids[i],
                                      elt.first->address,
                                      start, end, min, max
                                  });

                i++;
            }
            big_macro->addCommand(macro);
        }
    }

    CommandDispatcher<> disp{stack};
    disp.submitCommand(big_macro);
}



