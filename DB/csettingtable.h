#ifndef CSETTINGTABLE_H
#define CSETTINGTABLE_H
#include "cdbtablebase.h"


enum EsettingTableFieldIndex {
    SETTING_ID_IDX,
    SETTING_CWD_IDX,
    SETTING_ORGINATION_NAME_IDX,
    SETTING_LOCATION_IDX,
    SETTING_SAPTUAM_MIN_IDX,
    SETTING_FONT_SIZE_IDX,
    SETTING_FY_START_IDX,
    SETTING_FY_END_IDX,
    SETTING_PASSWORD_IDX,
    SETTING_MAX_IDX
};

struct SsettingData : public SdbObject {
    int m_idx;
    QString m_cwd;
    QString m_orgination;
    QString m_location;
    QString m_saptuamMin;
    int   m_fontSize;
    QDate   m_fyStart;
    QDate   m_fyEnd;
    QString m_password;
    SsettingData():SdbObject() {
        m_idx = 0;
        m_cwd = "";
        m_orgination = "";
        m_location = "";
        m_saptuamMin = "";
        m_fontSize = 0;
        m_fyStart = QDate(1900, 1, 1);
        m_fyEnd = QDate(2500, 1, 1);
        m_password = "";

    }
    ~SsettingData(){}
    int save();
    void deleteObj(){}
};

class CsettingTable : public CdbTableBase, public CsdSingleton<CsettingTable>
{
    friend class CsdSingleton<CsettingTable>;

public:
    CsettingTable();
    ~CsettingTable();

    SsettingData* getSetting();

};

#endif // CSETTINGTABLE_H
