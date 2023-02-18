#ifndef _BVS1_HPP_
#define _BVS1_HPP_

#include <cstdint>
#include <vector>
#include "bct.hpp"

namespace TRIAD_6
{
	enum class FramebufferMode
	{
		Graphics, Text
	};

	class BVS_1
	{
		public:
			BVS_1();
			~BVS_1();

			inline void WriteToFramebuffer(BCT::UWord address, BCT::UTryte data)
			{
			}	

			inline uint8_t *GetFramebuffer()
			{
				return framebuffer.data();
			}

			inline uint8_t *GetColorIndexTable()
			{
				return color_index_table.data();
			}

			inline uint8_t *GetPaletteTable()
			{
				return palette_table.data();
			}
		private:
			std::vector<uint8_t> framebuffer;
			std::vector<uint8_t> color_index_table;
			std::vector<uint8_t> palette_table;
	};
}

#endif
