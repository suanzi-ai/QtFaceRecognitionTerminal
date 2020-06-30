#ifndef DETECTTIPWIDGET_H
#define DETECTTIPWIDGET_H

#include <QWidget>
#include <QImage>

class DetectTipWidget : public QWidget
{
    Q_OBJECT

public:
    DetectTipWidget(QWidget *parent = nullptr);
    ~DetectTipWidget() override;

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void rxResult(QRect &rect);
    void hideSelf();

private:
    QRect rect_;

};

#endif

