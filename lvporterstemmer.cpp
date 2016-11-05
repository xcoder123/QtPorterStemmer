/******************************************************************

   Original authors: B. Frakes and C. Cox, 1986.

   Latvian Stemming author: Karlis Kreslins, 1996

   Rewriten for Qt by: Raivis Strogonovs (http://morf.lv), 2016

   Licensed under GPLv3. See LICENCE.md file


   Special thanks to Karlis Kreslins for his work on stemming algorithm for Latvian
   Thesis: https://dspace.lboro.ac.uk/dspace-jspui/bitstream/2134/7433/2/360860.pdf

**/

#include "lvporterstemmer.h"

/*****************************************************************************/
/*****************   Private Defines and Data Structures   *******************/
#define IsVowel(c)        QString("aāeēiīouū").contains(c)

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
static int ReplaceEnd( QString & word, RuleList * rule );
static int CompStopW( QString & word, RuleList * rule );
static int ReplaceW( QString & word, RuleList * rule );

static int IsLatVowel( QChar ch );


/******************************************************************************/
/*****************   Initialized Private Data Structures   ********************/

static RuleList step0a_rules[] =
           {
                {001, "aiz", LAMBDA, 2, -1, -1, NULL},
                {002, "ap", LAMBDA, 1, -1, -1, NULL},
                {003, "ar", LAMBDA, 1, -1, -1, NULL},
                {004, "apakš", LAMBDA, 4, -1, -1, NULL},
                {005, "ārpus", LAMBDA, 4, -1, -1, NULL},
                {006, "augšpus", LAMBDA, 6, -1, -1, NULL},
                {007, "bez", LAMBDA, 2, -1, -1, NULL},
                {010, "caur", LAMBDA, 3, -1, -1, NULL},
                {011, "dēļ", LAMBDA, 2, -1, -1, NULL},
                {012, "gar", LAMBDA, 2, -1, -1, NULL},
                {013, "iekš", LAMBDA, 3, -1, -1, NULL},
                {014, "iz", LAMBDA, 1, -1, -1, NULL},
                {015, "kopš", LAMBDA, 3, -1, -1, NULL},
                {016, "labad", LAMBDA, 4, -1, -1, NULL},
                {017, "lejpus", LAMBDA, 5, -1, -1, NULL},
                {020, "līdz", LAMBDA, 3, -1, -1, NULL},
                {021, "no", LAMBDA, 1, -1, -1, NULL},
                {022, "otrpus", LAMBDA, 5, -1, -1, NULL},
                {023, "pa", LAMBDA, 1, -1, -1, NULL},
                {024, "par", LAMBDA, 2, -1, -1, NULL},
                {025, "pār", LAMBDA, 2, -1, -1, NULL},
                {026, "pēc", LAMBDA, 2, -1, -1, NULL},
                {027, "pie", LAMBDA, 2, -1, -1, NULL},
                {030, "pirms", LAMBDA, 4, -1, -1, NULL},
                {031, "pret", LAMBDA, 3, -1, -1, NULL},
                {032, "priekš", LAMBDA, 5, -1, -1, NULL},
                {033, "starp", LAMBDA, 4, -1, -1, NULL},
                {034, "šaipus", LAMBDA, 5, -1, -1, NULL},
                {035, "uz", LAMBDA, 1, -1, -1, NULL},
                {036, "viņpus", LAMBDA, 5, -1, -1, NULL},
                {037, "virs", LAMBDA, 3, -1, -1, NULL},
                {040, "virspus", LAMBDA, 6, -1, -1, NULL},
                {041, "zem", LAMBDA, 2, -1, -1, NULL},
                {042, "un", LAMBDA, 1, -1, -1, NULL},
                {043, "bet", LAMBDA, 2, -1, -1, NULL},
                {044, "jo", LAMBDA, 1, -1, -1, NULL},
                {045, "ja", LAMBDA, 1, -1, -1, NULL},
                {046, "ka", LAMBDA, 1, -1, -1, NULL},
                {047, "lai", LAMBDA, 2, -1, -1, NULL},
                {050, "tomēr", LAMBDA, 4, -1, -1, NULL},
                {051, "tikko", LAMBDA, 4, -1, -1, NULL},
                {052, "turpretī", LAMBDA, 7, -1, -1, NULL},
                {053, "arī", LAMBDA, 2, -1, -1, NULL},
                {054, "kaut", LAMBDA, 3, -1, -1, NULL},
                {055, "gan", LAMBDA, 2, -1, -1, NULL},
                {056, "tādēļ", LAMBDA, 4, -1, -1, NULL},
                {057, "tā", LAMBDA, 1, -1, -1, NULL},
                {060, "ne", LAMBDA, 1, -1, -1, NULL},
                {061, "tikvien", LAMBDA, 6, -1, -1, NULL},
                {062, "vien", LAMBDA, 3, -1, -1, NULL},
                {063, "kā", LAMBDA, 1, -1, -1, NULL},
                {064, "ir", LAMBDA, 1, -1, -1, NULL},
                {065, "te", LAMBDA, 1, -1, -1, NULL},
                {066, "vai", LAMBDA, 2, -1, -1, NULL},
                {067, "kamēr", LAMBDA, 4, -1, -1, NULL},
                {070, "apakšpus", LAMBDA, 7, -1, -1, NULL},
                {071, "ar", LAMBDA, 1, -1, -1, NULL},
                {072, "diezin", LAMBDA, 5, -1, -1, NULL},
                {073, "ik", LAMBDA, 1, -1, -1, NULL},
                {074, "it", LAMBDA, 1, -1, -1, NULL},
                {075, "taču", LAMBDA, 3, -1, -1, NULL},
                {076, "nu", LAMBDA, 1, -1, -1, NULL},
                {077, "pat", LAMBDA, 2, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step0b_rules[] =
           {
                {001, "tiklab", LAMBDA, 5, -1, -1, NULL},
                {002, "iekšpus", LAMBDA, 6, -1, -1, NULL},
                {003, "nedz", LAMBDA, 3, -1, -1, NULL},
                {004, "tik", LAMBDA, 2, -1, -1, NULL},
                {005, "nevis", LAMBDA, 4, -1, -1, NULL},
                {006, "turpretim", LAMBDA, 8, -1, -1, NULL},
                {007, "jeb", LAMBDA, 2, -1, -1, NULL},
                {010, "iekam", LAMBDA, 4, -1, -1, NULL},
                {011, "iekām", LAMBDA, 4, -1, -1, NULL},
                {012, "iekāms", LAMBDA, 5, -1, -1, NULL},
                {013, "kolīdz", LAMBDA, 5, -1, -1, NULL},
                {014, "līdzko", LAMBDA, 5, -1, -1, NULL},
                {015, "tiklīdz", LAMBDA, 6, -1, -1, NULL},
                {016, "jebšu", LAMBDA, 4, -1, -1, NULL},
                {017, "tālab", LAMBDA, 4, -1, -1, NULL},
                {020, "tāpēc", LAMBDA, 4, -1, -1, NULL},
                {021, "nekā", LAMBDA, 3, -1, -1, NULL},
                {022, "itin", LAMBDA, 3, -1, -1, NULL},
                {023, "jā", LAMBDA, 1, -1, -1, NULL},
                {024, "jau", LAMBDA, 2, -1, -1, NULL},
                {025, "jel", LAMBDA, 2, -1, -1, NULL},
                {026, "nē", LAMBDA, 1, -1, -1, NULL},
                {027, "nezin", LAMBDA, 4, -1, -1, NULL},
                {030, "tad", LAMBDA, 2, -1, -1, NULL},
                {031, "tikai", LAMBDA, 4, -1, -1, NULL},
                {032, "vis", LAMBDA, 2, -1, -1, NULL},
                {033, "droši", LAMBDA, 4, -1, -1, NULL},
                {034, "diemžēl", LAMBDA, 5, -1, -1, NULL},
                {035, "tak", LAMBDA, 2, -1, -1, NULL},
                {036, "nebūt", LAMBDA, 4, -1, -1, NULL},
                {037, "varbūt", LAMBDA, 5, -1, -1, NULL},
                {040, "klau", LAMBDA, 3, -1, -1, NULL},
                {041, "lūk", LAMBDA, 2, -1, -1, NULL},
                {042, "iekams", LAMBDA, 5, -1, -1, NULL},
                {043, "vien", LAMBDA, 3, -1, -1, NULL},
                {044, "es", LAMBDA, 1, -1, -1, NULL},
                {045, "manis", LAMBDA, 4, -1, -1, NULL},
                {046, "man", LAMBDA, 2, -1, -1, NULL},
                {047, "mani", LAMBDA, 3, -1, -1, NULL},
                {050, "manī", LAMBDA, 3, -1, -1, NULL},
                {051, "mēs", LAMBDA, 2, -1, -1, NULL},
                {052, "mūsu", LAMBDA, 3, -1, -1, NULL},
                {053, "mums", LAMBDA, 3, -1, -1, NULL},
                {054, "mūs", LAMBDA, 2, -1, -1, NULL},
                {055, "mūsos", LAMBDA, 4, -1, -1, NULL},
                {056, "tu", LAMBDA, 1, -1, -1, NULL},
                {057, "tevis", LAMBDA, 4, -1, -1, NULL},
                {060, "tev", LAMBDA, 2, -1, -1, NULL},
                {061, "tevi", LAMBDA, 3, -1, -1, NULL},
                {062, "tevī", LAMBDA, 3, -1, -1, NULL},
                {063, "jūs", LAMBDA, 2, -1, -1, NULL},
                {064, "jūsu", LAMBDA, 3, -1, -1, NULL},
                {065, "jums", LAMBDA, 3, -1, -1, NULL},
                {066, "jūsos", LAMBDA, 4, -1, -1, NULL},
                {067, "viņš", LAMBDA, 3, -1, -1, NULL},
                {070, "viņa", LAMBDA, 3, -1, -1, NULL},
                {071, "viņam", LAMBDA, 4, -1, -1, NULL},
                {072, "viņu", LAMBDA, 3, -1, -1, NULL},
                {073, "viņā", LAMBDA, 3, -1, -1, NULL},
                {074, "viņi", LAMBDA, 3, -1, -1, NULL},
                {075, "viņiem", LAMBDA, 5, -1, -1, NULL},
                {076, "viņus", LAMBDA, 4, -1, -1, NULL},
                {077, "viņos", LAMBDA, 4, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step0c_rules[] =
           {
                {001, "viņas", LAMBDA, 4, -1, -1, NULL},
                {002, "viņai", LAMBDA, 4, -1, -1, NULL},
                {003, "viņām", LAMBDA, 4, -1, -1, NULL},
                {004, "viņās", LAMBDA, 4, -1, -1, NULL},
                {005, "kurš", LAMBDA, 3, -1, -1, NULL},
                {006, "kura", LAMBDA, 3, -1, -1, NULL},
                {007, "kuram", LAMBDA, 4, -1, -1, NULL},
                {010, "kuru", LAMBDA, 3, -1, -1, NULL},
                {011, "kurā", LAMBDA, 3, -1, -1, NULL},
                {012, "kuri", LAMBDA, 3, -1, -1, NULL},
                {013, "kuriem", LAMBDA, 5, -1, -1, NULL},
                {014, "kurus", LAMBDA, 4, -1, -1, NULL},
                {015, "kuros", LAMBDA, 4, -1, -1, NULL},
                {016, "kuras", LAMBDA, 4, -1, -1, NULL},
                {017, "kurai", LAMBDA, 4, -1, -1, NULL},
                {020, "kurām", LAMBDA, 4, -1, -1, NULL},
                {021, "kurās", LAMBDA, 4, -1, -1, NULL},
                {022, "viss", LAMBDA, 3, -1, -1, NULL},
                {023, "visa", LAMBDA, 3, -1, -1, NULL},
                {024, "visam", LAMBDA, 4, -1, -1, NULL},
                {025, "visu", LAMBDA, 3, -1, -1, NULL},
                {026, "visā", LAMBDA, 3, -1, -1, NULL},
                {027, "visi", LAMBDA, 3, -1, -1, NULL},
                {030, "visiem", LAMBDA, 5, -1, -1, NULL},
                {031, "visus", LAMBDA, 4, -1, -1, NULL},
                {032, "visos", LAMBDA, 4, -1, -1, NULL},
                {033, "visas", LAMBDA, 4, -1, -1, NULL},
                {034, "visai", LAMBDA, 4, -1, -1, NULL},
                {035, "visām", LAMBDA, 4, -1, -1, NULL},
                {036, "visās", LAMBDA, 4, -1, -1, NULL},
                {037, "sevis", LAMBDA, 4, -1, -1, NULL},
                {040, "sev", LAMBDA, 2, -1, -1, NULL},
                {041, "sevi", LAMBDA, 3, -1, -1, NULL},
                {042, "sevī", LAMBDA, 3, -1, -1, NULL},
                {043, "kas", LAMBDA, 2, -1, -1, NULL},
                {044, "kā", LAMBDA, 1, -1, -1, NULL},
                {045, "kam", LAMBDA, 2, -1, -1, NULL},
                {046, "ko", LAMBDA, 1, -1, -1, NULL},
                {047, "kur", LAMBDA, 2, -1, -1, NULL},
                {050, "tas", LAMBDA, 2, -1, -1, NULL},
                {051, "tā", LAMBDA, 1, -1, -1, NULL},
                {052, "tam", LAMBDA, 2, -1, -1, NULL},
                {053, "to", LAMBDA, 1, -1, -1, NULL},
                {054, "tajā", LAMBDA, 3, -1, -1, NULL},
                {055, "tai", LAMBDA, 2, -1, -1, NULL},
                {056, "tanī", LAMBDA, 3, -1, -1, NULL},
                {057, "tās", LAMBDA, 3, -1, -1, NULL},
                {060, "tie", LAMBDA, 2, -1, -1, NULL},
                {061, "tiem", LAMBDA, 3, -1, -1, NULL},
                {062, "tos", LAMBDA, 2, -1, -1, NULL},
                {063, "tais", LAMBDA, 3, -1, -1, NULL},
                {064, "tajos", LAMBDA, 4, -1, -1, NULL},
                {065, "tanīs", LAMBDA, 4, -1, -1, NULL},
                {066, "tām", LAMBDA, 2, -1, -1, NULL},
                {067, "tajās", LAMBDA, 4, -1, -1, NULL},
                {070, "šis", LAMBDA, 2, -1, -1, NULL},
                {071, "šī", LAMBDA, 1, -1, -1, NULL},
                {072, "šā", LAMBDA, 1, -1, -1, NULL},
                {073, "šim", LAMBDA, 2, -1, -1, NULL},
                {074, "šo", LAMBDA, 1, -1, -1, NULL},
                {075, "šai", LAMBDA, 2, -1, -1, NULL},
                {076, "šajā", LAMBDA, 3, -1, -1, NULL},
                {077, "šinī", LAMBDA, 3, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };


static RuleList step0d_rules[] =
           {
                {001, "šīs", LAMBDA, 2, -1, -1, NULL},
                {002, "šās", LAMBDA, 2, -1, -1, NULL},
                {003, "šie", LAMBDA, 2, -1, -1, NULL},
                {004, "šiem", LAMBDA, 3, -1, -1, NULL},
                {005, "šām", LAMBDA, 2, -1, -1, NULL},
                {006, "šos", LAMBDA, 2, -1, -1, NULL},
                {007, "šais", LAMBDA, 3, -1, -1, NULL},
                {010, "šajos", LAMBDA, 4, -1, -1, NULL},
                {011, "šajās", LAMBDA, 4, -1, -1, NULL},
                {012, "šinīs", LAMBDA, 4, -1, -1, NULL},
                {013, "mans", LAMBDA, 3, -1, -1, NULL},
                {014, "mana", LAMBDA, 3, -1, -1, NULL},
                {015, "manam", LAMBDA, 4, -1, -1, NULL},
                {016, "manu", LAMBDA, 3, -1, -1, NULL},
                {017, "manī", LAMBDA, 3, -1, -1, NULL},
                {021, "maniem", LAMBDA, 5, -1, -1, NULL},
                {022, "manus", LAMBDA, 4, -1, -1, NULL},
                {023, "manos", LAMBDA, 4, -1, -1, NULL},
                {024, "manas", LAMBDA, 4, -1, -1, NULL},
                {025, "manai", LAMBDA, 4, -1, -1, NULL},
                {026, "manām", LAMBDA, 4, -1, -1, NULL},
                {027, "manās", LAMBDA, 4, -1, -1, NULL},
                {030, "tavs", LAMBDA, 3, -1, -1, NULL},
                {031, "tava", LAMBDA, 3, -1, -1, NULL},
                {032, "tavam", LAMBDA, 4, -1, -1, NULL},
                {033, "tavu", LAMBDA, 3, -1, -1, NULL},
                {034, "tavā", LAMBDA, 3, -1, -1, NULL},
                {035, "tavi", LAMBDA, 3, -1, -1, NULL},
                {036, "taviem", LAMBDA, 5, -1, -1, NULL},
                {037, "tavus", LAMBDA, 4, -1, -1, NULL},
                {040, "tavos", LAMBDA, 4, -1, -1, NULL},
                {041, "tavas", LAMBDA, 4, -1, -1, NULL},
                {042, "tavai", LAMBDA, 4, -1, -1, NULL},
                {043, "tavām", LAMBDA, 4, -1, -1, NULL},
                {044, "tavās", LAMBDA, 4, -1, -1, NULL},
                {045, "savs", LAMBDA, 3, -1, -1, NULL},
                {046, "sava", LAMBDA, 3, -1, -1, NULL},
                {047, "savam", LAMBDA, 4, -1, -1, NULL},
                {050, "savu", LAMBDA, 3, -1, -1, NULL},
                {051, "savā", LAMBDA, 3, -1, -1, NULL},
                {052, "savi", LAMBDA, 3, -1, -1, NULL},
                {053, "saviem", LAMBDA, 5, -1, -1, NULL},
                {054, "savus", LAMBDA, 4, -1, -1, NULL},
                {055, "savos", LAMBDA, 4, -1, -1, NULL},
                {056, "savas", LAMBDA, 4, -1, -1, NULL},
                {057, "savai", LAMBDA, 4, -1, -1, NULL},
                {060, "savām", LAMBDA, 4, -1, -1, NULL},
                {061, "savās", LAMBDA, 4, -1, -1, NULL},
                {062, "cits", LAMBDA, 3, -1, -1, NULL},
                {063, "cita", LAMBDA, 3, -1, -1, NULL},
                {064, "citam", LAMBDA, 4, -1, -1, NULL},
                {065, "citu", LAMBDA, 3, -1, -1, NULL},
                {066, "citā", LAMBDA, 3, -1, -1, NULL},
                {067, "citi", LAMBDA, 3, -1, -1, NULL},
                {070, "citiem", LAMBDA, 5, -1, -1, NULL},
                {071, "citus", LAMBDA, 4, -1, -1, NULL},
                {072, "citos", LAMBDA, 4, -1, -1, NULL},
                {073, "citas", LAMBDA, 4, -1, -1, NULL},
                {074, "citai", LAMBDA, 4, -1, -1, NULL},
                {075, "citām", LAMBDA, 4, -1, -1, NULL},
                {076, "citās", LAMBDA, 4, -1, -1, NULL},
                {077, "dažs", LAMBDA, 3, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step0e_rules[] =
           {
                {001, "daža", LAMBDA, 3, -1, -1, NULL},
                {002, "dažam", LAMBDA, 4, -1, -1, NULL},
                {003, "dažu", LAMBDA, 3, -1, -1, NULL},
                {004, "dažā", LAMBDA, 3, -1, -1, NULL},
                {005, "daži", LAMBDA, 3, -1, -1, NULL},
                {006, "dažiem", LAMBDA, 5, -1, -1, NULL},
                {007, "dažus", LAMBDA, 4, -1, -1, NULL},
                {010, "dažos", LAMBDA, 4, -1, -1, NULL},
                {011, "dažas", LAMBDA, 4, -1, -1, NULL},
                {012, "dažai", LAMBDA, 4, -1, -1, NULL},
                {013, "dažām", LAMBDA, 4, -1, -1, NULL},
                {014, "dažās", LAMBDA, 4, -1, -1, NULL},
                {015, "kāds", LAMBDA, 3, -1, -1, NULL},
                {016, "kāda", LAMBDA, 3, -1, -1, NULL},
                {017, "kādam", LAMBDA, 4, -1, -1, NULL},
                {020, "kādu", LAMBDA, 3, -1, -1, NULL},
                {021, "kādā", LAMBDA, 3, -1, -1, NULL},
                {022, "kādi", LAMBDA, 3, -1, -1, NULL},
                {023, "kādiem", LAMBDA, 5, -1, -1, NULL},
                {024, "kādus", LAMBDA, 4, -1, -1, NULL},
                {025, "kādos", LAMBDA, 4, -1, -1, NULL},
                {026, "kādas", LAMBDA, 4, -1, -1, NULL},
                {027, "kādai", LAMBDA, 4, -1, -1, NULL},
                {030, "kādām", LAMBDA, 4, -1, -1, NULL},
                {031, "kādās", LAMBDA, 4, -1, -1, NULL},
                {032, "kurš", LAMBDA, 3, -1, -1, NULL},
                {033, "kura", LAMBDA, 3, -1, -1, NULL},
                {034, "kuram", LAMBDA, 4, -1, -1, NULL},
                {035, "kuru", LAMBDA, 3, -1, -1, NULL},
                {036, "kurā", LAMBDA, 3, -1, -1, NULL},
                {037, "kuri", LAMBDA, 3, -1, -1, NULL},
                {040, "kuriem", LAMBDA, 5, -1, -1, NULL},
                {041, "kurus", LAMBDA, 4, -1, -1, NULL},
                {042, "kuros", LAMBDA, 4, -1, -1, NULL},
                {043, "kuras", LAMBDA, 4, -1, -1, NULL},
                {044, "kurai", LAMBDA, 4, -1, -1, NULL},
                {045, "kurām", LAMBDA, 4, -1, -1, NULL},
                {046, "kurās", LAMBDA, 4, -1, -1, NULL},
                {047, "tāds", LAMBDA, 3, -1, -1, NULL},
                {050, "tāda", LAMBDA, 3, -1, -1, NULL},
                {051, "tādam", LAMBDA, 4, -1, -1, NULL},
                {052, "tādu", LAMBDA, 3, -1, -1, NULL},
                {053, "tādā", LAMBDA, 3, -1, -1, NULL},
                {054, "tādi", LAMBDA, 3, -1, -1, NULL},
                {055, "tādiem", LAMBDA, 5, -1, -1, NULL},
                {056, "tādus", LAMBDA, 4, -1, -1, NULL},
                {057, "tādos", LAMBDA, 4, -1, -1, NULL},
                {060, "tādas", LAMBDA, 4, -1, -1, NULL},
                {061, "tādai", LAMBDA, 4, -1, -1, NULL},
                {062, "tādām", LAMBDA, 4, -1, -1, NULL},
                {063, "tādās", LAMBDA, 4, -1, -1, NULL},
                {064, "šāds", LAMBDA, 3, -1, -1, NULL},
                {065, "šāda", LAMBDA, 3, -1, -1, NULL},
                {066, "šādam", LAMBDA, 4, -1, -1, NULL},
                {067, "šādu", LAMBDA, 3, -1, -1, NULL},
                {070, "šādā", LAMBDA, 3, -1, -1, NULL},
                {071, "šādi", LAMBDA, 3, -1, -1, NULL},
                {072, "šādiem", LAMBDA, 5, -1, -1, NULL},
                {073, "šādus", LAMBDA, 4, -1, -1, NULL},
                {074, "šādos", LAMBDA, 4, -1, -1, NULL},
                {075, "šādas", LAMBDA, 4, -1, -1, NULL},
                {076, "šādai", LAMBDA, 4, -1, -1, NULL},
                {077, "šādām", LAMBDA, 4, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step0f_rules[] =
           {
                {001, "šādās", LAMBDA, 4, -1, -1, NULL},
                {002, "katrs", LAMBDA, 4, -1, -1, NULL},
                {003, "katra", LAMBDA, 4, -1, -1, NULL},
                {004, "katram", LAMBDA, 5, -1, -1, NULL},
                {005, "katru", LAMBDA, 4, -1, -1, NULL},
                {006, "katrā", LAMBDA, 4, -1, -1, NULL},
                {007, "katri", LAMBDA, 4, -1, -1, NULL},
                {010, "katriem", LAMBDA, 6, -1, -1, NULL},
                {011, "katrus", LAMBDA, 5, -1, -1, NULL},
                {012, "katros", LAMBDA, 5, -1, -1, NULL},
                {013, "katras", LAMBDA, 5, -1, -1, NULL},
                {014, "katrai", LAMBDA, 5, -1, -1, NULL},
                {015, "katrām", LAMBDA, 5, -1, -1, NULL},
                {016, "katrās", LAMBDA, 5, -1, -1, NULL},
                {017, "manējs", LAMBDA, 5, -1, -1, NULL},
                {020, "manējā", LAMBDA, 5, -1, -1, NULL},
                {021, "manējam", LAMBDA, 6, -1, -1, NULL},
                {022, "manēju", LAMBDA, 5, -1, -1, NULL},
                {023, "manējā", LAMBDA, 5, -1, -1, NULL},
                {024, "manēji", LAMBDA, 5, -1, -1, NULL},
                {025, "manējiem", LAMBDA, 7, -1, -1, NULL},
                {026, "manējus", LAMBDA, 6, -1, -1, NULL},
                {027, "manējos", LAMBDA, 6, -1, -1, NULL},
                {030, "manējais", LAMBDA, 7, -1, -1, NULL},
                {032, "manējo", LAMBDA, 5, -1, -1, NULL},
                {033, "manēie", LAMBDA, 6, -1, -1, NULL},
                {034, "manējās", LAMBDA, 6, -1, -1, NULL},
                {035, "manējai", LAMBDA, 6, -1, -1, NULL},
                {036, "manējām", LAMBDA, 6, -1, -1, NULL},
                {037, "manējas", LAMBDA, 6, -1, -1, NULL},
                {040, "tavējs", LAMBDA, 5, -1, -1, NULL},
                {041, "tavēja", LAMBDA, 5, -1, -1, NULL},
                {042, "tavējam", LAMBDA, 6, -1, -1, NULL},
                {043, "tavēju", LAMBDA, 5, -1, -1, NULL},
                {044, "tavējā", LAMBDA, 5, -1, -1, NULL},
                {045, "tavēji", LAMBDA, 5, -1, -1, NULL},
                {046, "tavējiem", LAMBDA, 7, -1, -1, NULL},
                {047, "tavējus", LAMBDA, 6, -1, -1, NULL},
                {050, "tavējos", LAMBDA, 6, -1, -1, NULL},
                {051, "tavējais", LAMBDA, 7, -1, -1, NULL},
                {053, "tavējo", LAMBDA, 5, -1, -1, NULL},
                {054, "tavējie", LAMBDA, 6, -1, -1, NULL},
                {055, "tavējās", LAMBDA, 6, -1, -1, NULL},
                {056, "tavējai", LAMBDA, 6, -1, -1, NULL},
                {057, "tavējām", LAMBDA, 6, -1, -1, NULL},
                {060, "tavējas", LAMBDA, 6, -1, -1, NULL},
                {061, "savējs", LAMBDA, 5, -1, -1, NULL},
                {062, "savēja", LAMBDA, 5, -1, -1, NULL},
                {063, "savējam", LAMBDA, 6, -1, -1, NULL},
                {064, "savēju", LAMBDA, 5, -1, -1, NULL},
                {065, "savējā", LAMBDA, 5, -1, -1, NULL},
                {066, "savēji", LAMBDA, 5, -1, -1, NULL},
                {067, "savējiem", LAMBDA, 7, -1, -1, NULL},
                {070, "savējus", LAMBDA, 6, -1, -1, NULL},
                {071, "savējos", LAMBDA, 6, -1, -1, NULL},
                {072, "savējais", LAMBDA, 7, -1, -1, NULL},
                {074, "savējo", LAMBDA, 5, -1, -1, NULL},
                {075, "savējie", LAMBDA, 6, -1, -1, NULL},
                {076, "savējās", LAMBDA, 6, -1, -1, NULL},
                {077, "savējai", LAMBDA, 6, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step0g_rules[] =
           {
                {001, "savējām", LAMBDA, 6, -1, -1, NULL},
                {002, "savējas", LAMBDA, 6, -1, -1, NULL},
                {003, "viņējs", LAMBDA, 5, -1, -1, NULL},
                {004, "viņēja", LAMBDA, 5, -1, -1, NULL},
                {005, "viņējamm", LAMBDA, 6, -1, -1, NULL},
                {006, "viņēju", LAMBDA, 5, -1, -1, NULL},
                {007, "viņējā", LAMBDA, 5, -1, -1, NULL},
                {010, "viņēji", LAMBDA, 5, -1, -1, NULL},
                {011, "viņējiem", LAMBDA, 7, -1, -1, NULL},
                {012, "viņējus", LAMBDA, 6, -1, -1, NULL},
                {013, "viņējos", LAMBDA, 6, -1, -1, NULL},
                {014, "viņējais", LAMBDA, 7, -1, -1, NULL},
                {016, "viņējo", LAMBDA, 5, -1, -1, NULL},
                {017, "viņējie", LAMBDA, 6, -1, -1, NULL},
                {020, "viņējās", LAMBDA, 6, -1, -1, NULL},
                {021, "viņējai", LAMBDA, 6, -1, -1, NULL},
                {022, "viņējām", LAMBDA, 6, -1, -1, NULL},
                {023, "viņējas", LAMBDA, 6, -1, -1, NULL},
                {024, "jūsējs", LAMBDA, 5, -1, -1, NULL},
                {025, "jūsēja", LAMBDA, 5, -1, -1, NULL},
                {026, "jūsējam", LAMBDA, 6, -1, -1, NULL},
                {027, "jūsēju", LAMBDA, 5, -1, -1, NULL},
                {030, "jūsējā", LAMBDA, 5, -1, -1, NULL},
                {031, "jūsēji", LAMBDA, 5, -1, -1, NULL},
                {032, "jūsējiem", LAMBDA, 7, -1, -1, NULL},
                {033, "jūsējus", LAMBDA, 6, -1, -1, NULL},
                {034, "jūsējos", LAMBDA, 6, -1, -1, NULL},
                {035, "jūsējais", LAMBDA, 7, -1, -1, NULL},
                {037, "jūsējo", LAMBDA, 5, -1, -1, NULL},
                {040, "jūsējie", LAMBDA, 6, -1, -1, NULL},
                {041, "jūsējās", LAMBDA, 6, -1, -1, NULL},
                {042, "jūsējai", LAMBDA, 6, -1, -1, NULL},
                {043, "jūsējām", LAMBDA, 6, -1, -1, NULL},
                {044, "jūsējas", LAMBDA, 6, -1, -1, NULL},
                {045, "mūsējs", LAMBDA, 5, -1, -1, NULL},
                {046, "mūsēja", LAMBDA, 5, -1, -1, NULL},
                {047, "mūsējam", LAMBDA, 6, -1, -1, NULL},
                {050, "mūsēju", LAMBDA, 5, -1, -1, NULL},
                {051, "musējā", LAMBDA, 5, -1, -1, NULL},
                {052, "mūsēji", LAMBDA, 5, -1, -1, NULL},
                {053, "mūsējiem", LAMBDA, 7, -1, -1, NULL},
                {054, "mūsējus", LAMBDA, 6, -1, -1, NULL},
                {055, "mūsējos", LAMBDA, 6, -1, -1, NULL},
                {056, "mūsējais", LAMBDA, 7, -1, -1, NULL},
                {060, "mūsējo", LAMBDA, 5, -1, -1, NULL},
                {061, "mūsējie", LAMBDA, 6, -1, -1, NULL},
                {062, "mūsējās", LAMBDA, 6, -1, -1, NULL},
                {063, "mūsējai", LAMBDA, 6, -1, -1, NULL},
                {064, "mūsējām", LAMBDA, 6, -1, -1, NULL},
                {065, "mūsējas", LAMBDA, 6, -1, -1, NULL},
                {066, "šitas", LAMBDA, 4, -1, -1, NULL},
                {067, "šitā", LAMBDA, 3, -1, -1, NULL},
                {070, "šitam", LAMBDA, 4, -1, -1, NULL},
                {071, "šito", LAMBDA, 3, -1, -1, NULL},
                {072, "šitai", LAMBDA, 4, -1, -1, NULL},
                {073, "šitie", LAMBDA, 4, -1, -1, NULL},
                {074, "šitiem", LAMBDA, 5, -1, -1, NULL},
                {075, "šitos", LAMBDA, 4, -1, -1, NULL},
                {076, "šitšs", LAMBDA, 4, -1, -1, NULL},
                {077, "šitām", LAMBDA, 4, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step0h_rules[] =
           {
                {001, "šitāds", LAMBDA, 5, -1, -1, NULL},
                {002, "šitāda", LAMBDA, 5, -1, -1, NULL},
                {003, "šitādam", LAMBDA, 6, -1, -1, NULL},
                {004, "šitādu", LAMBDA, 5, -1, -1, NULL},
                {005, "šitādā", LAMBDA, 5, -1, -1, NULL},
                {006, "šitādas", LAMBDA, 6, -1, -1, NULL},
                {007, "šitādai", LAMBDA, 6, -1, -1, NULL},
                {010, "šitādi", LAMBDA, 5, -1, -1, NULL},
                {011, "šitādiem", LAMBDA, 7, -1, -1, NULL},
                {012, "šitādus", LAMBDA, 6, -1, -1, NULL},
                {013, "šitādos", LAMBDA, 6, -1, -1, NULL},
                {014, "šitādām", LAMBDA, 6, -1, -1, NULL},
                {015, "šitādās", LAMBDA, 6, -1, -1, NULL},
                {016, "ikkatrs", LAMBDA, 6, -1, -1, NULL},
                {017, "ikkatra", LAMBDA, 6, -1, -1, NULL},
                {020, "ikkatram", LAMBDA, 7, -1, -1, NULL},
                {021, "ikkatru", LAMBDA, 6, -1, -1, NULL},
                {022, "ikkatrā", LAMBDA, 6, -1, -1, NULL},
                {023, "ikkatras", LAMBDA, 7, -1, -1, NULL},
                {024, "ikkatrai", LAMBDA, 7, -1, -1, NULL},
                {025, "ikkatri", LAMBDA, 6, -1, -1, NULL},
                {026, "ikkatriem", LAMBDA, 8, -1, -1, NULL},
                {027, "ikkatrus", LAMBDA, 7, -1, -1, NULL},
                {030, "ikkatros", LAMBDA, 7, -1, -1, NULL},
                {032, "ikkatrām", LAMBDA, 7, -1, -1, NULL},
                {033, "ikkatrās", LAMBDA, 7, -1, -1, NULL},
                {034, "jebkāds", LAMBDA, 6, -1, -1, NULL},
                {035, "jebkāda", LAMBDA, 6, -1, -1, NULL},
                {036, "jebkādam", LAMBDA, 7, -1, -1, NULL},
                {037, "jebkādu", LAMBDA, 6, -1, -1, NULL},
                {040, "jebkādā", LAMBDA, 6, -1, -1, NULL},
                {041, "jebkādas", LAMBDA, 7, -1, -1, NULL},
                {042, "jebkādai", LAMBDA, 7, -1, -1, NULL},
                {043, "jebkādi", LAMBDA, 6, -1, -1, NULL},
                {044, "jebkādiem", LAMBDA, 8, -1, -1, NULL},
                {045, "jebkādus", LAMBDA, 7, -1, -1, NULL},
                {046, "jebkādos", LAMBDA, 7, -1, -1, NULL},
                {047, "jebkādām", LAMBDA, 7, -1, -1, NULL},
                {050, "jebkādās", LAMBDA, 7, -1, -1, NULL},
                {051, "jebkas", LAMBDA, 5, -1, -1, NULL},
                {053, "jebkā", LAMBDA, 4, -1, -1, NULL},
                {054, "jebkam", LAMBDA, 5, -1, -1, NULL},
                {055, "jebko", LAMBDA, 4, -1, -1, NULL},
                {056, "jebkurš", LAMBDA, 6, -1, -1, NULL},
                {057, "jebkura", LAMBDA, 6, -1, -1, NULL},
                {060, "jebkuram", LAMBDA, 7, -1, -1, NULL},
                {061, "jebkuru", LAMBDA, 6, -1, -1, NULL},
                {062, "jebkurā", LAMBDA, 6, -1, -1, NULL},
                {063, "jebkuras", LAMBDA, 7, -1, -1, NULL},
                {064, "jebkurai", LAMBDA, 7, -1, -1, NULL},
                {065, "jebkuri", LAMBDA, 6, -1, -1, NULL},
                {066, "jebkuriem", LAMBDA, 8, -1, -1, NULL},
                {067, "jebkurus", LAMBDA, 7, -1, -1, NULL},
                {070, "jebkuros", LAMBDA, 7, -1, -1, NULL},
                {071, "jebkurām", LAMBDA, 7, -1, -1, NULL},
                {072, "jebkurās", LAMBDA, 7, -1, -1, NULL},
                {074, "ikkurš", LAMBDA, 5, -1, -1, NULL},
                {075, "ikkura", LAMBDA, 5, -1, -1, NULL},
                {076, "ikkuram", LAMBDA, 6, -1, -1, NULL},
                {077, "ikkuru", LAMBDA, 5, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step0i_rules[] =
           {
                {001, "ikkurā", LAMBDA, 5, -1, -1, NULL},
                {002, "ikkuras", LAMBDA, 6, -1, -1, NULL},
                {003, "ikkurai", LAMBDA, 6, -1, -1, NULL},
                {004, "ikkuri", LAMBDA, 5, -1, -1, NULL},
                {005, "ikkuriem", LAMBDA, 7, -1, -1, NULL},
                {006, "ikkurus", LAMBDA, 6, -1, -1, NULL},
                {007, "ikkuros", LAMBDA, 6, -1, -1, NULL},
                {010, "ikkurām", LAMBDA, 6, -1, -1, NULL},
                {011, "ikkurās", LAMBDA, 6, -1, -1, NULL},
                {012, "ikviens", LAMBDA, 6, -1, -1, NULL},
                {013, "ikviena", LAMBDA, 6, -1, -1, NULL},
                {014, "ikvienam", LAMBDA, 7, -1, -1, NULL},
                {016, "ikvienu", LAMBDA, 6, -1, -1, NULL},
                {017, "ikvienā", LAMBDA, 6, -1, -1, NULL},
                {020, "ikvienas", LAMBDA, 7, -1, -1, NULL},
                {021, "ikvienai", LAMBDA, 7, -1, -1, NULL},
                {022, "ikvieni", LAMBDA, 6, -1, -1, NULL},
                {023, "ikvieniem", LAMBDA, 8, -1, -1, NULL},
                {024, "ikvienus", LAMBDA, 7, -1, -1, NULL},
                {025, "ikvienos", LAMBDA, 7, -1, -1, NULL},
                {026, "ikvienām", LAMBDA, 7, -1, -1, NULL},
                {027, "ikvienās", LAMBDA, 7, -1, -1, NULL},
                {030, "nekas", LAMBDA, 4, -1, -1, NULL},
                {031, "nekā", LAMBDA, 3, -1, -1, NULL},
                {032, "nekam", LAMBDA, 4, -1, -1, NULL},
                {033, "neko", LAMBDA, 3, -1, -1, NULL},
                {034, "nekāds", LAMBDA, 5, -1, -1, NULL},
                {035, "nekāda", LAMBDA, 5, -1, -1, NULL},
                {037, "nekādam", LAMBDA, 6, -1, -1, NULL},
                {040, "nekādu", LAMBDA, 5, -1, -1, NULL},
                {041, "nekādā", LAMBDA, 5, -1, -1, NULL},
                {042, "nekādas", LAMBDA, 6, -1, -1, NULL},
                {043, "nekādai", LAMBDA, 6, -1, -1, NULL},
                {044, "nekādi", LAMBDA, 5, -1, -1, NULL},
                {045, "nekādiem", LAMBDA, 7, -1, -1, NULL},
                {046, "nekādus", LAMBDA, 6, -1, -1, NULL},
                {047, "nekādos", LAMBDA, 6, -1, -1, NULL},
                {050, "nekādām", LAMBDA, 6, -1, -1, NULL},
                {051, "nekādās", LAMBDA, 6, -1, -1, NULL},
                {052, "neviens", LAMBDA, 6, -1, -1, NULL},
                {053, "neviena", LAMBDA, 6, -1, -1, NULL},
                {054, "nevienam", LAMBDA, 7, -1, -1, NULL},
                {055, "nevienu", LAMBDA, 6, -1, -1, NULL},
                {056, "nevienā", LAMBDA, 6, -1, -1, NULL},
                {060, "nevienās", LAMBDA, 6, -1, -1, NULL},
                {061, "nevienai", LAMBDA, 7, -1, -1, NULL},
                {062, "nevieni", LAMBDA, 6, -1, -1, NULL},
                {063, "nevieniem",LAMBDA, 8, -1, -1, NULL},
                {064, "nevienus", LAMBDA, 7, -1, -1, NULL},
                {065, "nevienos", LAMBDA, 7, -1, -1, NULL},
                {066, "nevienām", LAMBDA, 7, -1, -1, NULL},
                {067, "nevienās", LAMBDA, 7, -1, -1, NULL},
                {070, "pats", LAMBDA, 3, -1, -1, NULL},
                {071, "paša", LAMBDA, 3, -1, -1, NULL},
                {072, "pašam", LAMBDA, 4, -1, -1, NULL},
                {073, "pagu", LAMBDA, 3, -1, -1, NULL},
                {074, "pašu", LAMBDA, 3, -1, -1, NULL},
                {075, "pati", LAMBDA, 3, -1, -1, NULL},
                {076, "pašas", LAMBDA, 4, -1, -1, NULL},
                {077, "pašai", LAMBDA, 4, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step0j_rules[] =
           {
                {001, "paši", LAMBDA, 3, -1, -1, NULL},
                {002, "pašiem", LAMBDA, 5, -1, -1, NULL},
                {003, "pašus", LAMBDA, 4, -1, -1, NULL},
                {004, "pašos", LAMBDA, 4, -1, -1, NULL},
                {005, "pašām", LAMBDA, 4, -1, -1, NULL},
                {006, "pašās", LAMBDA, 4, -1, -1, NULL},
                {007, "būt", LAMBDA, 2, -1, -1, NULL},
                {010, "biju", LAMBDA, 3, -1, -1, NULL},
                {011, "biji", LAMBDA, 3, -1, -1, NULL},
                {012, "bija", LAMBDA, 3, -1, -1, NULL},
                {013, "bijām", LAMBDA, 4, -1, -1, NULL},
                {014, "bijāt", LAMBDA, 4, -1, -1, NULL},
                {015, "esmu", LAMBDA, 3, -1, -1, NULL},
                {016, "esi", LAMBDA, 2, -1, -1, NULL},
                {017, "esam", LAMBDA, 3, -1, -1, NULL},
                {020, "esat", LAMBDA, 3, -1, -1, NULL},
                {021, "būšu", LAMBDA, 3, -1, -1, NULL},
                {022, "būsi", LAMBDA, 3, -1, -1, NULL},
                {023, "būs", LAMBDA, 2, -1, -1, NULL},
                {024, "būsim", LAMBDA, 4, -1, -1, NULL},
                {025, "būsiet", LAMBDA, 5, -1, -1, NULL},
                {026, "tikt", LAMBDA, 3, -1, -1, NULL},
                {027, "tiku", LAMBDA, 3, -1, -1, NULL},
                {030, "tiki", LAMBDA, 3, -1, -1, NULL},
                {031, "tika", LAMBDA, 3, -1, -1, NULL},
                {032, "tikām", LAMBDA, 4, -1, -1, NULL},
                {033, "tikāt", LAMBDA, 4, -1, -1, NULL},
                {034, "tieku", LAMBDA, 4, -1, -1, NULL},
                {035, "tiec", LAMBDA, 3, -1, -1, NULL},
                {037, "tiek", LAMBDA, 3, -1, -1, NULL},
                {040, "tiekam", LAMBDA, 5, -1, -1, NULL},
                {041, "tiekat", LAMBDA, 5, -1, -1, NULL},
                {042, "tikšu", LAMBDA, 4, -1, -1, NULL},
                {043, "tiks", LAMBDA, 3, -1, -1, NULL},
                {044, "tiksim", LAMBDA, 5, -1, -1, NULL},
                {045, "tiksiet", LAMBDA, 6, -1, -1, NULL},
                {046, "tapt", LAMBDA, 3, -1, -1, NULL},
                {047, "tapi", LAMBDA, 3, -1, -1, NULL},
                {050, "tapāt", LAMBDA, 4, -1, -1, NULL},
                {051, "topat", LAMBDA, 4, -1, -1, NULL},
                {052, "tapšu", LAMBDA, 4, -1, -1, NULL},
                {053, "tapsi", LAMBDA, 4, -1, -1, NULL},
                {054, "taps", LAMBDA, 3, -1, -1, NULL},
                {055, "tapsim", LAMBDA, 5, -1, -1, NULL},
                {056, "tapsiet", LAMBDA, 6, -1, -1, NULL},
                {057, "kļūt", LAMBDA, 3, -1, -1, NULL},
                {060, "kļuvu", LAMBDA, 4, -1, -1, NULL},
                {061, "kļuvi", LAMBDA, 4, -1, -1, NULL},
                {062, "kļuva", LAMBDA, 4, -1, -1, NULL},
                {063, "kļuvām", LAMBDA, 5, -1, -1, NULL},
                {064, "kļuvāt", LAMBDA, 5, -1, -1, NULL},
                {065, "kļūstu", LAMBDA, 5, -1, -1, NULL},
                {066, "kļūsti", LAMBDA, 5, -1, -1, NULL},
                {067, "kļūst", LAMBDA, 4, -1, -1, NULL},
                {070, "kļūstam", LAMBDA, 6, -1, -1, NULL},
                {071, "kļūstat", LAMBDA, 6, -1, -1, NULL},
                {072, "kļūšu", LAMBDA, 4, -1, -1, NULL},
                {073, "kļūsi", LAMBDA, 4, -1, -1, NULL},
                {074, "kļūs", LAMBDA, 3, -1, -1, NULL},
                {075, "kļūsim", LAMBDA, 5, -1, -1, NULL},
                {076, "kļūsiet", LAMBDA, 6, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step0k_rules[] =
           {
                {001, "ūja", LAMBDA, 2, -1, -1, NULL},
                {002, "urrā", LAMBDA, 3, -1, -1, NULL},
                {003, "urā", LAMBDA, 2, -1, -1, NULL},
                {004, "re", LAMBDA, 1, -1, -1, NULL},
                {005, "pag", LAMBDA, 2, -1, -1, NULL},
                {006, "raug", LAMBDA, 3, -1, -1, NULL},
                {007, "paraug", LAMBDA, 5, -1, -1, NULL},
                {010, "ai", LAMBDA, 1, -1, -1, NULL},
                {011, "ak", LAMBDA, 1, -1, -1, NULL},
                {012, "palūk", LAMBDA, 4, -1, -1, NULL},
                {013, "nudien", LAMBDA, 5, -1, -1, NULL},
                {014, "ekur", LAMBDA, 3, -1, -1, NULL},
                {015, "kuš", LAMBDA, 2, -1, -1, NULL},
                {016, "skat", LAMBDA, 3, -1, -1, NULL},
                {017, "paskat", LAMBDA, 5, -1, -1, NULL},
                {020, "paklau", LAMBDA, 5, -1, -1, NULL},
                {021, "ņau", LAMBDA, 2, -1, -1, NULL},
                {022, "rau", LAMBDA, 2, -1, -1, NULL},
                {023, "parau", LAMBDA, 4, -1, -1, NULL},
                {024, "nu", LAMBDA, 1, -1, -1, NULL},
                {025, "tpū", LAMBDA, 2, -1, -1, NULL},
                {026, "vau", LAMBDA, 1, -1, -1, NULL},
                {027, "redz", LAMBDA, 3, -1, -1, NULL},
                {030, "varēt", LAMBDA, 4, -1, -1, NULL},
                {031, "var", LAMBDA, 2, -1, -1, NULL},
                {032, "varat", LAMBDA, 4, -1, -1, NULL},
                {033, "varēju", LAMBDA, 5, -1, -1, NULL},
                {034, "varēji", LAMBDA, 5, -1, -1, NULL},
                {035, "varēja", LAMBDA, 5, -1, -1, NULL},
                {036, "varējām", LAMBDA, 6, -1, -1, NULL},
                {037, "varējāt", LAMBDA, 6, -1, -1, NULL},
                {040, "varēšu", LAMBDA, 5, -1, -1, NULL},
                {041, "varēsi", LAMBDA, 5, -1, -1, NULL},
                {042, "varēs", LAMBDA, 4, -1, -1, NULL},
                {043, "varēsim", LAMBDA, 6, -1, -1, NULL},
                {044, "varēsiet", LAMBDA, 7, -1, -1, NULL},
                {045, "kāpēc", LAMBDA, 4, -1, -1, NULL},
                {046, "kādēļ", LAMBDA, 4, -1, -1, NULL},
                {047, "kālab", LAMBDA, 4, -1, -1, NULL},
                {050, "tālab", LAMBDA, 4, -1, -1, NULL},
                {051, "kālabad", LAMBDA, 6, -1, -1, NULL},
                {052, "tālabad", LAMBDA, 6, -1, -1, NULL},
                {053, "kamdēļ", LAMBDA, 5, -1, -1, NULL},
                {054, "tamdēļ", LAMBDA, 5, -1, -1, NULL},
                {055, "bezgala", LAMBDA, 6, -1, -1, NULL},
                {056, "nenieka", LAMBDA, 6, -1, -1, NULL},
                {057, "samērā", LAMBDA, 5, -1, -1, NULL},
                {060, "vērā", LAMBDA, 3, -1, -1, NULL},
                {061, "visupēc", LAMBDA, 6, -1, -1, NULL},
                {062, "tagad", LAMBDA, 4, -1, -1, NULL},
                {063, "kad", LAMBDA, 2, -1, -1, NULL},
                {064, "jebkad", LAMBDA, 5, -1, -1, NULL},
                {065, "nekad", LAMBDA, 4, -1, -1, NULL},
                {066, "šad", LAMBDA, 2, -1, -1, NULL},
                {067, "še", LAMBDA, 1, -1, -1, NULL},
                {070, "tādējādi", LAMBDA, 7, -1, -1, NULL},
                {071, "visādi", LAMBDA, 5, -1, -1, NULL},
                {072, "visvisādi", LAMBDA, 8, -1, -1, NULL},
                {073, "citādi", LAMBDA, 5, -1, -1, NULL},
                {074, "parasti", LAMBDA, 6, -1, -1, NULL},
                {075, "dikti", LAMBDA, 4, -1, -1, NULL},
                {076, "ļoti", LAMBDA, 3, -1, -1, NULL},
                {077, "velti", LAMBDA, 4, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step0l_rules[] =
           {
                {001, "pēkšņi", LAMBDA, 5, -1, -1, NULL},
                {002, "respektīvi", LAMBDA, 9, -1, -1, NULL},
                {003, "līdzi", LAMBDA, 4, -1, -1, NULL},
                {004, "pretī", LAMBDA, 4, -1, -1, NULL},
                {005, "labāk", LAMBDA, 4, -1, -1, NULL},
                {006, "pēcāk", LAMBDA, 4, -1, -1, NULL},
                {007, "citādāk", LAMBDA, 6, -1, -1, NULL},
                {010, "savādāk", LAMBDA, 6, -1, -1, NULL},
                {011, "turpmāk", LAMBDA, 6, -1, -1, NULL},
                {012, "pārāk", LAMBDA, 4, -1, -1, NULL},
                {013, "agrāk", LAMBDA, 4, -1, -1, NULL},
                {014, "vairāk", LAMBDA, 5, -1, -1, NULL},
                {015, "visvairāk", LAMBDA, 8, -1, -1, NULL},
                {016, "mazāk", LAMBDA, 4, -1, -1, NULL},
                {017, "drīzāk", LAMBDA, 5, -1, -1, NULL},
                {020, "visbiežāk", LAMBDA, 8, -1, -1, NULL},
                {021, "cik", LAMBDA, 2, -1, -1, NULL},
                {022, "necik", LAMBDA, 4, -1, -1, NULL},
                {023, "šitik", LAMBDA, 4, -1, -1, NULL},
                {024, "atkal", LAMBDA, 4, -1, -1, NULL},
                {025, "tūdaļ", LAMBDA, 4, -1, -1, NULL},
                {026, "pakaļ", LAMBDA, 4, -1, -1, NULL},
                {027, "iepakaļ", LAMBDA, 6, -1, -1, NULL},
                {030, "nopakaļ", LAMBDA, 6, -1, -1, NULL},
                {031, "visnotaļ", LAMBDA, 7, -1, -1, NULL},
                {032, "atpakaļ", LAMBDA, 6, -1, -1, NULL},
                {033, "palaikam", LAMBDA, 7, -1, -1, NULL},
                {034, "aplam", LAMBDA, 4, -1, -1, NULL},
                {035, "piemēram", LAMBDA, 7, -1, -1, NULL},
                {036, "apmēram", LAMBDA, 6, -1, -1, NULL},
                {037, "nepagalam", LAMBDA, 8, -1, -1, NULL},
                {040, "pavisam", LAMBDA, 6, -1, -1, NULL},
                {041, "nepavisam", LAMBDA, 8, -1, -1, NULL},
                {042, "paretam", LAMBDA, 6, -1, -1, NULL},
                {043, "šimbrīžam", LAMBDA, 8, -1, -1, NULL},
                {044, "joprojām", LAMBDA, 7, -1, -1, NULL},
                {045, "aumaļām", LAMBDA, 6, -1, -1, NULL},
                {046, "lēnām", LAMBDA, 4, -1, -1, NULL},
                {047, "pamazām", LAMBDA, 6, -1, -1, NULL},
                {050, "gaužām", LAMBDA, 5, -1, -1, NULL},
                {051, "aizgūtnēm", LAMBDA, 8, -1, -1, NULL},
                {052, "pārpārēm", LAMBDA, 7, -1, -1, NULL},
                {053, "caurcaurēm", LAMBDA, 9, -1, -1, NULL},
                {054, "pamazītēm", LAMBDA, 8, -1, -1, NULL},
                {055, "pretim", LAMBDA, 5, -1, -1, NULL},
                {056, "iepretim", LAMBDA, 7, -1, -1, NULL},
                {057, "prom", LAMBDA, 3, -1, -1, NULL},
                {060, "patlaban", LAMBDA, 7, -1, -1, NULL},
                {061, "diezgan", LAMBDA, 6, -1, -1, NULL},
                {062, "šeitan", LAMBDA, 5, -1, -1, NULL},
                {063, "secen", LAMBDA, 4, -1, -1, NULL},
                {064, "šobaltdien", LAMBDA, 9, -1, -1, NULL},
                {065, "kādudien", LAMBDA, 7, -1, -1, NULL},
                {066, "citudien", LAMBDA, 7, -1, -1, NULL},
                {067, "daždien", LAMBDA, 6, -1, -1, NULL},
                {070, "mūždien", LAMBDA, 6, -1, -1, NULL},
                {071, "arvien", LAMBDA, 5, -1, -1, NULL},
                {072, "aizvien", LAMBDA, 6, -1, -1, NULL},
                {073, "varen", LAMBDA, 4, -1, -1, NULL},
                {074, "sen", LAMBDA, 2, -1, -1, NULL},
                {075, "pasen", LAMBDA, 4, -1, -1, NULL},
                {076, "nesen", LAMBDA, 4, -1, -1, NULL},
                {077, "bāztin", LAMBDA, 5, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };


static RuleList step0m_rules[] =
           {
                {001, "drusciņ", LAMBDA, 6, -1, -1, NULL},
                {002, "mazdrusciņ", LAMBDA, 9, -1, -1, NULL},
                {003, "tūliņ", LAMBDA, 4, -1, -1, NULL},
                {004, "mazlietiņ", LAMBDA, 8, -1, -1, NULL},
                {005, "neparko", LAMBDA, 6, -1, -1, NULL},
                {006, "vienkop", LAMBDA, 6, -1, -1, NULL},
                {007, "kurp", LAMBDA, 3, -1, -1, NULL},
                {010, "šurp", LAMBDA, 3, -1, -1, NULL},
                {011, "turp", LAMBDA, 3, -1, -1, NULL},
                {012, "vispār", LAMBDA, 5, -1, -1, NULL},
                {013, "viscaur", LAMBDA, 6, -1, -1, NULL},
                {014, "jebkur", LAMBDA, 5, -1, -1, NULL},
                {015, "nekur", LAMBDA, 4, -1, -1, NULL},
                {016, "visur", LAMBDA, 4, -1, -1, NULL},
                {017, "šur", LAMBDA, 2, -1, -1, NULL},
                {020, "tur", LAMBDA, 2, -1, -1, NULL},
                {021, "citur", LAMBDA, 4, -1, -1, NULL},
                {022, "vietumis", LAMBDA, 7, -1, -1, NULL},
                {023, "retumis", LAMBDA, 6, -1, -1, NULL},
                {024, "reizumis", LAMBDA, 7, -1, -1, NULL},
                {025, "sensenis", LAMBDA, 7, -1, -1, NULL},
                {026, "vairs", LAMBDA, 4, -1, -1, NULL},
                {027, "papildus", LAMBDA, 7, -1, -1, NULL},
                {030, "pārmijus", LAMBDA, 7, -1, -1, NULL},
                {031, "blakus", LAMBDA, 5, -1, -1, NULL},
                {032, "ieblakus", LAMBDA, 7, -1, -1, NULL},
                {033, "līdztekus", LAMBDA, 8, -1, -1, NULL},
                {034, "aplinkus", LAMBDA, 7, -1, -1, NULL},
                {035, "izklaidus", LAMBDA, 8, -1, -1, NULL},
                {036, "vienlaidus", LAMBDA, 9, -1, -1, NULL},
                {037, "neviļus", LAMBDA, 6, -1, -1, NULL},
                {040, "abpus", LAMBDA, 4, -1, -1, NULL},
                {041, "vienpus", LAMBDA, 5, -1, -1, NULL},
                {042, "katrpus", LAMBDA, 6, -1, -1, NULL},
                {043, "otrpus", LAMBDA, 5, -1, -1, NULL},
                {044, "virspus", LAMBDA, 6, -1, -1, NULL},
                {045, "papriekš", LAMBDA, 7, -1, -1, NULL},
                {046, "iepriekš", LAMBDA, 7, -1, -1, NULL},
                {047, "klāt", LAMBDA, 3, -1, -1, NULL},
                {050, "labprāt", LAMBDA, 6, -1, -1, NULL},
                {051, "nelabprāt", LAMBDA, 8, -1, -1, NULL},
                {052, "manuprāt", LAMBDA, 7, -1, -1, NULL},
                {053, "mūsuprāt", LAMBDA, 7, -1, -1, NULL},
                {054, "tīšuprāt", LAMBDA, 7, -1, -1, NULL},
                {055, "tavuprāt", LAMBDA, 7, -1, -1, NULL},
                {056, "ciet", LAMBDA, 3, -1, -1, NULL},
                {057, "mazliet", LAMBDA, 5, -1, -1, NULL},
                {060, "vienviet", LAMBDA, 7, -1, -1, NULL},
                {061, "vienuviet", LAMBDA, 8, -1, -1, NULL},
                {062, "dažviet", LAMBDA, 6, -1, -1, NULL},
                {063, "beidzot", LAMBDA, 6, -1, -1, NULL},
                {064, "visbeidzot", LAMBDA, 9,-1,-1, NULL},
                {065, "vairākkārt", LAMBDA, 9, -1, -1, NULL},
                {066, "pirmkārt", LAMBDA, 7, -1, -1, NULL},
                {067, "vienkārt", LAMBDA, 7, -1, -1, NULL},
                {070, "galvenokārt", LAMBDA, 10, -1, -1, NULL},
                {071, "apkārt", LAMBDA, 5, -1, -1, NULL},
                {072, "visapkārt", LAMBDA, 8, -1, -1, NULL},
                {073, "citkārt", LAMBDA, 6, -1, -1, NULL},
                {074, "daudzkārt", LAMBDA, 8, -1, -1, NULL},
                {075, "dažkārt", LAMBDA, 6, -1, -1, NULL},
                {076, "nost", LAMBDA, 3, -1, -1, NULL},
                {077, "pārlieku", LAMBDA, 7, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step0n_rules[] =
           {
                {001, "šiet", LAMBDA, 3, -1, -1, NULL},
                {002, "tūlīt", LAMBDA, 4, -1, -1, NULL},
                {003, "pirmīt", LAMBDA, 5, -1, -1, NULL},
                {004, "maķenīt", LAMBDA, 6, -1, -1, NULL},
                {005, "atstatu", LAMBDA, 6, -1, -1, NULL},
                {006, "maz", LAMBDA, 2, -1, -1, NULL},
                {007, "pamaz", LAMBDA, 4, -1, -1, NULL},
                {010, "nemaz", LAMBDA, 4, -1, -1, NULL},
                {011, "vismaz", LAMBDA, 5, -1, -1, NULL},
                {012, "daudzmaz", LAMBDA, 7, -1, -1, NULL},
                {013, "bezmaz", LAMBDA, 5, -1, -1, NULL},
                {014, "vienlīdz", LAMBDA, 7, -1, -1, NULL},
                {015, "puslīdz", LAMBDA, 6, -1, -1, NULL},
                {016, "daudz", LAMBDA, 4, -1, -1, NULL},
                {017, "nedaudz", LAMBDA, 6, -1, -1, NULL},
                {020, "reiz", LAMBDA, 3, -1, -1, NULL},
                {021, "ikreiz", LAMBDA, 5, -1, -1, NULL},
                {022, "kādreiz", LAMBDA, 6, -1, -1, NULL},
                {023, "vairākreiz", LAMBDA, 9, -1, -1, NULL},
                {024, "cikreiz", LAMBDA, 6, -1, -1, NULL},
                {025, "tikreiz", LAMBDA, 6, -1, -1, NULL},
                {026, "vēlreiz", LAMBDA, 6, -1, -1, NULL},
                {027, "nākamreiz", LAMBDA, 8, -1, -1, NULL},
                {030, "viņreiz", LAMBDA, 6, -1, -1, NULL},
                {031, "šoreiz", LAMBDA, 5, -1, -1, NULL},
                {032, "toreiz", LAMBDA, 5, -1, -1, NULL},
                {033, "pašreiz", LAMBDA, 6, -1, -1, NULL},
                {034, "nākošreiz", LAMBDA, 8, -1, -1, NULL},
                {035, "citreiz", LAMBDA, 6, -1, -1, NULL},
                {036, "citureiz", LAMBDA, 7, -1, -1, NULL},
                {037, "daudzreiz", LAMBDA, 8, -1, -1, NULL},
                {040, "uzreiz", LAMBDA, 5, -1, -1, NULL},
                {041, "dažreiz", LAMBDA, 6, -1, -1, NULL},
                {042, "drīz", LAMBDA, 3, -1, -1, NULL},
                {043, "gandrīz", LAMBDA, 6, -1, -1, NULL},
                {044, "allaž", LAMBDA, 4, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };


static RuleList step1a_rules[] =
           {
                {100, "ies", LAMBDA, 2, -1, 0, NULL},
                {101, "iem", LAMBDA, 2, -1, -1, NULL},
                {102, "ām", LAMBDA, 1, -1, -1, NULL},
                {103, "am", LAMBDA, 1, -1, -1, NULL},
                {104, "ēm", LAMBDA, 1, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step1a1_rules[] =
           {
                {100, "em", LAMBDA, 1, -1, -1, NULL},
                {101, "īm", LAMBDA, 1, -1, -1, NULL},
                {102, "im", LAMBDA, 1, -1, -1, NULL},
                {103, "um", LAMBDA, 1, -1, -1, NULL},
                {104, "us", LAMBDA, 1, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };


static RuleList step1a2_rules[] =
           {
                {100, "as", LAMBDA, 1, -1, -1, NULL},
                {101, "es", LAMBDA, 1, -1, -1, NULL},
                {102, "u", LAMBDA, 0, -1, -1, NULL},
                {103, "os", LAMBDA, 1, -1, -1, NULL},
                {104, "ai", LAMBDA, 1, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step1a3_rules[] =
           {
                {100, "t", LAMBDA, 0, -1, 0, NULL},
                {101, "u", LAMBDA, 0, -1, 0, NULL},
                {102, "ei", LAMBDA, 1, -1, -1, NULL},
                {103, "ij", LAMBDA, 1, -1, -1, NULL},
                {104, "is", LAMBDA, 1, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step1a4_rules[] =
           {
                {100, "ēs", LAMBDA, 1, -1, -1, NULL},
                {101, "is", LAMBDA, 1, -1, -1, NULL},
                {102, "ais", LAMBDA, 2, -1, -1, NULL},
                {103, "ie", LAMBDA, 1, -1, -1, NULL},
                {104, "s", LAMBDA, 0, -1, -1, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step1a5_rules[] =
           {
                {100, "š", LAMBDA, 0, -1, -1, NULL},
                {101, "a", LAMBDA, 0, -1, 0, NULL},
                {102, "i", LAMBDA, 0, -1, -1, NULL},
                {103, "e", LAMBDA, 0, -1, -1, NULL},
                {104, "ā", LAMBDA, 0, -1, 0, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step1a6_rules[] =
           {
                {100, "ē", LAMBDA, 0, -1, 0, NULL},
                {101, "ī", LAMBDA, 0, -1, 0, NULL},
                {102, "ū", LAMBDA, 0, -1, 0, NULL},
                {103, "o", LAMBDA, 0, -1, 0, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
           };

static RuleList step1b1_rules[] =
           {
                {108,  "pj",        "p",   1,  0, -1,  NULL},
                {109,  "bj",        "b",   1,  0, -1,  NULL},
                {110,  "mj",        "m",   1,  0, -1,  NULL},
                {111,  "vj",        "v",     1,  0, -1,  NULL},
                {112,  "šļ",        "sl",     1,  0, -1,  NULL},
                {113,  "žņ",        "zn",     1,  0, -1,  NULL},
                {114,  "dž",        "dz",     1,  0, 1,  NULL},
                {115,  "šņ",        "sn",     1,  0, 0,  NULL},
                {116,  "žļ",        "zl",     1,  0, -1,  NULL},
                {117,  "ļņ",        "ln",     1,  0, -1,  NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
            };

static RuleList step2_rules[] =
           {
                {203, "acionāl", "acion", 6, 3, 0, NULL},
                {204, "ācij", "āc", 3, 1, 0, NULL},
                {205, "ārij", "ār" ,3, 1, 0, NULL},
                {206, "iecīb", "iec", 4, 2, 0, NULL},
                {207, "ainīb", "ain", 4, 2, 0, NULL},
                {208, "ādīb", "ād", 3, 1, 0, NULL},
                {209, "ātīb", "āt", 3, 1, 0, NULL},
                {210, "dzīb", "dz", 3, 1, 0, NULL},
                {211, "āfij", "āf", 3, 1, 0, NULL},
                {212, "omij", "om", 3, 1, 0, NULL},
                {213, "oģij", "oģ", 3, 1, 0, NULL},
                {214, "orij", "or" ,3, 1, 0, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
            };

static RuleList step3_rules[] =
           {
                {301, "iek", LAMBDA, 2, -1, 1, NULL},
                {302, "iec", LAMBDA, 2, -1, 1, NULL},
                {303, "niek", LAMBDA, 3, -1, 0, NULL},
                {304, "niec", LAMBDA, 3, -1, 0, NULL},
                {305, "nīc", LAMBDA, 2, -1, 1, NULL},
                {306, "ain", LAMBDA, 2, -1, 0, NULL},
                {307, "ant", LAMBDA, 2, -1, 2, NULL},
                {308, "ier", LAMBDA, 2, -1, 1, NULL},
                {309, "iet", LAMBDA, 2, -1, 1, NULL},
                {310, "inā", LAMBDA, 2, -1, 1, NULL},
                {311, "ing", LAMBDA, 2, -1, 1, NULL},
                {312, "ism", LAMBDA, 2, -1, 1, NULL},
                {313, "isk", LAMBDA, 2, -1, 0, NULL},
                {314, "ist", LAMBDA, 2, -1, 1, NULL},
                {315, "šan", LAMBDA, 2, -1, 0, NULL},
                {316, "iem", LAMBDA, 2, -1, 1, NULL},
                {317, "ām", LAMBDA, 1, -1, 1, NULL},
                {318, "am", LAMBDA, 1, -1, 2, NULL},
                {319, "ēm", LAMBDA, 1, -1, 1, NULL},
                {320, "em", LAMBDA, 1, -1, 2, NULL},
                {321, "īm", LAMBDA, 1, -1, 2, NULL},
                {322, "im", LAMBDA, 1, -1, 2, NULL},
                {323, "um", LAMBDA, 1, -1, 2, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
            };


static RuleList step4_rules[] =
           {
                {401, "āb", LAMBDA, 1, -1, 2, NULL},
                {402, "ād", LAMBDA, 1, -1, 1, NULL},
                {403, "āj", LAMBDA, 1, -1, 1, NULL},
                {404, "āl", LAMBDA, 1, -1, 3, NULL},
                {405, "ān", LAMBDA, 1, -1, 1, NULL},
                {406, "ār", LAMBDA, 1, -1, 1, NULL},
                {407, "āt", LAMBDA, 1, -1, 1, NULL},
                {408, "āz", LAMBDA, 1, -1, 1, NULL},
                {409, "āž", LAMBDA, 1, -1, 1, NULL},
                {410, "al", LAMBDA, 1, -1, 2, NULL},
                {411, "av", LAMBDA, 1, -1, 1, NULL},
                {412, "ēj", LAMBDA, 1, -1, 2, NULL},
                {413, "ēk", LAMBDA, 1, -1, 1, NULL},
                {414, "ēt", LAMBDA, 1, -1, 1, NULL},
                {415, "ēz", LAMBDA, 1, -1, 1, NULL},
                {416, "ej", LAMBDA, 1, -1, 1, NULL},
                {417, "el", LAMBDA, 1, -1, 3, NULL},
                {418, "er", LAMBDA, 1, -1, 2, NULL},
                {419, "īb", LAMBDA, 1, -1, 1, NULL},
                {420, "īc", LAMBDA, 1, -1, 1, NULL},
                {421, "īd", LAMBDA, 1, -1, 3, NULL},
                {422, "īg", LAMBDA, 1, -1, 1, NULL},
                {423, "īj", LAMBDA, 1, -1, 1, NULL},
                {424, "īt", LAMBDA, 1, -1, 2, NULL},
                {425, "īv", LAMBDA, 1, -1, 2, NULL},
                {426, "īz", LAMBDA, 1, -1, 1, NULL},
                {427, "ij", LAMBDA, 1, -1, 1, NULL},
                {428, "il", LAMBDA, 1, -1, 2, NULL},
                {429, "ik", LAMBDA, 1, -1, 2, NULL},
                {430, "iņ", LAMBDA, 1, -1, 1, NULL},
                {431, "ol", LAMBDA, 1, -1, 2, NULL},
                {432, "oņ", LAMBDA, 1, -1, 0, NULL},
                {433, "on", LAMBDA, 1, -1, 1, NULL},
                {434, "or", LAMBDA, 1, -1, 1, NULL},
                {435, "ot", LAMBDA, 1, -1, 1, NULL},
                {436, "ul", LAMBDA, 1, -1, 2, NULL},
                {437, "īn", LAMBDA, 1, -1, 0, NULL},
                {438, "aj", LAMBDA, 1, -1, 0, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
            };

static RuleList step6_rules[] =
           {
                {601, "šun", "sun", 0,0,0, NULL},
                {000,  NULL,        NULL,    0,  0,  0,  NULL},
            };


static QString iflatv = QString("ĀāČčĒēĢģĪīĶķĻļŅņŠšŪūŽž");
static QString Vlatv = QString("āīēū");

/*****************************************************************************/
/********************   Private Function Declarations   **********************/

static int IsLatVowel(QChar ch)
{
    if (IsVowel(ch))
        return true;
    else
    {
        return Vlatv.contains(ch);
    }
}/*IsLatVowel*/

/*static int islatv(QChar ch)
{
    return iflatv.contains(ch);
}*/

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
            case 0: state = (IsLatVowel(c)) ? 1 : 2;
                   break;
            case 1: state = (IsLatVowel(c)) ? 1 : 2;
                   if ( 2 == state ) result++;
                   break;
            case 2: state = (IsLatVowel(c) || ('y' == c)) ? 1 : 2;
                   break;
        }
    }


    return( result );

} /* WordSize */







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

static int CompStopW( QString &word, RuleList* rule)
{
    while ( 0 != rule->id )
    {
        if(word == rule->old_end)
        {
            word = "";
            break;
        }
        rule++;
    }

    return rule->id;
} /* CompStopW */


static int ReplaceW( QString &word, RuleList* rule)
{
    while ( 0 != rule->id )
    {
        if(word == rule->old_end)
        {
            word = rule->new_end;
            break;
        }
        rule++;
    }

    return rule->id;
} /* ReplaceW */



LVPorterStemmer::LVPorterStemmer()
{

}

QString LVPorterStemmer::stem(QString word)
{
    //int rule;    /* which rule is fired in replacing an end */


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
    (void)CompStopW( word, step0a_rules);
    (void)CompStopW( word, step0b_rules);
    (void)CompStopW( word, step0c_rules);
    (void)CompStopW( word, step0d_rules);
    (void)CompStopW( word, step0e_rules);
    (void)CompStopW( word, step0f_rules);
    (void)CompStopW( word, step0g_rules);
    (void)CompStopW( word, step0h_rules);
    (void)CompStopW( word, step0i_rules);
    (void)CompStopW( word, step0j_rules);
    (void)CompStopW( word, step0k_rules);
    (void)CompStopW( word, step0l_rules);
    (void)CompStopW( word, step0m_rules);
    (void)CompStopW( word, step0n_rules);

    (void)ReplaceEnd( word, step1a_rules );
    (void)ReplaceEnd( word, step1a1_rules);
    (void)ReplaceEnd( word, step1a2_rules);
    (void)ReplaceEnd( word, step1a3_rules);
    (void)ReplaceEnd( word, step1a4_rules);
    (void)ReplaceEnd( word, step1a5_rules);
    (void)ReplaceEnd( word, step1a6_rules);

    (void)ReplaceEnd( word, step1b1_rules);
    (void)ReplaceEnd( word, step2_rules);
    (void)ReplaceEnd( word, step3_rules);

    (void)ReplaceEnd( word, step4_rules);

    (void)ReplaceW( word, step6_rules );


    return word;
}
