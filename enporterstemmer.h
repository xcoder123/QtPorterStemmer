/******************************************************************

   Original authors: B. Frakes and C. Cox, 1986.

   Rewriten for Qt by: Raivis Strogonovs (http://morf.lv), 2016

   Licensed under GPLv3. See LICENCE.md file

**/

#ifndef ENPORTERSTEMMER_H
#define ENPORTERSTEMMER_H

#include <QString>
//#include <QDebug>

class ENPorterStemmer
{
public:
    ENPorterStemmer();
    static QString stem(QString word);
};

#endif // ENPORTERSTEMMER_H
