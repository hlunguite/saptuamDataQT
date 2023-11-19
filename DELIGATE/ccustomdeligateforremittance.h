#ifndef CCUSTOMDELIGATEFORREMITTANCE_H
#define CCUSTOMDELIGATEFORREMITTANCE_H

#include <QItemDelegate>


enum EremitDLGTableIDX { REMIT_DLG_TABLE_ID,
                         REMIT_DLG_TABLE_DATE,
                         REMIT_DLG_TABLE_RECEIPT_BOOK_NO,
                         REMIT_DLG_TABLE_REEIPT_SL_NO ,
                         REMIT_DLG_TABLE_MIN ,
                         REMIT_DLG_TABLE_PIAKSAN,
                         REMIT_DLG_TABLE_AMOUNT ,
                         REMIT_DLG_TABLE_LOCAL_SHARE,
                         REMIT_DLG_TABLE_HQ_SHARE };

class CcustomDeligateForRemittance : public QItemDelegate
{
public:
    explicit CcustomDeligateForRemittance(QObject *parent = nullptr);

    // QAbstractItemDelegate interface
public:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;


    void setAccountList(const QStringList &newAccountList);

private:
    QStringList m_accountList;
};

#endif // CCUSTOMDELIGATEFORREMITTANCE_H
