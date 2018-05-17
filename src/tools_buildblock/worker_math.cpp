#include "worker_math.h"

//! \todo Not hardcoded.
#include "../exprtk/exprtk.hpp"

#include <stir/BasicCoordinate.h>

Worker_Math::Worker_Math(QObject *parent) :
    Worker(parent)
{

}

Worker_Math::~Worker_Math()
{}


Worker_Math::Worker_Math(QString* _expr,
                                 std::shared_ptr<stir::VoxelsOnCartesianGrid<float> > _A,
                                 std::shared_ptr<stir::VoxelsOnCartesianGrid<float> > _B,
                                 std::shared_ptr<stir::VoxelsOnCartesianGrid<float> > _C,
                         QObject *parent) :
    Worker(parent), A(_A), B(_B), C(_C)
{
expr = _expr;
}

void Worker_Math::process()
{
    typedef exprtk::symbol_table<float> symbol_table_t;
    typedef exprtk::expression<float>     expression_t;
    typedef exprtk::parser<float>             parser_t;

    float __A, __B;

    symbol_table_t symbol_table;
    symbol_table.add_variable("A", __A);
    symbol_table.add_variable("B", __B);
    //symbol_table.add_constants();


    expression_t expression;
    expression.register_symbol_table(symbol_table);

    parser_t parser;
    parser.compile(expr->toStdString() ,expression);

    stir::BasicCoordinate<3,int> coord = stir::make_coordinate(0, 0, 0);

    for(coord[1]= (*A).get_min_index();
        coord[1]<= (*A).get_max_index();++coord[1])
        for(coord[2]= (*A)[0].get_min_index();
            coord[2]<= (*A)[0].get_max_index();++coord[2])
            for(coord[3]= (*A)[0][0].get_min_index();
                coord[3]<= (*A)[0][0].get_max_index();++coord[3])
            {
                __A = (*A)[coord];
                __B = (*B)[coord];
                float y = expression.value();
                if ( y != y)
                    (*C)[coord] = 0.0;
                else
                    (*C)[coord] = y;
                //printf("%19.15f\t%19.15f\t%19.15f\n",_C[coord],A,B);
            }
}
