#include "csettingtable.h"

CsettingTable::CsettingTable():
    CdbTableBase(SETTING_TABLE)
{
    addTableCol("settingID", FIELD_TYPE_PRIMARY_KEY, "Setting ID", true, SETTING_TABLE_START_NO);
    addTableCol("settingCWD", FIELD_TYPE_STRING, "CWD", false);
    addTableCol("settingOrg", FIELD_TYPE_STRING, "Org", false);
    addTableCol("settingLoc", FIELD_TYPE_STRING, "Loc", false);
    addTableCol("settingSaptuamMin", FIELD_TYPE_STRING, "Saptuam Min", false);
    addTableCol("settingFontSize", FIELD_TYPE_INT, "Font Size", false);
    addTableCol("settingFYStart", FIELD_TYPE_DATE, "FY Start", false);
    addTableCol("settingFYEnd", FIELD_TYPE_STRING, "FY End", false);
    addTableCol("settingPassword", FIELD_TYPE_STRING, "Password", false);

 }

CsettingTable::~CsettingTable()
{

}

SsettingData *CsettingTable::getSetting()
{
    QString conditon;
    SsettingData* setting = nullptr;
    TobjectList values;
    if (getTableValues(conditon, values)) {
        const Tobjects& obj = values.at(0);
        setting = new SsettingData;
        setting->m_idx = obj.at(SETTING_ID_IDX).toInt();
        setting->m_cwd = obj.at(SETTING_CWD_IDX).toString();
        setting->m_orgination = obj.at(SETTING_ORGINATION_NAME_IDX).toString();
        setting->m_location = obj.at(SETTING_LOCATION_IDX).toString();
        setting->m_saptuamMin = obj.at(SETTING_SAPTUAM_MIN_IDX).toString();
        setting->m_fontSize = obj.at(SETTING_FONT_SIZE_IDX).toInt();
        setting->m_fyStart = obj.at(SETTING_FY_START_IDX).toDate();
        setting->m_fyEnd = obj.at(SETTING_FY_END_IDX).toDate();
        setting->m_password = obj.at(SETTING_PASSWORD_IDX).toString();

    }

    return setting;
}



int SsettingData::save()
{


    QVector<QVariant> data(SETTING_MAX_IDX);
    data[SETTING_ID_IDX] = m_idx;
    data[SETTING_CWD_IDX] = m_cwd;
    data[SETTING_ORGINATION_NAME_IDX] = m_orgination;
    data[SETTING_LOCATION_IDX] = m_location;
    data[SETTING_SAPTUAM_MIN_IDX] = m_saptuamMin;
    data[SETTING_FONT_SIZE_IDX] = m_fontSize;
    data[SETTING_FY_START_IDX] = m_fyStart;
    data[SETTING_FY_END_IDX] = m_fyEnd;
    data[SETTING_PASSWORD_IDX] = m_password;


    if (m_idx == 0) {
        data[SETTING_ID_IDX] = 0;
        m_idx = CsettingTable::Object()->insertInTable(data).toInt();
    } else {
        // update
        /*for (unsigned int i = SETTING_CWD_IDX; i < SETTING_MAX_IDX; ++i) {
            CsettingTable::Object()->updateValue(data.at(i), i, m_idx, SETTING_ID_IDX);
        }*/
    }
    return m_idx;
    //return 0;
}
