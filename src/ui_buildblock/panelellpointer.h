#ifndef PANELELLPOINTER_H
#define PANELELLPOINTER_H

#include <QWidget>

namespace Ui {
class PanelEllPointer;
}

class PanelEllPointer : public QWidget
{
    Q_OBJECT

public:
    explicit PanelEllPointer(QWidget *parent = 0);
    ~PanelEllPointer();

    bool selectEllipse() const;

    bool selectCircle() const;

signals:
    void selectionType(const bool&);

private slots:

    void on_actionselectionTypeChanged_triggered();

private:
    Ui::PanelEllPointer *ui;

    bool concentric;
};

#endif // PANELELLPOINTER_H
