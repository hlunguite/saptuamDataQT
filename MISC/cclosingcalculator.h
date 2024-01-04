#ifndef CCLOSINGCALCULATOR_H
#define CCLOSINGCALCULATOR_H
#include <QDate>
#include <map>
#include <set>
using accountTwoAmountType = std::map<int, std::pair<double, double> >;
using accountCashORBankAmountType = std::map<QString, double >;
using accountDeptAmountType = std::map<int, double>;
class CclosingCalculator
{
public:

    CclosingCalculator();
    void closingBetween(QDate fromDate, QDate toDate);
    void closingAsOn(QDate toDate);

    void calculateClosing(QDate fromDate, QDate toDate);
    void calculateClosing(QDate fromDate, QString query);
    void calculateClosingAsOn(QDate openingDate);
    //void calculateClosing(QDate openingDate);

    double cashOpening() const;

    double cashClosing() const;

    double bankOpening() const;

    double bankClosing() const;
    const accountDeptAmountType& getClosing() { return m_closingValues;}
    const accountDeptAmountType& getOpening() { return m_openingValues;}

    QDate lastTransDate() const;

    accountTwoAmountType accountIncomeAndPayment() const;

private:
    double              m_cashOpening;
    double              m_cashClosing;
    double              m_bankOpening;
    double              m_bankClosing;
    int                 m_remittanceID;
    int                 m_requestID;
    QDate               m_lastTransDate;
    std::map<int, int>  m_accountDeptMap;
    std::set<int>       m_accountWithoutDept;
    //std::map<int, std::pair<double, double> > m_deptClosing;
    accountDeptAmountType   m_closingValues;
    accountDeptAmountType   m_openingValues;

    accountTwoAmountType m_accountIncomeAndPayment;
    void calculateClosingAsOnInternal(QDate date, accountDeptAmountType& closingValue,
                                      accountCashORBankAmountType& cashBankValue);
    QString getQeryStr(QDate fromDate, QDate toDate);
    void getAccountIncomePaymentForQuery(QString query,
                                         accountTwoAmountType& processedDate,
                                         accountCashORBankAmountType& cashBankValue);


};

#endif // CCLOSINGCALCULATOR_H
