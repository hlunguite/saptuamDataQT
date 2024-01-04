#ifndef CACCOUNTLEDGER_H
#define CACCOUNTLEDGER_H
#include <map>
#include <QDate>
#include "ctransactiontable.h"
#include <QStringList>
class CaccountLedger {
protected:
    bool                                        m_isBankOrCashLedger;
    double                                      m_totalIncome;
    double                                      m_totalPayment;
    QMap<QDate, QVector<StransactionData*> >    m_transactions;
    QStringList                                 m_csvTrans;
public:
    CaccountLedger(bool isBankOrCash = false);
    virtual ~CaccountLedger();
    void addTransaction(StransactionData* trans);
    void clear() { m_transactions.clear(); }
    QStringList getCSVTransList() const { return m_csvTrans; }
    //double getTotalIncome() const { return m_totalIncome;}
    //double getTotalPayment() const { return m_totalPayment;}
    virtual QString getHTMLTable(int accountID, bool addDeptInCSV, bool indiviualAccount, QString replaceAccountName = QString()) ;
};
class CremitttanceLedger: public CaccountLedger {
public:
    CremitttanceLedger():CaccountLedger(false){}


    // CaccountLedger interface
public:
    virtual QString getHTMLTable(int accountID, bool addDeptInCSV, bool indiviualAccount, QString replaceAccountName) override;
};

class CloanLedger: public CaccountLedger {
public:
    CloanLedger():CaccountLedger(false), m_loanAccountProcessed(false){}
    bool processTransForLoanAcc();
    ~CloanLedger();
private:
    bool m_loanAccountProcessed = false;
    std::map<int, std::vector<StransactionData*> > m_loanTransaction;




    // CaccountLedger interface
public:
    virtual QString getHTMLTable(int accountID, bool addDeptInCSV, bool indiviualAccount, QString replaceAccountName) override;
};


#endif // CACCOUNTLEDGER_H
