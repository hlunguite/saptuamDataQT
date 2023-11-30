#include "caccounttreemodel.h"
#include "cdepartmentaccounttable.h"
#include "caccounttable.h"
#include "cdepartmenttable.h"
#include "cremittancesetuptable.h"
#include "cpaymentaccounttable.h"
CaccountTreeModel::CaccountTreeModel(QObject* parent):
    QAbstractItemModel(parent)
{
    populateAccountandDept();
}

CaccountTreeModel::~CaccountTreeModel()
{

}

QModelIndex
CaccountTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0){
        qDebug()<<"parent col isnot equal 0";
        return QModelIndex();
    }

    int parentId = parent.internalId();

    SdbObjects* objects =  CdepartmentAccountTable::Object()->getAllDepartAccountWithParent(parentId);
    if (objects) {
        int size = objects->m_listofObject.size();
        //qDebug()<<"outside size "<<size;
        for (int i = 0; i < size; ++i) {
            if (i == row) {
                const SdepartmentAccountData* accData = static_cast<SdepartmentAccountData*>(objects->m_listofObject.at(i));

                int id = accData->m_id;
                if (id > 0) {
                    delete objects;
                    return createIndex(row, column, id);
                }
            }
        }

        delete objects;
    }

    return QModelIndex();
}

QModelIndex
CaccountTreeModel::parent(const QModelIndex &child) const
{
    if (!child.isValid()){
        return QModelIndex();
    }
    int childid = child.internalId();
    if (childid == 0){
        return QModelIndex();
    }
    //qDebug()<<"Child id is "<<childid;
    int parentId = -1;
    int row = -1;
    SdepartmentAccountData* data = CdepartmentAccountTable::Object()->getDeptAccountDataForID(childid);
    if (data) {
        parentId = data->m_parentID;
        //qDebug()<<" parent for child "<<childid<<" parent "<<parentId;
        delete data;
    }
    if (parentId > -1) {
        SdbObjects* objects = CdepartmentAccountTable::Object()->getAllDepartAccountWithParent(parentId);
        if (objects) {
            int size = objects->size();
            for (int i = 0; i < size; ++i) {
                const SdepartmentAccountData* data = (SdepartmentAccountData*)objects->at(i);
                if (childid == data->m_id) {
                    row = i;
                    break;
                }
            }
            delete objects;
        }

        if (row >= 0) {
            //qDebug()<<"Id is "<<childid<<" parent "<<parentId<<" row "<<row;
            return createIndex(row, 0, parentId);
        }
    }
    //qDebug()<<"empty index for Child id is "<<childid<<" parentId "<<parentId;

    return QModelIndex();

}

int
CaccountTreeModel::rowCount(const QModelIndex &parent) const
{
    int parentId = parent.internalId();

    int count = 0;
    SdbObjects* objects = CdepartmentAccountTable::Object()->getAllDepartAccountWithParent(parentId);
    if (objects) {
        count = objects->size();
        delete objects;
    }
    //qDebug()<<"row count for parent "<<parentId<<" "<<count<<"\n";
    return count;
}

int
CaccountTreeModel::columnCount(const QModelIndex &parent) const
{
    /*TfilterObjType filterObj;
    CdepartmentAccountObject* deptAccObj = (CdepartmentAccountObject*)CdepartmentAccountTable::Object()->filter(filterObj);
    int count = 0;
    if (deptAccObj) {
        if (deptAccObj->count()) {
            count = deptAccObj->at(0).count();
        }
    }*/
    const QVector<QString > & header = CdepartmentAccountTable::Object()->getTableHeader();
    return header.size();
}

QVariant
CaccountTreeModel::data(const QModelIndex &index, int role) const
{

    int col = index.column();
    if(role == Qt::DisplayRole){
        int id = index.internalId();
        SdepartmentAccountData* data  = CdepartmentAccountTable::Object()->getDeptAccountDataForID(id);
        if (data) {
            //qDebug()<<"found for id";
            /*
            DEPARTMENT_ACCOUNT_ID_IDX,
            ,
            DEPARTMENT_ACCOUNT_PARENT_ID
*/
            if (col == DEPARTMENT_ACCOUNT_DEPARTMENT_OR_ACCOUNT_ID) {
                int accId = data->m_deptOrAccountID;
                delete data;
                //qDebug()<<id<<" CaccountTreeModel account or dept "<<m_idAccountDeptNameMap[accId]<<" accId "<<accId<<" parent "<<data->m_parentID;
                 return m_idAccountDeptNameMap[accId];
            }
            else if (col == DEPARTMENT_ACCOUNT_IS_DEPT) {

                int accountid = data->m_deptOrAccountID;
                delete data;
                //qDebug()<<id<<" CaccountTreeModel "
                return m_idAccountTypeMap[accountid];
            }
            else if (DEPARTMENT_ACCOUNT_PARENT_ID == col) {
                if (data->m_isDept) {
                    return "";
                }
                int accountid = data->m_deptOrAccountID;
                delete data;
                return m_remittanceDetail[accountid];
            }
            int id = data->m_id;
            delete data;
            return id;
        }

    }
    else if(Qt::TextAlignmentRole == role){
        return Qt::AlignLeft;
    }

    return QVariant();
}

QVariant
CaccountTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    //qDebug()<<"headerData call";

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole){

        const QVector<QString > & header = CdepartmentAccountTable::Object()->getTableHeader();
        //qDebug()<<section<<" header size "<<header.size();
        QVariant v =  header.at(section);
        if (section == DEPARTMENT_ACCOUNT_IS_DEPT) {
           return "Account Type";
        } if (section == DEPARTMENT_ACCOUNT_PARENT_ID) {
            return "Remittance/Payment Account Detail";
        }
        return v;
    }

    return QVariant();
}

bool
CaccountTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    qDebug()<<"Set header data call";
      Q_ASSERT(0);
    return false;
}

void CaccountTreeModel::refreshTable()
{
    populateAccountandDept();
    beginResetModel();
    endResetModel();
}

void CaccountTreeModel::populateAccountandDept()
{
    m_idAccountDeptNameMap.clear();
    m_nameAccountDeptIdMap.clear();
    m_remittanceDetail.clear();
    TobjectList values;
    QString condition;
    if (CdepartmentTable::Object()->getTableValues(condition, values)){

        int size = values.size();
        for (int i = 0; i < size; ++i) {
           const Tobjects& dept = values.at(i);
            int id = dept.at(DEPARTMENT_ID_IDX).toInt();
            QString name = dept.at(DEPARTMENT_NAME_IDX).toString();
            m_idAccountDeptNameMap.insert(id, name);
            m_nameAccountDeptIdMap.insert(name, id);
            m_idAccountTypeMap.insert(id, "");
        }
    }
    values.clear();
    if (CaccountTable::Object()->getTableValues(condition, values)){

        int size = values.size();
        for (int i = 0; i < size; ++i) {
            const Tobjects& account = values.at(i);
            int id = account.at(ACCOUNT_ID_IDX).toInt();
            QString name = account.at(ACCOUNT_NAME_IDX).toString();
            m_idAccountDeptNameMap.insert(id, name);
            m_nameAccountDeptIdMap.insert(name, id);
            int accountType = account.at(ACCOUNT_TYPE_IDX).toInt();
            QString accountTypeStr;
            switch(accountType) {
                case INCOME_ACCOUNT_TYPE: accountTypeStr = "Income Account";break;
                case PAYMENT_ACCOUNT_TYPE: accountTypeStr = "Payment Account";break;
                case BANK_ACCOUNT_TYPE: accountTypeStr = gBankAccountName;break;
                case REQUEST_ACCOUNT_TYPE: accountTypeStr = gRequestAccountName;  break;
                case REMITTANCE_ACCOUNT_TYPE: accountTypeStr = gRemittanceAccountName;  break;
                case LOAN_ACCOUNT_TYPE: accountTypeStr = gLoanAccountName;  break;

                default: Q_ASSERT(0);
            }
            m_idAccountTypeMap.insert(id, accountTypeStr);
            QString remitDetail;
            SremittanceSetupTableData * remitData = CremittanceSetupTable::Object()->getRemittanceSetupForAccount(id);
            if (remitData) {

                SremittanceNameData* nameData = CremittanceNameTable::Object()->getRemittanceNameData(remitData->m_nameID);
                SremitrancePercentData* pcData = CremittancePercentTable::Object()->getRemittancePercentageForID(remitData->m_percentageID);
                delete remitData;
                if (nameData) {
                    remitDetail = nameData->m_name + " ";
                    delete nameData;
                }
                if (pcData) {
                    remitDetail += "Local Share: " + QString::number(pcData->m_localShare) + "% ";
                    remitDetail += "HQ Share: " + QString::number(pcData->m_hqShare) + "% ";

                }
            } else {
                int paymentAccountID = account.at(ACCOUNT_PAYMENT_ACCOUNT_ID_IDX).toInt();
                if (paymentAccountID > 0) {
                    SpaymentAccountData* data = CpaymentAccountTable::Object()->getPaymentAccountDataForId(paymentAccountID);
                    if (data) {
                        remitDetail = data->m_accountName;
                        delete data;
                    }

                }
            }
            m_remittanceDetail.insert(id, remitDetail);
        }
    }

}

