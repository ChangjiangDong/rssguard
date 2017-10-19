// For license of this file, see <object-root-folder>/LICENSE.md.

#include "gui/squeezelabel.h"

SqueezeLabel::SqueezeLabel(QWidget* parent) : QLabel(parent) {}

void SqueezeLabel::paintEvent(QPaintEvent* event) {
  if (m_squeezedTextCache != text()) {
    m_squeezedTextCache = text();
    QFontMetrics fm = fontMetrics();

    if (fm.width(m_squeezedTextCache) > contentsRect().width()) {
      setText(fm.elidedText(text(), Qt::ElideMiddle, width()));
    }
  }

  QLabel::paintEvent(event);
}
