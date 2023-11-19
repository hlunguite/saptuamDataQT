#ifndef CPRINTUTILS_H
#define CPRINTUTILS_H
#include<QString>
#include<QStringList>
class CprintUtils
{
private:
    bool        m_landscape;
    QString     m_text;
    QStringList m_csvTransection;
public:
    CprintUtils(QString html, bool isLandscape = false);
    CprintUtils(QStringList csvlist);
    void printPDF();
    void printCSV();
};

#endif // CPRINTUTILS_H
