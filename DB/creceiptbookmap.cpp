#include "creceiptbookmap.h"
#include "creceiptbooktable.h"
#include "csdutils.h"
#include <algorithm>

SreceiptBook::SreceiptBook(int bookNo, int startNo, int endNo)
{
    m_id = 0;
    m_bookNo = bookNo;
    m_startNo = startNo;
    m_endNo = endNo;
    m_receiptNo.clear();
    m_cancelledList.clear();
    m_usedList.clear();
    for (int i = startNo; i <= endNo; ++i) {
        m_receiptNo.insert(i);
    }
    m_available = m_receiptNo.size();
    m_isValid = true;
    m_error.clear();
}

SreceiptBook::SreceiptBook(const SreceiptBook &that)
{
    m_id = that.m_id;
    m_bookNo = that.m_bookNo;
    m_startNo = that.m_startNo;
    m_endNo = that.m_endNo;
    m_receiptNo = that.m_receiptNo;
    m_available = that.m_available;
    m_cancelledList = that.m_cancelledList;
    m_usedList = that.m_usedList;
    m_error = that.m_error;
}

SreceiptBook &SreceiptBook::operator =(const SreceiptBook &that)
{
    if (this != &that) {
        m_id = that.m_id;
        m_bookNo = that.m_bookNo;
        m_startNo = that.m_startNo;
        m_endNo = that.m_endNo;
        m_available = that.m_available;
        m_receiptNo = that.m_receiptNo;
        m_usedList = that.m_usedList;

        m_error = that.m_error;
    }
    return *this;
}

QString SreceiptBook::name() const
{
    //int count = m_endNo - m_startNo + 1;

    QString ret = QString::number(m_bookNo);


    QString receipt = CsdUtils::makeReceiptString(m_receiptNo);
    if (receipt.isEmpty() == false) {
        ret += ":" + receipt;
    }


    return ret;
}



QVector<Sreceipt> SreceiptBook::getAllAvailableReceipt()
{
    QVector<Sreceipt> availableReceipt;
    for (auto receiptNo : m_receiptNo) {

        Sreceipt receipt;
        receipt.m_receiptBookNo = m_bookNo;
        receipt.m_receiptBookSlNo = receiptNo;
        receipt.m_receiptBookTableId = m_id;
        receipt.m_isUse = false;
        availableReceipt.push_back(receipt);
    }
    return availableReceipt;
}

bool SreceiptBook::markReceiptUse(int receiptSl)
{
    bool ret = false;

    if (m_receiptNo.empty() == false) {
        auto isDelete = m_receiptNo.erase(receiptSl);
        if (isDelete) {
            m_usedList.insert(receiptSl);
            ret = true;
        }
    }

    return ret;
}

bool SreceiptBook::markReceiptCancelled(int receiptSl)
{
    bool ret = false;

    if (m_receiptNo.empty() == false) {
        auto isDelete1 = m_receiptNo.erase(receiptSl);
        auto isDelete2 = m_usedList.erase(receiptSl);

        if (isDelete1 || isDelete2) {
            m_cancelledList.insert(receiptSl);
            ret = true;
        }
    }

    return ret;
}

bool SreceiptBook::isReceiptAvailable(int receiptSl)
{
    bool ret = false;
    if (receiptSl > 0) {
        std::set<int>::iterator fn = m_receiptNo.find(receiptSl);
        if (fn != m_receiptNo.end()) {
            ret = true;
        }
    }
    return ret;
}

bool SreceiptBook::isReceiptUsed(int receiptSl)
{
    bool ret = false;
    if (receiptSl > 0) {
        std::set<int>::iterator fn = m_usedList.find(receiptSl);
        if (fn != m_usedList.end()) {
            ret = true;
        }
    }
    return ret;
}

bool SreceiptBook::isReceiptCancelled(int receiptSl)
{
    bool ret = false;
    if (receiptSl > 0) {
        std::set<int>::iterator fn = m_cancelledList.find(receiptSl);
        if (fn != m_cancelledList.end()) {
            ret = true;
        }
    }
    return ret;
}



int SreceiptBook::save()
{
    m_error.clear();
    if (m_bookNo == 0) {
        return 0;
    }

    if (m_bookNo == 0 || m_startNo == 0 || m_endNo == 0 || m_startNo > m_endNo) {
        if (m_startNo > m_endNo) {
            m_error = "Receipt start number cannot be greater than end number";
        } else {
            m_error = "Receipt cannot be 0";
        }
        return 0;
    }

    if (m_id == 0) {



        //check if tring to add same receipt book;
        std::set<int> allreceipt;
        allreceipt.insert(m_receiptNo.begin(), m_receiptNo.end());
        allreceipt.insert(m_cancelledList.begin(), m_cancelledList.end());
        allreceipt.insert(m_usedList.begin(), m_usedList.end());

        SreceiptBook receiptBook = CreceiptBookTable::Object()->getReciptBookByBookNo(m_bookNo);
        if (receiptBook.isValid()) {

            bool isAlreadyCancelled = false;
            if (m_startNo == m_endNo) {
                //readd cancelled receipt only if start and end are same;
                if (receiptBook.m_cancelledList.find(m_startNo) != receiptBook.m_cancelledList.end()) {
                    isAlreadyCancelled = true;
                    m_receiptNo.insert(m_startNo);
                }
            }

            std::set<int> currentAll;
            if (isAlreadyCancelled == false) {
                std::set<int> toremove;
                for (int i : m_receiptNo) {
                    if (receiptBook.m_cancelledList.find(i) != receiptBook.m_cancelledList.end()) {
                        toremove.insert(i);
                        //m_error = "Receipt no: " + QString::number(i) + " is already cancelled";
                        //return 0;
                    }
                    if (receiptBook.m_usedList.find(i) != receiptBook.m_usedList.end()) {
                        toremove.insert(i);
                        //m_error = "Receipt no: " + QString::number(i) + " is already used";
                        //return 0;
                       // qDebug()<<"Receipt no: " + QString::number(i) + " is already used";
                    }
                }
                for (auto i : toremove) {
                    m_receiptNo.erase(i);
                }
            }


            currentAll.insert(receiptBook.m_receiptNo.begin(), receiptBook.m_receiptNo.end());
            currentAll.insert(receiptBook.m_cancelledList.begin(), receiptBook.m_cancelledList.end());
            currentAll.insert(receiptBook.m_usedList.begin(), receiptBook.m_usedList.end());




            std::set<int> intersection;
            std::set_union(allreceipt.begin(),
                           allreceipt.cend(),
                           currentAll.begin(),
                           currentAll.cend(),
                           std::inserter(intersection, intersection.begin()));

            m_receiptNo.insert(receiptBook.m_receiptNo.begin(), receiptBook.m_receiptNo.end());
            m_cancelledList.insert(receiptBook.m_cancelledList.begin(), receiptBook.m_cancelledList.end());
            m_usedList.insert(receiptBook.m_usedList.begin(), receiptBook.m_usedList.end());

            if (isAlreadyCancelled) {
                m_cancelledList.erase(m_startNo);
            }

            m_startNo = *intersection.begin();
            m_endNo = *intersection.rbegin();
            m_id = receiptBook.m_id;

        }
    }

    QStringList receiptNoList;
    for (auto i : m_receiptNo) {
       //qDebug()<<"Receipt no: " + QString::number(i) + " to add";

        receiptNoList.push_back(QString::number(i));
    }
    QStringList cancelNoList;
    for (auto i : m_cancelledList) {
        cancelNoList.push_back(QString::number(i));
        //qDebug()<<"Receipt no: " + QString::number(i) + " to cancel";

    }
    QStringList usedNoList;
    for (auto i : m_usedList) {
        usedNoList.push_back(QString::number(i));
        //qDebug()<<"Receipt no: " + QString::number(i) + " to use";

    }
    int count = receiptNoList.size() + cancelNoList.size() + usedNoList.size();
    if (count < 1) {
        m_error = "Receipt size is 0";

    }
    if (m_error.isEmpty() == false) {
        m_id = 0;
        return 0;
    }
    m_available = m_receiptNo.size();

    if (m_id == 0) {
        Tobjects receiptBook(RECEIPT_BOOK_TABLE_MAX_IDX);
        receiptBook[RECEIPT_BOOK_TABLE_ID_IDX] = 0;
        receiptBook[RECEIPT_BOOK_TABLE_RECEIPT_BOOK_NO_IDX] = m_bookNo;
        receiptBook[RECEIPT_BOOK_TABLE_RECEIPT_START_IDX] = m_startNo;
        receiptBook[RECEIPT_BOOK_TABLE_RECEIPT_END_IDX] = m_endNo;
        receiptBook[RECEIPT_BOOK_TABLE_RECEIPT_SL_LIST_IDX] = receiptNoList.join(gReceipSeprator);
        receiptBook[RECEIPT_BOOK_TABLE_RECEIPT_CANCELLED_LIST_IDX] = cancelNoList.join(gReceipSeprator);
        receiptBook[RECEIPT_BOOK_TABLE_RECEIPT_USED_LIST_IDX] = usedNoList.join(gReceipSeprator);
        receiptBook[RECEIPT_BOOK_TABLE_ADD_DATE_IDX] =  QDate::currentDate();
        m_id = CreceiptBookTable::Object()->insertInTable(receiptBook).toInt();


    } else {
        CreceiptBookTable::Object()->updateValue(m_bookNo, RECEIPT_BOOK_TABLE_RECEIPT_BOOK_NO_IDX, m_id, RECEIPT_BOOK_TABLE_ID_IDX);
        CreceiptBookTable::Object()->updateValue(m_startNo, RECEIPT_BOOK_TABLE_RECEIPT_START_IDX, m_id, RECEIPT_BOOK_TABLE_ID_IDX);
        CreceiptBookTable::Object()->updateValue(m_endNo, RECEIPT_BOOK_TABLE_RECEIPT_END_IDX, m_id, RECEIPT_BOOK_TABLE_ID_IDX);
        CreceiptBookTable::Object()->updateValue(receiptNoList.join(gReceipSeprator), RECEIPT_BOOK_TABLE_RECEIPT_SL_LIST_IDX, m_id, RECEIPT_BOOK_TABLE_ID_IDX);
        CreceiptBookTable::Object()->updateValue(cancelNoList.join(gReceipSeprator), RECEIPT_BOOK_TABLE_RECEIPT_CANCELLED_LIST_IDX, m_id, RECEIPT_BOOK_TABLE_ID_IDX);
        CreceiptBookTable::Object()->updateValue( usedNoList.join(gReceipSeprator), RECEIPT_BOOK_TABLE_RECEIPT_USED_LIST_IDX, m_id, RECEIPT_BOOK_TABLE_ID_IDX);



    }

    return m_id;
}

void SreceiptBook::deleteObj()
{

    Q_ASSERT(0);
}



CreceiptBookMap::CreceiptBookMap()
{
    populateReceiptMap();
    populateReceiptAccountMap();
}

CreceiptBookMap::~CreceiptBookMap()
{

}

void CreceiptBookMap::populate()
{
    populateReceiptMap();
    populateReceiptAccountMap();
}

void CreceiptBookMap::populateReceiptMap()
{

    m_receiptBookNoReceiptBookMap.clear();
    m_receiptBookTableIdToReceiptBookNo.clear();
    m_receiptBookNoToReceiptBookTableId.clear();
    QVector<SreceiptBook> books = CreceiptBookTable::Object()->getReceiptBooks();
    for (auto& book: books) {
        m_receiptBookNoReceiptBookMap[book.m_bookNo] = book;
        m_receiptBookTableIdToReceiptBookNo[book.m_id] = book.m_bookNo;
        m_receiptBookNoToReceiptBookTableId[book.m_bookNo]  = book.m_id;
    }
}

void CreceiptBookMap::populateReceiptAccountMap()
{
    if (m_receiptBookNoReceiptBookMap.isEmpty()) {
        populateReceiptMap();
    }
    m_accountAvailableReceipts.clear();
   // m_accountIDAccountReceiptVectorIDs.clear();
    m_receiptBookAccountIDsMap.clear();


    QVector<SaccountReceipt> accountReceipts =  CaccountReceiptBook::Object()->getAllAccountReceipt();
    for (auto accountReceipt:accountReceipts) {
        int accountID = accountReceipt.m_accountTableID;
        int receiptBookTableID = accountReceipt.m_receiptBoookTableID;
        SreceiptBook receiptBook = getReceiptBookFromID(receiptBookTableID);
        if (receiptBook.isValid() && receiptBook.getAvailableCount() > 0) {

            m_accountAvailableReceipts.push_back(accountReceipt);
           // m_accountIDAccountReceiptVectorIDs[accountID].push_back(idx);
            m_receiptBookAccountIDsMap[receiptBook.m_bookNo].insert(accountID);
        }

    }


}

QVector<Sreceipt> CreceiptBookMap::getAllAvailableReceipt(int book)
{
    QVector<Sreceipt> available;
    if (book > 0) {
        QMap<int, SreceiptBook>::iterator fn = m_receiptBookNoReceiptBookMap.find(book);
        if (fn != m_receiptBookNoReceiptBookMap.end()) {

            auto receipbBook =  fn.value();
            QVector<Sreceipt>availableBook = receipbBook.getAllAvailableReceipt();
            for (auto receipt: availableBook) {
                available.push_back(receipt);
            }

        }
    }
    return available;
}

bool CreceiptBookMap::markReceiptUse(int book, int receiptSl)
{
    bool ret = false;
    if (book > 0 && receiptSl > 0) {

        QMap<int, SreceiptBook>::iterator fn = m_receiptBookNoReceiptBookMap.find(book);
        if (fn != m_receiptBookNoReceiptBookMap.end()) {
            auto receipbBook = fn.value();
            if (receipbBook.markReceiptUse(receiptSl)) {
                receipbBook.save();
                ret = true;
                populateReceiptMap();

            }
        }
    }

    return ret;
}

bool CreceiptBookMap::markReceiptCancelled(int book, int receiptSl)
{
    bool ret = false;
    if (book > 0 && receiptSl > 0) {
        QMap<int, SreceiptBook >::iterator fn = m_receiptBookNoReceiptBookMap.find(book);
        if (fn != m_receiptBookNoReceiptBookMap.end()) {
            auto receipbBook= fn.value();
            if (receipbBook.markReceiptCancelled(receiptSl)) {
                receipbBook.save();
                ret = true;
                populateReceiptMap();

            }

        }
    }
    return ret;
}

bool CreceiptBookMap::isReceiptAvailable(int book, int receiptSl)
{
    bool ret = false;
    if (book > 0 && receiptSl > 0) {
        QMap<int, SreceiptBook >::iterator fn = m_receiptBookNoReceiptBookMap.find(book);
        if (fn != m_receiptBookNoReceiptBookMap.end()) {
            auto receipbBook= fn.value();
            if (receipbBook.isReceiptAvailable(receiptSl)) {
                ret = true;

            }
        }
    }
    return ret;
}

int CreceiptBookMap::getReceiptAvailableCount(int book)
{
    int count = 0;
    if (book > 0) {
        QMap<int, SreceiptBook>::iterator fn = m_receiptBookNoReceiptBookMap.find(book);
        if (fn != m_receiptBookNoReceiptBookMap.end()) {
            count = fn.value().m_receiptNo.size();
        }
    }
    return count;
}

SreceiptBook CreceiptBookMap::getReceiptBookFromID(int bookid)
{
    SreceiptBook book;
    if (bookid > 0) {
        auto fn = m_receiptBookTableIdToReceiptBookNo.find(bookid);
        if (fn != m_receiptBookTableIdToReceiptBookNo.end()) {
            int bookno = fn.value();
            return getReceiptBookFromBookNo(bookno);
        }
    }
    return book;
}

SreceiptBook CreceiptBookMap::getReceiptBookFromBookNo(int bookno)
{
    SreceiptBook book;
    if (bookno > 0) {
        QMap<int, SreceiptBook>::iterator fn = m_receiptBookNoReceiptBookMap.find(bookno);
        if (fn != m_receiptBookNoReceiptBookMap.end()) {
            return fn.value();
        }
    }
    return book;
}

bool CreceiptBookMap::getIsReceiptAssignToAccount(int receiptBook, int accountID)
{
    bool ret = false;
    if (receiptBook > 0 && accountID > 0) {
        auto fn = m_receiptBookAccountIDsMap.find(receiptBook);
        if (fn != m_receiptBookAccountIDsMap.end()) {
            auto fn1 = fn.value().find(accountID);
            if (fn1 != fn.value().end()) {
                ret = true;
            }
        }
    }
    return ret;
}

QVector<SreceiptBook> CreceiptBookMap::getAvailableReceiptBook()
{
    QVector<SreceiptBook> available;
    QVector<SreceiptBook> books = CreceiptBookTable::Object()->getReceiptBooks();
    for (auto& book: books) {
        if (book.getAvailableCount() > 0) {
            available.push_back(book);
        }
    }
    return available;
}

const std::set<int> &CreceiptBookMap::getAccountIDsForReceiptBook(int receiptBook)
{
    return m_receiptBookAccountIDsMap[receiptBook];
}

