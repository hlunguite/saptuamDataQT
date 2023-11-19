#ifndef QCONTACTTABLEMODEL_H
#define QCONTACTTABLEMODEL_H

#include <QSqlTableModel>

class CcontactTableModel : public QSqlTableModel
{
public:
    CcontactTableModel(QObject *parent = Q_NULLPTR, QSqlDatabase db = QSqlDatabase());
    ~CcontactTableModel();

    // QAbstractItemModel interface
public:
    virtual QVariant data(const QModelIndex &index, int role) const;
};

#endif // QCONTACTTABLEMODEL_H
