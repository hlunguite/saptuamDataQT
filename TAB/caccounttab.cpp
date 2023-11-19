#include "caccounttab.h"
#include "VIEWSANDMODEL/caccounttreemodel.h"
#include "VIEWSANDMODEL/csortfilterproxymodelfortreemodel.h"
#include "VIEWSANDMODEL/cdeselectabletreeview.h"
#include "DLG/cnewaccountdlg.h"
#include "DLG/cnewdeptdlg.h"
#include <QDebug>
#include <QHeaderView>
#include "DB/cdepartmenttable.h"
#include "DB/caccounttable.h"
#include "DB/cdepartmentaccounttable.h"
#include "DB/caccountmap.h"
#include "cremittancesetuptable.h"
CaccountTab::CaccountTab(QWidget *parent) : QWidget(parent),
    m_currentAccountID(0), m_currentDeptID(0)
{
    m_virticalBox = new QVBoxLayout();

    m_newAccount = new QAction("New &Account", this);
    m_newAccount->setStatusTip("Create a new account type");
    connect(m_newAccount,SIGNAL(triggered()), this, SLOT(createNewAccount()));
    m_newDept = new QAction("New &Department", this);
    m_newDept->setStatusTip("Create a new department");
    connect(m_newDept, SIGNAL(triggered()), this, SLOT(createNewDept()));
    m_editAccount = new QAction("Edit Account", this);
    m_editAccount->setStatusTip("Edit Account");
    connect(m_editAccount, SIGNAL(triggered()), this, SLOT(editAccount()));
    m_editDept = new QAction("Edit Dept", this);
    m_editDept->setStatusTip("Edit Department");
    connect(m_editDept, SIGNAL(triggered()), this, SLOT(editDept()));

    m_toolBar = new QToolBar("Account Tab toolbar");
    m_toolBar->addAction(m_newAccount);
    m_toolBar->addSeparator();
    m_toolBar->addAction(m_newDept);

    m_virticalBox->addWidget(m_toolBar);

    m_accountTreeModel = new CaccountTreeModel();

    m_model = new CSortFilterProxyModelForTreeModel();
    m_model->setDynamicSortFilter(true);
    m_model->setSourceModel(m_accountTreeModel);

    m_treeView = new CdeselectableTreeView();
    m_treeView->setAlternatingRowColors(false);
    m_treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_treeView->setModel(m_model);
    m_treeView->expandAll();
    //m_treeView->expandToDepth(0);
    m_treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_treeView->setColumnHidden(DEPARTMENT_ACCOUNT_ID_IDX, true);

    connect(m_treeView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));

    for (int col = 0; col < m_model->columnCount(); ++col) {
        m_treeView->resizeColumnToContents(col);
    }

    m_virticalBox->addWidget(m_treeView);
    setLayout(m_virticalBox);
    resizeTree();
    CaccountMap::Object()->populateAccountMap();
}

CaccountTab::~CaccountTab()
{
    delete m_virticalBox;
    delete m_treeView;
    delete m_model;
    delete m_accountTreeModel;
    delete m_newAccount;
    delete m_newDept;
    delete m_editAccount;
    delete m_editDept;
    CaccountMap::Destroy();
}

void CaccountTab::setAccountTabWidth(int width)
{
    int scaledszed = width*.96;
    //int half = scaledszed/2;
    int height = m_treeView->height();
    //qDebug()<<"Final width "<<half;
    m_treeView->resize(scaledszed,height);
    resizeTree();
}

void CaccountTab::refreshAccounts()
{
    m_accountTreeModel->refreshTable();
    CaccountMap::Object()->populateAccountMap();
}


void CaccountTab::customContextMenu(QPoint pos)
{
    m_currentAccountID = 0;
    m_currentDeptID = 0;
    QPoint point(0,pos.y());
    QModelIndex index = m_treeView->indexAt(point);
    QMenu *menu= new QMenu(this);
    if (index.isValid() == false){
        menu->addAction(m_newAccount);
        menu->addAction(m_newDept);
    } else {

        QString nameAtIndex = index.data().toString();
        //qDebug()<<"accont name is "<<accountName;
        addContextMenu(menu, nameAtIndex);


    }

    menu->popup(m_treeView->viewport()->mapToGlobal(pos));
}

void CaccountTab::createNewAccount()
{
    CnewAccountDlg newAccountDlg(false);
    newAccountDlg.setWindowTitle("New Account");
    if (newAccountDlg.exec()) {
    } else {
        qDebug()<<"cancel\n";
    }
    refreshAccounts();
}

void CaccountTab::createNewDept()
{
    CnewDeptDlg newDept(false);
    newDept.setWindowTitle("New Department");
    if (newDept.exec()) {
    } else {

    }
    refreshAccounts();
}

void CaccountTab::editAccount()
{
    CnewAccountDlg editAccountDlg(true);
    editAccountDlg.setWindowTitle("Edit Account");
    editAccountDlg.setAccountIDForEdit(m_currentAccountID);
    if (editAccountDlg.exec()) {

    }
    refreshAccounts();
}

void CaccountTab::editDept()
{
    CnewDeptDlg editDept(true);
    editDept.setWindowTitle("Edit Department");
    editDept.setDeptTableID(m_currentDeptID);
    if (editDept.exec()) {

    }
    refreshAccounts();

}
/*
void CaccountTab::insertRow(QVector<QVariant> &data)
{
    //m_accountTreeModel->insertRowsData(data);
}*/

void CaccountTab::resizeTree()
{
    int width = m_treeView->width();
    //int half = width/2;
    //int quarter = half/2;
    QHeaderView * header = m_treeView->header();
    header->resizeSection(DEPARTMENT_ACCOUNT_DEPARTMENT_OR_ACCOUNT_ID, width*.40);
    header->resizeSection(DEPARTMENT_ACCOUNT_IS_DEPT, width*.20);
    header->resizeSection(DEPARTMENT_ACCOUNT_PARENT_ID, width*.40);
}

void CaccountTab::addContextMenu(QMenu *menu, QString nameAtIndex)
{
    menu->addAction(m_newAccount);
    menu->addAction(m_newDept);
    m_currentAccountID = 0;
    m_currentDeptID = 0;
    SaccountData* accountData = CaccountTable::Object()->getAccountDataForName(nameAtIndex);
    if (accountData) {
        if (accountData->m_accountType != REMITTANCE_ACCOUNT_TYPE &&
                accountData->m_accountType != REQUEST_ACCOUNT_TYPE &&
                accountData->m_accountType != BANK_ACCOUNT_TYPE) {
            m_currentAccountID =  accountData->m_id;
            QString name = "Edit " +  accountData->m_accountName;
            m_editAccount->setText(name);
            menu->addAction(m_editAccount);
        }
        delete accountData;
    } else {
        SdepartmentData* deptData = CdepartmentTable::Object()->getDepartmentDataByName(nameAtIndex);
        if (deptData) {
            m_currentDeptID = deptData->m_id;
            QString name = "Edit " + deptData->m_deptName;
            m_editDept->setText(name);
            menu->addAction(m_editDept);
            delete deptData;
        }
    }

}

