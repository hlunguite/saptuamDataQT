#ifndef CRECEIPTBOOKTABLE_H
#define CRECEIPTBOOKTABLE_H
#include "cdbtablebase.h"
#include <QStringList>
#include <QString>
#include <QVector>
struct SreceiptBook;
enum EreceiptBookTableFieldIndex {
    RECEIPT_BOOK_TABLE_ID_IDX,
    RECEIPT_BOOK_TABLE_RECEIPT_BOOK_NO_IDX,
    RECEIPT_BOOK_TABLE_RECEIPT_START_IDX,
    RECEIPT_BOOK_TABLE_RECEIPT_END_IDX,
    RECEIPT_BOOK_TABLE_RECEIPT_SL_LIST_IDX,
    RECEIPT_BOOK_TABLE_RECEIPT_CANCELLED_LIST_IDX,
    RECEIPT_BOOK_TABLE_RECEIPT_USED_LIST_IDX,
    RECEIPT_BOOK_TABLE_ADD_DATE_IDX,
    RECEIPT_BOOK_TABLE_MAX_IDX
};






class CreceiptBookTable : public CdbTableBase, public CsdSingleton<CreceiptBookTable>
{
    friend class CsdSingleton<CreceiptBookTable>;
public:
    QVector<SreceiptBook> getReceiptBooks();
    SreceiptBook getReceiptBookById(int bookid); // receipt book base on table id
    SreceiptBook getReciptBookByBookNo(int receiptBookNo); // receipt books base on receipt book no

    CreceiptBookTable();
    ~CreceiptBookTable();
private:
    SreceiptBook getReceiptBook(const QSqlQuery & q);


};



#endif // CRECEIPTBOOKTABLE_H
