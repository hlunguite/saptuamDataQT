#include "cbulkupdatedlg.h"
#include "ui_cbulkupdatedlg.h"
#include "MISC/CdlgDefine.h"
#include "ccontacttable.h"
#include "cpersontable.h"
#include "csdutils.h"
#include "ctransactiontable.h"
#include "caccountmap.h"
CbulkUpdateDlg::CbulkUpdateDlg(QWidget *parent)
    : CdlgBase(parent)
    , ui(new Ui::CbulkUpdateDlg)
{
    ui->setupUi(this);

    std::map<int, int > nameIDParentIDMap;
    std::map<int, QVector<QString> > idNameMap;
    std::map<int, QString> relationShipMap;
    QVector<SpersonData *>* persons = CpersonTable::Object()->getAllPresonData();
    for (auto person : *persons) {
        int id = person->m_id;
        QString title = person->m_title;
        QString firstName = person->m_firstName;
        QString lastName = person->m_lastName;
        QString relationshi = person->m_relationship;
        int parentID = person->m_parentOrSpousePersonId;
        //QString name = CsdUtils::createFullName(title, firstName, lastName);
        if (parentID > 0) {
            nameIDParentIDMap[id] = parentID;
            relationShipMap[id] = relationshi;
        }
        idNameMap[id].push_back(title);
        idNameMap[id].push_back(firstName);
        idNameMap[id].push_back(lastName);

        //nameIDParentIDMap[id] = std::make_pair(name, parentID);
    }
    delete persons;


    QString condition;// =  CcontactTable::Object()->getColName(CONTACT_PERSON_ID_IDX) + " > 0";
    TobjectList values;
    if ( CcontactTable::Object()->getTableValues(condition, values)) {
        int size = values.size();
        for (int i = 0; i < size; ++i) {
            int contactID = values.at(i).at(CONTACT_ID_IDX).toInt();
            QString fullName = values.at(i).at(CONTACT_FULL_NAME_IDX).toString();
            QString fullNameWithoutTitle = values.at(i).at(CONTACT_FULL_NAME_IDX_WITHOUT_TITLE).toString();
            int id = values.at(i).at(CONTACT_PERSON_ID_IDX).toInt();

            //
           // m_nameIDMap[fullName].insert(contactID);
           // m_nameIDMap[fullNameWithoutTitle].insert(contactID);
           // m_allFullName.insert(contactID, fullName);
            if (id > 0) {
                std::map<int, int >::iterator fn = nameIDParentIDMap.find(id);
                QString parentTitle;
                QString parentFirstName;
                QString parentLastName;
                QString relationship;
                if (fn != nameIDParentIDMap.end()) {
                    int parentID = fn->second;
                    QVector<QString>& n = idNameMap[parentID];
                    parentTitle = n.at(0);
                    parentFirstName = n.at(1);
                    parentLastName = n.at(2);
                    //parentName = idNameMap[parentID];
                    relationship = relationShipMap[id];
                }
                QString parentFullName;
                QString parentFullNameWithoutTitle;
                if (parentFirstName.isEmpty() == false) {
                    parentFullName = CsdUtils::createFullName(parentTitle, parentFirstName, parentLastName);
                    parentFullNameWithoutTitle = CsdUtils::createFullName("", parentFirstName, parentLastName);
                }
                QString name = CsdUtils::createFullNameWithSpouse(fullName, relationship, parentFullName);
                //qDebug()<<name<<" person id "<<id<<" contact id "<<contactID;
                m_minList.push_back(name);
                m_nameContactIDMap[name] = contactID;
                m_contactIDNameMap[contactID] = name;
                m_namePersonIDMap[name] = id;
               // m_nameCompleter.push_back(name);
            } else {
                m_nameContactIDMap[fullName] = contactID;
                m_contactIDNameMap[contactID] = fullName;
            }
        }
    }
    m_minList.sort();

    m_nameCompleter = new QCompleter(m_minList, this);
    m_nameCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    m_nameCompleter->setWrapAround(true);
    m_nameCompleter->setFilterMode(Qt::MatchContains);
    ui->m_changedName->setCompleter(m_nameCompleter);
    ui->m_nameToChangeLineEdit->setCompleter(m_nameCompleter);
    ui->m_accountCombo->addItem("");
    const QStringList& accountList = CaccountMap::Object()->getAccountList();
    ui->m_accountCombo->addItems(accountList);

    ui->m_dateFrom->setDate(QDate::currentDate());
    ui->m_dateTo->setDate(QDate::currentDate());


    //m_nameToMergeCompleter =new
}

CbulkUpdateDlg::~CbulkUpdateDlg()
{
    delete ui;
}

void CbulkUpdateDlg::on_m_mergeNameOpt_clicked(bool checked)
{
    if (checked) {
        resize();
    }
}

void CbulkUpdateDlg::resize()
{

    int windowwidth = geometry().size().width();
    int windowheight = geometry().size().height();
    int x = ui->m_updateOptBox->geometry().x();
    int y = ui->m_updateOptBox->geometry().y();
    int height = ui->m_updateOptBox->height();
    ui->m_updateOptBox->setGeometry(x, y, DEPT_ACCOUNT_COMBO_SIZE.width()*2, height );

    y += height + GAP;

    bool isMergeName = ui->m_mergeNameOpt->isChecked();
    if (isMergeName) {
        ui->m_changedName->setPlaceholderText("Name to merge");
        ui->m_changedName->show();
        ui->m_nameToChangeLineEdit->show();
        ui->m_nameToChangeLineEdit->setPlaceholderText("Merge name to");


    } else {
        ui->m_changedName->hide();
        ui->m_nameToChangeLineEdit->hide();
    }
    ui->m_fromDateLbl->hide();
    ui->m_toDateLbl->hide();
    ui->m_accountLbl->hide();
    ui->m_transTable->hide();
    ui->m_selectAllBtn->hide();

    bool isChangeTransData = ui->m_changeTransDateOpt->isChecked();
    bool isChangeTransName = ui->m_changeTrannsactionNameOpt->isChecked();
    if (isChangeTransData || isChangeTransName) {

        ui->m_dateFrom->show();
        ui->m_dateTo->show();
        ui->m_accountCombo->show();
        ui->m_fromDateLbl->show();
        ui->m_toDateLbl->show();
        ui->m_accountLbl->show();
        ui->m_transTable->show();
        ui->m_transTable->clearContents();
        ui->m_transTable->setRowCount(0);
        ui->m_selectAllBtn->show();

        if (isChangeTransName) {
            ui->m_nameToChangeLineEdit->show();
            ui->m_nameToChangeLineEdit->setPlaceholderText("Name for transaction");

            ui->m_changedName->setPlaceholderText("Change Name to");
            ui->m_changedName->show();

        }
    } else {
        ui->m_dateFrom->hide();
        ui->m_dateTo->hide();
        ui->m_accountCombo->hide();
        ui->m_nameToChangeLineEdit->setPlaceholderText("Name to change");

        ui->m_changedName->setPlaceholderText("Change Name to");
    }

    if (isMergeName || isChangeTransName) {
        ui->m_nameToChangeLineEdit->setGeometry(x, y, DEPT_ACCOUNT_COMBO_SIZE.width()*2, DEPT_ACCOUNT_COMBO_SIZE.height());

        y += GAP + DEPT_ACCOUNT_COMBO_SIZE.height();
        ui->m_changedName->setGeometry(x, y, DEPT_ACCOUNT_COMBO_SIZE.width()*2, DEPT_ACCOUNT_COMBO_SIZE.height());

         y += GAP + DEPT_ACCOUNT_COMBO_SIZE.height();
    }
    if (isChangeTransName || isChangeTransData) {
        int x1 = x;

        int y1 = y;
        int y2 = y + GAP + DATE_SIZE.height();;
        ui->m_fromDateLbl->setGeometry(x1,y1, DATE_SIZE.width(), DATE_SIZE.height());
        ui->m_dateFrom->setGeometry(x1, y2, DATE_SIZE.width(), DATE_SIZE.height());
        x1 += DATE_SIZE.width() + GAP;

        ui->m_toDateLbl->setGeometry(x1, y1, DATE_SIZE.width(), DATE_SIZE.height());
        ui->m_dateTo->setGeometry(x1, y2, DATE_SIZE.width(), DATE_SIZE.height());
        x1 += DATE_SIZE.width() + GAP;

        ui->m_accountLbl->setGeometry(x1, y1, DEPT_ACCOUNT_COMBO_SIZE.width(),  DEPT_ACCOUNT_COMBO_SIZE.height());
        ui->m_accountCombo->setGeometry(x1, y2,  DEPT_ACCOUNT_COMBO_SIZE.width(),  DEPT_ACCOUNT_COMBO_SIZE.height());
        x1 +=  DEPT_ACCOUNT_COMBO_SIZE.width() + GAP;
        ui->m_selectAllBtn->setGeometry(x1, y2, BUTTON_SIZE.width(), BUTTON_SIZE.height());


        y = y2 + GAP + DATE_SIZE.height();

        if (isChangeTransData) {
            ui->m_fromDateLbl->setText("Change Date from");
            ui->m_toDateLbl->setText("Change Date To");
        } else {
            ui->m_fromDateLbl->setText("Select date ");
            ui->m_toDateLbl->setText("between");
        }
        int height = windowheight - y - (3*GAP) - (BUTTON_SIZE.height());
        int width = windowwidth - 2*x;
        ui->m_transTable->setGeometry(x, y, width, height);
        ui->m_transTable->horizontalHeader()->resizeSection(0 ,width*.40);
        ui->m_transTable->horizontalHeader()->resizeSection(1 ,width*.10);
        ui->m_transTable->horizontalHeader()->resizeSection(2 ,width*.20);
        ui->m_transTable->horizontalHeader()->resizeSection(3 ,width*.30);
        ui->m_transTable->horizontalHeader()->resizeSection(4 ,0);


        y += height + GAP;
    }


    x =  windowwidth/2 - (BUTTON_SIZE.width() + GAP);
    y =  windowheight - (2*GAP) - (BUTTON_SIZE.height());
    ui->m_okBtn->setGeometry(x, y, BUTTON_SIZE.width(), BUTTON_SIZE.height());
    x += (BUTTON_SIZE.width() + GAP);
    ui->m_cancelBtn->setGeometry(x, y, BUTTON_SIZE.width(), BUTTON_SIZE.height());

    populateTable();

}

bool CbulkUpdateDlg::mergeName()
{
    QString nameToChange = ui->m_nameToChangeLineEdit->text().trimmed();
    int nameToChangeContactID = m_nameContactIDMap[nameToChange];
    int nameToChangePersonID = m_namePersonIDMap[nameToChange];
    QString mergeName = ui->m_changedName->text().trimmed();
    int mergeNameContactID = m_nameContactIDMap[mergeName];
    if (nameToChangeContactID > 0 && mergeNameContactID > 0) {
        //qDebug()<<"Replace "<<nameToChange<<" contactid "<<nameToChangeContactID<<" person id "<<nameToChangePersonID<<" with "<<mergeName;
        QVector<StransactionData*> allTrans = CtransactionTable::Object()->getAllTransactionForContact(nameToChangeContactID);
        for (StransactionData* data : allTrans)  {
           // qDebug()<<data->m_fromId<<" "<<data->m_amount;
            data->m_fromId = mergeNameContactID;
            data->save();
            delete data;
        }
        ScontactData contact;
        contact.m_idx = nameToChangeContactID;
        contact.deleteObj();
        if (nameToChangePersonID > 0) {
            SpersonData person;
            person.m_id = nameToChangePersonID;
            person.deleteObj();
        }

        emit updateContactTable();
        emit updateTransaction();

        return true;
    } else {
        if (nameToChangeContactID < 1) {
            QString msg = "Name to change \"" + nameToChange + "\" is not part of contact ";
            message(msg);
        }
        else if (mergeNameContactID < 1) {
            QString msg = "Name change to \"" + nameToChange + "\" is not part of contact ";
            message(msg);
        }
    }

    return false;
}

void CbulkUpdateDlg::populateTable()
{
    bool isChangeTransData = ui->m_changeTransDateOpt->isChecked();
    bool isChangeTransName = ui->m_changeTrannsactionNameOpt->isChecked();
    if (isChangeTransData == false && isChangeTransName == false) {
        return;
    }
    QString startDate = ui->m_dateFrom->date().toString("yyyy-MM-dd");
    QString toDate = ui->m_dateTo->date().toString("yyyy-MM-dd");

    QString filter = " WHERE (" + CtransactionTable::Object()->getColName(TRANSACTION_STATUS_IDX) + "=1) ";
    QString accountName = ui->m_accountCombo->currentText();
    int accountID = 0;
    if (accountName.isEmpty() == false) {
        accountID = CaccountMap::Object()->getAccountID(accountName);
    }
    if (accountID > 0) {
        filter += " AND (" + CtransactionTable::Object()->getColName(TRANSACTION_ACCOUNT_IDX) + "=" + QString::number(accountID) + ")";
    }

    QString name = ui->m_nameToChangeLineEdit->text().trimmed();
    if (name.isEmpty() == false) {
        int contactID = m_nameContactIDMap[name];
        if (contactID > 0) {
             filter += " AND (" + CtransactionTable::Object()->getColName(TRANSACTION_FROM_IDX) + "=" + QString::number(contactID) + ")";
        }
    }
    if (isChangeTransName) {
        filter += " AND (" +  CtransactionTable::Object()->getColName(TRANSACTION_DATE_IDX) + " BETWEEN \"" + startDate  + "\" AND \"";
        filter += toDate +  "\")";
    } else {
        filter += " AND (" +  CtransactionTable::Object()->getColName(TRANSACTION_DATE_IDX) + " BETWEEN \"" + startDate  + "\" AND \"";
        filter += startDate +  "\")";
    }
    ui->m_transTable->clearContents();
    ui->m_transTable->setRowCount(0);

    QString query = "SELECT * FROM " +  CtransactionTable::Object()->getTableName() + filter;
    QVector<StransactionData*> results = CtransactionTable::Object()->getAllTransaction(query);

    for (auto transData: results) {

        int rowcount = ui->m_transTable->rowCount();
        ui->m_transTable->insertRow(rowcount);

        QString name = m_contactIDNameMap[transData->m_fromId];
        QTableWidgetItem *item1 = new QTableWidgetItem();
        if (name.isEmpty() == false) {
            item1->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item1->setCheckState(Qt::Unchecked);
            item1->setText(name);
        } else {
           // qDebug()<<"Empty name for "<<transData->m_fromId<<" trans id "<<transData->m_id;
            item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
        }
        ui->m_transTable->setItem(rowcount, 0, item1);

        QString dateStr = transData->m_date.toString("dd MMM yyyy");
        QTableWidgetItem *item2 = new QTableWidgetItem();
        if (dateStr.isEmpty() == false) {
            item2->setText(dateStr);
        } else {
            item2->setFlags(item2->flags() & ~Qt::ItemIsEditable);
        }
        ui->m_transTable->setItem(rowcount, 1, item2);

        QString amount = CsdUtils::convertAmountToStringWithSign(transData->m_amount);
        QTableWidgetItem *item3 = new QTableWidgetItem();
        if (amount.isEmpty() == false) {
            item3->setText(amount);
            item3->setTextAlignment(Qt::AlignRight);
        } else {
            item3->setFlags(item3->flags() & ~Qt::ItemIsEditable);
        }
        ui->m_transTable->setItem(rowcount, 2, item3);


        QString account = CaccountMap::Object()->getAccountName(transData->m_accountId);
        QTableWidgetItem *item4 = new QTableWidgetItem();
        if (account.isEmpty() == false) {
            item4->setText(account);
        } else {
            item4->setFlags(item4->flags() & ~Qt::ItemIsEditable);
        }
        ui->m_transTable->setItem(rowcount, 3, item4);

        QString id = QString::number(transData->m_id);
        QTableWidgetItem *item5 = new QTableWidgetItem();
        if (id.isEmpty() == false) {
            item5->setText(id);
        } else {
            item5->setFlags(item5->flags() & ~Qt::ItemIsEditable);
        }
        ui->m_transTable->setItem(rowcount, 4, item5);

        delete transData;
    }


//TRANSACTION_ID_IDX     TRANSACTION_FROM_IDX,
//    TRANSACTION_DATE_IDX,
     // CtransactionTable::Object()->updateValue(data.at(i), i , m_id, TRANSACTION_ID_IDX);

}

bool CbulkUpdateDlg::updateData(bool updateName)
{
    int numrow = ui->m_transTable->rowCount();
    if (numrow == 0) {
        QString msg = "No data to update";
        message(msg);
        return false;
    }
    QDate dateTo = ui->m_dateTo->date();
    QString mergeName = ui->m_changedName->text().trimmed();
    int mergeNameContactID =  0;
    if (mergeName.isEmpty() == false) {
        mergeNameContactID = m_nameContactIDMap[mergeName];
    }

    for (int i = 0; i < numrow; ++i) {
        QTableWidgetItem * name = ui->m_transTable->item(i,0);
        if (name && name->text().isEmpty() == false && name->checkState() == Qt::Checked) {
             //QTableWidgetItem * account = ui->m_transTable->item(i,3);
             QTableWidgetItem * id = ui->m_transTable->item(i,4);
             int transID = id->text().toInt();
             if (transID) {
                 if (updateName) {
                     CtransactionTable::Object()->updateValue(mergeNameContactID,  TRANSACTION_FROM_IDX, transID, TRANSACTION_ID_IDX);
                 } else {
                     CtransactionTable::Object()->updateValue(dateTo,  TRANSACTION_DATE_IDX, transID, TRANSACTION_ID_IDX);
                 }
             }
             //qDebug()<<name->text()<<" "<<account->text()<<" id "<<id->text().toInt();

        }
    }
    return true;
}

void CbulkUpdateDlg::resizeEvent(QResizeEvent *event)
{
    resize();
}




void CbulkUpdateDlg::on_m_noOpt_clicked(bool checked)
{
    if (checked) {
        resize();
    }
}


void CbulkUpdateDlg::on_m_changeTransDateOpt_clicked(bool checked)
{
    if (checked) {
        resize();
    }
}


void CbulkUpdateDlg::on_m_changeTrannsactionNameOpt_clicked(bool checked)
{
    if (checked) {
        resize();
    }
}



void CbulkUpdateDlg::on_m_cancelBtn_clicked()
{
    emit reject();
}


void CbulkUpdateDlg::on_m_okBtn_clicked()
{
    if (ui->m_mergeNameOpt->isChecked()) {
        if (mergeName() == false) {
            return;
        }
    }
    if (ui->m_changeTrannsactionNameOpt->isChecked()) {
        QString nameToChange = ui->m_nameToChangeLineEdit->text().trimmed();
        int nameToChangeContactID = m_nameContactIDMap[nameToChange];
        QString mergeName = ui->m_changedName->text().trimmed();
        int mergeNameContactID = m_nameContactIDMap[mergeName];
        if (nameToChangeContactID == 0) {
            QString msg = "Name to change \"" + nameToChange +"\" is not in contact";
            message(msg);
            return;
        }

        if (mergeNameContactID == 0) {
            QString msg = "Name change to \"" + mergeName +"\" is not in contact";
            message(msg);
            return;
        }
        if(updateData(true) == false) {
            return;
        }

    }
    if (ui->m_changeTransDateOpt->isChecked()) {
        if(updateData(false) == false) {
            return;
        }
    }
    emit accept();
}


void CbulkUpdateDlg::on_m_nameToChangeLineEdit_editingFinished()
{
    resize();
}


void CbulkUpdateDlg::on_m_accountCombo_currentTextChanged(const QString &arg1)
{
    resize();
}


void CbulkUpdateDlg::on_m_dateTo_dateChanged(const QDate &date)
{
    resize();
}


void CbulkUpdateDlg::on_m_dateFrom_dateChanged(const QDate &date)
{
    resize();
}


void CbulkUpdateDlg::on_m_selectAllBtn_clicked()
{
    int numrow = ui->m_transTable->rowCount();
    for (int i = 0; i < numrow; ++i) {
        QTableWidgetItem * name = ui->m_transTable->item(i,0);
        if (name && name->text().isEmpty() == false) {
            name->setCheckState(Qt::Checked);
        }
    }
}

