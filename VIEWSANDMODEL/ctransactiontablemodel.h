#ifndef CTRANSACTIONTABLEMODEL_H
#define CTRANSACTIONTABLEMODEL_H
#include <QSqlTableModel>
#include <QSet>

class CtransactionTableModel : public QSqlTableModel
{
    Q_OBJECT

public:
    CtransactionTableModel(QObject *parent = Q_NULLPTR, QSqlDatabase db = QSqlDatabase());
    ~CtransactionTableModel();
    void setColEditable(int col);
    // QAbstractItemModel interface
public:
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role);
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
private:
    QSet<int>		m_editableColumn;
};

#endif // CTRANSACTIONTABLEMODEL_H
