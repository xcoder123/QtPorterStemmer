# QtPorterStemmer
B. Frakes and C. Cox Porter Stemmer reimplmented in Qt. Aim of this project is to provide a stemmer for Latvian language

# English
This is a quick reimplementation of Frakes & Cox Porter Stemmer in Qt.

Main aim of the project is to reimplement Karlis Kreslins Porter Stemmer in Qt/C++.
https://dspace.lboro.ac.uk/dspace-jspui/bitstream/2134/7433/2/360860.pdf

Even though I have reimplemented English Porter Stemmer, for english and some other languages it is advisable to use snowball library

More projects on my blog: http://morf.lv

## Usage

You have to classes available ENPorterStemmer and LVPorterStemmer, which are porter stemmer for english and latvian respectivly

For example english porter stemmer can be used like this:
`ENPorterStemmer::stem("driving");`


And Latvian porter stemmer can be used like this:
`LVPorterStemmer::stem("reliģija");`

**stem()** function in both classes are static. No need to create an object.





# Latvian
Projekta mērķis ir ātri pārrakstīt Dr. Kārļa Krēsliņa Portera modificēto algoritmu uz Qt platformas
https://dspace.lboro.ac.uk/dspace-jspui/bitstream/2134/7433/2/360860.pdf

Lai arī es esmu pārrakstījis orģinālo Porter Stemmer algoritmu uz Qt platformas, ieteicams angļu valodai tomēr lietot snowball bibliotēku

Mans blogs: http://morf.lv

## Pielietojums

Tev ir pieejamas divas klases ENPorterStemmer un LVPorterStemmer, respektīvi "stemeris" priekš Angļu valodas un Latviešu valodas

Angļu Portera Stemera klasi var lietot šādi:
`ENPorterStemmer::stem("driving");`

Un Latvieša šādi:
`LVPorterStemmer::stem("reliģija");`


**stem()** funkcijas abās klasēs ir statiskas. Tā kā nav nepieciešams izveidot objektu.


![screenshot_en](https://cloud.githubusercontent.com/assets/7244389/20033144/1bb25da2-a392-11e6-9bfb-bf731a3cb630.png)
![screenshot_lv](https://cloud.githubusercontent.com/assets/7244389/20033143/1bad10c2-a392-11e6-9f05-3e454fc88593.png)

