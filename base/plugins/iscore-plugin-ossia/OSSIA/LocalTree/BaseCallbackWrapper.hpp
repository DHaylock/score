#pragma once
#include "BaseProperty.hpp"

namespace Ossia
{
namespace LocalTree
{
class ISCORE_PLUGIN_OSSIA_EXPORT BaseCallbackWrapper : public BaseProperty
{
    public:
        using BaseProperty::BaseProperty;
        ~BaseCallbackWrapper()
        {
        }

        ossia::net::address_base::callback_index callbackIt{};
};
}
}
