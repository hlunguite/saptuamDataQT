#include "cremittancesetuptable.h"

CremittancePercentTable::CremittancePercentTable():
    CdbTableBase(REMITTANCE_PERCENT_TABLE)

{
    addTableCol("remittancePercentageID", FIELD_TYPE_PRIMARY_KEY, "Perentage ID", true, REMITTANCE_PERCENT_TABLE_START_NO);
    addTableCol("localShare", FIELD_TYPE_FLOAT, "Local Share", false);
    addTableCol("hqShare", FIELD_TYPE_FLOAT, "Hq Share", false);
}


SremitrancePercentData *CremittancePercentTable::getRemittancePercentageForID(int ID)
{
    SremitrancePercentData* data = nullptr;
    if (ID > 0) {
        QString tosearch = QString::number(ID);
        QString condition = getSearchString(tosearch, REMITTANCE_PERCENT_TABLE_ID_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            const Tobjects& obj = values.at(0);
            data = new SremitrancePercentData;
            data->m_id = obj.at(REMITTANCE_PERCENT_TABLE_ID_IDX).toInt();
            data->m_localShare = obj.at(REMITTANCE_PERCENT_TABLE_LOCAL_SHARE_IDX).toInt();
            data->m_hqShare = obj.at(REMITTANCE_PERCENT_TABLE_HQ_SHARE_IDX).toInt();

        }
    }
    return data;
}

SremitrancePercentData *CremittancePercentTable::getRemittancePercentage(int hqShare, int localShare)
{
    SremitrancePercentData* data = nullptr;
    if (hqShare > 0 || localShare > 0) {
        QString hqShareStr = QString::number(hqShare);
        QString condition = getSearchString(hqShareStr, REMITTANCE_PERCENT_TABLE_HQ_SHARE_IDX);
        QString localShareStr = QString::number(localShare);
        condition += " AND " + getSearchString(localShareStr, REMITTANCE_PERCENT_TABLE_LOCAL_SHARE_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            const Tobjects& obj = values.at(0);
            data = new SremitrancePercentData;
            data->m_id = obj.at(REMITTANCE_PERCENT_TABLE_ID_IDX).toInt();
            data->m_localShare = obj.at(REMITTANCE_PERCENT_TABLE_LOCAL_SHARE_IDX).toInt();
            data->m_hqShare = obj.at(REMITTANCE_PERCENT_TABLE_HQ_SHARE_IDX).toInt();
        }
    }
    return data;
}

void CremittancePercentTable::serializeTable(QDataStream &out, const Tobjects &obj)
{
    SremitrancePercentData data;
    data.m_id = obj.at(REMITTANCE_PERCENT_TABLE_ID_IDX).toInt();
    data.m_localShare = obj.at(REMITTANCE_PERCENT_TABLE_LOCAL_SHARE_IDX).toInt();
    data.m_hqShare = obj.at(REMITTANCE_PERCENT_TABLE_HQ_SHARE_IDX).toInt();
    data.serialize(out);
}

void CremittancePercentTable::desserializeTable(QDataStream &in)
{
    SremitrancePercentData data;
    data.desserialize(in);
}


int SremitrancePercentData::save()
{
    SremitrancePercentData* data = CremittancePercentTable::Object()-> getRemittancePercentage(m_hqShare, m_localShare);

    if (data) {
        m_id = data->m_id;
        delete data;
    } else {
        Tobjects obj(REMITTANCE_PERCENT_TABLE_MAX_FIELD);
        obj[REMITTANCE_PERCENT_TABLE_ID_IDX] = 0;
        obj[REMITTANCE_PERCENT_TABLE_LOCAL_SHARE_IDX] = m_localShare;
        obj[REMITTANCE_PERCENT_TABLE_HQ_SHARE_IDX] = m_hqShare;
        m_id = CremittancePercentTable::Object()->insertInTable(obj).toInt();
    }
    return m_id;
}

void SremitrancePercentData::deleteObj()
{
    if (m_id > 0) {
        CremittancePercentTable::Object()->deleteValue(m_id, REMITTANCE_PERCENT_TABLE_ID_IDX);
        m_id = 0;
    }


}

void SremitrancePercentData::serialize(QDataStream &out)
{
    out<<m_id<<m_localShare<<m_hqShare;
    //qDebug()<<"dumping pc "<<m_id<<" "<<m_localShare<<" "<<m_hqShare;

}

void SremitrancePercentData::desserialize(QDataStream &in)
{
    in>>m_id>>m_localShare>>m_hqShare;
    qDebug()<<"reading pc "<<m_id<<" "<<m_localShare<<" "<<m_hqShare;

}

CremittanceNameTable::CremittanceNameTable():
     CdbTableBase(REMITTANCE_NAME_TABLE)
{
    addTableCol("remittanceNameID", FIELD_TYPE_PRIMARY_KEY, "Name ID", true, REMITTANCE_NAME_TABLE_START_NO);
    addTableCol("remittanceName", FIELD_TYPE_STRING, "Name", false);
}

SremittanceNameData *CremittanceNameTable::getRemittanceNameData(QString name)
{
    SremittanceNameData* data = nullptr;
    if (name.isEmpty() == false) {
        QString condition = getSearchString(name, REMITTANCE_NAME_TABLE_NAME_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            const Tobjects& obj = values.at(0);
            data = new SremittanceNameData;
            data->m_id = obj.at(REMITTANCE_NAME_TABLE_ID_IDX).toInt();
            data->m_name = obj.at(REMITTANCE_NAME_TABLE_NAME_IDX).toString();
        }
    }
    return data;
}

SremittanceNameData *CremittanceNameTable::getRemittanceNameData(int id)
{
    SremittanceNameData* data = nullptr;
    if (id > 0) {
        QString tosearch = QString::number(id);
        QString condition = getSearchString(tosearch, REMITTANCE_NAME_TABLE_ID_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            const Tobjects& obj = values.at(0);
            data = new SremittanceNameData;
            data->m_id = obj.at(REMITTANCE_NAME_TABLE_ID_IDX).toInt();
            data->m_name = obj.at(REMITTANCE_NAME_TABLE_NAME_IDX).toString();
        }
    }
    return data;
}

QStringList CremittanceNameTable::getAllRemittanceName()
{
    QStringList namelist;
    QString condition;
    TobjectList values;
    if (getTableValues(condition, values)) {
        int size = values.size();
        for (int i = 0; i <size; ++i) {
            namelist.push_back(values.at(i).at(REMITTANCE_NAME_TABLE_NAME_IDX).toString());
        }
    }
    namelist.sort();
    return namelist;
}

void CremittanceNameTable::serializeTable(QDataStream &out, const Tobjects &obj)
{
    SremittanceNameData data;
    data.m_id = obj.at(REMITTANCE_NAME_TABLE_ID_IDX).toInt();
    data.m_name = obj.at(REMITTANCE_NAME_TABLE_NAME_IDX).toString();
    data.serialize(out);

}

void CremittanceNameTable::desserializeTable(QDataStream &in)
{
    SremittanceNameData data;
    data.desserialize(in);
}

int SremittanceNameData::save()
{
    SremittanceNameData * data = CremittanceNameTable::Object()->getRemittanceNameData(m_name);
    if (data) {
        m_id = data->m_id;
        delete data;
        return m_id;
    }  else {
        if (m_id > 0) {
            SremittanceNameData * data = CremittanceNameTable::Object()->getRemittanceNameData(m_id);
            if (data) {
                CremittanceNameTable::Object()->updateValue(m_name, REMITTANCE_NAME_TABLE_NAME_IDX, m_id, REMITTANCE_NAME_TABLE_ID_IDX);
                delete data;
                return m_id;
            }
        }
        Tobjects obj(REMITTANCE_NAME_TABLE_MAX_IDX);
        obj[REMITTANCE_NAME_TABLE_ID_IDX] = 0;
        obj[REMITTANCE_NAME_TABLE_NAME_IDX] = m_name;
        m_id = CremittanceNameTable::Object()->insertInTable(obj).toInt();
    }
    return m_id;
}

void SremittanceNameData::deleteObj()
{
    if (m_id > 0) {
        CremittanceNameTable::Object()->deleteValue(m_id, REMITTANCE_NAME_TABLE_ID_IDX);
    }
}

void SremittanceNameData::serialize(QDataStream &out)
{
    out<<m_id<<m_name;
}

void SremittanceNameData::desserialize(QDataStream &in)
{
    in>>m_id>>m_name;
    qDebug()<<"reading remit name "<<m_id<<" "<<m_name;
}

CremittanceSetupTable::CremittanceSetupTable():
    CdbTableBase(REMITTANCE_SETUP_TABLE)
{
    addTableCol("remittanceSetupID", FIELD_TYPE_PRIMARY_KEY, "Setup ID", true, REMITTANCE_SETUP_TABLE_START_NO);
    addTableCol("remittanceSetupAccountID", FIELD_TYPE_INT, "Account ID");
    addTableCol("remittanceSetupNameID", FIELD_TYPE_INT, "Name ID");
    addTableCol("remittanceSetupPercentID", FIELD_TYPE_INT, "Percent ID");


}

SremittanceSetupTableData *CremittanceSetupTable::getRemittanceSetupForAccount(int acountID)
{
    SremittanceSetupTableData* data = nullptr;
    if (acountID > 0) {
        QString tosearch = QString::number(acountID);
        QString condition = getSearchString(tosearch, REMITTANCE_SETUP_TABLE_ACCOUNT_ID_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            const Tobjects& obj = values.at(0);
            data = new SremittanceSetupTableData;
            data->m_id = obj.at(REMITTANCE_SETUP_TABLE_ID_IDX).toInt();
            data->m_accountID = obj.at(REMITTANCE_SETUP_TABLE_ACCOUNT_ID_IDX).toInt();
            data->m_nameID = obj.at(REMITTANCE_SETUP_TABLE_NAME_ID_IDX).toInt();
            data->m_percentageID = obj.at(REMITTANCE_SETUP_TABLE_PERCENTAGE_ID_IDX).toInt();
        }

    }
    return data;
}

QVector<SremittanceSetupTableData *> *CremittanceSetupTable::getRemitanceSetupForPercentID(int percentID)
{
    QVector<SremittanceSetupTableData*>* dataV = nullptr;
    if (percentID > 0) {
        QString tosearch = QString::number(percentID);
        QString condition = getSearchString(tosearch, REMITTANCE_SETUP_TABLE_PERCENTAGE_ID_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            dataV = new QVector<SremittanceSetupTableData*>;
            for (int i = 0; i < values.size(); ++i) {
                const Tobjects& obj = values.at(i);
                SremittanceSetupTableData* data = new SremittanceSetupTableData;
                data->m_id = obj.at(REMITTANCE_SETUP_TABLE_ID_IDX).toInt();
                data->m_accountID = obj.at(REMITTANCE_SETUP_TABLE_ACCOUNT_ID_IDX).toInt();
                data->m_nameID = obj.at(REMITTANCE_SETUP_TABLE_NAME_ID_IDX).toInt();
                data->m_percentageID = obj.at(REMITTANCE_SETUP_TABLE_PERCENTAGE_ID_IDX).toInt();
                dataV->push_back(data);
            }
        }

    }
    return dataV;
}

bool CremittanceSetupTable::deleteRemittanePCIfNotUSe(int percentID)
{
    bool ret = false;
    if (percentID > 0) {
        QString tosearch = QString::number(percentID);
        QString condition = getSearchString(tosearch, REMITTANCE_SETUP_TABLE_PERCENTAGE_ID_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            ret = false;
        } else {
            SremitrancePercentData * data = CremittancePercentTable::Object()->getRemittancePercentageForID(percentID);
            if (data) {
                data->deleteObj();
                delete data;
                ret = true;
            }
        }

    }
    return ret;
}

QVector<SremittanceSetupTableData *>* CremittanceSetupTable::getRemitanceSetupForName(QString name)
{
    QVector<SremittanceSetupTableData*>* nameList = nullptr;
    if (name.isEmpty() == false) {
        SremittanceNameData *nameData = CremittanceNameTable::Object()->getRemittanceNameData(name);
        int id = 0;
        if (nameData) {
            id = nameData->m_id;
            delete nameData;
        } else {
            return nullptr;
        }



        QString tosearch = QString::number(id);
        QString condition = getSearchString(tosearch, REMITTANCE_SETUP_TABLE_NAME_ID_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            nameList = new QVector<SremittanceSetupTableData*>;
            for (int i = 0; i < values.size(); ++i) {
                const Tobjects& obj = values.at(i);
                SremittanceSetupTableData* data = new SremittanceSetupTableData;
                data->m_id = obj.at(REMITTANCE_SETUP_TABLE_ID_IDX).toInt();
                data->m_accountID = obj.at(REMITTANCE_SETUP_TABLE_ACCOUNT_ID_IDX).toInt();
                data->m_nameID = obj.at(REMITTANCE_SETUP_TABLE_NAME_ID_IDX).toInt();
                data->m_percentageID = obj.at(REMITTANCE_SETUP_TABLE_PERCENTAGE_ID_IDX).toInt();
                nameList->push_back(data);
            }
        }

    }
    return nameList;
}

bool CremittanceSetupTable::deleteREmitanceNameIfNotUSe(QString name)
{
    bool ret = false;
    if (name.isEmpty() == false) {
        SremittanceNameData *nameData = CremittanceNameTable::Object()->getRemittanceNameData(name);
        int id = 0;
        if (nameData) {
            id = nameData->m_id;
        } else {
            return ret;
        }



        QString tosearch = QString::number(id);
        QString condition = getSearchString(tosearch, REMITTANCE_SETUP_TABLE_NAME_ID_IDX);
        TobjectList values;
        if (getTableValues(condition, values)) {
            ret = false;
        } else {
            nameData->deleteObj();
            ret = true;
        }
        delete nameData;


    }
    return ret;
}

void CremittanceSetupTable::serializeTable(QDataStream &out, const Tobjects &obj)
{
    SremittanceSetupTableData  data;
    data.m_id = obj.at(REMITTANCE_SETUP_TABLE_ID_IDX).toInt();
    data.m_accountID = obj.at(REMITTANCE_SETUP_TABLE_ACCOUNT_ID_IDX).toInt();
    data.m_nameID = obj.at(REMITTANCE_SETUP_TABLE_NAME_ID_IDX).toInt();
    data.m_percentageID = obj.at(REMITTANCE_SETUP_TABLE_PERCENTAGE_ID_IDX).toInt();
    data.serialize(out);
}

void CremittanceSetupTable::desserializeTable(QDataStream &in)
{
    SremittanceSetupTableData  data;
    data.desserialize(in);
}

int SremittanceSetupTableData::save()
{
    SremittanceSetupTableData* data = CremittanceSetupTable::Object()->getRemittanceSetupForAccount(m_accountID);
    if (data) {
        m_id = data->m_id;
        CremittanceSetupTable::Object()->updateValue(m_nameID, REMITTANCE_SETUP_TABLE_NAME_ID_IDX, m_id, REMITTANCE_SETUP_TABLE_ID_IDX);
        CremittanceSetupTable::Object()->updateValue(m_percentageID, REMITTANCE_SETUP_TABLE_PERCENTAGE_ID_IDX, m_id, REMITTANCE_SETUP_TABLE_ID_IDX);
        delete data;
    } else {
        Tobjects obj (REMITTANCE_SETUP_TABLE_MAX_IDX);
        obj[REMITTANCE_SETUP_TABLE_ID_IDX] = 0;
        obj[REMITTANCE_SETUP_TABLE_ACCOUNT_ID_IDX] = m_accountID;
        obj[REMITTANCE_SETUP_TABLE_NAME_ID_IDX] = m_nameID;
        obj[REMITTANCE_SETUP_TABLE_PERCENTAGE_ID_IDX] = m_percentageID;
        m_id = CremittanceSetupTable::Object()->insertInTable(obj).toInt();
    }
    return m_id;

}

void SremittanceSetupTableData::deleteObj()
{
    if (m_id > 0) {
        CremittanceSetupTable::Object()->deleteValue(m_id, REMITTANCE_SETUP_TABLE_ID_IDX);
        m_id  = 0;
    }
}

void SremittanceSetupTableData::serialize(QDataStream &out)
{
    out<<m_id<<m_accountID<<m_nameID<<m_percentageID;
}

void SremittanceSetupTableData::desserialize(QDataStream &in)
{
    in>>m_id>>m_accountID>>m_nameID>>m_percentageID;
    //qDebug()<<"reading remit setup "<<m_id<<" "<<m_accountID<<" "<<m_percentageID;

}
