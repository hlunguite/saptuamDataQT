#include "caddresstab.h"
#include <QDebug>
#include <QTableView>
#include <QMenu>
#include <QHeaderView>
#include <QMessageBox>
#include "VIEWSANDMODEL/ccontacttablemodel.h"
#include "csdutils.h"
#include "DB/ccontacttable.h"
#include "DB/cpersontable.h"
#include "DLG/ccontactdlg.h"
#include "DB/cothercontacttable.h"
#include "ccontactmap.h"

CaddressTab::CaddressTab(QWidget *parent):
    QTabWidget(parent), m_selectedID(0),
    m_tabIndex(26,0)
{
    m_addContact = new QAction("New Contact", this);
    m_editContact = new QAction("Edit Contact", this);
    m_getAllTransaction = new QAction("Get All Transaction", this);
    setupTab();

    connect(m_addContact, SIGNAL(triggered()), this, SLOT(addContact()));
    connect(m_editContact, SIGNAL(triggered()), this, SLOT(editSelectMember()));
    connect(m_getAllTransaction, SIGNAL(triggered()), this, SLOT(getAllTransaction()));
    CcontactMap::Object()->populateContactMap();

}

CaddressTab::~CaddressTab()
{
    delete m_addContact;
    delete m_editContact;
    delete m_getAllTransaction;
    CcontactMap::Destroy();

}

void CaddressTab::search(QString searchText, QString searchField)
{
    if(searchField.isEmpty() || searchText.isEmpty()){
        refreshTable();
        return;
    }
    QString whenstatement;
    if(searchField.contains("Min")){
        whenstatement = " WHERE " +  CcontactTable::Object()->getColName(CONTACT_FULL_NAME_IDX);
        whenstatement += " LIKE \"%" + CsdUtils::ucfirst(searchText) +"%\"";
    }
    else if(searchField.contains("Phone", Qt::CaseInsensitive)){
        whenstatement = " WHERE " + CcontactTable::Object()->getColName(CONTACT_PHONE_IDX);
        whenstatement += " LIKE \"%" + searchText + "%\"";
        bool ok;
        searchText.toLongLong(&ok);
        if(ok == false){
            searchText.toInt(&ok);
            if (ok == false){
                refreshTable();
                return;
            }
        }
    }else {
        return;
    }

    QString query = "SELECT * FROM " + CcontactTable::Object()->getTableName() + whenstatement;

   // qDebug()<<"Searching for "<<query;
    QSqlQuery q(CcontactTable::Object()->getDataBase());
    QVector<QPair<int, QString> > idNamePairVect;

    if(q.exec(query)){

        while(q.next()){
            int pid = q.value(CONTACT_ID_IDX).toInt();
            QString name = q.value(CONTACT_FULL_NAME_IDX_WITHOUT_TITLE).toString().trimmed().simplified();
            idNamePairVect.push_back(qMakePair(pid,name));
        }

    }
     refreshForSearch(idNamePairVect);
    //refreshTable();
}

void CaddressTab::addContact()
{
    CcontactDLG contactDlg;
    contactDlg.setWindowTitle("New Contact");
    if (contactDlg.exec()) {
        CcontactMap::Object()->addEditContact(&contactDlg);

    }
    refreshTable();
}

void CaddressTab::editSelectMember()
{
    m_selectedID = 0;
    QTableView *tableView = (QTableView*)this->currentWidget();
    QItemSelectionModel *selectionModel = tableView->selectionModel();
    QModelIndexList indexes = selectionModel->selectedRows();
    QModelIndex index;
    //  int row = -1;
    int size = indexes.size();
    if(size == 0){
        return;
    }
    if(size > 1){
        QMessageBox::warning(this,"Saptuam Data","Select only one row");
    }
    index  = indexes.at(0);
    // qDebug()<<"row no is "<<index.row();
    QSqlTableModel * proxyModel = (CcontactTableModel*)tableView->model();
    QSqlRecord record = proxyModel->record(index.row());
    m_selectedID = record.value(CONTACT_ID_IDX).toInt();

    if (m_selectedID > 0) {

        CcontactDLG contactDlg;
        contactDlg.setWindowTitle("Edit Contact");
        contactDlg.setContactEditID(m_selectedID);
        if (contactDlg.exec()) {

            CcontactMap::Object()->addEditContact(&contactDlg);

        }

        refreshTable();

    }
    m_selectedID = 0;
}

void CaddressTab::getAllTransaction()
{
    qDebug()<<"get all deposit";
}

void CaddressTab::customContextMenu(QPoint pos)
{
    QTableView *tableView = (QTableView*)this->currentWidget();
    QModelIndex index = tableView->indexAt(pos);
    QMenu *menu=new QMenu(this);
    menu->addAction(m_addContact);
    m_selectedID = 0;
    if (index.isValid()){

        QSqlTableModel * proxyModel = (CcontactTableModel*) tableView->model();
        QSqlRecord record = proxyModel->record(index.row());
        m_selectedID = record.value(CONTACT_ID_IDX).toInt();
        QString min = record.value(CONTACT_FULL_NAME_IDX).toString();
        m_getAllTransaction->setText(QString("Get all transaction for ") +min);
        menu->addAction(m_getAllTransaction);
        if (!(min == gBankCashDepositName ||
              min == gBankChargesName ||
              min == gBankInterestName ||
              min == gBankCashWithdrawalName)) {
            m_editContact->setText(QString("Edit ") + min);
            menu->addAction(m_editContact);
        }
        //menu->addAction(addHouseHold);
    }
    menu->popup(tableView->viewport()->mapToGlobal(pos));
}

void CaddressTab::doubleClick(QModelIndex index)
{
    qDebug()<<" double click";
}

void CaddressTab::setupTab()
{
    QStringList groups;
    groups << "ABC" << "DEF" << "GHI" << "JKL" << "MNO" << "PQR" << "STU" << "VW" << "XYZ";
    QChar ch('A');
    const QVector<QString> & header  = CcontactTable::Object()->getTableHeader();

    for (int i = 0; i < groups.size(); ++i) {
        QString groupName = groups.at(i);
        QString tableName = CcontactTable::Object()->getTableName();
        QSqlTableModel * tableModel = new CcontactTableModel( this, CcontactTable::Object()->getDataBase());
        tableModel->setTable(tableName);
        tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
        QString filter ;// = "contactFullName LIKE \"K%\"";
        for(int j = 0; j < groupName.size(); ++j){
            if(filter.isEmpty() == false) filter += " OR ";
            QString filterStr = "\"" + QString(groupName.at(j)) + "%\"";
            filter += CcontactTable::Object()->getColName(CONTACT_FULL_NAME_IDX_WITHOUT_TITLE) + " LIKE " + filterStr;
            ushort diff = groupName.at(j).toUpper().unicode() - ch.unicode();
            if (diff >=0 && diff <26) {
                //qDebug()<<"group char "<<groupName.at(j).toUpper().unicode()<<" - "<<ch.unicode()<<" diff "<<diff<<" value "<<i;
                m_tabIndex[diff] = i;
            }
        }
        //qDebug()<<"Group name "<<groupName<<" "<<filter;
        m_originalFilter.push_back(filter);
        tableModel->setFilter(filter);
        tableModel->select();
        tableModel->sort(CONTACT_FULL_NAME_IDX_WITHOUT_TITLE, Qt::AscendingOrder);
        //tableModel->sort(CONTACT_FULL_NAME_IDX,Qt::AscendingOrder);
        for(int j = 0; j < header.size(); ++j){
            QString head = header.at(j);
            if (head.contains("spouse", Qt::CaseInsensitive)) {
                head = "Parent/Spouse";
            }
            tableModel->setHeaderData(j, Qt::Horizontal, head);
        }
        QTableView *tableView = new QTableView;
        tableView->setModel(tableModel);

        tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
        tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableView->setSelectionMode(QAbstractItemView::SingleSelection);
        tableView->setSortingEnabled(true);
        tableView->horizontalHeader()->setStretchLastSection(true);
        tableView->verticalHeader()->hide();
        tableView->setContextMenuPolicy(Qt::CustomContextMenu);
        for(int j = 0; j < header.size(); ++j){
            if(CcontactTable::Object()->getIsHeaderHidden(j))
                tableView->hideColumn(j);

        }
        connect(tableView,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(customContextMenu(QPoint)));
        connect(tableView,SIGNAL(doubleClicked(QModelIndex)), this, SLOT(doubleClick(QModelIndex)));
        m_tabText.push_back(groupName);
        addTab(tableView, groupName);
    }

}

void CaddressTab::refreshTable()
{
    int size = this->count();
    for(int i = 0; i < size ; ++i){
        QTableView *tableView = (QTableView*)this->widget(i);
        if(tableView){
            QSqlTableModel * tableModel = (CcontactTableModel*)tableView->model();
            QString filter = m_originalFilter.at(i);
            tableModel->setFilter(filter);
            tableModel->select();
            QString text = m_tabText.at(i);
            this->setTabText(i,text);

        }
    }

}

void CaddressTab::refreshForSearch(QVector<QPair<int, QString> > &idNamePairVect)
{
    int size = idNamePairVect.size();

    QVector<QString> localFilter(this->count(),"");
    QChar ch('A');
    for(int i = 0; i < size; ++i){
        int id = idNamePairVect.at(i).first;
        QString name = idNamePairVect.at(i).second;
        ushort diff = name.at(0).toUpper().unicode() - ch.unicode();
        if (diff >=0 && diff <26) {
            int tabid = m_tabIndex.at(diff);
            //qDebug()<<" char is  "<<name.at(0).toUpper().unicode()<<" - "<<ch.unicode()<<" diff "<<diff<<" value "<<tabid;

            QString filter = localFilter[tabid];
            if(filter.isEmpty() == false) filter += " OR ";
            filter +=  CcontactTable::Object()->getColName(CONTACT_ID_IDX) + "=" + QString::number(id);
            localFilter[tabid] = filter;
        }
    }
    size = this->count();
    bool currrSet = false;
    for(int i = 0; i < size ; ++i){
        QString filter = localFilter.at(i);
        //qDebug()<<"Tab filter "<<i<<" filter"<<filter;
        //if(filter.isEmpty()) continue;
        QTableView *tableView = (QTableView*)this->widget(i);
        if(tableView){
            QSqlTableModel * tableModel = (CcontactTableModel*)tableView->model();

            QString text = m_tabText.at(i);

            if (filter.isEmpty() == false) {
                text += "*";
                if(currrSet == false){
                    this->setCurrentIndex(i);
                    currrSet = true;
                }
            } else {
                filter =  CcontactTable::Object()->getColName(CONTACT_ID_IDX) + "=0000"; // dummyid
            }
            tableModel->setFilter(filter);
            tableModel->select();
            this->setTabText(i,text);


        }
    }

}



void CaddressTab::resizeEvent(QResizeEvent * e)
{
    Q_UNUSED(e);
    int size = this->count();
    int width = geometry().size().width();
    for(int i = 0; i < size ; ++i){
        QTableView *tableView = (QTableView*)this->widget(i);
        if(tableView){
            tableView->horizontalHeader()->resizeSection(CONTACT_FULL_NAME_IDX,0.35*width);
            tableView->horizontalHeader()->resizeSection(CONTACT_FULL_NAME_WITH_SPOUSE_IDX,0.35*width);
            tableView->horizontalHeader()->resizeSection(CONTACT_PHONE_IDX,0.30*width);

        }

    }
}
