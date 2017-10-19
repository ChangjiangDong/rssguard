// For license of this file, see <object-root-folder>/LICENSE.md.

#ifndef COMBOBOXWITHSTATUS_H
#define COMBOBOXWITHSTATUS_H

#include "gui/widgetwithstatus.h"

#include <QComboBox>

class ComboBoxWithStatus : public WidgetWithStatus {
  Q_OBJECT

  public:

    // Constructors and destructors.
    explicit ComboBoxWithStatus(QWidget* parent = 0);
    virtual ~ComboBoxWithStatus();

    inline QComboBox* comboBox() const {
      return static_cast<QComboBox*>(m_wdgInput);
    }

};

#endif // COMBOBOXWITHSTATUS_H
