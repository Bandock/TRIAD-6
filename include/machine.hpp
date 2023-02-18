#ifndef _MACHINE_HPP_
#define _MACHINE_HPP_

#include <cstdint>
#include <vector>
#include <chrono>
#include <type_traits>
#include "cpu.hpp"
#include "bvs1.hpp"

namespace TRIAD_6
{
	template <typename T, typename MachineType> requires Utility::TypeCompatible<T, BCT::Tryte, BCT::UTryte>
	T memory_read(BCT::UWord address, MachineType *current_machine)
	{
		T data = 0;
		if (address >= BCT::UWord(0) && address <= BCT::UWord(static_cast<uint32_t>(current_machine->RAM) - 1))
		{
			data = BCT::ReadTryte<T>(current_machine->memory.data(), address);
		}
		else if (address >= BCT::GetValueInSeptemvigesimal<BCT::UWord, '3', '0', '0', '0'>() && address <= BCT::GetValueInSeptemvigesimal<BCT::UWord, '5', 'Q', 'Q', 'Q'>())
		{
			data = BCT::ReadTryte<T>(current_machine->Video.GetFramebuffer(), address - BCT::GetValueInSeptemvigesimal<BCT::UWord, '3', '0', '0', '0'>());
		}
		else if (address >= BCT::GetValueInSeptemvigesimal<BCT::UWord, 'B', '0', '8', '0'>() && address <= BCT::GetValueInSeptemvigesimal<BCT::UWord, 'B', '0', '8', 'Q'>())
		{
			data = BCT::ReadTryte<T>(current_machine->Video.GetColorIndexTable(), address - BCT::GetValueInSeptemvigesimal<BCT::UWord, 'B', '0', '8', '0'>());
		}
		else if (address >= BCT::GetValueInSeptemvigesimal<BCT::UWord, 'B', '0', '9', '0'>() && address <= BCT::GetValueInSeptemvigesimal<BCT::UWord, 'B', '0', '9', 'Q'>())
		{
			data = BCT::ReadTryte<T>(current_machine->Video.GetPaletteTable(), address - BCT::GetValueInSeptemvigesimal<BCT::UWord, 'B', '0', '9', 'Q'>());
		}
		return data;
	}

	template <typename T, typename MachineType> requires Utility::TypeCompatible<T, BCT::Tryte, BCT::UTryte>
	void memory_write(BCT::UWord address, MachineType *current_machine, T data)
	{
		if (address >= BCT::UWord(0) && address <= BCT::UWord(static_cast<uint32_t>(current_machine->RAM) - 1))
		{
			BCT::WriteTryte<T>(current_machine->memory.data(), address, data);
		}
		else if (address >= BCT::GetValueInSeptemvigesimal<BCT::UWord, '3', '0', '0', '0'>() && address <= BCT::GetValueInSeptemvigesimal<BCT::UWord, '5', 'Q', 'Q', 'Q'>())
		{
			// BCT::WriteTryte<T>(current_machine->Video.GetFramebuffer(), address - BCT::GetValueInSeptemvigesimal<BCT::UWord, '3', '0', '0', '0'>(), data);
		}
	}

	class Machine
	{
		public:
			Machine();
			~Machine();
			void operator()(std::chrono::high_resolution_clock::time_point &current_tp);

			template <typename T, typename MachineType> requires Utility::TypeCompatible<T, BCT::Tryte, BCT::UTryte>
			friend T memory_read(BCT::UWord address, MachineType *current_machine);

			template <typename T, typename MachineType> requires Utility::TypeCompatible<T, BCT::Tryte, BCT::UTryte>
			friend void memory_write(BCT::UWord address, MachineType *current_machine, T data);
		private:
			CPU<Machine, BCT::Tryte (*)(BCT::UWord, Machine *), void (*)(BCT::UWord, Machine *, BCT::Tryte), BCT::UTryte (*)(BCT::UWord, Machine *), void (*)(BCT::UWord, Machine *, BCT::UTryte)> MainCPU;
			size_t RAM;
			std::vector<uint8_t> memory;
			BVS_1 Video;
			std::chrono::high_resolution_clock::time_point cpu_tp;
			double cpu_accumulator;
	};
}

#endif
