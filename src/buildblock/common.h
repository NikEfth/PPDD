#ifndef COMMON_H
#define COMMON_H

#include <QStringList>
#include <QListWidgetItem>
#include <qwt_color_map.h>
#include <QMdiSubWindow>
#include <QCloseEvent>
#include <QRgb>
#include <QMap>

#include "stir/Array.h"
#include "stir/VoxelsOnCartesianGrid.h"
#include "stir/CartesianCoordinate3D.h"

#include <algorithm>
#include <functional>
#include <cassert>
#include <iomanip>
#include <numeric>

namespace Viewer {

//! The names of the views that are used by projection data or images
enum Orientation {SinogramView, ViewgramView, Transverse, Coronal, Sagittal, Next, None,
                  All};
//! \obsolete
class CommonToAll
{
public:
    CommonToAll()
    {
        viewOrders.insert(1, "Sinogram");
        viewOrders.insert(2, "Viewgram");
        viewOrders.insert(3, "Transverse");
        viewOrders.insert(4, "Coronal");
        viewOrders.insert(5, "Sagittal");
    }
    QMap<qint8, QString> viewOrders;
};

//! Simple template to handle positions. It is often used as cursor.
//! It handles tranformations between Oriantation (s).
template <class T>
class SimpleVector1D
{
public:
    SimpleVector1D(){}
    SimpleVector1D(const T& _x,
                   T _o = 0.0)
        :x(_x),ox(_o)
    {}
    ~SimpleVector1D()
    {}

    inline void set(const T& _x)
    {
        x = _x;
    }

    inline void setOrigin(const T& _o)
    {
        ox = _o;
    }

    inline T getEuDistance()
    {
        return std::abs<T>(x-ox);
    }

    //! Terminal Coordinate x
    T x;
    //! Origin Coordinate x
    T ox;
};

//! The 2D version of the SimpleVector1D.
template <class T>
class SimpleVector2D : public SimpleVector1D<T>
{
public:
    SimpleVector2D(){}
    SimpleVector2D(const T& _x, const T& _y,
                   T _ox = 0, T _oy = 0)
        :SimpleVector1D<T>(_x, _ox), y(_y), oy(_oy)
    {}
    ~SimpleVector2D()
    {}

    SimpleVector2D(const stir::CartesianCoordinate3D<int>& _c)
        :y(_c[2])
    {
        this->x = _c[3];
    }

    inline void set(const T& _x, const T& _y,
                    T _ox = 0, T _oy = 0)
    {
        this->x = _x;
        this->ox = _ox;
        y = _y;
        oy = _oy;
    }

    inline double getAzimuthalAngle()
    {
        return atan2(y-oy, this->x-this->ox);
    }

    inline QPoint toQPoint()
    {
        return QPoint(this->x, oy);
    }
    //! Terminal Coordinate y
    T y;
    ///! Origin Coordinate y
    T oy;
};

//! The 3D version of SimpleVector2D
template <class T>
class SimpleVector3D : public SimpleVector2D<T>
{
public:
    SimpleVector3D(){}
    SimpleVector3D(const T& _x, const T& _y, const T& _z,
                   T _ox = 0, T _oy = 0, T _oz = 0)
        : SimpleVector2D<T>(_x, _y, _ox, _oy), z(_z), oz(_oz)
    {}
    ~SimpleVector3D()
    {}

    inline void set(const T& _x, const T& _y, const T& _z,
                    T _ox = 0, T _oy = 0, T _oz = 0)
    {
        this->x = _x; this->y = _y; z = _z;
        this->ox = _ox; this->oy = _oy; oz = _oz;
    }

    //! Actually, this is more an asign function but can sort out
    //! the Orientation .
    inline void append(const QPoint& _p, const Orientation* _o)
    {
        if (*_o == Transverse)
        {
            this->x = _p.x();
            this->y = _p.y();
        }
        else if (*_o == Coronal)
        {
            this->z = _p.y();
            this->x = _p.x();
        }
        else
        {
            this->z = _p.y();
            this->y = _p.x();
        }
    }
    //! Returns a stir::CartesianCoordinate with the same values.
    stir::BasicCoordinate<3,T> getCartesianCoordinate()
    {
        return stir::make_coordinate<T>(z, this->y, this->x);
    }

    //! Actually, this is more an asign function but can sort out
    //! the Orientation .
    inline void append(const SimpleVector2D<T>& _p, const Orientation* _o)
    {
        if (*_o == Transverse)
        {
            this->x = _p.x;
            this->y = _p.y;
        }
        else if (*_o == Coronal)
        {
            this->z = _p.y;
            this->x = _p.x;
        }
        else
        {
            this->z = _p.y;
            this->y = _p.x;
        }
    }

    //! Reduces the number of dimensions by one w.r.t the Orientation.
    inline SimpleVector2D<T> getCross(Orientation _o)
    {
        if (_o == Transverse)
        {
            return SimpleVector2D<T>(this->x,
                                     this->y);
        }
        else if (_o == Coronal)
        {
            return SimpleVector2D<T>(this->x,z);
        }
        else
        {
            return SimpleVector2D<T>(this->y,z);
        }
    }

    //! Reduces the number of dimensions by two w.r.t the Orientation.
    inline SimpleVector1D<T> getPosition(Orientation _o)
    {
        if (_o == Transverse)
        {
            return SimpleVector1D<T>(z);
        }
        else if (_o == Coronal)
        {
            return SimpleVector1D<T>(this->y);
        }
        else
        {
            return SimpleVector1D<T>(this->x);
        }
    }

    inline double getPolarAngle()
    {

    }

    //! Terminal Coordinate z
    T z;
    //! Origin Coordinate z
    T oz;
};

class Int2Int
{
public:
    Int2Int() {}
    Int2Int(const int& _x, const int& _y) :
        x(_x), y(_y) {}
    int x;
    int y;
};

template <typename T>
class B2B
{
public:
    B2B() {
        status = false;
        x = 0;
        y = 0;
    }
    B2B(const T& _x, const T& _y) :
        x(_x), y(_y) {
        status = true;
    }
    T x;
    T y;

    bool status;
};

class BSerious
{
public:
    BSerious(){}

    BSerious(const int& _x, const int& _y) :
        coords(_x, _y)
    {}

    double meanValue() const
    {
        double sum = 0.0;
        for (double i : data)
            sum += i;
        return static_cast<double> (sum)/ data.size();
    }

    QVector<double> data;
    Int2Int coords;

    void append(const double &t)
    {
        data.append(t);
    }

    bool operator==(const BSerious& b)
    {
        if (   this->coords.x == b.coords.x &&
               this->coords.y == b.coords.y)
            return true;

        return false;
    }

    bool operator==(const Int2Int& b)
    {
        if (   this->coords.x == b.x &&
               this->coords.y == b.y)
            return true;

        return false;
    }

    BSerious operator=(const BSerious& b)
    {
        this->data = b.data;
        this->coords.x = b.coords.x;
        this->coords.y = b.coords.y;
    }
};

//https://tuket.github.io/2017-09-30-sorting-multiple-vectors-in-the-same-way-using-c-variadic-templates/

template <typename T>
void applyPermutation(
    const QVector<unsigned>& order,
    QVector<T>& t)
{
    assert(order.size() == t.size());
    QVector<T> st(t.size());
    for(unsigned i=0; i<t.size(); i++)
    {
        st[i] = t[order[i]];
    }
    t = st;
}

template <typename T, typename... S>
void applyPermutation(
    const QVector<unsigned>& order,
    QVector<T>& t,
    QVector<S>&... s)
{
    applyPermutation(order, t);
    applyPermutation(order, s...);
}

template <typename T, typename Compare>
void getSortPermutation(
   QVector<unsigned>& out,
    const QVector<T>& v,
    Compare compare = std::less<T>())
{
    out.resize(v.size());
    std::iota(out.begin(), out.end(), 0);

    std::sort(out.begin(), out.end(),
        [&](unsigned i, unsigned j){ return compare(v[i], v[j]); });
}

template<typename T, typename Compare, typename... SS>
void sortVectors(
    const QVector<T>& t,
    Compare comp,
    QVector<SS>... ss)
{
    QVector<unsigned> order;
    getSortPermutation(order, t, comp);
    applyPermutation(order, ss...);
}

// make less verbose for the usual ascending order
template<typename T, typename... SS>
void sortVectorsAscending(
    const QVector<T>& t,
    QVector<SS>&... ss)
{
    sortVectors(t, std::less<T>(), ss...);
}

//! Colormaps. Defaults at BW grayscale.
class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap():
        QwtLinearColorMap()
    {
        set_BW();
        Nstops = 256;
        my_list <<"BW" <<"WB" <<"Viridis"<<"QWT"<<"Jet";
    }

    ~ColorMap()
    { }

    //! Get the list with all the available options
    static QStringList getColormapList()
    {
        QStringList ret;
        ret<<"BW"<<"WB"<<"Viridis"<<"QWT"<<"Jet";
        return ret;
    }

    //! Set the active ColorMap by name
    void setColormap(QString name)
    {
        switch (my_list.indexOf(name)) {
        case 0:
            this->set_BW();
            break;
        case 1:
            this->set_WB();
            break;
        case 2:
            this->set_Viridis();
            break;
        case 3:
            this->set_qwt();
            break;
        case 4:
            this->set_JET();
            break;
        default:
            this->set_BW();
            break;
        }
    }

    //! Set the active ColorMap by index
    void setColormap(int index)
    {
        switch (index) {
        case 0:
            this->set_BW();
            break;
        case 1:
            this->set_WB();
            break;
        case 2:
            this->set_Viridis();
            break;
        case 3:
            this->set_qwt();
            break;
        case 4:
            this->set_JET();
            break;
        default:
            this->set_BW();
            break;
        }
    }

    //! Set Black - White grayscale
    inline void set_BW()
    {
        background = Qt::black;
        peak = Qt::yellow;
        setColorInterval(Qt::black, Qt::white);
    }

    //! Set White - Black grayscale
    inline void set_WB()
    {
        background = Qt::white;
        peak = Qt::black;
        setColorInterval(Qt::white, Qt::black);
    }

    //! Set Jet, popular by old Matlab
    inline void set_JET()
    {
        background = Qt::blue;
        setColorInterval(Qt::blue, Qt::red);
        peak = Qt::white;
        addColorStop( 0.35, Qt::cyan );
        addColorStop( 0.5, Qt::green );
        addColorStop( 0.75, Qt::yellow );
    }

    //! Set QWT default ColorMap
    inline void set_qwt()
    {
        background = Qt::darkCyan;
        setColorInterval(Qt::darkCyan, Qt::red);
        peak = Qt::yellow;
        addColorStop( 0.1, Qt::cyan );
        addColorStop( 0.6, Qt::green );
        addColorStop( 0.95, Qt::yellow );
    }

    //! Set Viridis ColorMap
    inline void set_Viridis()
    {
        double step = 1.0 / Nstops;
        double cur = 0.0;

        for (int i = 0; i < Nstops; i++, cur+=step)
        {
            _data[i] = QColor(_viridis_data[i][0],
                    _viridis_data[i][1],
                    _viridis_data[i][2]);

            addColorStop(cur,_data[i]);
        }

        background = _data[0];
        peak = _data[Nstops-1];
        setColorInterval(_data[0], _data[Nstops-1]);
    }

    //! Returns the first Color of the ColorMap
    inline QColor get_background() const
    { return background; }

    //! Returns the last Color of the ColorMap
    inline QColor get_peak_color() const
    { return peak; }

private:
    //! Number of color stops \todo Adjustable
    int Nstops;
    QColor background, peak;

    QColor _data[256];

    QStringList my_list;
    //! Vidiris array. I could find the functions.
    quint8 _viridis_data[256][3] = {
        68, 1,  84,
        68, 2,  85,
        68, 3,  87,
        69, 5,  88,
        69, 6,  90,
        69, 8,  91,
        70, 9,  92,
        70, 11, 94,
        70, 12, 95,
        70, 14, 97,
        71, 15, 98,
        71, 17, 99,
        71, 18, 101,
        71, 20, 102,
        71, 21, 103,
        71, 22, 105,
        71, 24, 106,
        72, 25, 107,
        72, 26, 108,
        72, 28, 110,
        72, 29, 111,
        72, 30, 112,
        72, 32, 113,
        72, 33, 114,
        72, 34, 115,
        72, 35, 116,
        71, 37, 117,
        71, 38, 118,
        71, 39, 119,
        71, 40, 120,
        71, 42, 121,
        71, 43, 122,
        71, 44, 123,
        70, 45, 124,
        70, 47, 124,
        70, 48, 125,
        70, 49, 126,
        69, 50, 127,
        69, 52, 127,
        69, 53, 128,
        69, 54, 129,
        68, 55, 129,
        68, 57, 130,
        67, 58, 131,
        67, 59, 131,
        67, 60, 132,
        66, 61, 132,
        66, 62, 133,
        66, 64, 133,
        65, 65, 134,
        65, 66, 134,
        64, 67, 135,
        64, 68, 135,
        63, 69, 135,
        63, 71, 136,
        62, 72, 136,
        62, 73, 137,
        61, 74, 137,
        61, 75, 137,
        61, 76, 137,
        60, 77, 138,
        60, 78, 138,
        59, 80, 138,
        59, 81, 138,
        58, 82, 139,
        58, 83, 139,
        57, 84, 139,
        57, 85, 139,
        56, 86, 139,
        56, 87, 140,
        55, 88, 140,
        55, 89, 140,
        54, 90, 140,
        54, 91, 140,
        53, 92, 140,
        53, 93, 140,
        52, 94, 141,
        52, 95, 141,
        51, 96, 141,
        51, 97, 141,
        50, 98, 141,
        50, 99, 141,
        49, 100,141,
        49, 101,141,
        49, 102,141,
        48, 103,141,
        48, 104,141,
        47, 105,141,
        47, 106,141,
        46, 107,142,
        46, 108,142,
        46, 109,142,
        45, 110,142,
        45, 111,142,
        44, 112,142,
        44, 113,142,
        44, 114,142,
        43, 115,142,
        43, 116,142,
        42, 117,142,
        42, 118,142,
        42, 119,142,
        41, 120,142,
        41, 121,142,
        40, 122,142,
        40, 122,142,
        40, 123,142,
        39, 124,142,
        39, 125,142,
        39, 126,142,
        38, 127,142,
        38, 128,142,
        38, 129,142,
        37, 130,142,
        37, 131,141,
        36, 132,141,
        36, 133,141,
        36, 134,141,
        35, 135,141,
        35, 136,141,
        35, 137,141,
        34, 137,141,
        34, 138,141,
        34, 139,141,
        33, 140,141,
        33, 141,140,
        33, 142,140,
        32, 143,140,
        32, 144,140,
        32, 145,140,
        31, 146,140,
        31, 147,139,
        31, 148,139,
        31, 149,139,
        31, 150,139,
        30, 151,138,
        30, 152,138,
        30, 153,138,
        30, 153,138,
        30, 154,137,
        30, 155,137,
        30, 156,137,
        30, 157,136,
        30, 158,136,
        30, 159,136,
        30, 160,135,
        31, 161,135,
        31, 162,134,
        31, 163,134,
        32, 164,133,
        32, 165,133,
        33, 166,133,
        33, 167,132,
        34, 167,132,
        35, 168,131,
        35, 169,130,
        36, 170,130,
        37, 171,129,
        38, 172,129,
        39, 173,128,
        40, 174,127,
        41, 175,127,
        42, 176,126,
        43, 177,125,
        44, 177,125,
        46, 178,124,
        47, 179,123,
        48, 180,122,
        50, 181,122,
        51, 182,121,
        53, 183,120,
        54, 184,119,
        56, 185,118,
        57, 185,118,
        59, 186,117,
        61, 187,116,
        62, 188,115,
        64, 189,114,
        66, 190,113,
        68, 190,112,
        69, 191,111,
        71, 192,110,
        73, 193,109,
        75, 194,108,
        77, 194,107,
        79, 195,105,
        81, 196,104,
        83, 197,103,
        85, 198,102,
        87, 198,101,
        89, 199,100,
        91, 200,98,
        94, 201,97,
        96, 201,96,
        98, 202,95,
        100,203,93,
        103,204,92,
        105,204,91,
        107,205,89,
        109,206,88,
        112,206,86,
        114,207,85,
        116,208,84,
        119,208,82,
        121,209,81,
        124,210,79,
        126,210,78,
        129,211,76,
        131,211,75,
        134,212,73,
        136,213,71,
        139,213,70,
        141,214,68,
        144,214,67,
        146,215,65,
        149,215,63,
        151,216,62,
        154,216,60,
        157,217,58,
        159,217,56,
        162,218,55,
        165,218,53,
        167,219,51,
        170,219,50,
        173,220,48,
        175,220,46,
        178,221,44,
        181,221,43,
        183,221,41,
        186,222,39,
        189,222,38,
        191,223,36,
        194,223,34,
        197,223,33,
        199,224,31,
        202,224,30,
        205,224,29,
        207,225,28,
        210,225,27,
        212,225,26,
        215,226,25,
        218,226,24,
        220,226,24,
        223,227,24,
        225,227,24,
        228,227,24,
        231,228,25,
        233,228,25,
        236,228,26,
        238,229,27,
        241,229,28,
        243,229,30,
        246,230,31,
        248,230,33,
        250,230,34,
        253,231,36};
};


class Mapper
{
public:
    static QVector<float> from(const stir::Array<2, float>& input)
    {
        QVector<float> ret;
        return ret;
    }

    template <int num_dimensions>
    static stir::Array<num_dimensions, float> from(const QVector<float>& input,
                                                   const stir::IndexRange<num_dimensions>& ranges)
    {
        stir::Array<num_dimensions, float> ret(ranges);

        typename stir::Array<num_dimensions,float>::full_iterator it = ret.begin_all();

        for (float i : input)
        {
            *it = i;
            ++it;
        }

        return ret;
    }

    static QVector<float> from(const stir::VoxelsOnCartesianGrid<float>& input)
    {
        QVector<float> ret(input.get_x_size()*input.get_y_size()*input.get_z_size());
        stir::CartesianCoordinate3D<int> coord = stir::make_coordinate(0,0,0);
        int i = 0;

        for(coord[1]=input.get_min_z() ;coord[1]<= input.get_max_z();++coord[1])
            for(coord[2]=input.get_min_y() ;coord[2]<=input.get_max_y();++coord[2])
                for(coord[3]=input.get_min_x();coord[3]<=input.get_max_x();++coord[3])
                {
                    ret[i] = input[coord];
                    i++;
                }

        return ret;
    }

    static stir::VoxelsOnCartesianGrid<float> from(const QVector<float>& input,
                                                   const stir::CartesianCoordinate3D<float>& origin,
                                                   const stir::BasicCoordinate<3,float>& grid_spacing)
    {
        stir::VoxelsOnCartesianGrid<float> ret;

        return ret;
    }
private:
    Mapper(){}
};

//template stir::Array<1, float> from(const QVector<float>&);
}
#endif // COMMON_H
