const double
display_screen_raster::get_max_value_ptr() const
{ return d_spectrogram->interval(Qt::XAxis).maxValue();}

const double
display_screen_raster::get_min_value_ptr() const
{ return  d_spectrogram->interval(Qt::XAxis).minValue();}

const double*
display_screen::get_max_vis_value_ptr() const
{ return &this->vis_max;}

const double*
display_screen::get_min_vis_value_ptr() const
{ return  &this->vis_min;}

void
display_screen::setAddressAtParent(const qint32& id)
{
    myAddress = id;
}

void
display_screen::set_active(bool state)
{
    this->drawSelectionFrame(state);
}

void
display_screen::drawSelectionFrame(bool state)
{
    if(state)
        this->setFrameStyle(QFrame::Box);
    else
        this->setFrameStyle(QFrame::Plain);
    this->replot();
}


void display_screen_raster::set_max_value(const double& new_max)
{
    max_value = new_max;
}

void display_screen_raster::set_min_value(const double& new_min)
{
    min_value = new_min;
}

void display_screen_raster::set_min_max_values(const double& new_min, const double& new_max)
{
    min_value = new_min;
    max_value = new_max;
}

QVector<double>::iterator
display_screen_raster::get_data_begin()
{
    return  my_data.begin();
}

QVector<double>::iterator
display_screen_raster::get_data_end()
{
    return  my_data.end();
}

QVector<double>::ConstIterator
display_screen_raster::get_data_const_begin() const
{
    return  my_data.constBegin();
}

QVector<double>::ConstIterator
display_screen_raster::get_data_const_end() const
{
    return  my_data.end();
}
