#pragma once
#include <QSlider>
#include <score/tools/Clamp.hpp>
#include <limits>

#include <score_lib_base_export.h>

namespace score
{
/**
 * @brief The DoubleSlider class
 *
 * Always between 0. - 1.
 */
class SCORE_LIB_BASE_EXPORT DoubleSlider final : public QSlider
{
  Q_OBJECT
  static const constexpr double max = std::numeric_limits<int>::max() / 65536.;

public:
  DoubleSlider(QWidget* parent) : QSlider{Qt::Horizontal, parent}
  {
    setMinimum(0);
    setMaximum(1.01 * max);

    connect(this, &QSlider::valueChanged, this, [&](int val) {
      emit valueChanged(double(val) / max);
    });
  }

  virtual ~DoubleSlider();

  void setValue(double val)
  {
    val = clamp(val, 0, 1);
    blockSignals(true);
    QSlider::setValue(val * max);
    blockSignals(false);
  }

  double value() const
  {
    return QSlider::value() / max;
  }

signals:
  void valueChanged(double);
};
}
