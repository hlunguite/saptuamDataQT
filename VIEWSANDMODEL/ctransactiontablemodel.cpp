#include "ctransactiontablemodel.h"
#include "DB/ctransactiontable.h"
#include "DB/caccountmap.h"
#include "DB/ccontactmap.h"
#include "ctransactionutils.h"
#include "csdutils.h"
CtransactionTableModel::CtransactionTableModel(QObject *parent, QSqlDatabase db):
     QSqlTableModel(parent,db)
{

}

CtransactionTableModel::~CtransactionTableModel()
{

}

void CtransactionTableModel::setColEditable(int col)
{
    m_editableColumn.insert(col);
}


QVariant CtransactionTableModel::data(const QModelIndex &index, int role) const
{
    QVariant d = QSqlTableModel::data(index, role);
    int col = index.column();
    if (role == Qt::DisplayRole ) {
        if (col == TRANSACTION_FROM_IDX) {
            int id = d.toInt();
            return CcontactMap::Object()->getContanceName(id);
        }
        if (col == TRANSACTION_ACCOUNT_IDX) {
            int id = d.toInt();
            return CaccountMap::Object()->getAccountName(id);
        }
        if (col == TRANSACTION_DATE_IDX) {
             return d.toDate().toString("dd MMM yyyy");
        }
        if (col == TRANSACTION_AMOUNT_IDX) {
            QString amount;
             amount =  CsdUtils::convertAmountToStringWithSign(d.toDouble()) ;
            return amount;
        }

        if (col == TRANSACTION_MODE_IDX) {
            EtransactionMode mode = (EtransactionMode)d.toInt();
            return CtransactionUtils::Object()->getTransactionModeStr(mode);
        }

        if (col == TRANSACTION_TYPE_IDX) {
            int type = d.toInt();
            /*int row = index.row();
            QModelIndex idx = index.model()->index(row, TRANSACTION_FROM_IDX);
            d = QSqlTableModel::data(idx, role);
            int id = d.toInt();
            idx = index.model()->index(row, TRANSACTION_TYPE_IDX);
            d = QSqlTableModel::data(idx, role);
*/
            //qDebug()<<"Name is "<< CcontactMap::Object()->getContanceName(id)<<" type "<<type<<" "<<d.toInt();
            return CtransactionUtils::Object()->getTransactionTypeStr((EtransactionType)type);

        }

        /*if (col == TRANSACTION_RECTIPT_BOOK_IDX || col == TRANSACTION_RECTIPT_NO_IDX) {
            int type = d.toInt();
            if (type == 0) {
                return "";
            }
        }*/

    }else if (Qt::TextAlignmentRole == role){

        if (col ==  TRANSACTION_AMOUNT_IDX) {
            return ((int)Qt::AlignRight | (int)Qt::AlignVCenter);
        }
        return ((int)Qt::AlignLeft | (int)Qt::AlignVCenter);
    }
    return d;
}

bool CtransactionTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return QSqlTableModel::setData(index, value, role);

}

Qt::ItemFlags CtransactionTableModel::flags(const QModelIndex &index) const
{
    if (m_editableColumn.empty()) {
        return QSqlTableModel::flags(index);
    }
    int col = index.column();
    if (m_editableColumn.contains(col)) {
            return Qt::ItemIsEnabled|Qt::ItemIsSelectable|Qt::ItemIsEditable;
    }
    return Qt::ItemIsEnabled|Qt::ItemIsSelectable;
}
