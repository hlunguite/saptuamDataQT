#include "chtmlutils.h"
#include <QDebug>
#include "csdutils.h"
#include "QRegularExpression"
ChtmlUtils::ChtmlUtils()
{
    m_cellHeight = "15";
    //m_fontSize = "9";
    //m_fontSize =" style=\"font-size:" + QString::number(9,'f',1) +"px;\" ";
    m_fontSize =" style=\"font-size:" + QString::number(CsdUtils::getFontSize()) +"px;\" ";
    //m_fontSize = "style=\"font-size:9px;\"";

    resetTableData();

}

QString ChtmlUtils::getHtmlPage(QStringList list)
{

    QString html ;
    /*"<!DOCTYPE html>  <html> <head><style> table { font-family: arial, sans-serif;";
    html += "border-collapse: collapse; width: 100%;}";
    html += "td, th {border: 2px solid #dddddd; padding: 8px; }";
    html += "tr:nth-child(even) {     background-color: #dddddd;  } </style>";
    html += "</head>      <body>";
*/
    foreach(const QString line, list) {
        html += line;
    }
    return html;
}

QString ChtmlUtils::getHtml(bool header, bool localHeader)
{
    QString html;
    if (header) {
        html += getHeader();
    }
    html += getTableHTML();
    return html;
}

QString ChtmlUtils::cellHeight() const
{
    return m_cellHeight;
}

void ChtmlUtils::setCellHeight(const QString &newCellHeight)
{
    m_cellHeight = newCellHeight;
}
/*
QString ChtmlUtils::fontSize() const
{
    return m_fontSize;
}*/

/*void ChtmlUtils::setFontSize(const QString &newFontSize)
{
    //m_fontSize = newFontSize;
    m_fontSize =" style=\"font-size:" + newFontSize +"px;\" ";
    //m_fontSize =" style=\"font-size:" + QString::number(9) +"px;\" ";


}*/

int ChtmlUtils::tableColCount() const
{
    return m_tableColCount;
}

void ChtmlUtils::setTableColCount(int newTableColCount)
{
    m_tableColCount = newTableColCount;
}

QVector<int> ChtmlUtils::tableColSize() const
{
    return m_tableColSize;
}

void ChtmlUtils::addTableColSize(int size)
{
    if(m_tableStage != NO_HTML_TABLE && m_tableStage != CLOSE_HTML_TABLE) // create size before you create a table
         Q_ASSERT(0);
    m_tableColSize.push_back(size);
}

bool ChtmlUtils::isTableHeader() const
{
    return m_isTableHeader;
}

void ChtmlUtils::setIsTableHeader(bool newIsTableHeader)
{
    m_isTableHeader = newIsTableHeader;
}

void ChtmlUtils::openTable()
{
    if(m_tableStage != NO_HTML_TABLE  && m_tableStage != CLOSE_HTML_TABLE){ // can open html only in case of no table is open currently
        Q_ASSERT(0);
    }

    m_tableStage = CREATE_HTML_TABLE;
    m_tableColCount =  0;
    if(m_isTableHeader) {
       m_tableColSize.clear();
    }
    m_htmlTable  += "<table width=\"100%\" class=\"bdr_LB\" cellpadding=\"4\" cellspacing=\"0\" border=\"0\">";
    m_tableRowCount = 0;
}

void ChtmlUtils::createHeaderRow()
{
    if (m_tableStage !=  CREATE_HTML_TABLE ) { // table need to be created first
        Q_ASSERT(0);
    }
    resetTableData(); // reset everything to be sure
    m_tableStage = CREATE_HTML_TABLE_HEADER;
    m_htmlTable +="<tr " + m_fontSize  + ">";


    ++m_tableRowCount;
    setIsTableHeader(true);
}

void ChtmlUtils::createTableCellHeader(QString colname, int colno, int cellsize)
{
    QString str;
    if((CREATE_HTML_TABLE_HEADER != m_tableStage) && (ADD_HTML_HEADER_CELL != m_tableStage)){
        Q_ASSERT(0);
    }

    if(m_isTableHeader){
        if (colname.isEmpty() || (colno < 0) || (cellsize > 100) || (cellsize < 0)) {
            Q_ASSERT(0);
        }
        if(colno%2 == 0)
            str += "<th style=\"border-left: 1px solid #000;\" ";
        else
            str += "<th style=\"border-left: 1px solid #000; border-right: 1px solid #000;\" ";

        str += "bgcolor=\"#E2E2E2\" width=\" " +QString::number(cellsize) +"%\" align=\"center\" "+  m_cellHeight  +" >";

        str += "<b>"+ colname +"</b></th>";
    } else {
        Q_ASSERT(0);
    }
    m_tableStage = ADD_HTML_HEADER_CELL;
    m_htmlTable += str;
    m_tableColSize.push_back(cellsize);
}

void ChtmlUtils::closeHeaderRow()
{
    if (ADD_HTML_HEADER_CELL != m_tableStage) {
           Q_ASSERT(0);
    }
    m_htmlTable += "</tr>";
    m_tableStage = CLOSE_HTML_HEADER_ROW;
}

void ChtmlUtils::createRow(int defaultcolno)
{
    if( (((CLOSE_HTML_HEADER_ROW != m_tableStage )&& (m_isTableHeader == true))  ||
         ((m_tableStage != CREATE_HTML_TABLE) &&(m_isTableHeader == false) ))
            && (m_tableStage != CLOSE_HTML_TABLE_ROW)) {
        Q_ASSERT(0);
    }
    setIsTableHeader(false);
    QString line = "<tr " + m_fontSize + ">";
    m_htmlTable += line;

    //qDebug()<<"html row:"<<line;

    m_tableStage = ADD_HTML_TABLE_ROW;
    m_tableColCount = defaultcolno;
    ++m_tableRowCount;
}

void ChtmlUtils::createCell(QString celldata,
                            EhtmlCellAlign align,
                            EhtmlCellValign valign,
                            EhtmlCellBoarder border,
                            EhtmlCellBorderStle borderStyle,
                            QString bgcolor)
{

    if(m_tableStage != ADD_HTML_TABLE_ROW  && m_tableStage != ADD_HTML_TABLE_CELL) {
        Q_ASSERT(0);
    }

    if(m_tableColCount >= m_tableColSize.size()){
        qDebug()<<"Table col count "<<m_tableColCount<<" is greate than col size "<<m_tableColSize.size();
        Q_ASSERT(0);
    }

    int cellsize = m_tableColSize.at(m_tableColCount);
    QString line = "<td  ";
    line += createBorder(border, borderStyle);

    if(bgcolor.isEmpty() == false)
        line += " bgcolor=\"" +bgcolor +"\" ";

    line += " width=\"" + QString::number(cellsize) +"%\"  height=\""+ m_cellHeight +"\" ";

    line += createAlign(align);
    line += createValign(valign);

    line += ">" + celldata + "</td>";

    m_htmlTable += line;
    ++m_tableColCount;
    m_tableStage = ADD_HTML_TABLE_CELL;
}

void ChtmlUtils::createMergeCell(QString cellData,
                                 int noofcoltomerge,
                                 bool colmerge,
                                 EhtmlCellAlign align,
                                 EhtmlCellValign vaign,
                                 EhtmlCellBoarder border,
                                 EhtmlCellBorderStle borderStyle,
                                 QString bgcolor)
{
    if (m_tableStage != ADD_HTML_TABLE_ROW  && m_tableStage != ADD_HTML_TABLE_CELL) {
        Q_ASSERT(0);
    }

    QString colspan;
    if(colmerge) {
        colspan =" colspan=\"";
    }
    else {
        colspan = " rowspan=\"";
    }
    colspan +=  QString::number(noofcoltomerge);
    colspan += "\" ";

    QString line = "<td  ";
    line += createBorder(border, borderStyle);
    if(bgcolor.isEmpty() == false) {
        line += " bgcolor=\"" +bgcolor +"\" ";
    }
    int cellsize = m_tableColSize.at(m_tableColCount);
    if(colmerge){
        cellsize= 0;
        for(int i = 0; i < noofcoltomerge; ++i){
            cellsize += m_tableColSize.at(m_tableColCount + 1);
        }
    }
    line += " width=\"" + QString::number(cellsize) +"%\" "+ m_cellHeight;
    line += createAlign(align);
    line += createValign(vaign);

    line += colspan +">" + cellData + "</td>";
    m_htmlTable += line;

    if(colmerge) {
        m_tableColCount += noofcoltomerge;
    } else {
        ++m_tableColCount;
    }
    m_tableStage = ADD_HTML_TABLE_CELL;

}

void ChtmlUtils::closeRow(int colwithmergerow)
{
    if(m_tableStage != ADD_HTML_TABLE_CELL) {
        Q_ASSERT(0);
    }
    if((colwithmergerow + m_tableColCount) != m_tableColSize.size()){
        qDebug()<<"talble col count "<<m_tableColCount <<" header count "<<m_tableColSize.size();
        Q_ASSERT(0);
    }
    m_tableStage = CLOSE_HTML_TABLE_ROW ;
    m_tableColCount = 0;
    m_htmlTable += QString("</tr>");
}

void ChtmlUtils::closeTable()
{
    if(m_tableStage != CLOSE_HTML_TABLE_ROW){
        Q_ASSERT(0);
    }
    m_tableStage = CLOSE_HTML_TABLE;
    m_htmlTable += "</table>";
    m_tableColSize.clear();
}

void ChtmlUtils::formatString(QString str, bool paragraph, bool bold)
{
    QString str1;
   // qDebug()<<"before "<<str;
    str.replace(QRegularExpression("[\r\n]"), "<br>");

    //qDebug()<<"afer "<<str;
    if (bold) {
        str = "<strong>" + str + "</strong>";
    }
    if(paragraph){
        str1 ="<p " + m_fontSize+ ">" + str + "</p>";
    }else {
       str1  = "<span " + m_fontSize + ">"  + str + "</span>";

    }
    m_htmlTable += str1;
}

QString ChtmlUtils::addMultipleBlank(int noofline)
{
    QString str;
    for(int i = 0; i <noofline; ++i) {
        str += "<p>&nbsp;</p>";
    }
    return str;
}

QString ChtmlUtils::getTwoSignator()
{
    ChtmlUtils hUtils;
    hUtils.setIsTableHeader(false);
    hUtils.addTableColSize(50);
    hUtils.addTableColSize(50);
    hUtils.openTable();
    hUtils.createRow();
    QString str =" <p>&nbsp;</p>";
    str += "<p><b>Finance Secretary</b></p>";
    hUtils.createCell(str, HTML_CENTER_ALIGN);
    str = "<p>&nbsp;</p>";
    str += "<p><b>Treasurer</b></p> ";
    hUtils.createCell(str, HTML_CENTER_ALIGN);
    hUtils.closeRow();
    hUtils.closeTable();
    return  hUtils.getTableHTML();

}


void ChtmlUtils::resetTableData()
{
    m_htmlTable.clear();
    m_tableColCount = 0;
    m_tableRowCount = 0;
    m_tableColSize.clear();
    m_isTableHeader = false;
    m_tableStage = NO_HTML_TABLE;
}

QString ChtmlUtils::getTableHTML()
{
    if( NO_HTML_TABLE != m_tableStage &&  m_tableStage != CLOSE_HTML_TABLE){
        qDebug()<<m_tableStage <<" is not equal "<<CLOSE_HTML_TABLE;
           Q_ASSERT(0);
    }

    QString ret = m_htmlTable;
    resetTableData();
    return ret;
}

QString ChtmlUtils::getHeader()
{
    QStringList headerList = CsdUtils::getHeaderText();
    QString org  = headerList.at(0);
    QString loc = headerList.at(1);
    QString subheader = headerList.at(2);

    QString header =" <p style=\"text-align: center;\"> <span style=\"font-size:14px;\"> <strong>"+org+"</strong></span> <br />";
    header += "<span style=\"font-size:10px;\"><strong>" + subheader +"</strong></span><br/>";
    header +="<span style=\"font-size:14px;\"><strong>"+loc+"</strong></span></p>";
    return header;
}

QString ChtmlUtils::getPageBreak()
{
    return "<p style=\"page-break-after:always;\">&nbsp;</p>";
}



QString ChtmlUtils::createAlign(EhtmlCellAlign align)
{
    QString line;
    switch(align){
    case HTML_NO_ALIGN : break;
    case HTML_LEFT_ALIGN :
        line += " align=\"left\" ";
        break;
    case HTML_CENTER_ALIGN:
        line += " align=\"center\" ";
        break;
    case HTML_RIGHT_ALIGN:
        line += " align=\"right\" ";
        break;
    }
    return line;
}

QString ChtmlUtils::createValign(EhtmlCellValign align)
{
    QString line;
    switch(align){
        case HTML_V_TOP_ALIGN:
                line = " valign=\"top\" ";
                break;
        case HTML_V_MIDDLE_ALIGN:
                line = " valign=\"middle\" ";
                break;
        case HTML_V_BOTTOM_ALIGN:
                line = " valign=\"bottom\" ";
                break;
    }
    return line;
}

QString ChtmlUtils::createBorder(EhtmlCellBoarder border, EhtmlCellBorderStle borderStyle)
{
    QString line = " style=\"";
     QString stylel;
     QString styler;
     QString stylet;
     QString styleb;
     switch(borderStyle) {
         case HTML_NORMAL_BORDER_STYLE: stylel = "1px solid #000;";
                             styler = stylet = styleb = stylel;
                 break;
         case HTML_DOUBLE_BORDER_STYLE:
                             stylel = "1px double #000;";
                             styler = stylet = styleb = stylel;
             break;
         case HTML_DOTTED_BORDER_STYLE: stylel = "1px dotted #000;";
                             styler = stylet = styleb = stylel;
             break;
         case HTML_DOUBLE_BOTTOM_STYLE:
                             styleb = "1px double #000;";
                             styler = stylet  = stylel = "1px solid #000;";

                 break;

     }
     if(border == HTML_NO_BORDER)
         line = "";
     else {
         if(HTML_LEFT_BORDER&border) line += "border-left:" +stylel;
         if(border&HTML_RIGHT_BORDER) line += "border-right:" + styler;
         if(border&HTML_BOTTOM_BORDER) line += "border-bottom:"+styleb;
         if(border&HTML_TOP_BORDER ) line += "border-top:"+ stylet;

         line += "\" ";
     }

     return line;
}
