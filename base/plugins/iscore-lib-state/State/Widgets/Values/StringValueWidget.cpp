#include <iscore/widgets/MarginLess.hpp>
#include <QGridLayout>
#include <QLineEdit>

#include <State/Widgets/Values/ValueWidget.hpp>
#include "StringValueWidget.hpp"

class QWidget;

namespace State
{
StringValueWidget::StringValueWidget(const QString &value, QWidget *parent)
    : ValueWidget{parent}
{
    auto lay = new iscore::MarginLess<QGridLayout>{this};
    m_value = new QLineEdit;
    lay->addWidget(m_value);
    m_value->setText(value);
}

State::Value StringValueWidget::value() const
{
    return State::Value{m_value->text()};
}
}
