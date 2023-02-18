#include "machine.hpp"
#include "utility.hpp"
#include <cstring>
#include <fmt/core.h>

TRIAD_6::Machine::Machine() : MainCPU(4000000.0, this, TRIAD_6::memory_read<TRIAD_6::BCT::Tryte, TRIAD_6::Machine>, TRIAD_6::memory_write<TRIAD_6::BCT::Tryte, TRIAD_6::Machine>, TRIAD_6::memory_read<TRIAD_6::BCT::UTryte, TRIAD_6::Machine>, TRIAD_6::memory_write<TRIAD_6::BCT::UTryte, TRIAD_6::Machine>), RAM(TRIAD_6::Utility::KiloTrytesToTrytes(1)), memory(TRIAD_6::Utility::BCT_TrytesToBytes(RAM)), cpu_tp(std::chrono::high_resolution_clock::now()), cpu_accumulator(0.0)
{
	memory_write<BCT::UTryte, Machine>(0, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'P', '3'>());
	memory_write<BCT::Tryte, Machine>(1, this, BCT::Tryte(-1));
	memory_write<BCT::UTryte, Machine>(2, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', '0'>());
	memory_write<BCT::UTryte, Machine>(3, this, BCT::UTryte(27));
	memory_write<BCT::UTryte, Machine>(4, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'P', '3'>());
	memory_write<BCT::Tryte, Machine>(5, this, BCT::Tryte(+1));
	memory_write<BCT::UTryte, Machine>(6, this, BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', '0'>());
	memory_write<BCT::Tryte, Machine>(7, this, BCT::Tryte(-27));
	memory_write<BCT::UTryte, Machine>(8, this, BCT::UTryte(2));
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
