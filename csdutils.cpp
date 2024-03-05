#include "csdutils.h"
#include <QDir>
#include <QDateTime>

QString CsdUtils::m_location = "CENTRAL DELHI";
QString CsdUtils::m_orgination = "EVANGELICAL BAPTIST CONVENTION CHURCH";
QString CsdUtils::m_developers ="Guite Software Inc";
QString CsdUtils::m_softwarename = "SaptuamData";
QString CsdUtils::m_saptuamMin = "";
QString CsdUtils::m_currentWdir = "";
int     CsdUtils::m_maxHeight = 0;
int     CsdUtils::m_maxWidth =  0;
QDate	CsdUtils::m_fyStartDate = QDate();
QDate   CsdUtils::m_fyEndDate = QDate();
float   CsdUtils::m_fontSize = 0;
QString CsdUtils::m_dbName = "";
QString CsdUtils::m_shortName = "";

CsdUtils::CsdUtils()
{

}

CsdUtils::~CsdUtils()
{

}

QString CsdUtils::ucfirst(QString str)
{
    if (str.size() < 1) {
        return "";
    }
    //QRegExp expr("(\\s+)");
    //QString to  = " ";
    str = str.trimmed().simplified(); //str.replace(expr, to).toLower();
    QStringList tokens = str.split(" ",Qt::SkipEmptyParts);
    QList<QString>::iterator tokItr = tokens.begin();

    for (tokItr = tokens.begin(); tokItr != tokens.end(); ++tokItr) {
        // qDebug()<<"Token "<<*tokItr<<" "<<(*tokItr).size();

        (*tokItr) = (*tokItr).at(0).toUpper() + (*tokItr).mid(1);
     //   qDebug()<<"Token after "<<*tokItr;
    }

    return tokens.join(" ");
}

QString CsdUtils::abbravate(QString str)
{
    str = str.trimmed().simplified(); //str.replace(expr, to).toLower();
    QStringList tokens = str.split(" ",Qt::SkipEmptyParts);
    QList<QString>::iterator tokItr = tokens.begin();
    str.clear();
    for (tokItr = tokens.begin(); tokItr != tokens.end(); ++tokItr) {
        // qDebug()<<"Token "<<*tokItr<<" "<<(*tokItr).size();

        str += (*tokItr).at(0).toUpper();
     //   qDebug()<<"Token after "<<*tokItr;
    }

    return str;
}

QString CsdUtils::convertToWord(QString amount)
{
    int len = amount.length() ;
    if(len < 1) return "";

    double amtf = amount.toFloat();
    return convertToWord(amtf);

}
QString numberToWord(int num){
    if(num == 0) return 0;

    QString word;
    if(num /10000000> 0){
        word += numberToWord(num/10000000) + " crore ";
        num %= 10000000;
    }

    if(num/100000 > 0){

        word += numberToWord(num/100000) + " lakh ";
        num %= 100000;
    }
    if(num/1000 > 0){
        word += numberToWord(num/1000) + " thousand ";
        num %= 1000;
    }
    if(num/100 > 0){
        word += numberToWord(num/100) + " hundred ";
        num %=100;
    }
    if(num > 0){
       if(num < 20){
            QStringList digits;
            if(!word.isEmpty()) word += "and ";
            digits<< "zero"<< "one"<< "two"<< "three"<< "four"<<"five"<< "six"<< "seven"<< "eight"<< "nine"<<"ten" <<"eleven"<< "twelve"<< "thirteen" << "fourteen"<< "fifteen"<< "sixteen"<< "seventeen"<<"eighteen"<< "nineteen";
            word += digits.at(num);
       }else {
           QStringList tenMultiple;
           tenMultiple <<""<< ""<< "twenty"<< "thirty"<< "forty" <<"fifty" <<"sixty"<< "seventy"<< "eighty" <<"ninety";
           word += tenMultiple.at(num/10);
           word += " " +numberToWord(num%10);
       }
    }
  return word;
}

QString CsdUtils::convertToWord(double amtf)
{
    int amti = (int)amtf;
     int paise = 0;
     int dec = (amtf -amti)*100;
     // qDebug()<<"amount "<<amtf<<" "<<amti<<" "<<dec <<"  "<<amtf-amti;
     if( (dec) > 0){
         paise = dec;
     }
     //len = amountSub.size();
     QString paisestring ;
     if(paise > 0)
         paisestring = numberToWord(paise);
     QString word = numberToWord(amti);
     if(paise> 0)
         word += " and " + paisestring +" paise";
     return word;
}

QString CsdUtils::makedir(QString dirname)
{
    QString logdir = getCurrentWdir() + "/" + dirname;
    QDir dir (logdir);
    if(dir.exists() == false){
        dir.mkpath(".");
    }
    return logdir;
}

QString CsdUtils::simplifyString(QString str)
{
    if(str.isEmpty()) return str;

    str.replace(","," ");
    str.replace(".",". ");
    str = str.trimmed().simplified().toLower();
    str = ucfirst(str);
    return str;
}

bool CsdUtils::isOverlap(int x1, int y1, int x2, int y2)
{
    int end = std::min(y1, y2);
    int start = std::max(x1, x2);
    int diff = end - start;
    bool ret = diff >= 0 ? true : false;
    return ret;
}

QString CsdUtils::getSaptuamMin()
{
    return m_saptuamMin;
}

void CsdUtils::setSaptuamMin(const QString &value)
{
    m_saptuamMin = value;
}

void CsdUtils::setCurrentWdir(QString val)
{
    m_currentWdir = val;
}

QString CsdUtils::getCurrentWdir()
{
    return m_currentWdir;
}

void CsdUtils::setOrgination(QString val)
{
    m_orgination  = val;
}

void CsdUtils::setLocation(QString val)
{
    m_location = val;
}

QString CsdUtils::getOrgination()
{
    return m_orgination;
}

QString CsdUtils::getLocaton()
{
    return m_location;
}

QString CsdUtils::getSaptuamShortName()
{
    return m_shortName;
}

void CsdUtils::setSaptuamShortName(const QString &value)
{
    m_shortName = value;
}

float CsdUtils::getFontSize()
{
    return m_fontSize;
}

void CsdUtils::setFontSize(float value)
{
    m_fontSize = value;
}

QDate CsdUtils::getFyStartDate()
{
    return m_fyStartDate;
}

void CsdUtils::setFyStartDate(QDate date)
{
    m_fyStartDate = date;
}

QDate CsdUtils::getFyEndDate()
{
    return m_fyEndDate;
}

void CsdUtils::setFyEndDate(QDate date)
{
    m_fyEndDate = date;
}


QString CsdUtils::getDbName()
{
    return m_dbName;
}

void CsdUtils::setDbName(const QString &dbName)
{
    m_dbName = dbName;
}

QString CsdUtils::createFullName(QString title, QString firstName, QString lastName)
{
    QString fullName;
    if (title.isEmpty() == false) {
        fullName = title.trimmed().simplified() + " ";
    }
    fullName += firstName.trimmed().simplified();
    if (lastName.isEmpty() == false) {
        fullName += " " + lastName.trimmed().simplified();
    }
    return ucfirst(fullName.trimmed());
}

QString CsdUtils::createFullNameWithSpouse(QString name, QString relationship, QString spouse)
{
    QString fullname = name.trimmed().simplified();
    if (spouse.isEmpty() == false) {
        QString closing;
        if (relationship.isEmpty() == false) {
            fullname += " " + relationship.trimmed().simplified() + " ";
        } else {
            fullname += " [";
            closing = "]";
        }
        fullname += spouse.trimmed().simplified() + closing;
    }

    return ucfirst(fullname.trimmed());
}
int CsdUtils::maxHeight()
{
    return m_maxHeight;
}

void CsdUtils::setMaxHeight(int maxHeight)
{
    m_maxHeight = maxHeight;
}
int CsdUtils::maxWidth()
{
    return m_maxWidth;
}

void CsdUtils::setMaxWidth(int maxWidth)
{
    m_maxWidth = maxWidth;
}

QString CsdUtils::convertAmountToStringWithSign(double d)
{
    QString formatStr;
    if (d != 0) {
        QLocale locale(QLocale::English ,QLocale::India);
        formatStr = locale.toCurrencyString(d, "₹");
    }
    return formatStr;

}

QString CsdUtils::converAmountToString(double d, char format, int precision)
{
    QString ret;
    if (d != 0) {
       ret = QString::number(d, format, precision);

    }
    return ret;
}

QString CsdUtils::makeReceiptString(const std::set<int>& receipts)
{
    QString ret;
    int previous = 0;

    QString start;

    for (auto receipt: receipts) {
       if (start.isEmpty()) {
            start = QString::number(receipt);
            previous = receipt;
       } else {
            int current =  receipt;
            if ((previous + 1) != current) {

                QString end = QString::number(previous);
                if (ret.isEmpty() == false) {
                    ret += "; ";
                }
                if (end == start) {
                    ret += end;
                } else {
                    ret += start +" - " + end;
                }

                previous = current;
                start =  QString::number(current);
            } else {
                previous = current;
            }
       }
    }


    if (start.isEmpty() == false) {
        QString end = QString::number(previous);
        if (ret.isEmpty() == false) {
           ret += "; ";
        }
        if (end == start) {
           ret += end;
        } else {
           ret += start +" - " + end;
        }
    }

    return ret;
}

QStringList CsdUtils::getHeaderText()
{
    QString org  = "EVANGELICAL BAPTIST CONVENTION CHURCH";
    QString loc = getSaptuamMin();
    QString subheader = "Reg. Under Society Act 1860 Ministry of Homes Affairs And Section 12A of the Income Tax Act 1961";
    QStringList headers;
    headers.push_back(org);
    headers.push_back(loc);
    headers.push_back(subheader);

    return headers;
}

QDate CsdUtils::getNextDay(QDate date)
{
    //get next day of prevClosing
    std::vector<int> noofDays = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    int year = date.year();
    if (QDate::isLeapYear(year)) {
        noofDays[1] = 29;
    } else {
        noofDays[1] = 28;

    }
    int day = date.day();
    int month = date.month();
    if (noofDays[month -1] == day) { // last day
        day = 1;
        if (month == 12) {
           month = 1;
            year +=1;
        }else {
           ++month;
        }
    } else {
        day += 1;
    }
    date = QDate(year, month, day);
    return date;
}

QDate CsdUtils::getPrevDay(QDate date)
{
    int day = date.day();
    int month = date.month();
    int year = date.year();
    std::vector<int> noofDays = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    //get prev date
    if (day == 1) { // first day of the month get previous month last day;
        if (month == 1) {
           month = 12;
           year -=1;
        } else {
           month -=1;
        }
        if (QDate::isLeapYear(year)) {
           noofDays[1] = 29;
        } else {
           noofDays[1] = 28;

        }
        day = noofDays[month -1]; // last day of the month
    } else {
        day = day -1;
    }
    date = QDate(year, month, day);
    return date;
}





