/******************************************************************

   Original authors: B. Frakes and C. Cox, 1986.

   Rewriten for Qt by: Raivis Strogonovs (http://morf.lv), 2016

   Licensed under GPLv3. See LICENCE.md file

**/

#include "enporterstemmer.h"

/*****************************************************************************/
/*****************   Private Defines and Data Structures   *******************/
#define IsVowel(c)        ('a'==(c)||'e'==(c)||'i'==(c)||'o'==(c)||'u'==(c))

typedef struct {
           int id;                 /* returned if rule fired */
           QString old_end;          /* suffix replaced */
           QString new_end;          /* suffix replacement */
           int old_offset;         /* from end of word to start of suffix */
           int new_offset;         /* from beginning to end of new suffix */
           int min_root_size;      /* min root word size for replacement */
           int (*condition)(QString &);     /* the replacement test function */
           } RuleList;

//static char LAMBDA[1] = "";        /* the constant empty string */
static QString LAMBDA = "";
//static char *end;
static int endIndex;

/*****************************************************************************/
/********************   Private Function Declarations   **********************/
static int WordSize( QString &word  );
static int ContainsVowel( QString &word );
static int EndsWithCVC( QString &word  );
static int AddAnE( QString &word  );
static int RemoveAnE( QString &word  );
static int ReplaceEnd( QString & word, RuleList * rule );


/******************************************************************************/
/*****************   Initialized Private Data Structures   ********************/

static RuleList step1a_rules[] =
           {
                {101,  "sses",      "ss",    3,  1, -1,  NULL},
                {102,  "ies",       "i",     2,  0, -1,  NULL},
                {103,  "ss",        "ss",    1,  1, -1,  NULL},
                {104,  "s",         LAMBDA,  0, -1, -1,  NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step1b_rules[] =
           {
                {105,  "eed",       "ee",    2,  1,  0,  NULL},
                {106,  "ed",        LAMBDA,  1, -1, -1,  ContainsVowel},
                {107,  "ing",       LAMBDA,  2, -1, -1,  ContainsVowel},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step1b1_rules[] =
           {
             {108,  "at",        "ate",   1,  2, -1,  NULL},
             {109,  "bl",        "ble",   1,  2, -1,  NULL},
             {110,  "iz",        "ize",   1,  2, -1,  NULL},
             {111,  "bb",        "b",     1,  0, -1,  NULL},
             {112,  "dd",        "d",     1,  0, -1,  NULL},
             {113,  "ff",        "f",     1,  0, -1,  NULL},
             {114,  "gg",        "g",     1,  0, -1,  NULL},
             {115,  "mm",        "m",     1,  0, -1,  NULL},
             {116,  "nn",        "n",     1,  0, -1,  NULL},
             {117,  "pp",        "p",     1,  0, -1,  NULL},
             {118,  "rr",        "r",     1,  0, -1,  NULL},
             {119,  "tt",        "t",     1,  0, -1,  NULL},
             {120,  "ww",        "w",     1,  0, -1,  NULL},
             {121,  "xx",        "x",     1,  0, -1,  NULL},
             {122,  LAMBDA,      "e",    -1,  0, -1,  AddAnE},
             {000,  NULL,        NULL,    0,  0,  0,  NULL},
             };

static RuleList step1c_rules[] =
           {
             {123,  "y",         "i",      0,  0, -1,  ContainsVowel},
             {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step2_rules[] =
           {
             {203,  "ational",   "ate",   6,  2,  0,  NULL},
             {204,  "tional",    "tion",  5,  3,  0,  NULL},
             {205,  "enci",      "ence",  3,  3,  0,  NULL},
             {206,  "anci",      "ance",  3,  3,  0,  NULL},
             {207,  "izer",      "ize",   3,  2,  0,  NULL},
             {208,  "abli",      "able",  3,  3,  0,  NULL},
             {209,  "alli",      "al",    3,  1,  0,  NULL},
             {210,  "entli",     "ent",   4,  2,  0,  NULL},
             {211,  "eli",       "e",     2,  0,  0,  NULL},
             {213,  "ousli",     "ous",   4,  2,  0,  NULL},
             {214,  "ization",   "ize",   6,  2,  0,  NULL},
             {215,  "ation",     "ate",   4,  2,  0,  NULL},
             {216,  "ator",      "ate",   3,  2,  0,  NULL},
             {217,  "alism",     "al",    4,  1,  0,  NULL},
             {218,  "iveness",   "ive",   6,  2,  0,  NULL},
             {219,  "fulnes",    "ful",   5,  2,  0,  NULL},
             {220,  "ousness",   "ous",   6,  2,  0,  NULL},
             {221,  "aliti",     "al",    4,  1,  0,  NULL},
             {222,  "iviti",     "ive",   4,  2,  0,  NULL},
             {223,  "biliti",    "ble",   5,  2,  0,  NULL},
             {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step3_rules[] =
           {
             {301,  "icate",     "ic",    4,  1,  0,  NULL},
             {302,  "ative",     LAMBDA,  4, -1,  0,  NULL},
             {303,  "alize",     "al",    4,  1,  0,  NULL},
             {304,  "iciti",     "ic",    4,  1,  0,  NULL},
             {305,  "ical",      "ic",    3,  1,  0,  NULL},
             {308,  "ful",       LAMBDA,  2, -1,  0,  NULL},
             {309,  "ness",      LAMBDA,  3, -1,  0,  NULL},
             {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step4_rules[] =
           {
             {401,  "al",        LAMBDA,  1, -1,  1,  NULL},
             {402,  "ance",      LAMBDA,  3, -1,  1,  NULL},
             {403,  "ence",      LAMBDA,  3, -1,  1,  NULL},
             {405,  "er",        LAMBDA,  1, -1,  1,  NULL},
             {406,  "ic",        LAMBDA,  1, -1,  1,  NULL},
             {407,  "able",      LAMBDA,  3, -1,  1,  NULL},
             {408,  "ible",      LAMBDA,  3, -1,  1,  NULL},
             {409,  "ant",       LAMBDA,  2, -1,  1,  NULL},
             {410,  "ement",     LAMBDA,  4, -1,  1,  NULL},
             {411,  "ment",      LAMBDA,  3, -1,  1,  NULL},
             {412,  "ent",       LAMBDA,  2, -1,  1,  NULL},
             {423,  "sion",      "s",     3,  0,  1,  NULL},
             {424,  "tion",      "t",     3,  0,  1,  NULL},
             {415,  "ou",        LAMBDA,  1, -1,  1,  NULL},
             {416,  "ism",       LAMBDA,  2, -1,  1,  NULL},
             {417,  "ate",       LAMBDA,  2, -1,  1,  NULL},
             {418,  "iti",       LAMBDA,  2, -1,  1,  NULL},
             {419,  "ous",       LAMBDA,  2, -1,  1,  NULL},
             {420,  "ive",       LAMBDA,  2, -1,  1,  NULL},
             {421,  "ize",       LAMBDA,  2, -1,  1,  NULL},
             {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step5a_rules[] =
           {
             {501,  "e",         LAMBDA,  0, -1,  1,  NULL},
             {502,  "e",         LAMBDA,  0, -1, -1,  RemoveAnE},
             {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step5b_rules[] =
           {
             {503,  "ll",        "l",     1,  0,  1,  NULL},
             {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };


/*****************************************************************************/
/********************   Private Function Declarations   **********************/

/*FN***************************************************************************

       WordSize( word )

   Returns: int -- a weird count of word size in adjusted syllables

   Purpose: Count syllables in a special way:  count the number
            vowel-consonant pairs in a word, disregarding initial
            consonants and final vowels.  The letter "y" counts as a
            consonant at the beginning of a word and when it has a vowel
            in front of it; otherwise (when it follows a consonant) it
            is treated as a vowel.  For example, the WordSize of "cat"
            is 1, of "any" is 1, of "amount" is 2, of "anything" is 3.

   Plan:    Run a DFA to compute the word size

   Notes:   The easiest and fastest way to compute this funny measure is
            with a finite state machine.  The initial state 0 checks
            the first letter.  If it is a vowel, then the machine changes
            to state 1, which is the "last letter was a vowel" state.
            If the first letter is a consonant or y, then it changes
            to state 2, the "last letter was a consonant state".  In
            state 1, a y is treated as a consonant (since it follows
            a vowel), but in state 2, y is treated as a vowel (since
            it follows a consonant.  The result counter is incremented
            on the transition from state 1 to state 2, since this
            transition only occurs after a vowel-consonant pair, which
            is what we are counting.
**/

static int WordSize( QString &word )
{
    register int result;   /* WordSize of the word */
    register int state;    /* current state in machine */

    result = 0;
    state = 0;

                 /* Run a DFA to compute the word size */
    for(int i=0; i<word.length(); i++)
    {
        QChar c = word.at(i);

        switch ( state )
        {
            case 0: state = (IsVowel(c)) ? 1 : 2;
                   break;
            case 1: state = (IsVowel(c)) ? 1 : 2;
                   if ( 2 == state ) result++;
                   break;
            case 2: state = (IsVowel(c) || ('y' == c)) ? 1 : 2;
                   break;
        }
    }


    return( result );

} /* WordSize */


/*FN**************************************************************************

       ContainsVowel( word )

   Returns: int -- TRUE (1) if the word parameter contains a vowel,
            FALSE (0) otherwise.

   Purpose: Some of the rewrite rules apply only to a root containing
            a vowel, where a vowel is one of "aeiou" or y with a
            consonant in front of it.

   Plan:    Obviously, under the definition of a vowel, a word contains
            a vowel iff either its first letter is one of "aeiou", or
            any of its other letters are "aeiouy".  The plan is to
            test this condition.

   Notes:   None
**/

static int ContainsVowel( QString &word )
{

    if(word.isEmpty())
        return false;
    else
        return( IsVowel(word.at(0)) || (NULL != strpbrk(word.toStdString().c_str()+1,"aeiouy")) );


    /*if ( EOS == *word )
      return( FALSE );
    else
      return( IsVowel(*word) || (NULL != strpbrk(word+1,"aeiouy")) );*/


} /* ContainsVowel */


/*FN**************************************************************************

       EndsWithCVC( word )

   Returns: int -- TRUE (1) if the current word ends with a
            consonant-vowel-consonant combination, and the second
            consonant is not w, x, or y, FALSE (0) otherwise.

   Purpose: Some of the rewrite rules apply only to a root with
            this characteristic.

   Plan:    Look at the last three characters.

   Notes:   None
**/

static int EndsWithCVC( QString &word )
{
    int length = word.length();         /* for finding the last three characters */

    if ( length < 2 )
        return( false );
    else
    {
        endIndex = word.length()-1;
        return( QString("aeiouwxy").contains( word.at(endIndex--) )
                && QString("aeiouy").contains( word.at(endIndex--) )
                && QString("aeiou").contains( word.at(endIndex) )
              );
    }

} /* EndsWithCVC */


/*FN**************************************************************************

       AddAnE( word )

   Returns: int -- TRUE (1) if the current word meets special conditions
            for adding an e.

   Purpose: Rule 122 applies only to a root with this characteristic.

   Plan:    Check for size of 1 and a consonant-vowel-consonant ending.

   Notes:   None
**/

static int AddAnE( QString &word )
{
    return( (1 == WordSize(word)) && EndsWithCVC(word) );
} /* AddAnE */


/*FN**************************************************************************

       RemoveAnE( word )

   Returns: int -- TRUE (1) if the current word meets special conditions
            for removing an e.

   Purpose: Rule 502 applies only to a root with this characteristic.

   Plan:    Check for size of 1 and no consonant-vowel-consonant ending.

   Notes:   None
**/

static int RemoveAnE( QString &word )
{
    return( (1 == WordSize(word)) && !EndsWithCVC(word) );
} /* RemoveAnE */


/*FN**************************************************************************

       ReplaceEnd( word, rule )

   Returns: int -- the id for the rule fired, 0 is none is fired

   Purpose: Apply a set of rules to replace the suffix of a word

   Plan:    Loop through the rule set until a match meeting all conditions
            is found.  If a rule fires, return its id, otherwise return 0.
            Connditions on the length of the root are checked as part of this
            function's processing because this check is so often made.

   Notes:   This is the main routine driving the stemmer.  It goes through
            a set of suffix replacement rules looking for a match on the
            current suffix.  When it finds one, if the root of the word
            is long enough, and it meets whatever other conditions are
            required, then the suffix is replaced, and the function returns.
**/

static int ReplaceEnd( QString &word, RuleList *rule )
{

    int ending;   /* set to start of possible stemmed suffix */
    QChar tmp_ch;             /* save replaced character when testing */

    while ( 0 != rule->id )
    {
//        qDebug() << "rule ID" << rule->id;
        ending = endIndex - rule->old_offset;
        if ( ending >= 0 )
        {
//            qDebug() << word.right(word.length() - ending) << rule->old_end;
            if ( word.right(word.length() - ending) == rule->old_end )
            {
//                tmp_ch = word.at(ending);
//                *ending = EOS;

                if ( rule->min_root_size < WordSize(word) )
                    if ( !rule->condition || (*rule->condition)(word) )
                    {
                        word = word.left( word.length()-rule->old_end.length() );
                        word.append( rule->new_end );
//                        (void)strcat( word, rule->new_end );
                        endIndex = word.length() - 1;
                        break;
                    }

//                *ending = tmp_ch;
            }
        }

        rule++;
    }

    return( rule->id );

} /* ReplaceEnd */

ENPorterStemmer::ENPorterStemmer()
{

}

QString ENPorterStemmer::stem(QString word)
{
    int rule;    /* which rule is fired in replacing an end */

    /* Part 1: Check to ensure the word is all alphabetic */
    word = word.toLower();
    for(int i=0; i<word.length(); i++)
    {
        if(!word.at(i).isLetter())
            return word;
    }

    endIndex = word.length()-1;

//    qDebug() << word << endIndex << ContainsVowel(word) << WordSize(word);

                /*  Part 2: Run through the Porter algorithm */
    (void)ReplaceEnd( word, step1a_rules );
    rule = ReplaceEnd( word, step1b_rules );
    if ( (106 == rule) || (107 == rule) )
      (void)ReplaceEnd( word, step1b1_rules );
    (void)ReplaceEnd( word, step1c_rules );

    (void)ReplaceEnd( word, step2_rules );

    (void)ReplaceEnd( word, step3_rules );

    (void)ReplaceEnd( word, step4_rules );

    (void)ReplaceEnd( word, step5a_rules );
    (void)ReplaceEnd( word, step5b_rules );


    return word;
}
