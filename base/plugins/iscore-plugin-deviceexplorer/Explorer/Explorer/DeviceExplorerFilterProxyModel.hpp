#pragma once

#include <QSortFilterProxyModel>

class QModelIndex;
class QObject;


namespace DeviceExplorer
{
class DeviceExplorerFilterProxyModel final : public QSortFilterProxyModel
{
    public:
        explicit DeviceExplorerFilterProxyModel(QObject* parent = nullptr);

        void setColumn(int col);

    protected:
        bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

        bool filterAcceptsRowItself(int sourceRow, const QModelIndex& sourceParent) const;
        bool hasAcceptedChildren(int sourceRow, const QModelIndex& sourceParent) const;

    protected:
        int m_col;

};
}
