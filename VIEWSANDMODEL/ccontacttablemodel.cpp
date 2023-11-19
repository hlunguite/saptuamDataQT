#include "ccontacttablemodel.h"
#include "DB/ccontacttable.h"
#include "DB/cpersontable.h"
CcontactTableModel::CcontactTableModel(QObject *parent, QSqlDatabase db):
    QSqlTableModel(parent,db)
{

}

CcontactTableModel::~CcontactTableModel()
{

}



QVariant CcontactTableModel::data(const QModelIndex &index, int role) const
{
    QVariant d = QSqlTableModel::data(index, role);

    if (role == Qt::DisplayRole) {
        int row = index.row();
        if (index.column() == CONTACT_FULL_NAME_WITH_SPOUSE_IDX) {


            QModelIndex personIndex = this->index(row, CONTACT_PERSON_ID_IDX);
            int contactID = QSqlTableModel::data(personIndex,role).toInt();
            if (contactID > 0) {
                SpersonData* person =  CpersonTable::Object()->getPersonData(contactID);
                if (person) {
                    QString parentfullName = person->getParentFullName();
                    delete person;
                    return parentfullName;
                }
            }
            return "";
        }
        if (index.column() == CONTACT_PHONE_IDX) {
            QModelIndex personIndex = this->index(row, CONTACT_PERSON_ID_IDX);
            int contactID = QSqlTableModel::data(personIndex,role).toInt();
            if (contactID > 0) {
                SpersonData* person =  CpersonTable::Object()->getPersonData(contactID);
                if (person) {
                    QString fullName = person->getFullName();
                    QString phone = person->m_phone;
                    //qDebug()<<"contact phoneno "<<fullName<<" "<<d<<" "<<person->m_phone;
                    delete person;
                    return phone;
                }
            }
        }
    } else if (role == Qt::TextAlignmentRole) {
        return ((int)Qt::AlignLeft | (int)Qt::AlignVCenter);
    }
    return d;
}
