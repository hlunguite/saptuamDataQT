#include "ctransactiontab.h"
#include "ctransactiontable.h"
#include "ccontacttable.h"
#include <QHeaderView>
#include <QMenu>
#include "cedittransactiondlg.h"
#include "CsdDefine.h"
#include "cfiltertransactiondlg.h"

CtransactionTab::CtransactionTab(QWidget *parent) : QWidget(parent),
    m_selectedId(0)
{
    m_filter = CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX);
    m_filter += "=1";
    addToolBar();


    m_transationTableModel = new CtransactionTableModel(this, CtransactionTable::Object()->getDataBase());
    m_transationTableModel->setTable(CtransactionTable::Object()->getTableName());
    m_transationTableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    m_transationTableModel->setFilter(m_filter);
    m_transationTableModel->select();
    m_transationTableModel->sort(TRANSACTION_ID_IDX,Qt::DescendingOrder);
    m_table = new QTableView();
    m_table->setModel(m_transationTableModel);
    m_table->setSortingEnabled(true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->hide();
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    const QVector<QString> & header  = CtransactionTable::Object()->getTableHeader();
    int headerCount = header.size();

    for(int j = 0; j < headerCount; ++j){
        if(CtransactionTable::Object()->getIsHeaderHidden(j)) {
            m_table->hideColumn(j);
        } else {
            m_transationTableModel->setHeaderData(j, Qt::Horizontal, header.at(j));

        }
    }

    m_editTrans = new QAction("Edit", this);
    connect(m_editTrans, SIGNAL(triggered()), this, SLOT(editTrans()));
    m_markInvalid = new QAction("Mark Invalid", this);
    connect(m_markInvalid, SIGNAL(triggered()), this, SLOT(markInvalid()));

    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_table,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));




    m_vlayout = new QVBoxLayout();
    m_vlayout->addWidget(m_toolBar);
    m_vlayout->addWidget(m_table);
    setLayout(m_vlayout);
}

CtransactionTab::~CtransactionTab()
{
    delete m_editTrans;
    delete m_markInvalid;
    delete m_newTransaction;
    delete m_importBankStatment;
    delete m_enterReceiptNo;
    delete m_hqRemittance;
    delete m_summaryReport;
    delete m_filterTable;
    delete m_searchLabel;
    delete m_searchLine;
    delete m_searchInLabel;
    delete m_searchField;
    delete m_sendSMS;
    /*delete m_table;
    delete m_toolBar;
    delete m_table;
    delete m_transationTableModel;*/
    delete m_vlayout;
}

void CtransactionTab::refreshTable()
{
    m_transationTableModel->setFilter(m_filter);

    m_transationTableModel->select();

}

void CtransactionTab::customContextMenu(QPoint pos)
{
    m_selectedId = 0;
    QPoint point(0,pos.y());
    QModelIndex index = m_table->indexAt(point);
    QMenu *menu=new QMenu(this);
    menu->addAction(m_newTransaction);
    //qDebug()<<"is valid "<<index.isValid()<<" row "<<index.row()<<" "<<index.column()<<" "<<index.data();

    if(index.isValid()){
        m_selectedId = index.data().toInt();
        if (m_selectedId > 0) {
            menu->addAction(m_editTrans);
            menu->addAction(m_markInvalid);

        }
    }
    menu->popup(m_table->viewport()->mapToGlobal(pos));

}

void CtransactionTab::editTrans()
{
    if (m_selectedId > 0) {
        //qDebug()<<"Edit trans "<<m_selectedId;

        CeditTransactionDlg editTrans;
        editTrans.setIdForEdit(m_selectedId);
        editTrans.setWindowTitle("Edit Transaction");

        if (editTrans.exec()) {

        }

        refreshTable();
    }
}

void CtransactionTab::markInvalid()
{
    if (m_selectedId > 0) {
        //qDebug()<<"Mark invalid "<<m_selectedId;

        CeditTransactionDlg editTrans;
        editTrans.setIdForEdit(m_selectedId);
        editTrans.setMarkInvalid(true);
        editTrans.setWindowTitle("Mark Transaction Invalid");

        if (editTrans.exec()) {

        }
        refreshTable();

    }
}

void CtransactionTab::newTransaction()
{
    emit signalFromTransactionTab(NEW_TRANSACTION_SIGNAL);
}

void CtransactionTab::addReceiptNo()
{
    emit signalFromTransactionTab(ASSIGN_RECEIPT_SIGNAK);

}

void CtransactionTab::importBankStatment()
{
    emit signalFromTransactionTab(IMPORT_BANK_STATEMENT);
}

void CtransactionTab::processBankStatement()
{
    emit signalFromTransactionTab(PROCESS_BANK_STATEMENT);
}
void CtransactionTab::remittance()
{
    emit signalFromTransactionTab(REMITTANCE_SIGNAL);
}


void CtransactionTab::summaryReport()
{
    emit signalFromTransactionTab(SUMMARY_SIGNAL);
}

void CtransactionTab::filterTable()
{
    m_filter = CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX);
    m_filter += "=1";
    CfilterTransactionDlg dlg;
    dlg.setFilter(m_filter);
    dlg.setWindowTitle("Filter Transaction");
    if (dlg.exec()) {
        QString filter = dlg.filter();
        if (filter.isEmpty() == false) {
            m_filter = filter;
        }
    }
    //qDebug()<<"filter "<<m_filter;
    refreshTable();
}

void CtransactionTab::search()
{
    QString toSearch = m_searchLine->text().trimmed().simplified();
    int searchIn = m_searchField->currentIndex();
    if (toSearch.isEmpty() || searchIn == 0) {
        refreshTable();
        return;
    }


    QString filter ;//= CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1";
//    list<<"From/Particular"<<"Ref/Cheque No"<<"Receipt Book"<<"Receipt No";
    if (searchIn == 1) {
        QVector<QVariant> ids =  CcontactTable::Object()->findName(toSearch);
        if (ids.isEmpty() == false) {
            filter += " AND (";
        }
        for(int i = 0; i<ids.size(); ++i ) {
            if (i > 0) {
                filter +=" OR";
            }
            filter += " " + CtransactionTable::Object()->getColName(TRANSACTION_FROM_IDX)+"="+ids.at(i).toString();

        }
        if (ids.isEmpty() == false) {
            filter += ")";
        }

    } else {
        toSearch = " LIKE \"%" + toSearch + "%\"";

        if (searchIn == 2) {
            filter += " AND " + CtransactionTable::Object()->getColName(TRANSACTION_REF_IDX) + toSearch;
        } else if (searchIn == 3) {
            filter += " AND " + CtransactionTable::Object()->getColName(TRANSACTION_RECTIPT_BOOK_IDX) + toSearch;

        } else if (searchIn == 4) {
            filter += " AND " + CtransactionTable::Object()->getColName(TRANSACTION_RECTIPT_NO_IDX) + toSearch;
        }
    }
    if (filter.isEmpty()) {
        filter = " AND " + CtransactionTable::Object()->getColName(TRANSACTION_FROM_IDX)+"=0000";
    }
    filter = CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1" + filter;

    m_transationTableModel->setFilter(filter);
    m_transationTableModel->select();


}

void CtransactionTab::search(int i)
{
    Q_UNUSED(i);
    search();
}

void CtransactionTab::sendSMS()
{
    emit signalFromTransactionTab(SEND_SMS_SIGNAL);

}

void CtransactionTab::addToolBar()
{

    m_toolBar = new QToolBar("Transaction Toolbar");
    m_newTransaction = new QAction("New Transaction", this);
    m_newTransaction->setToolTip("New Transaction");
    m_toolBar->addAction(m_newTransaction);
    m_toolBar->addSeparator();
    connect(m_newTransaction, SIGNAL(triggered()), this, SLOT(newTransaction()));

    m_importBankStatment = new QAction("Import Bank Statement", this);
    m_toolBar->addAction(m_importBankStatment);
    m_toolBar->addSeparator();
    connect(m_importBankStatment, SIGNAL(triggered()), this, SLOT(importBankStatment()));

    m_processBankStatement = new QAction("Process Bank Statement", this);
    m_toolBar->addAction(m_processBankStatement);
    m_toolBar->addSeparator();
    connect(m_processBankStatement, SIGNAL(triggered()), this, SLOT(processBankStatement()));

    m_enterReceiptNo = new QAction("Assign Receipt", this);
    m_enterReceiptNo->setToolTip("Assign Receipt to Transactions");
    m_toolBar->addAction(m_enterReceiptNo);
    m_toolBar->addSeparator();
    connect(m_enterReceiptNo, SIGNAL(triggered()), this, SLOT(addReceiptNo()));

    m_hqRemittance  = new QAction("Remittance", this);
    m_toolBar->addAction(m_hqRemittance);
    m_toolBar->addSeparator();
    connect(m_hqRemittance, SIGNAL(triggered()), this, SLOT(remittance()));


    m_summaryReport = new QAction("Summary Report", this);
    m_toolBar->addAction(m_summaryReport);
    m_toolBar->addSeparator();
    connect(m_summaryReport, SIGNAL(triggered()), this, SLOT(summaryReport()));

    m_sendSMS = new QAction("Send SMS", this);
    m_toolBar->addAction(m_sendSMS);
    m_toolBar->addSeparator();
    connect(m_sendSMS, SIGNAL(triggered()), this, SLOT(sendSMS()));


    m_filterTable = new QAction("Filter Table", this);
    m_filterTable->setToolTip("Filter table to display");
    m_toolBar->addAction(m_filterTable);
    m_toolBar->addSeparator();
    connect(m_filterTable, SIGNAL(triggered()), this, SLOT(filterTable()));

    m_searchLabel = new QLabel();
    m_searchLabel->setText("Search");
    m_toolBar->addWidget(m_searchLabel);

    m_searchLine = new QLineEdit();
    m_searchLine->setFixedWidth(150);
    m_searchLine->setMinimumWidth(120);
    m_toolBar->addWidget(m_searchLine);
    connect(m_searchLine, SIGNAL(editingFinished()), this, SLOT(search()));

    m_searchInLabel = new QLabel();
    m_searchInLabel->setText(" in ");
    m_toolBar->addWidget(m_searchInLabel);

    m_searchField = new QComboBox();
    m_searchField->addItem("");
    QStringList list;
    list<<"From/Particular"<<"Ref/Cheque No"<<"Receipt Book"<<"Receipt No";
    m_searchField->addItems(list);
    connect(m_searchField, SIGNAL(currentIndexChanged(int)), this, SLOT(search(int)));
    m_toolBar->addWidget(m_searchField);

}



void CtransactionTab::resizeEvent(QResizeEvent * e)
{
    Q_UNUSED(e);
    int width = geometry().size().width();
    m_table->horizontalHeader()->resizeSection(TRANSACTION_ID_IDX,0.05*width);
    m_table->horizontalHeader()->resizeSection(TRANSACTION_FROM_IDX,0.15*width);
    m_table->horizontalHeader()->resizeSection(TRANSACTION_AMOUNT_IDX,0.10*width);
    m_table->horizontalHeader()->resizeSection(TRANSACTION_ACCOUNT_IDX,0.13*width);
    m_table->horizontalHeader()->resizeSection(TRANSACTION_DATE_IDX,0.07*width);
    m_table->horizontalHeader()->resizeSection(TRANSACTION_MODE_IDX,0.07*width);
    m_table->horizontalHeader()->resizeSection(TRANSACTION_TYPE_IDX,0.08*width);
    m_table->horizontalHeader()->resizeSection(TRANSACTION_REF_IDX,0.07*width);
    m_table->horizontalHeader()->resizeSection(TRANSACTION_RECTIPT_BOOK_IDX,0.08*width);
    m_table->horizontalHeader()->resizeSection(TRANSACTION_RECTIPT_NO_IDX,0.08*width);
    m_table->horizontalHeader()->resizeSection(TRANSACTION_PARTICULAR_IDX, 0.10*width);
}
