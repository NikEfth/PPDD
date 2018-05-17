#include "src/tools_buildblock/worker_splitter.h"
#include "stir/PixelsOnCartesianGrid.h"


void Worker_Splitter::process()
{
    int num_poss = data->get_z_size();
    const IndexRange<3> range = data->get_index_range();

    int num_subsets = ceil(static_cast<float>(num_poss) / split_slice);

    int maxZ = 0;
    for (int i = 0; i < num_subsets; i++)
    {
        int minZ = maxZ;
        maxZ = minZ+split_slice;

        if(maxZ >= num_poss)
            maxZ = num_poss;

        BasicCoordinate<3,int> new_min = make_coordinate(0, range[1].get_min_index(), range[2].get_min_index());
        BasicCoordinate<3,int> new_max = make_coordinate((split_slice), range[1].get_max_index(), range[2].get_max_index());

        const IndexRange<3> nrange(new_min, new_max);

        VoxelsOnCartesianGrid<float> tmp(nrange,
                                         data->get_origin(),
                                         data->get_voxel_size());

        get_subset(tmp, nrange, minZ, maxZ);

        split_list.append(tmp);
    }
    emit resultReady();
}

//void Worker_Splitter::process()
//{
//    int slitStep = sliceSize*splitSlice;

//    int numZ = data.size() / sliceSize;


//    int numSlices = numZ / splitSlice;

//    for (int i = 0; i < numSlices; i++)
//    {
//        QVector<float> curSlice(slitStep);
//        for (int j = 0; j < slitStep; j++)
//        {
//            curSlice[j] = data.at(j*(i+1));
//        }
//        split_list.append(curSlice);
//    }

//}

Worker_Splitter::Worker_Splitter(QWidget *parent)
    :Worker(parent)
{}

Worker_Splitter::~Worker_Splitter()
{
}

void
Worker_Splitter::get_subset(VoxelsOnCartesianGrid<float>& _tmp, const IndexRange<3>& new_range, const int& _minZ, const int& _maxZ)
{
    if (new_range.get_min_index() >= data->get_index_range().get_min_index() &&
            new_range.get_max_index() <= data->get_index_range().get_max_index())
    {
        PixelsOnCartesianGrid<float> f;

        for(int z = _minZ, t = 0; z < _maxZ; ++z, ++t)
        {
            f = data->get_plane(z);
            _tmp.set_plane(f, t);
        }
    }
}
