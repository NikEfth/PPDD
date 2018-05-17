#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QCheckBox>

namespace Ui {
class Dialog;
}


//!
//! \brief The Dialog class
//! This a dialog about the view orientation. But in the future it might
//! become a more generic dialog box.
//!
class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QString space, QWidget *parent = 0);
    ~Dialog();

    int ret;
    QCheckBox * chkBox;

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::Dialog *ui;
    QDialogButtonBox *buttonBox;

    QColor my_color;

    QPushButton *btn_sino, *btn_view, *btn_sag, *btn_coron, *btn_trans;

    QLineEdit *edit;
};

#endif 
