#include "cpaymentaccounttab.h"
#include <QHeaderView>
#include <QMenu>
#include <QSqlTableModel>
#include "cpaymentaccounttable.h"
#include "cpaymentaccountdlg.h"

CpaymentAccountTab::CpaymentAccountTab(QWidget *parent)
    : QWidget{parent}
{

    m_tableModel = new QSqlTableModel(this, CpaymentAccountTable::Object()->getDataBase());
    m_tableModel->setTable(CpaymentAccountTable::Object()->getTableName());

    m_tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //m_tableModel->setFilter("");
    m_tableModel->select();
    m_tableModel->sort(PAYMENT_ACCOUNT_ID_IDX,Qt::AscendingOrder);
    m_table = new QTableView();
    m_table->setModel(m_tableModel);
    m_table->setSortingEnabled(true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->verticalHeader()->hide();
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

    m_tableModel->setHeaderData(PAYMENT_ACCOUNT_ID_IDX, Qt::Horizontal, "ID");
    m_tableModel->setHeaderData(PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX, Qt::Horizontal, "Account Name");
    m_tableModel->setHeaderData(PAYMENT_ACCOUNT_NAME_IDX, Qt::Horizontal, "Payment Account Name");
    m_tableModel->setHeaderData(PAYMENT_ACCOUNT_NATURE_OF_EXPENSE_IDX, Qt::Horizontal, "Nature of Expense");


    m_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_table,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));


    m_toolBar = new QToolBar("Payment Account Toolbar");
    m_newPaymentAccount = new QAction("New Payment Account", this);
    m_toolBar->addAction(m_newPaymentAccount);
    m_toolBar->addSeparator();
    connect(m_newPaymentAccount, SIGNAL(triggered()), this, SLOT(newPaymentAccount()));

    m_editPaymentAccount = new QAction("Edit", this);
    connect(m_editPaymentAccount, SIGNAL(triggered()), this, SLOT(editPaymentAccount()));

    m_editID = 0;

    m_vlayout = new QVBoxLayout();
    m_vlayout->addWidget(m_toolBar);
    m_vlayout->addWidget(m_table);
    setLayout(m_vlayout);
}

void CpaymentAccountTab::customContextMenu(QPoint pos)
{
    m_editID = 0;

    QPoint point(0,pos.y());
    QModelIndex index = m_table->indexAt(point);
    QMenu *menu=new QMenu(this);
    menu->addAction(m_newPaymentAccount);

    if(index.isValid()){
        m_editID = index.data().toInt();
        if (m_editID > 0) {
            menu->addAction(m_editPaymentAccount);


        }
    }
    menu->popup(m_table->viewport()->mapToGlobal(pos));
}

void CpaymentAccountTab::newPaymentAccount()
{
    qDebug()<<"new payment account";
    CpaymentAccountDlg dlg;
    dlg.setWindowTitle("New Payment Acccount");
    dlg.exec();

}

void CpaymentAccountTab::editPaymentAccount()
{
    qDebug()<<"Edit payment account "<<m_editID;
    CpaymentAccountDlg dlg;
    dlg.setWindowTitle("Edit Payment Acccount");
    dlg.exec();
}

void CpaymentAccountTab::resizeEvent(QResizeEvent *event)
{
    int width = geometry().size().width();
    m_table->horizontalHeader()->resizeSection(PAYMENT_ACCOUNT_ID_IDX,0.05*width);
    m_table->horizontalHeader()->resizeSection(PAYMENT_ACCOUNT_ACCOUNT_NAME_IDX,0.3*width);
    m_table->horizontalHeader()->resizeSection(PAYMENT_ACCOUNT_NAME_IDX,0.3*width);
    m_table->horizontalHeader()->resizeSection(PAYMENT_ACCOUNT_NATURE_OF_EXPENSE_IDX,0.3        *width);

}
