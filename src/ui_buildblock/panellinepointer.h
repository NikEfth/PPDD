#ifndef PANELLINEPOINTER_H
#define PANELLINEPOINTER_H

#include <QWidget>

namespace Ui {
class PanelLinePointer;
}

class PanelLinePointer : public QWidget
{
    Q_OBJECT

public:
    explicit PanelLinePointer(QWidget *parent = 0);
    ~PanelLinePointer();
    //! Return the number of rays to be used.
    int getNumRays() const;

    bool getFidelity() const;

    bool getAntialiazing() const;

signals:

    void selectionType(const bool&);

    void new_options();

private slots:
    void updateOptions();

private:
    Ui::PanelLinePointer *ui;

    int numRays;
};

#endif // PANELLINEPOINTER_H
