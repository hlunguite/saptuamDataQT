#ifndef CACCOUNTTREEMODEL_H
#define CACCOUNTTREEMODEL_H
#include <QAbstractItemModel>
#include <QSqlQuery>
#include <QMap>
//https://www.mimec.org/blog/qsqlquerymodel-and-qtreeview-part0
//http://webissues.sourcearchive.com/documentation/1.0~beta2plus-psvn15030/dir_ade0198d597f17bffb912a3bc708a6d0.html
class CaccountTreeModel : public QAbstractItemModel
{
    //Q_OBJECT
public:
    explicit CaccountTreeModel(QObject* parent = nullptr);
    ~CaccountTreeModel();

    // QAbstractItemModel interface
public:
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent) const;
    virtual int columnCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role);
    //void insertRowsData(const QVector<QVariant> & data);
    void refreshTable();
private:
    //bool setData(const QVector<QVariant> & data);
    void populateAccountandDept();
    QMap<int, QString>	m_idAccountDeptNameMap;
    QMap<QString, int>  m_nameAccountDeptIdMap;
    QMap<int, QString>  m_idAccountTypeMap;
    QMap<int, QString>  m_remittanceDetail;

};

#endif // CACCOUNTTREEMODEL_H
