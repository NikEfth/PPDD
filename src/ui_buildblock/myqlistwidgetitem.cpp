#include "myqlistwidgetitem.h"

void
myQListWidgetItem::set_mdiWindow_ptr(Screen_manager* new_screen_ptr)
{
    my_screen_ptr = new_screen_ptr;
    this->setText(my_screen_ptr->getFileName());
    this->setToolTip(my_screen_ptr->getFullFilePath());
}

Screen_manager*
myQListWidgetItem::get_mdiWindow_ptr() const
{
    return my_screen_ptr;
}
