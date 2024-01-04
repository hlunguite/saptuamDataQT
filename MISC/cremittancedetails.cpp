#include "cremittancedetails.h"
#include "ctransactiontable.h"
#include "cremittancetable.h"
#include "csdutils.h"
#include "ccontactmap.h"
#include <cmath>

#include "caccountmap.h"
SremitTransDetail::SremitTransDetail(QString str)
{
    m_transID = 0;
    QStringList split = str.split("#");
    if (split.size() == 5) {
        m_from = split.at(0);
        m_amount = split.at(1).toDouble();
        m_receiptBook = split.at(2);
        m_receiptNo = split.at(3);
        m_accountID = split.at(4).toInt();
    }
}

QString SremitTransDetail::toString()
{
    QString amountStr = CsdUtils::converAmountToString(m_amount,'f',2);

    QString str = m_from + "#" + amountStr + "#" + m_receiptBook + "#" + m_receiptNo+ "#" + QString::number(m_accountID);
    /*QString m_from;
    QString m_amount;
    QString m_receiptBook;
    QString m_receiptNo;*/
    QStringList split = str.split("#");
    //qDebug()<<split;
    return str;
}

CremittanceDetails::CremittanceDetails(int remittanceID):m_remittanceTableID(remittanceID)
{


    QString filter = "(" +CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1)";
    QString query = "SELECT * FROM " + CtransactionTable::Object()->getTableName() + " WHERE ";
    query += CtransactionTable::Object()->getColName(TRANSACTION_REMITTANCE_ID_IDX) + "=" + QString::number(m_remittanceTableID) ;


    QVector<SremittanceDetailData*>* vData = CremittanceDetailTable::Object()->getRemittanceDetailForTableID(m_remittanceTableID);
    if (vData) {
        for (auto data : *vData) {
            int	accountTableID = data->m_accuntTableID;
            double	localShare = data->m_localShare;
            double   hqShare = data->m_hqShare;
            m_accountPct[accountTableID] = std::make_pair(hqShare,localShare);
            delete data;
        }
        delete vData;
    }


    QVector<StransactionData*> remitTrans = CtransactionTable::Object()->getAllTransaction(query);
    m_remittanceTransaction.clear();
    m_accountDeptLocalHqAmt.clear();
    //add remittance expense
    for (auto transData: remitTrans) {
        double amount = transData->m_amount;
        int transAccountID = transData->m_accountId;

        std::map<int, std::pair<double, double> >::iterator fn = m_accountPct.find(transAccountID);

        double hqPC = 0;
        double localPC = 100;
        if (fn != m_accountPct.end()) {
            hqPC = fn->second.first;
            localPC = fn->second.second;
        } else {
            //QString accountName = CaccountMap::Object()->getAccountName(transAccountID);
        }

        double hq = round((amount*hqPC)/(double)100);
        double local = round((amount*localPC)/(double)100);
        std::pair<double, double> & amt = m_accountDeptLocalHqAmt[transAccountID];
        amt.second += hq;
        amt.first += local;


         QString min = CcontactMap::Object()->getContanceName(transData->m_fromId);

        SremitTransDetail remitTransDetail(transData->m_id, min, amount, transData->m_reeiptBook, transData->m_receiptSlNo, transAccountID);
        m_remittanceTransaction.push_back(remitTransDetail);
        delete transData;
    }

    QVector<SremittanceReconcileData*>* vrData = CremittanceReconcileTable::Object()->getRemittanceReconcileForTableID(m_remittanceTableID);
    if (vrData) {
       // qDebug()<<"reconcile found";
        for (auto data :*vrData) {
            //int	 accontDepID = data->m_accontOrDeptTableID;
            //bool isAccount = data->m_isAccount;
            QString	str = data->m_str;
            SremitTransDetail detail(str);
            m_remittanceTransaction.push_back(detail);
            int id = data->m_accontOrDeptTableID;
            double amount = detail.m_amount;
            double hqPC = 0;
            double localPC = 100;
            auto fn = m_accountPct.find(id);

            if (fn != m_accountPct.end()) {
                hqPC = fn->second.first;
                localPC = fn->second.second;
            }
            double hq = round((amount*hqPC)/(double)100);
            double local = round((amount*localPC)/(double)100);
            std::pair<double, double> & amt = m_accountDeptLocalHqAmt[id];
            amt.second += hq;
            amt.first += local;



            delete data;
        }
        delete vrData;
    }
}
