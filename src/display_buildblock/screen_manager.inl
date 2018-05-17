#include <QFileInfo>

inline QString Screen_manager::getFullFilePath() const
{
    return myFullFileName;
}

inline QString Screen_manager::getFileName() const
{
    QFileInfo fi(myFullFileName);
    return fi.fileName();
}

inline const QString* Screen_manager::getFullFilePath_ptr() const
{
    return &myFullFileName;
}

//inline QString* Screen_manager::getFileName_ptr() const
//{
//    QFileInfo fi(myFullFileName);
//    return fi.fileName();
//}

inline void Screen_manager::setFullFilePath(const QString& _s)
{
    myFullFileName = _s;
    setWindowTitle(getFileName());
}

inline qint16 Screen_manager::getColorMapIndex() const
{
    return activeScreenContrainer->getColorMapIndex();
}

inline qint16 Screen_manager::getViewportsIndex() const
{
// /   return num_viewports;
}

inline void Screen_manager::setGrouped(bool g)
{
    is_grouped = g;
}

inline bool Screen_manager::isGrouped() const
{
    return is_grouped;
}

inline const Orientation* Screen_manager::getDisplayLayout() const
{
    return &viewOrder;
}

inline SimpleVector3D<int>*
Screen_manager::getCurrentCursor() const
{
    return currentCursor;
}

inline bool
Screen_manager::getAxisStatus() const
{
    return axisStatus;
}

inline CartesianCoordinate3D<int>
Screen_manager::getCurrentCursorCartesianCoordinate()
{
    return currentCursor->getCartesianCoordinate();
}

inline bool Screen_manager::getCursorStatus() const
{
    return cursorStatus;
}

double Screen_manager::get_min_value_all() const
{
    return minValueAll;
}

double Screen_manager::get_max_value_all() const
{
    return maxValueAll;
}
