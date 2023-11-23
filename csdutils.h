#ifndef CSDUTILS_H
#define CSDUTILS_H
#include <QString>
#include <QDate>
#include <set>
class CsdUtils
{
public:
    CsdUtils();
    ~CsdUtils();
    static QString ucfirst( QString str);
    static QString abbravate(QString str);
    static QString convertToWord(QString amount);
    static QString convertToWord(double amtf);
    static QString makedir(QString dirname);
    static QString simplifyString(QString);
    static bool isOverlap(int x1, int y1, int x2, int y2);

    static QString getSaptuamMin();
    static void setSaptuamMin(const QString &value);
    static void setCurrentWdir(QString);
    static QString getCurrentWdir();
    static void setOrgination(QString);
    static void setLocation(QString);
    static QString getOrgination();
    static QString getLocaton();
    static QString getSaptuamShortName();
    static void setSaptuamShortName(const QString& value);

    static float getFontSize();
    static void setFontSize(float value);

    static QDate getFyStartDate();
    static void  setFyStartDate(QDate date);
    static QDate getFyEndDate();
    static void  setFyEndDate(QDate date);


    static QString getDbName();
    static void setDbName(const QString &dbName);
    static QString createFullName(QString title, QString firstName, QString lastName);
    static QString createFullNameWithSpouse(QString name, QString relationship, QString spouse);

    static int maxHeight();
    static void setMaxHeight(int maxHeight);

    static int maxWidth();
    static void setMaxWidth(int maxWidth);
    static QString converAmountToString(double d, char format = 'f', int precision = 2, QString append = QString(""));
    static QString makeReceiptString(const std::set<int>& receipts);
    static QStringList getHeaderText();

    static QDate getNextDay(QDate date);
    static QDate getPrevDay(QDate date);

private:
    static QString m_saptuamMin;
    static QString m_developers;
    static QString m_softwarename;
    static QString m_currentWdir;
    static QString m_location;
    static QString m_orgination;
    static float   m_fontSize;
    static QDate   m_fyStartDate;
    static QDate   m_fyEndDate;
    static QString m_dbName;
    static int     m_maxHeight;
    static int     m_maxWidth;
    static QString m_shortName;

};

#endif // CSDUTILS_H
