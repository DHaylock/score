#pragma once
#include <score_lib_base_export.h>
#include <memory>
class QGraphicsObject;
class QGraphicsItem;
class QGraphicsView;
/**
 * @brief deleteGraphicsItem Properly delete a QGraphicsObject
 * @param item item to delete
 *
 * Simply using deleteLater() is generally not enough, the
 * item has to be removed from the scene else there will be crashes.
 */
SCORE_LIB_BASE_EXPORT void deleteGraphicsObject(QGraphicsObject* item);
SCORE_LIB_BASE_EXPORT void deleteGraphicsItem(QGraphicsItem* item);

SCORE_LIB_BASE_EXPORT
QGraphicsView* getView(QGraphicsItem& self);

template <typename T>
struct graphics_item_ptr
{
  T* impl{};
  graphics_item_ptr() = default;
  graphics_item_ptr(const graphics_item_ptr&) = default;
  graphics_item_ptr(graphics_item_ptr&&) = default;
  graphics_item_ptr& operator=(const graphics_item_ptr&) = default;
  graphics_item_ptr& operator=(graphics_item_ptr&&) = default;

  graphics_item_ptr(T* p) : impl{p}
  {
  }

  ~graphics_item_ptr()
  {
    deleteGraphicsItem(impl);
  }

  auto operator=(T* other)
  {
    impl = other;
  }

  operator bool() const
  {
    return impl;
  }

  operator T*() const
  {
    return impl;
  }

  T& operator*() const
  {
    return *impl;
  }

  T* operator->() const
  {
    return impl;
  }
};
