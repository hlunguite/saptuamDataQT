#include "csortfilterproxymodelfortreemodel.h"
#include <QRegularExpression>
CSortFilterProxyModelForTreeModel::CSortFilterProxyModelForTreeModel()
{

}

CSortFilterProxyModelForTreeModel::~CSortFilterProxyModelForTreeModel()
{

}

void
CSortFilterProxyModelForTreeModel::setFilterData(int col, QString matchString)
{
    if( m_filterDataStr.contains(col)){
        m_filterDataStr[col] = matchString;
    }else {
        m_filterDataStr.insert(col,matchString);
    }
    invalidateFilter();
}

void
CSortFilterProxyModelForTreeModel::setFilterData(int col, int matchInt)
{
    if(m_filterDataInt.contains(col)){
        m_filterDataInt[col] = matchInt;
    }else {
        m_filterDataInt.insert(col, matchInt);
    }
    invalidateFilter();
}

bool
CSortFilterProxyModelForTreeModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if(m_filterDataInt.empty() && m_filterDataStr.empty()) return true;
    QMap<int,int>::const_iterator beg = m_filterDataInt.begin();
    QMap<int,int>::const_iterator e = m_filterDataInt.end();
    while(beg != e){
        int col = beg.key();
        int val = beg.value();
        ++beg;
        QModelIndex idx = sourceModel()->index(source_row,col,source_parent);
        if(idx.isValid()){
            if(sourceModel()->data(idx).toInt() == val){
                return false;
            }
        }
    }
    QMap<int,QString>::const_iterator it = m_filterDataStr.begin();
    QMap<int,QString>::const_iterator end = m_filterDataStr.end();
    while(it != end){
        int col = it.key() ;
        QString tosearch = it.value();
        ++it;
        QModelIndex index1 = sourceModel()->index(source_row,col,source_parent);
        if(index1.isValid()){
            QRegularExpression regExprFor(tosearch, QRegularExpression::CaseInsensitiveOption);
            QString tomatch = sourceModel()->data(index1).toString();
            bool condition = tomatch.contains(regExprFor);
            if(condition == false){

                return false;
            }
        }
    }
    return true;
}

