#ifndef DIGIT_KEY_WIDGET_H
#define DIGIT_KEY_WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

namespace suanzi {

class DigitKeyWidget : public QWidget {
  Q_OBJECT

 public:
  DigitKeyWidget(int width, int height, QWidget *parent = nullptr);
  ~DigitKeyWidget() override;

 private:
 	void setCommonAttribute(QPushButton *ppb);

 signals:
 	void switch_stacked_widget(int index);

 private slots:
 	void clicked_back();
 	void clicked_0_key();
	void clicked_1_key();
	void clicked_2_key();
	void clicked_3_key();
	void clicked_4_key();
	void clicked_5_key();
	void clicked_6_key();
	void clicked_7_key();
	void clicked_8_key();
	void clicked_9_key();
	void clicked_well_key();
	void clicked_backspace_key();
	void clicked_doorbell_key();
	void clicked_intercom_key();
	void clicked_video_key();
	void clicked_ok_key();
	void rx_menu_type(int menu_type);

 private:
 	int menu_type_;
	QLineEdit *ple_input_tip_;
  	QPushButton *ppb_doorbell_;
	QPushButton *ppb_intercom_;
	QPushButton *ppb_video_;
	QPushButton *ppb_ok_;
	int screen_width_;
	int screen_height_;
};

}  // namespace suanzi

#endif
