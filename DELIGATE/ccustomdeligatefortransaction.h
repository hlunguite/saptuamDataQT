#ifndef CCUSTOMDELIGATEFORTRANSACTION_H
#define CCUSTOMDELIGATEFORTRANSACTION_H
#include "ctransactionutils.h"
#include <QItemDelegate>
enum EtransactionCol {
    TRANS_FROM_TO_COL,
    TRANS_AMOUNT_COL,
    TRANS_ACCOUNT_COL,
    TRANS_TYPE_COL,
    TRANS_MODE_COL,
    TRANS_REF_COL,
    TRANS_PARTICULAR_REMARK_COL,
    TRANS_END_COL
};

class CcustomDeligateForTransaction : public QItemDelegate
{
    Q_OBJECT
public:
    CcustomDeligateForTransaction(QObject * parent = 0);
    ~CcustomDeligateForTransaction();
private slots:
    void commitAndCloseEditor();
    EtransactionType getTransTypeFromParticular(const QModelIndex &index) const;
    // QAbstractItemDelegate interface
public:
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

#endif // CCUSTOMDELIGATEFORTRANSACTION_H
