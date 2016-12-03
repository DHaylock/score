#include "JSCommandFactory.hpp"
#include <iscore/command/SerializableCommand.hpp>

const CommandParentFactoryKey& JS::CommandFactoryName()
{
  static const CommandParentFactoryKey key{"JS"};
  return key;
}
