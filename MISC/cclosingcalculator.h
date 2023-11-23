#ifndef CCLOSINGCALCULATOR_H
#define CCLOSINGCALCULATOR_H
#include <QDate>
#include <map>
#include <set>
class CclosingCalculator
{
public:
    CclosingCalculator();
    void calculateClosing(QDate fromDate, QDate toDate);
    void calculateClosing(QDate fromDate, QString query);
    void calculateClosing(QDate openingDate);

private:
    double              m_cashOpening;
    double              m_cashClosing;
    double              m_bankOpening;
    double              m_bankClosing;
    std::map<int, int>  m_accountDeptMap;
    std::set<int>       m_accountWithoutDept;
    //std::map<int, std::pair<double, double> > m_deptClosing;
    std::map<int, std::pair<double, double> > m_openingAndClosingBalance;
    std::map<int, std::pair<double, double> > m_accountIncomeAndPayment;
    QString getQeryStr(QDate fromDate, QDate toDate);
    void processQuery(QString query, std::map<int, std::pair<double, double> >& processedDate);

};

#endif // CCLOSINGCALCULATOR_H
