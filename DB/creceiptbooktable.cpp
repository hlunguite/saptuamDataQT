#include "creceiptbooktable.h"
#include "csdutils.h"
#include <QMap>
#include "creceiptbookmap.h"

CreceiptBookTable::CreceiptBookTable():
    CdbTableBase(RECEIPT_BOOK_TABLE)
{
    addTableCol("receiptBookID", FIELD_TYPE_PRIMARY_KEY, "ID", true, RECEIPT_BOOK_TABLE_START_NO);
    addTableCol("receiptBookNo", FIELD_TYPE_INT, "ReceiptBook");
    addTableCol("receiptBookStart", FIELD_TYPE_INT, "ReceiptBookStart");
    addTableCol("receiptBookEnd", FIELD_TYPE_INT, "ReceiptBookEnd");
    addTableCol("receiptSlNo", FIELD_TYPE_STRING, "Sl No");
    addTableCol("receiptBookCancel", FIELD_TYPE_STRING, "Cancelled""");
    addTableCol("receiptBookUsed", FIELD_TYPE_STRING, "Used");
    addTableCol("receiptBookAddDate", FIELD_TYPE_DATE, "add data");
}


QVector<SreceiptBook>
CreceiptBookTable::getReceiptBooks()
{

    QDate today = QDate::currentDate();
    QDate lastYear(today.year() -2, today.month(), today.day());

    QVector<SreceiptBook> receipts;
    QString whenstatement = " WHERE (" +  CreceiptBookTable::Object()->getColName(RECEIPT_BOOK_TABLE_ADD_DATE_IDX);
    whenstatement += " BETWEEN \"" + lastYear.toString("yyyy-MM-dd") +"\" AND \"" + today.toString("yyyy-MM-dd") +"\")";

    QString query = "SELECT * FROM " + getTableName() + whenstatement;
    query +=" ORDER BY "+ CreceiptBookTable::Object()->getColName(RECEIPT_BOOK_TABLE_RECEIPT_BOOK_NO_IDX);
    query += " ASC";

    QSqlQuery q(getDataBase());
    if(q.exec(query)){
        while(q.next()){
            SreceiptBook book = getReceiptBook(q);
            receipts.push_back(book);
        }

    }
    return receipts;
}

SreceiptBook CreceiptBookTable::getReceiptBook(const QSqlQuery &q)
{
    int pid = q.value(RECEIPT_BOOK_TABLE_ID_IDX).toInt();
    SreceiptBook book;
    book.m_id = pid;
    book.m_bookNo = q.value(RECEIPT_BOOK_TABLE_RECEIPT_BOOK_NO_IDX).toInt();
    book.m_startNo = q.value(RECEIPT_BOOK_TABLE_RECEIPT_START_IDX).toInt();
    book.m_endNo = q.value(RECEIPT_BOOK_TABLE_RECEIPT_END_IDX).toInt();
    QString receiptNo = q.value(RECEIPT_BOOK_TABLE_RECEIPT_SL_LIST_IDX).toString();
    QStringList receiptList = receiptNo.split(gReceipSeprator, Qt::SkipEmptyParts);
    QStringList cancels = q.value(RECEIPT_BOOK_TABLE_RECEIPT_CANCELLED_LIST_IDX).toString().split(gReceipSeprator, Qt::SkipEmptyParts);
    QStringList used = q.value(RECEIPT_BOOK_TABLE_RECEIPT_USED_LIST_IDX).toString().split(gReceipSeprator, Qt::SkipEmptyParts);

    for (int i = 0; i < receiptList.size(); ++i) {
        book.m_receiptNo.insert(receiptList.at(i).toInt()) ;
    }
    for (int i = 0; i < cancels.size(); ++i) {
        book.m_cancelledList.insert(cancels.at(i).toInt());
    }
    for (int i = 0; i < used.size(); ++i) {
        book.m_usedList.insert(used.at(i).toInt());
    }
    book.m_isValid = true;
    return book;
}



SreceiptBook CreceiptBookTable::getReceiptBookById(int bookid)
{
    SreceiptBook receiptbook;
    QString whenstatement = " WHERE " +  getColName(RECEIPT_BOOK_TABLE_ID_IDX) +"=" + QString::number(bookid);
    QString query = "SELECT * FROM " + getTableName() + whenstatement;
    //qDebug()<<"get receipt book query "<<query;
    QSqlQuery q(getDataBase());
    if(q.exec(query)){
        while(q.next()){
            return getReceiptBook(q);
        }

    }

    return receiptbook;
}

SreceiptBook
CreceiptBookTable::getReciptBookByBookNo(int receiptBookNo)
{
   SreceiptBook receiptBooks;
    QString whenstatement = " WHERE " +  getColName(RECEIPT_BOOK_TABLE_RECEIPT_BOOK_NO_IDX) +"=" + QString::number(receiptBookNo);
    QString query = "SELECT * FROM " + getTableName() + whenstatement;
    //qDebug()<<"get receipt book query "<<query;
    QSqlQuery q(getDataBase());
    if(q.exec(query)){
        while(q.next()){
            SreceiptBook receiptbook = getReceiptBook(q);
            return receiptbook;
        }

    }
    return receiptBooks;
}
CreceiptBookTable::~CreceiptBookTable()
{

}






