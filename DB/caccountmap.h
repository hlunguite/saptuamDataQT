#ifndef CACCOUNTMAP_H
#define CACCOUNTMAP_H
#include "csdsingleton.h"
#include <QString>
#include <QMap>
#include <QStringList>
#include <Qvector>
#include <QSet>
#include "caccounttable.h"
#include <set>

class CaccountMap : public CsdSingleton<CaccountMap>
{
    friend class CsdSingleton<CaccountMap>;

public:
    void populateAccountMap();
    QString getAccountName(int id) const;
    int 	getAccountID(const QString& accontName) const;
    int		getAccountType(const QString& accountName) const;
    const QStringList& getAccountList() { return m_accountList;}
    const QStringList& getIncomeAccountList() { return m_incomeAccountList;}
    const QStringList& getPaymentAccountList() { return m_paymentAccountList;}
    const QStringList& getAccountListWithHqReceipt() { return m_accountListWithHqReceipt;}
    const std::set<int>& getAccountIDListWithHqReceipt() { return m_accountIDListWithHqReceipt;}
    const QStringList& getDeptList() { return m_departmentList;}
    QString getDeptName(int id) const;
    int		getDeptId(const QString& deptName) const;
    int 	getDeptForAccount(int accountID) const;
    bool    getAccounsForDept(int deptID, QSet<int>& accounts) const;
    const std::set<int>& getAllAccount() {return m_allAccount;}
    EaccountType getAccountType(int accountID);
    QString getPaymentAccountNameForAccountDepID(int id);
    int     getDefaultDept() const { return m_defaultDept;}

private:
    CaccountMap();
    ~CaccountMap();
    void populateAccountMapInt();
    void populateAccountDeptMap();

    QMap<int, QString> 	m_accountIdNameMap;
    QMap<QString, int>	m_accountNameIdMap;
    QMap<QString, int>  m_accountNameTypeMap;
    QStringList			m_accountList;
    QStringList         m_incomeAccountList;
    QStringList         m_paymentAccountList;
    QStringList			m_accountListWithHqReceipt;
    std::set<int>           m_accountIDListWithHqReceipt;
    std::set<int>       m_allAccount;


    QStringList			m_departmentList;
    QMap<int, QString>	m_deptIDNameMap;
    QMap<QString, int>  m_deptNameIDMap;
    QMap<int, QSet<int> >m_deptAccountsMap;
    QMap<int, int>      m_accountDeptMap;
    QMap<int, int> 		m_deptAccountTableMap;
    QMap<int,
         EaccountType>  m_accountTypes;
    QMap<int, QString>  m_paymentAccountIDNameMap;
    QMap<int, int>      m_accountDepPaymentAccountMap;
    int                 m_defaultDept;


};

#endif // CACCOUNTMAP_H
