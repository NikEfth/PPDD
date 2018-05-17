#ifndef MYQLISTWIDGETITEM_H
#define MYQLISTWIDGETITEM_H

#include <QListWidgetItem>

#include "src/display_buildblock/screen_manager.h"

class myQListWidgetItem : public QListWidgetItem
{
public:

    void set_mdiWindow_ptr(Screen_manager* new_screen);

    Screen_manager* get_mdiWindow_ptr() const;

private:
    Screen_manager* my_screen_ptr;
};

#endif // MYQLISTWIDGETITEM_H
