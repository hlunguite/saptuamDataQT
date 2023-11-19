#ifndef CNEWACCOUNTDLG_H
#define CNEWACCOUNTDLG_H

#include <QDialog>
#include <QMap>
#include <QSet>
#include <DB/cdbtablebase.h>
#include "cdlgbase.h"
#include <QCompleter>
#include "DB/cpaymentaccounttable.h"
#include "DB/caccounttable.h"
#include "DB/cdepartmenttable.h"
#include "DB/cdepartmentaccounttable.h"
#include "cremittancesetuptable.h"

namespace Ui {
class CnewAccountDlg;
}
struct SimportAccountData;
class CnewAccountDlg : public CdlgBase
{
    Q_OBJECT

public:
    explicit CnewAccountDlg(bool isEdit, QWidget *parent = 0);
    ~CnewAccountDlg();
    void setAccountIDForEdit(int accountIDForEdit);
    void setAccountToImport(SimportAccountData* importAccount);

    int accountID() const;

private slots:

    void on_m_useHqReceipt_clicked(bool checked);

    void on_m_cancelButton_clicked();

    void on_m_okButton_clicked();

    void on_m_accountTypeCombo_currentTextChanged(const QString &arg1);
    void on_m_deptCombo_currentTextChanged(const QString &arg1);

private:
    Ui::CnewAccountDlg *ui;
    int	    	m_accountIDForEdit;
    SaccountData* 			m_accountData ;
    SdepartmentAccountData* m_deptAccData;
    SremittanceSetupTableData* m_remitData;
    SremittanceNameData* m_nameData;
    SremitrancePercentData* m_pcData;

    QCompleter* m_remitNameCompleter;
    QCompleter* m_accountNameCompleter;


    QMap<int, QString> m_paymentAccountIDNameMap;
    QMap<QString, int> m_paymentAccuntNameIDMap;

    QMap<QString, int> m_deptNameIDMap;
    QMap<int, QString> m_deptIDNameMap;

    QMap<QString, int> m_accountNameIDMap;
    QMap<int, QString> m_accountIDNameMap;
    QMap<QString, QString> m_accountNameMap;

    //QMap<int, int> 		m_deptAccountTableMap;

    void populatePaymentAccount();
    void populateDepartment();
    void populateAccount();
    void populateDepartmentAccountTable();
    bool populateDataMember();
    bool populateDataMemberForNonSubAccount();
    bool validateAccountName();
    bool validateRemitSetup();
    void addPaymentAccountComboBox();
    int     m_accountID;
    bool    m_isImport;
    // QWidget interface
protected:
    virtual void resizeEvent(QResizeEvent *);
    void resize();
    void showhidefield();
};

#endif // CNEWACCOUNTDLG_H
