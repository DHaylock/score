#pragma once
#include <Process/LayerView.hpp>
#include <QTextLayout>

namespace Interpolation
{
class View final : public Process::LayerView
{
  Q_OBJECT
public:
  explicit View(QGraphicsItem* parent);
  virtual ~View();


signals:
  void dropReceived(const QMimeData& mime);

protected:
  void paint_impl(QPainter* painter) const override;
  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

private:
};
}
