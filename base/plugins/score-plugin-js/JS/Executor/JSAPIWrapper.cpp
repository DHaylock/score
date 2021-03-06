// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Explorer/DocumentPlugin/DeviceDocumentPlugin.hpp>

#include "JSAPIWrapper.hpp"
#include <ossia/editor/value/value.hpp>
#include <Explorer/DocumentPlugin/NodeUpdateProxy.hpp>
#include <Engine/score2OSSIA.hpp>
#include <ossia/detail/apply.hpp>

namespace JS
{


QJSValue APIWrapper::value(QJSValue address)
{
  // OPTIMIZEME : have State::Address::fromString return a optional<Address> to
  // have a single check.
  auto addr_str = address.toString();
  auto acc_opt = State::AddressAccessor::fromString(addr_str);
  if (acc_opt)
  {
    // FIXME this does not handle accessors at all
    const State::AddressAccessor& acc = *acc_opt;
    auto val = devices.updateProxy.try_refreshRemoteValue(acc.address);
    if (val)
    {
      return JS::convert::value(m_engine, *std::move(val));
    }
  }

  return {};
}

static ossia::value clone_val(const State::Address& addr, const Device::DeviceList& l)
{
  auto ossia_addr = Engine::score_to_ossia::address(addr, l);
  if(ossia_addr)
    return ossia_addr->value();
  return {};
}

QJSValue APIWrapper::clone(QJSValue address)
{
  // OPTIMIZEME : have State::Address::fromString return a optional<Address> to
  // have a single check.
  auto addr_str = address.toString();
  auto acc_opt = State::AddressAccessor::fromString(addr_str);
  if (acc_opt)
  {
    // FIXME this does not handle accessors at all
    const State::AddressAccessor& acc = *acc_opt;

    return JS::convert::value(
          m_engine,
          clone_val(acc.address, devices.list()));
  }

  return {};
}
namespace convert
{
QJSValue makeImpulse(QJSEngine& engine)
{
  auto obj = engine.newObject();
  obj.setProperty("impulse", 1);
  return obj;
}

struct ossia_value_visitor
{
  QJSEngine& engine;

public:
  using return_type = QJSValue;
  return_type operator()() const
  { return makeImpulse(engine); }
  return_type operator()(ossia::impulse) const
  { return makeImpulse(engine); }
  return_type operator()(int i) const
  { return i; }
  return_type operator()(float f) const
  { return f; }
  return_type operator()(bool b) const
  { return b; }
  return_type operator()(char c) const
  { return QString(QChar(c)); }
  return_type operator()(const std::string& s) const
  { return QString::fromStdString(s); }

  template<std::size_t N>
  return_type operator()(const std::array<float, N>& t) const
  {
    auto arr = engine.newArray(N);

    for (std::size_t i = 0; i < N; i++)
    {
      arr.setProperty(i, t[i]);
    }

    return arr;
  }

  return_type operator()(const std::vector<ossia::value>& t) const
  {
    auto arr = engine.newArray(t.size());

    int i = 0;
    for (const ossia::value& elt : t)
    {
      arr.setProperty(i++, elt.apply(*this));
    }

    return arr;
  }
};

QJSValue value(QJSEngine& engine, const ossia::value& val)
{
  return val.apply(ossia_value_visitor{engine});
}

QJSValue address(const State::AddressAccessor& val)
{
  return val.toString();
}

QJSValue message(QJSEngine& engine, const State::Message& mess)
{
  auto obj = engine.newObject();
  auto& strings = score::StringConstant();

  obj.setProperty(strings.address, address(mess.address));
  obj.setProperty(strings.value, value(engine, mess.value));
  return obj;
}

QJSValue time(const TimeVal& val)
{
  return val.msec();
}

QJSValue messages(QJSEngine& engine, const State::MessageList& messages)
{
  auto obj = engine.newArray(messages.size());
  for (int i = 0; i < messages.size(); i++)
  {
    obj.setProperty(i, message(engine, messages.at(i)));
  }
  return obj;
}

ossia::value value(const QJSValue& val)
{
  if (val.isUndefined() || val.isNull() || val.isError())
    return {};
  else if (val.isBool())
    return val.toBool();
  else if (val.isNumber())
    return val.toNumber();
  else if (val.isString())
    return val.toString().toStdString();
  else if (val.isArray())
  {
    State::list_t arr;

    QJSValueIterator it{val};
    while (it.hasNext())
    {
      it.next();
      arr.push_back(value(it.value()));
    }
    return arr;
  }
  else if (val.isObject())
  {
    if (val.hasProperty("impulse"))
    {
      return State::impulse{};
    }
  }

  return {};
}

State::Message message(const QJSValue& val)
{
  auto& strings = score::StringConstant();
  if (val.isObject())
  {
    auto score_addr = val.property(strings.address);
    auto score_val = val.property(strings.value);
    if (score_addr.isString())
    {
      auto res = State::AddressAccessor::fromString(score_addr.toString());
      if (res)
        return {*res, value(score_val)};
      return {};
    }
  }
  else if (val.isString())
  {
    auto score_addr = val.property(strings.address);
    auto res = State::AddressAccessor::fromString(score_addr.toString());
    if (res)
      return State::Message{*res, State::impulse{}};
  }

  return {};
}

State::MessageList messages(const QJSValue& val)
{
  State::MessageList ml;

  if (val.isArray())
  {
    QJSValueIterator it{val};
    while (it.hasNext())
    {
      it.next();
      auto mess = message(it.value());
      if (!mess.address.address.device.isEmpty())
      {
        ml.append(std::move(mess));
      }
    }
  }
  else if (val.isObject())
  {
    auto mess = message(val);
    if (!mess.address.address.device.isEmpty())
    {
      ml.append(std::move(mess));
    }
  }
  else if (val.isString())
  {
    auto res = State::AddressAccessor::fromString(val.toString());
    if (res)
      ml.append({*res, State::impulse{}});
  }

  return ml;
}
}
}
