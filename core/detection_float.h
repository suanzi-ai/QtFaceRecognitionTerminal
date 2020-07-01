#ifndef DETECTION_FLOAT_H
#define DETECTION_FLOAT_H

#include <QMetaType>

struct DetectionFloat {

    float x;
    float y;
    float width;
    float height;
    float landmark[5][2];
};

Q_DECLARE_METATYPE(DetectionFloat);

#endif
