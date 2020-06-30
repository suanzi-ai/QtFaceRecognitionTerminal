#ifndef RECOGNIZETIPWIDGET_H
#define RECOGNIZETIPWIDGET_H

#include <QWidget>
#include <QImage>
#include "person.h"

class RecognizeTipWidget : public QWidget
{
    Q_OBJECT

public:
    RecognizeTipWidget(QWidget *parent = nullptr);
    ~RecognizeTipWidget() override;

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void rxResult(Person person);
    void hideSelf();

private:
    Person person_;

};

#endif
