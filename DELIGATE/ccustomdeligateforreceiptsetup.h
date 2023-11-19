#ifndef CCUSTOMDELIGATEFORRECEIPTSETUP_H
#define CCUSTOMDELIGATEFORRECEIPTSETUP_H
#include <QItemDelegate>

enum EreceiptSetupColumn {
    RECEIPT_SETUP_RECEIPT_BOOK,
    RECEIPT_SETUP_RECEIPT_NO,
    RECEIPT_SETUP_ACCOUNT

};

class CcustomDeligateForReceiptSetup : public QItemDelegate
{
    Q_OBJECT
public:
    CcustomDeligateForReceiptSetup(QObject * parent = 0);
    ~CcustomDeligateForReceiptSetup();

private slots:
    void commitAndCloseEditor();
    // QAbstractItemDelegate interface
public:
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
};

#endif // CCUSTOMDELIGATEFORRECEIPTSETUP_H
