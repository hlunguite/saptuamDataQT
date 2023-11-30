#ifndef CREMITTANCEDETAILS_H
#define CREMITTANCEDETAILS_H
#include <map>
#include <set>
#include <QVector>
#include <vector>
struct StransactionData;
struct SremitTransDetail {
    int m_transID;
    QString m_from;
    double m_amount;
    QString m_receiptBook;
    QString m_receiptNo;
    int     m_accountID;
    SremitTransDetail(QString str);
    SremitTransDetail(int id, QString name, double amount, QString book, QString no, int accountID):
        m_transID(id),
        m_from(name), m_amount(amount), m_receiptBook(book), m_receiptNo(no), m_accountID(accountID){}
    QString toString();

};
struct StransForRemittance{
    double                          m_hqShare;
    double                          m_localShare;

    std::map<int, std::set<int> >   m_receipts;
    std::vector<SremitTransDetail>  m_allTrans;
    StransForRemittance() {
        m_hqShare = 0;
        m_localShare = 0;
        m_receipts.clear();
        m_allTrans.clear();
    }

};
class CremittanceDetails
{
public:
    CremittanceDetails(int remittanceID);
    const  QVector<SremitTransDetail>& getTransactionForRemittance() const { return m_remittanceTransaction;}
    const std::map<int, std::pair<double, double> >& getAccountPct() const { return  m_accountPct;}
    const std::map<int, std::pair<double, double> >& getAccountDeptLocalAndDeptAmt() const { return  m_accountDeptLocalHqAmt;}

private:
    int m_remittanceTableID;
    std::map<int, std::pair<double, double> >   m_accountPct;
    std::map<int, std::pair<double, double> >   m_accountDeptLocalHqAmt;
    QVector<SremitTransDetail>                  m_remittanceTransaction;

};

#endif // CREMITTANCEDETAILS_H
