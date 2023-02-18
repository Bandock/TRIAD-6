#include "bvs1.hpp"
#include "utility.hpp"

TRIAD_6::BVS_1::BVS_1() : framebuffer(TRIAD_6::Utility::BCT_TrytesToBytes(TRIAD_6::Utility::KiloTrytesToTrytes(1))), color_index_table(TRIAD_6::Utility::BCT_TrytesToBytes(27)), palette_table(TRIAD_6::Utility::BCT_TrytesToBytes(27))
{
}

TRIAD_6::BVS_1::~BVS_1()
{
}
