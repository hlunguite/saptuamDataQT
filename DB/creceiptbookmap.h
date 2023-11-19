#ifndef CRECEIPTBOOKMAP_H
#define CRECEIPTBOOKMAP_H

#include <csdsingleton.h>
#include <QString>
#include <QStringList>
#include <QMap>
#include <QVector>
#include <set>
using receiptNoType = int;
#include "caccountreceiptbook.h"

struct Sreceipt{

    int m_receiptBookNo;
    int m_receiptBookSlNo;
    int m_receiptBookTableId; //id of CreceiptBookTable
    bool m_isUse;
    Sreceipt(int bookno = 0, int slno = 0, int receiptBookTableId = 0, bool isUse = false){
        m_receiptBookNo = bookno;
        m_receiptBookSlNo = slno;
        m_receiptBookTableId = receiptBookTableId;
        m_isUse = isUse;
    }
    bool isValid() const { return m_receiptBookNo != 0 && m_receiptBookTableId != 0; }
};


struct SreceiptBook {
    int m_id;
    receiptNoType m_bookNo;
    receiptNoType m_startNo;
    receiptNoType m_endNo;
    std::set<receiptNoType> m_receiptNo; // all available receipt;
    std::set<receiptNoType> m_cancelledList;
    std::set<receiptNoType> m_usedList;
    int m_available;
    int m_isValid;
    QString m_error;
    SreceiptBook(){
        m_id = 0;
        m_bookNo = 0;
        m_startNo = 0;
        m_endNo = 0;
        m_receiptNo.clear();
        m_cancelledList.clear();
        m_usedList.clear();
        m_available = 0;
        m_isValid = false;
        m_error.clear();
    }
    SreceiptBook(int bookNo, int startNo, int endNo);
    SreceiptBook(const SreceiptBook& that);
    SreceiptBook& operator = (const SreceiptBook& that);
    QString name() const;
    QVector<Sreceipt> getAllAvailableReceipt();
    bool markReceiptUse(int receiptSl);
    bool markReceiptCancelled(int receiptSl);
    bool isReceiptAvailable(int receiptSl);
    bool isReceiptUsed(int receiptSl);
    bool isReceiptCancelled(int receiptSl);
    bool isValid() { return m_id > 0 && m_bookNo > 0;}
    bool getAvailableCount() const { return m_receiptNo.size();}
    int save();
    void deleteObj();
};


class CreceiptBookMap : public CsdSingleton<CreceiptBookMap>
{
    friend class CsdSingleton<CreceiptBookMap>;

public:
    CreceiptBookMap();
    ~CreceiptBookMap();
    void populate();
    QVector<Sreceipt> getAllAvailableReceipt(int book);
    bool markReceiptUse(int book, int receiptSl);
    bool markReceiptCancelled(int book, int receiptSl);
    bool isReceiptAvailable(int book, int receiptSl);
    int  getReceiptAvailableCount(int book);
    SreceiptBook getReceiptBookFromID(int bookid);
    SreceiptBook getReceiptBookFromBookNo(int bookno);
    bool getIsReceiptAssignToAccount(int receiptBook, int accountID);
    QVector<SreceiptBook> getAvailableReceiptBook();

    QVector<SaccountReceipt> getAllAccountReceipt() { return  m_accountAvailableReceipts;}
    const std::set<int>& getAccountIDsForReceiptBook(int receiptBook) ;

private:
    void populateReceiptMap();
    void populateReceiptAccountMap();

    QMap<int, SreceiptBook>  m_receiptBookNoReceiptBookMap;
    QMap<int, int>           m_receiptBookTableIdToReceiptBookNo;
    QMap<int, int>           m_receiptBookNoToReceiptBookTableId;

    QVector<SaccountReceipt>                m_accountAvailableReceipts;
    //QMap<int, std::vector<int> >            m_accountIDAccountReceiptVectorIDs;
    //QMap<int, std::vector<int> >            m_receiptBookBookNoAccountReceiptVectorIDs;
    QMap<int, std::set<int> >                    m_receiptBookAccountIDsMap;

};

#endif // CRECEIPTBOOKMAP_H
