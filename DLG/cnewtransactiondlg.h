#ifndef CNEWTRANSACTIONDLG_H
#define CNEWTRANSACTIONDLG_H

#include <QDialog>
#include <QAction>
#include <QVector>
#include <QMap>
#include <QTableWidgetItem>
#include "ccustomdeligatefortransaction.h"
#include "cbanktransactiontable.h"
#include "ctransactiontable.h"
namespace Ui {
class CnewTransactionDlg;
}
enum EtransErrorType {
    TRANS_IGNORE_ROW,
    TRANS_NO_ERROR,
    TRANS_EMPTY_NAME_ERROR,
    TRANS_NAME_NOT_FOUND_ERROR,
    TRANS_AMOUNT_EMPTY_ERROR,
    TRANS_AMOUNT_NOT_NUMBER_ERROR,
    TRANS_ACCOUNT_NOT_FOUND_ERROR,
    TRANS_ACCOUNT_EMPTY_ERROR,
    TRANS_MODE_EMPTY_ERROR,
    TRANS_REF_EMPTY_ERROR,
    TRANS_TYPE_EMPTY_ERROR,
    TRANS_NULL_CELL_ERROR,
    TRANS_AMOUNT_NOT_MATCH_ERROR,
    TRANS_EMPTY_BANK_IMPORT_ERROR,
    TRANS_MODE_AND_ACCOUNT_TYPE_MISMATCH,
    TRANS_INCOME_ACCOUNT_FOR_PAYMENT,
    TRANS_PAYMENT_ACCOUNT_FOR_INCOME,
    TRANS_BANK_AND_ENTRY_MISMATCH,
    TRANS_TYPE_MISMATCH

};
class CnewTransactionDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CnewTransactionDlg(bool bankImport = false, int importID = 0, QWidget *parent = 0);
    ~CnewTransactionDlg();
private slots:
    void deleteRow();
    void deleteAllRow();
    void on_m_cancelButton_clicked();

    void on_m_okButton_clicked();

    void on_m_transactionTable_itemChanged(QTableWidgetItem *item);

    void on_m_transactionTable_customContextMenuRequested(const QPoint &pos);


    void on_m_bankImportTable_itemClicked(QTableWidgetItem *item);

    void on_m_skipBtn_clicked();

    void on_m_rejectBtn_clicked();

private:
    Ui::CnewTransactionDlg *ui;
    bool								m_bankImport;
    int 								m_importID;
    int									m_currentBankTransDetailIdx;
    QAction* 							m_deleteRow;
    QAction* 							m_deleteAllRow;
    int 								m_rowNumber;
    bool								m_disableCheckRef;
    double								m_incomeTotal;
    double								m_paymentTotal;
    CcustomDeligateForTransaction* 		m_deligateForTransaction;
    QVector<QVector<QVariant> >			m_allRowData;
    QMap<QString, QPair<int, double > >	m_accountSummaryMap;
    QMap<QString, double>				m_bankTotalMap;
    QVector<SbanktransDetail>			m_bankTransDetail;
    QVector<int>						m_importTableId;
    void resize();
    EtransErrorType checkFromToCol(QTableWidgetItem* item);
    EtransErrorType checkAmount(QTableWidgetItem* item);
    EtransErrorType checkAccount(QTableWidgetItem* item);
    EtransErrorType checkMode(QTableWidgetItem* item);
    EtransErrorType checkRef(QTableWidgetItem* item);
    EtransErrorType checkType(QTableWidgetItem* item);
    bool			getIsModeRequireRef(int row);
    QString	getErrorString(EtransErrorType error);
    EtransErrorType calcTotal();
    EtransErrorType calcTotalNetTrans();
    EtransErrorType checkForValidityOfRow(int row, QVector<QVariant>& rowData);
    bool isEmptyOrNull(QTableWidgetItem *item);
    void addRefAndModeForRow(int row);
    void populateBankImport(int importID);
    void displayCurrentBankImport();
    void populateBankTransDetail();
    void moveToNextImport(bool skip = false);
    void addBankName(QSet<int>& fromIDs, QString bankName);
    bool checkRefAlreadyHasTrans();
    void addInPrevTable(QVector<StransactionData*>& allTrns);
    void addInPrevTable(int contactID1, int contactID2);
    bool		m_calcTotalCall;
    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *);
};

#endif // CNEWTRANSACTIONDLG_H
