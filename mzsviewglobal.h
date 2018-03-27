#ifndef MZSVIEWGLOBAL_H
#define MZSVIEWGLOBAL_H
#include <QStringList>

namespace mzsview {
    const static int TABLE_HEIGHT=200;
    const static int PADDING=10;
    const static int ROWS=8;
    const static int COLUMNS=8;
    const static double ERR=1E-8;
    const static int COLUMN_WIDTH[COLUMNS]={30,15,15,6,6,6,9,9};
    const static char* HEADS[COLUMNS]={"Tag","Cursor1 Value","Cursor2 Value","Select","Phase","Color","Min","Max"};
    const static char* DEFAULT_COLORS[COLUMNS]={"#ff0000","#0000ff","#00ff00","#000000",
                                                "#ff00ff","#aa00ff","#555500","#ffff00"};
    const static double DEFAULT_MINS[ROWS]={0,0,0,0,0,0,0,0};
    const static double DEFAULT_MAXS[ROWS]={12000,10000,2000,30,200,300,30,500};
    const static int DEFAULT_PHASE_STATE=0;//Qt::Unchecked
    const static int DEFAULT_SELECT_STATE=2;//Qt::Checked
    const static int TAG_COLUMN=0;
    const static int SELECT_COLUMN=3;
    const static int PHASE_COLUMN=4;
    const static int COLOR_COLUMN=5;
    const static int MIN_COLUMN=6;
    const static int MAX_COLUMN=7;
    const static int CURSOR1_COLUMN=1;
    const static int CURSOR2_COLUMN=2;
    const static char* CHECK_TEXTS[3]={"","",""};//"off","","on"};
    const static char DATETIME_FORMAT[]="yyyy/MM/dd hh:mm:ss";
    const static char AXISX_FORMAT[]="hh:mm";
    const static char AXISY_FORMAT[]="%i";
    const static char CURSOR_FORMAT[]="hh:mm:ss";
    const static char AXISX_TITLE[]="time";
    const static char AXISY_TITLE[]="data %";
    const static char DATA_DELIMITER[]=",";
}

#endif // MZSVIEWGLOBAL_H
