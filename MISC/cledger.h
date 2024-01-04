#ifndef CLEDGER_H
#define CLEDGER_H
#include "caccountledger.h"
#include <QMap>
#include <QSet>

class Cledger
{
public:
    Cledger();
    ~Cledger();
    void clear();
    void populateLedger( QString startDate,  QString toDate, QSet<int>& accounts, QString name = QString(), bool individualLedger = false);
    const QMap<int, QMap<int, CaccountLedger *> >& allAccountLedger() const;

    CaccountLedger& cashAccount();

    CaccountLedger& bankAccount();

private:
    CaccountLedger m_cashAccount;
    CaccountLedger m_bankAccount;
    QMap<int, QMap<int,
                   CaccountLedger*> > m_allAccountLedger;
};

#endif // CLEDGER_H
