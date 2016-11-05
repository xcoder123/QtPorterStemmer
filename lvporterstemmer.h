/******************************************************************

   Original authors: B. Frakes and C. Cox, 1986.

   Latvian Stemming author: Karlis Kreslins, 1996

   Rewriten for Qt by: Raivis Strogonovs (http://morf.lv), 2016

   Licensed under GPLv3. See LICENCE.md file


   Special thanks to Karlis Kreslins for his work on stemming algorithm for Latvian
   Thesis: https://dspace.lboro.ac.uk/dspace-jspui/bitstream/2134/7433/2/360860.pdf

**/

#ifndef LVPORTERSTEMMER_H
#define LVPORTERSTEMMER_H

#include <QString>
//#include <QDebug>

class LVPorterStemmer
{
public:
    LVPorterStemmer();
    static QString stem(QString word);
};

#endif // LVPORTERSTEMMER_H
