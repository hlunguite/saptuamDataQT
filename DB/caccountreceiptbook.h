#ifndef CACCOUNTRECEIPTBOOK_H
#define CACCOUNTRECEIPTBOOK_H
#include <QVector>
#include "cdbtablebase.h"
#include "creceiptbooktable.h"
struct SreceiptBook;
enum EaccountReceiptBookTableFieldIndex {
    ACCOUNT_RECEIPT_BOOK_TABLE_ID_IDX,
    ACCOUNT_RECEIPT_BOOK_TABLE_ACCOUNT_ID_IDX,
    ACCOUNT_RECEIPT_BOOK_TABLE_RECEIPT_BOOK_TABLE_ID_IDX,
    ACCOUNT_RECEIPT_BOOK_TABLE_MAX_IDX

};
struct SaccountReceipt{
    int m_accountReceiptID;
    int m_accountTableID;
    int m_receiptBoookTableID;
    SaccountReceipt();
    SaccountReceipt(const SaccountReceipt& that);
    SaccountReceipt & operator = (const SaccountReceipt& that);
    int save();
    void deleteAccountReceipt();
};

class CaccountReceiptBook : public CdbTableBase, public CsdSingleton<CaccountReceiptBook>
{
    friend class CsdSingleton<CaccountReceiptBook>;

private:
    CaccountReceiptBook();
    ~CaccountReceiptBook(){}
public:
    QVector<SaccountReceipt> getAllAccountReceipt();
   // bool getIsReceiptAssignToAccount(int receiptBookNo, int receiptSlNo, int accountId);
    //bool getISReceiptAssignToAccount(int receiptBookID, int accountId);


};

#endif // CACCOUNTRECEIPTBOOK_H
