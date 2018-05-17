#ifndef PANELRECTPOINTER_H
#define PANELRECTPOINTER_H

#include <QWidget>

namespace Ui {
class PanelRectPointer;
}

class PanelRectPointer : public QWidget
{
    Q_OBJECT

public:
    explicit PanelRectPointer(QWidget *parent = 0);
    ~PanelRectPointer();

    bool selectSquare() const;

    bool selectRect() const;

signals:
    void selectionType(const bool&);

private slots:
    void on_actionselectionTypeChanged_triggered();

private:
    Ui::PanelRectPointer *ui;

    bool square;
};

#endif // PANELRECTPOINTER_H
