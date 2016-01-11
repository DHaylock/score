#pragma once
#include <QList>

namespace iscore
{
class CommandStackFacade;
struct DocumentContext;
}

namespace Scenario
{
class StateModel;

void RefreshStates(const iscore::DocumentContext& doc);
void RefreshStates(const QList<const StateModel*>&,
                   iscore::CommandStackFacade&);
}
