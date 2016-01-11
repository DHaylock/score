#include "iscore_plugin_library.hpp"

#include <Library/Panel/LibraryPanelFactory.hpp>

using namespace iscore;

iscore_plugin_library::iscore_plugin_library() :
    QObject {},
    iscore::PanelFactory_QtInterface {}
{
}

std::vector<iscore::PanelFactory*> iscore_plugin_library::panels()
{
    return {new LibraryPanelFactory};
}
