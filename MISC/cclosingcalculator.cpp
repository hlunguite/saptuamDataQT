#include "cclosingcalculator.h"
#include "cclosingbalancetable.h"
#include "caccountmap.h"
CclosingCalculator::CclosingCalculator(const QDate& currentDate): m_closingDate(currentDate)
{
    const QStringList& depList = CaccountMap::Object()->getDeptList();
    for (auto dept : depList) {
        int deptID = CaccountMap::Object()->getDeptId(dept);
        QSet<int> accounts;
        if (CaccountMap::Object()->getAccounsForDept(deptID, accounts)){
            for (auto accountID : accounts) {
                m_accountDeptMap[accountID] = deptID;
            }
        } else {

        }

    }
    /*
    const QStringList& getAccountList() { return m_accountList;}
    int		getDeptId(const QString& deptName) const;
    bool    getAccounsForDept(int deptID, QSet<int>& accounts) const;
    */
    calculateClosing();
}

void CclosingCalculator::calculateClosing()
{

    SclosingData closingData = CclosingBalanceTable::Object()->getClosingForDate(m_closingDate, true);
    if (closingData.m_id) {

    } else {

    }
}
