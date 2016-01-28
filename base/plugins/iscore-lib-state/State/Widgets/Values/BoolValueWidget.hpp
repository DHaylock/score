#pragma once
#include <State/Value.hpp>
#include "ValueWidget.hpp"

class QComboBox;
class QWidget;

class ISCORE_LIB_STATE_EXPORT  BoolValueWidget : public ValueWidget
{
    public:
        BoolValueWidget(bool value, QWidget* parent = nullptr);

        State::Value value() const override;

    private:
        QComboBox* m_value;
};
