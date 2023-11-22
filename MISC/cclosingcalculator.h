#ifndef CCLOSINGCALCULATOR_H
#define CCLOSINGCALCULATOR_H
#include <QDate>
#include <map>
#include <set>
class CclosingCalculator
{
public:
    CclosingCalculator(const QDate& currentDate);
private:
    QDate               m_closingDate;
    std::map<int, int>  m_accountDeptMap;
    std::set<int>       m_accountWithoutDept;
    void calculateClosing();
};

#endif // CCLOSINGCALCULATOR_H
