#include "detect_tip_widget.hpp"

#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

#include "config.hpp"

using namespace suanzi;

DetectTipWidget::DetectTipWidget(int win_x, int win_y, int win_width,
                                 int win_height, QWidget *parent)
    : win_x_(win_x),
      win_y_(win_y),
      win_width_(win_width),
      win_height_(win_height),
      lost_age_(0),
      is_valid_(false),
      valid_count_(0),
      QWidget(parent) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);
}

DetectTipWidget::~DetectTipWidget() {}

void DetectTipWidget::paint(QPainter *painter) {
  if (rects_.size() > 0) {
    float sum_x = 0, sum_y = 0, sum_width = 0, sum_height = 0;
    float count = 0;

    auto it = rects_.end();
    QRect latest = rects_.back();
    do {
      it -= 1;

      QRect inter_section = latest.intersected(*it);
      QRect united_section = latest.united(*it);

      float iou = 1.f * inter_section.width() * inter_section.height() /
                  united_section.width() / united_section.height();

      float weight = pow(iou, 6);

      sum_x += weight * it->x();
      sum_y += weight * it->y();
      sum_width += weight * it->width();
      sum_height += weight * it->height();
      count += weight;

    } while (it != rects_.begin());

    float width = sum_width / count;
    float height = sum_height / count;
    float top_x = sum_x / count;
    float top_y = sum_y / count;
    float bottom_x = top_x + width;
    float bottom_y = top_y + height;
    // SZ_LOG_DEBUG("top_x={},top_y={},bottom_x={},bottom_y={},w={},h={}",
    // top_x,
    //              top_y, bottom_x, bottom_y, width, height);

    if (is_valid_)
      painter->setPen(QPen(QColor(0, 0, 255, 128), 5));
    else
      painter->setPen(QPen(QColor(255, 0, 0, 128), 5));
    painter->drawLine(top_x, top_y, top_x + width / 5, top_y);
    painter->drawLine(top_x, top_y, top_x, top_y + height / 5);
    painter->drawLine(top_x, bottom_y, top_x + width / 5, bottom_y);
    painter->drawLine(top_x, bottom_y, top_x, bottom_y - height / 5);
    painter->drawLine(bottom_x, top_y, bottom_x, top_y + height / 5);
    painter->drawLine(bottom_x, top_y, bottom_x - width / 5, top_y);
    painter->drawLine(bottom_x, bottom_y, bottom_x, bottom_y - height / 5);
    painter->drawLine(bottom_x, bottom_y, bottom_x - width / 5, bottom_y);

#ifdef DEBUG
    if (pose_.length() > 0) {
      painter->setPen(Qt::white);
      painter->drawPoints(landmarks_);
      painter->drawText(top_x, top_y - 20, pose_);
    }
#endif
  }
}

void DetectTipWidget::rx_display(DetectionRatio detection, bool to_clear,
                                 bool is_bgr) {
  int box_x = win_x_ + 1;
  int box_y = win_y_ + 1;
  int box_w = win_width_ - 1;
  int box_h = win_height_ - 1;

  if (!to_clear) {
    // display pose for debug
    landmarks_.clear();
    if (is_bgr) {
      for (int i = 0; i < SZ_LANDMARK_NUM; i++)
        landmarks_.push_back({(int)(detection.landmark[i][0] * box_w),
                              (int)(detection.landmark[i][1] * box_h)});

      pose_ = "Yaw: " + QString::number(detection.yaw, 'f', 1) +
              " Pitch: " + QString::number(detection.pitch, 'f', 1) +
              " Roll: " + QString::number(detection.roll, 'f', 1);
    } else
      pose_ = "";

    // resize to square
    float center_x = box_x + (detection.x + .5f * detection.width) * box_w;
    float center_Y = box_y + (detection.y + .5f * detection.height) * box_h;
    float size = .5f * (detection.width * box_w + detection.height * box_h);
    // SZ_LOG_DEBUG("x={},y={},w={},h={},c_x={},c_y={}", box_x, box_y, box_w,
    //              box_h, center_x, center_Y);

    QRect next_rect(center_x - .5f * size, center_Y - .5f * size, size, size);
    rects_.push_back(next_rect);
    if (rects_.size() > MAX_RECT_COUNT) rects_.erase(rects_.begin());

    if (is_bgr) is_valid_ = decide_valid(detection);

  } else {
    if (++lost_age_ > MAX_LOST_AGE) {
      rects_.clear();
      lost_age_ = 0;
      is_valid_ = false;
      valid_count_ = 0;
    }
  }

  QWidget *p = (QWidget *)parent();
  p->update();
}

// todo: merge decide_valid with detection_data bgr_valid
bool DetectTipWidget::decide_valid(DetectionRatio detection) {
  bool ret = false;
  if (detection.is_valid()) {
    static float x1 = 0;
    static float y1 = 0;
    static float w1 = 0;
    static float h1 = 0;

    static int stable_counter = 0;

    auto cfg = Config::get_detect();

    float x2 = detection.x, y2 = detection.y;
    float w2 = detection.width, h2 = detection.height;

    if (x1 > x2 + w2 || y1 > y2 + h2 || x1 + w1 < x2 || y1 + h1 < y2)
      ret = false;
    else {
      float overlay_w = std::min(x1 + w1, x2 + w2) - std::max(x1, x2);
      float overlay_h = std::min(y1 + h1, y2 + h2) - std::max(y1, y2);
      float iou = overlay_w * overlay_h / (w1 * h1 + w2 * h2) * 2;

      ret = iou >= cfg.min_tracking_iou;
    }

    x1 = x2;
    y1 = y2;
    w1 = w2;
    h1 = h2;

    if (ret)
      stable_counter++;
    else
      stable_counter = 0;

    ret = stable_counter >= cfg.min_tracking_number;
  }

  if (ret) {
    valid_count_ = 10;
    return true;
  } else
    return --valid_count_ > 0;
}
