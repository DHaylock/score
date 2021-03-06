// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Process/ProcessList.hpp>
#include <QApplication>
#include <QInputDialog>

#include <QString>
#include <QStringList>
#include <algorithm>
#include <utility>
#include <vector>

#include "AddProcessDialog.hpp"
#include <Process/ProcessFactory.hpp>
#include <Process/StateProcessFactoryList.hpp>
#include <score/plugins/customfactory/FactoryFamily.hpp>

namespace Scenario
{
AddProcessDialog::AddProcessDialog(
    const Process::ProcessFactoryList& plist, QWidget* parent)
    : QWidget{parent}, m_factoryList{plist}
{
  hide();
}

void AddProcessDialog::launchWindow()
{
  bool ok = false;

  std::vector<std::pair<QString, UuidKey<Process::ProcessModel>>>
      sortedFactoryList;
  for (const auto& factory : m_factoryList)
  {
    sortedFactoryList.push_back(
        std::make_pair(factory.prettyName(), factory.concreteKey()));
  }

  std::sort(
      sortedFactoryList.begin(),
      sortedFactoryList.end(),
      [](const auto& e1, const auto& e2) { return e1.first < e2.first; });

  QStringList nameList;
  for (const auto& elt : sortedFactoryList)
  {
    nameList.append(elt.first);
  }

  auto process_name = QInputDialog::getItem(
      qApp->activeWindow(),
      tr("Choose a process"),
      tr("Choose a process"),
      nameList,
      0,
      false,
      &ok);

  if (ok)
  {
    auto it = std::find_if(
        sortedFactoryList.begin(),
        sortedFactoryList.end(),
        [&](const auto& elt) { return elt.first == process_name; });
    SCORE_ASSERT(it != sortedFactoryList.end());
    emit okPressed(it->second);
  }
}

AddStateProcessDialog::AddStateProcessDialog(
    const Process::StateProcessList& plist, QWidget* parent)
    : QWidget{parent}, m_factoryList{plist}
{
  hide();
}

void AddStateProcessDialog::launchWindow()
{
  bool ok = false;

  std::vector<std::pair<QString, UuidKey<Process::StateProcessFactory>>>
      sortedFactoryList;
  for (const auto& factory : m_factoryList)
  {
    sortedFactoryList.push_back(
        std::make_pair(factory.prettyName(), factory.concreteKey()));
  }

  std::sort(
      sortedFactoryList.begin(),
      sortedFactoryList.end(),
      [](const auto& e1, const auto& e2) { return e1.first < e2.first; });

  QStringList nameList;
  for (const auto& elt : sortedFactoryList)
  {
    nameList.append(elt.first);
  }

  auto process_name = QInputDialog::getItem(
      qApp->activeWindow(),
      tr("Choose a state process"),
      tr("Choose a state process"),
      nameList,
      0,
      false,
      &ok);

  if (ok)
  {
    auto it = std::find_if(
        sortedFactoryList.begin(),
        sortedFactoryList.end(),
        [&](const auto& elt) { return elt.first == process_name; });
    SCORE_ASSERT(it != sortedFactoryList.end());
    emit okPressed(it->second);
  }
}
}
