#include "machine.hpp"
#include "utility.hpp"
#include <cstring>
#include <fmt/core.h>

TRIAD_6::Machine::Machine() : MainCPU(1000000.0, this, TRIAD_6::memory_read<TRIAD_6::BCT::Tryte, TRIAD_6::Machine>, TRIAD_6::memory_write<TRIAD_6::BCT::Tryte, TRIAD_6::Machine>, TRIAD_6::memory_read<TRIAD_6::BCT::UTryte, TRIAD_6::Machine>, TRIAD_6::memory_write<TRIAD_6::BCT::UTryte, TRIAD_6::Machine>), RAM(TRIAD_6::Utility::KiloTrytesToTrytes(1)), memory(TRIAD_6::Utility::BCT_TrytesToBytes(RAM)), cpu_tp(std::chrono::high_resolution_clock::now()), cpu_accumulator(0.0)
{
	memory_write<BCT::UTryte, Machine>(1458, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'P', '3'>());
	memory_write<BCT::Tryte, Machine>(1459, this, BCT::Tryte(1));
	memory_write<BCT::UTryte, Machine>(1460, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '0'>());
	memory_write<BCT::Tryte, Machine>(1461, this, BCT::Tryte(4));
	memory_write<BCT::UTryte, Machine>(1462, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '0'>());
	memory_write<BCT::Tryte, Machine>(1463, this, BCT::Tryte(3));
	memory_write<BCT::UTryte, Machine>(1464, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '0'>());
	memory_write<BCT::Tryte, Machine>(1465, this, BCT::Tryte(7));
	memory_write<BCT::UTryte, Machine>(1466, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '0'>());
	memory_write<BCT::Tryte, Machine>(1467, this, BCT::Tryte(2));
	memory_write<BCT::UTryte, Machine>(1468, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '0'>());
	memory_write<BCT::Tryte, Machine>(1469, this, BCT::Tryte(11));
	memory_write<BCT::UTryte, Machine>(1470, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '0'>());
	memory_write<BCT::Tryte, Machine>(1471, this, BCT::Tryte(5));
	memory_write<BCT::UTryte, Machine>(1472, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', '0'>());
	memory_write<BCT::UTryte, Machine>(1473, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, '0', '2'>());
	memory_write<BCT::UTryte, Machine>(1474, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, '0', '2'>());
}

TRIAD_6::Machine::~Machine()
{
}

void TRIAD_6::Machine::operator()(std::chrono::high_resolution_clock::time_point &current_tp)
{
	std::chrono::duration<double> delta_time = current_tp - cpu_tp;
	if (delta_time.count() > 0.25)
	{
		delta_time = std::chrono::duration<double>(0.25);
	}
	cpu_accumulator += delta_time.count();
	double cycles_per_second = MainCPU.GetCyclesPerSecond();
	double cycle_rate = 1.0 / cycles_per_second;
	if (cpu_accumulator >= cycle_rate)
	{
		uint32_t cycles = cpu_accumulator / cycle_rate;
		cpu_accumulator -= static_cast<double>(cycles) * cycle_rate;
		for (; cycles; --cycles)
		{
			MainCPU();
		}
	}
} 
