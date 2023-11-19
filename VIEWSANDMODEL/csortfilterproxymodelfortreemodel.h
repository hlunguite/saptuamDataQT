#ifndef CSORTFILTERPROXYMODELFORTREEMODEL_H
#define CSORTFILTERPROXYMODELFORTREEMODEL_H

#include <QSortFilterProxyModel>
#include <QMap>
class CSortFilterProxyModelForTreeModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    CSortFilterProxyModelForTreeModel();
    ~CSortFilterProxyModelForTreeModel();
    void setFilterData(int col, QString);
    void setFilterData(int col, int);
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const;
private:
    QMap<int,int>     m_filterDataInt;
    QMap<int,QString> m_filterDataStr;
};

#endif // CSORTFILTERPROXYMODELFORTREEMODEL_H
