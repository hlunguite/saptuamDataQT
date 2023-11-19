#ifndef CRECEIPTSETUPDLG_H
#define CRECEIPTSETUPDLG_H
//#include "ccustomdeligateforreceiptsetup.h"
#include "cdlgbase.h"
#include "caccountreceiptbook.h"
#include <set>
namespace Ui {
class CreceiptSetupDlg;
}


class CreceiptSetupDlg :  public CdlgBase
{
    Q_OBJECT

public:
    explicit CreceiptSetupDlg(QWidget *parent = nullptr);
    ~CreceiptSetupDlg();

private:
    Ui::CreceiptSetupDlg *ui;
    //CcustomDeligateForReceiptSetup* m_customDeligate;

    void resize();
    void clear();
    void updateAccountForAvailableReceiptTable(QString deptName);
    std::set<int>               m_allAccountWithHqReceipt;
    std::set<int>                m_currentAccountIDInAvailableReceiptTable;
    QStringList                 m_currentAccountNameInAvailableReceiptTable;
    QMap<int, int>               m_rowToAvailableReceiptMap;
    QMap<int, QMap<int, int> >  m_receiptAccountRow; // receiptID, accountID, and row number in accountReceiptMap.table
    QVector<SaccountReceipt>	m_accountReceipt;  // accountreceipt for each row of accountReceiptMap table
    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *);
private slots:
    void on_m_addReceiptBtn_clicked();
    void on_m_clearBtn_clicked();
    void on_m_cancelBtn_clicked();
    void addAvailableRow(QString receiptName, QString accountName);
    void addAvailableTable();
    void addAccountReceiptTable();
    void addAccoutMapRow(SaccountReceipt& accountReceipt);
    void on_m_addReceiptAccountBtn_clicked();
    void on_m_removeReceiptAccountBtn_clicked();
    void on_m_selDeptBox_currentTextChanged(const QString &arg1);
};

#endif // CRECEIPTSETUPDLG_H
