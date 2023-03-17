#ifndef _CPU_HPP_
#define _CPU_HPP_

#include <cstdint>
#include <chrono>
#include <concepts>
#include <array>
#include <fmt/core.h>
#include "bct.hpp"
#include "utility.hpp"

namespace TRIAD_6
{
	enum class CycleState
	{
		Fetch, Execute
	};

	enum class RegisterType
	{
		A, C, D, X, Y
	};

	enum class IndexRegisterType
	{
		None, X, Y
	};

	enum class CarryBorrowType
	{
		None, Carry, Borrow
	};

	enum class SetFlagType
	{
		OverflowUnderflow, CarryBorrow, Interrupt, BalancedUnbalancedControl
	};

	enum class BranchTestType
	{
		Positive, Zero, Negative, NotPositive, NotZero, NotNegative, Overflow, Neutral, Underflow, NotOverflow, NotNeutral, NotUnderflow, Carry, NoCarryBorrow, Borrow, NotCarry, NotNoCarryBorrow, NotBorrow
	};

	template <typename T>
	concept HasCurrentCycleState = requires(T obj)
	{
		{ obj.current_cycle_state } -> std::convertible_to<CycleState>;
	};

	namespace Instruction
	{
		template <typename T>
		void InvalidOperation(T *obj)
		{
		}

		template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback> requires HasCurrentCycleState<T>
		void NoOperation(T *obj)
		{
			if constexpr (next_callback != nullptr)
			{
				obj->instruction_callback = next_callback;
			}
			else
			{
				obj->current_cycle_state = CycleState::Fetch;
			}
		}

		template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback>
		void ReadUnbalancedTryte(T *obj)
		{
			obj->data_bus = obj->unbalanced_memory_read(obj->PC, obj->CurrentMachine);
			++obj->PC;
			obj->instruction_callback = next_callback;
		}

		template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback>
		void LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte(T *obj)
		{
			obj->address = obj->data_bus;
			obj->data_bus = obj->unbalanced_memory_read(obj->PC, obj->CurrentMachine);
			++obj->PC;
			obj->instruction_callback = next_callback;
		}


		template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback>
		void LoadDataIntoAddressLowerTryteAndReadUnbalancedTryteFromInitialAddress(T *obj)
		{
			BCT::UWord tmp = obj->address;
			obj->address = obj->data_bus;
			obj->data_bus = obj->unbalanced_memory_read(tmp, obj->CurrentMachine);
			obj->instruction_callback = next_callback;
		}

		template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback>
		void LoadDataIntoAddressUpperTryteAndReadUnbalancedTryteFromAddress(T *obj)
		{
			obj->address += (BCT::UWord(obj->data_bus) << 6);
			obj->data_bus = obj->unbalanced_memory_read(obj->address, obj->CurrentMachine);
			++obj->address;
			obj->instruction_callback = next_callback;
		}

		template <typename T, RegisterType destination_register> requires HasCurrentCycleState<T>
		void LoadFromMemoryToRegister_ImmediateValueMode(T *obj)
		{
			constexpr BCT::Tryte bu_flag_check = BCT::GetTryteFromTritData<-1, +1, -1, -1, -1, -1>();
			if constexpr (destination_register == RegisterType::A)
			{
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->A = obj->balanced_memory_read(obj->PC, obj->CurrentMachine);
					int16_t A_b = obj->A;
					if (A_b > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else if (A_b == 0)
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
					else
					{
						obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, +1, -1>();
					}
					// fmt::print("Signed Accumulator:  {}\n", static_cast<int16_t>(obj->A));
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					BCT::UTryte tmp = obj->unbalanced_memory_read(obj->PC, obj->CurrentMachine);
					obj->A = tmp;
					if (static_cast<uint16_t>(tmp) > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
					// fmt::print("Unsigned Accumulator:  {}\n", static_cast<uint16_t>(obj->A));
				}
			}
			else if constexpr (destination_register == RegisterType::C)
			{
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->C = obj->balanced_memory_read(obj->PC, obj->CurrentMachine);
					int16_t C_b = obj->C;
					if (C_b > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else if (C_b == 0)
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
					else
					{
						obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, +1, -1>();
					}
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					BCT::UTryte tmp = obj->unbalanced_memory_read(obj->PC, obj->CurrentMachine);
					obj->C = tmp;
					if (static_cast<uint16_t>(tmp) > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
				}
			}
			else if constexpr (destination_register == RegisterType::D)
			{
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->D = obj->balanced_memory_read(obj->PC, obj->CurrentMachine);
					int16_t D_b = obj->D;
					if (D_b > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else if (D_b == 0)
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
					else
					{
						obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, +1, -1>();
					}
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					BCT::UTryte tmp = obj->unbalanced_memory_read(obj->PC, obj->CurrentMachine);
					obj->D = tmp;
					if (static_cast<uint16_t>(tmp) > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
				}
			}
			else if constexpr (destination_register == RegisterType::X)
			{
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->X = obj->balanced_memory_read(obj->PC, obj->CurrentMachine);
					int16_t X_b = obj->X;
					if (X_b > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else if (X_b == 0)
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
					else
					{
						obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, +1, -1>();
					}
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					BCT::UTryte tmp = obj->unbalanced_memory_read(obj->PC, obj->CurrentMachine);
					obj->X = tmp;
					if (static_cast<uint16_t>(tmp) > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
				}
			}
			else if constexpr (destination_register == RegisterType::Y)
			{
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->Y = obj->balanced_memory_read(obj->PC, obj->CurrentMachine);
					int16_t Y_b = obj->Y;
					if (Y_b > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else if (Y_b == 0)
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
					else
					{
						obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, +1, -1>();
					}
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					BCT::UTryte tmp = obj->unbalanced_memory_read(obj->PC, obj->CurrentMachine);
					obj->Y = tmp;
					if (static_cast<uint16_t>(tmp) > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
				}
			}
			++obj->PC;
			obj->current_cycle_state = CycleState::Fetch;
		}

		template <typename T, RegisterType destination_register, IndexRegisterType index_register_used> requires HasCurrentCycleState<T>
		void LoadFromMemoryToRegister_AddressMode(T *obj)
		{
			constexpr BCT::Tryte bu_flag_check = BCT::GetTryteFromTritData<-1, +1, -1, -1, -1, -1>();
			obj->address += (static_cast<BCT::UWord>(obj->data_bus) << 6);
			obj->data_bus = 0;
			if constexpr (destination_register == RegisterType::A)
			{
				if constexpr (index_register_used == IndexRegisterType::X)
				{
					obj->address += BCT::UWord(obj->X);
				}
				else if constexpr (index_register_used == IndexRegisterType::Y)
				{
					obj->address += BCT::UWord(obj->Y);
				}
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->A = obj->balanced_memory_read(obj->address, obj->CurrentMachine);
					int16_t A_b = obj->A;
					if (A_b > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else if (A_b == 0)
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
					else
					{
						obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, +1, -1>();
					}
					// fmt::print("Signed Accumulator from Memory Address {}:  {}\n", static_cast<uint32_t>(obj->address), static_cast<int16_t>(obj->A));
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					BCT::UTryte tmp = obj->unbalanced_memory_read(obj->address, obj->CurrentMachine);
					obj->A = tmp;
					if (static_cast<uint16_t>(tmp) > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
					// fmt::print("Unsigned Accumulator from Memory Address {}:  {}\n", static_cast<uint32_t>(obj->address), static_cast<uint16_t>(obj->A));
				}
			}
			else if constexpr (destination_register == RegisterType::C)
			{
				if constexpr (index_register_used == IndexRegisterType::X)
				{
					obj->address += BCT::UWord(obj->X);
				}
				else if constexpr (index_register_used == IndexRegisterType::Y)
				{
					obj->address += BCT::UWord(obj->Y);
				}
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->C = obj->balanced_memory_read(obj->address, obj->CurrentMachine);
					int16_t C_b = obj->C;
					if (C_b > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else if (C_b == 0)
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
					else
					{
						obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, +1, -1>();
					}
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					BCT::UTryte tmp = obj->unbalanced_memory_read(obj->address, obj->CurrentMachine);
					obj->C = tmp;
					if (static_cast<uint16_t>(tmp) > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
				}
			}
			else if constexpr (destination_register == RegisterType::D)
			{
				if constexpr (index_register_used == IndexRegisterType::X)
				{
					obj->address += BCT::UWord(obj->X);
				}
				else if constexpr (index_register_used == IndexRegisterType::Y)
				{
					obj->address += BCT::UWord(obj->Y);
				}
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->D = obj->balanced_memory_read(obj->address, obj->CurrentMachine);
					int16_t D_b = obj->D;
					if (D_b > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else if (D_b == 0)
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
					else
					{
						obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, +1, -1>();
					}
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					BCT::UTryte tmp = obj->unbalanced_memory_read(obj->address, obj->CurrentMachine);
					obj->D = tmp;
					if (static_cast<uint16_t>(tmp) > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
				}
			}
			else if constexpr (destination_register == RegisterType::X)
			{
				if constexpr (index_register_used == IndexRegisterType::Y)
				{
					obj->address += BCT::UWord(obj->Y);
				}
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->X = obj->balanced_memory_read(obj->address, obj->CurrentMachine);
					int16_t X_b = obj->X;
					if (X_b > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else if (X_b == 0)
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
					else
					{
						obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, +1, -1>();
					}
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					BCT::UTryte tmp = obj->unbalanced_memory_read(obj->address, obj->CurrentMachine);
					obj->X = tmp;
					if (static_cast<uint16_t>(tmp) > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
				}
			}
			else if constexpr (destination_register == RegisterType::Y)
			{
				if constexpr (index_register_used == IndexRegisterType::X)
				{
					obj->address += BCT::UWord(obj->X);
				}
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->Y = obj->balanced_memory_read(obj->address, obj->CurrentMachine);
					int16_t Y_b = obj->Y;
					if (Y_b > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else if (Y_b == 0)
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
					else
					{
						obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, +1, -1>();
					}
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					BCT::UTryte tmp = obj->unbalanced_memory_read(obj->address, obj->CurrentMachine);
					obj->Y = tmp;
					if (static_cast<uint16_t>(tmp) > 0)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
					}
				}
			}
			obj->address = 0;
			obj->current_cycle_state = CycleState::Fetch;
		}

		template <typename T, RegisterType source_register, IndexRegisterType index_register_used> requires HasCurrentCycleState<T>
		void StoreFromRegisterToMemory_AddressMode(T *obj)
		{
			constexpr BCT::Tryte bu_flag_check = BCT::GetTryteFromTritData<-1, +1, -1, -1, -1, -1>();
			obj->address += (BCT::UWord(obj->data_bus) << 6);
			obj->data_bus = 0;
			if constexpr (source_register == RegisterType::A)
			{
				if constexpr (index_register_used == IndexRegisterType::X)
				{
					obj->address += BCT::UWord(obj->X);
				}
				else if constexpr (index_register_used == IndexRegisterType::Y)
				{
					obj->address += BCT::UWord(obj->Y);
				}
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->balanced_memory_write(obj->address, obj->CurrentMachine, obj->A);
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					obj->unbalanced_memory_write(obj->address, obj->CurrentMachine, obj->A);
				}
			}
			else if constexpr (source_register == RegisterType::C)
			{
				if constexpr (index_register_used == IndexRegisterType::X)
				{
					obj->address += BCT::UWord(obj->X);
				}
				else if constexpr (index_register_used == IndexRegisterType::Y)
				{
					obj->address += BCT::UWord(obj->Y);
				}
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->balanced_memory_write(obj->address, obj->CurrentMachine, obj->C);
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					obj->unbalanced_memory_write(obj->address, obj->CurrentMachine, obj->C);
				}
			}
			else if constexpr (source_register == RegisterType::D)
			{
				if constexpr (index_register_used == IndexRegisterType::X)
				{
					obj->address += BCT::UWord(obj->X);
				}
				else if constexpr (index_register_used == IndexRegisterType::Y)
				{
					obj->address += BCT::UWord(obj->Y);
				}
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->balanced_memory_write(obj->address, obj->CurrentMachine, obj->D);
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					obj->unbalanced_memory_write(obj->address, obj->CurrentMachine, obj->D);
				}
			}
			else if constexpr (source_register == RegisterType::X)
			{
				if constexpr (index_register_used == IndexRegisterType::Y)
				{
					obj->address += BCT::UWord(obj->Y);
				}
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->balanced_memory_write(obj->address, obj->CurrentMachine, obj->X);
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					obj->unbalanced_memory_write(obj->address, obj->CurrentMachine, obj->X);
				}
			}
			else if constexpr (source_register == RegisterType::Y)
			{
				if constexpr (index_register_used == IndexRegisterType::X)
				{
					obj->address += BCT::UWord(obj->X);
				}
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->balanced_memory_write(obj->address, obj->CurrentMachine, obj->Y);
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					obj->unbalanced_memory_write(obj->address, obj->CurrentMachine, obj->Y);
				}
			}
			obj->address = 0;
			obj->current_cycle_state = CycleState::Fetch;
		}

		template <typename T, CarryBorrowType carry_borrow> requires HasCurrentCycleState<T>
		void AddFromMemoryToAccumulator_ImmediateValueMode(T *obj)
		{
			constexpr BCT::Tryte cb_flag_check = BCT::GetTryteFromTritData<-1, -1, -1, +1, -1, -1>();
			constexpr BCT::Tryte bu_flag_check = BCT::GetTryteFromTritData<-1, +1, -1, -1, -1, -1>();
			if ((obj->F & bu_flag_check) == bu_flag_check)
			{
				BCT::Tryte tmp = obj->A + obj->balanced_memory_read(obj->PC, obj->CurrentMachine);
				if constexpr (carry_borrow == CarryBorrowType::Carry)
				{
					if ((obj->F & cb_flag_check) == cb_flag_check)
					{
						tmp += BCT::Tryte(1);
					}
				}
				else if constexpr (carry_borrow == CarryBorrowType::Borrow)
				{
					if ((~(obj->F) & cb_flag_check) == cb_flag_check)
					{
						tmp -= BCT::Tryte(1);
					}
				}
				int32_t A_b = obj->A;
				int32_t tmp_b = tmp;
				if (tmp_b > 0)
				{
					obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
				}
				else if (tmp_b == 0)
				{
					obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
				}
				else
				{
					obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, +1, -1>();
				}
				if (A_b >= 0)
				{
					if (tmp_b < A_b)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, +1, +1, -1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, 0, 0, -1>();
					}
				}
				else
				{
					if (tmp_b > A_b)
					{
						obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, -1, -1, +1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, 0, 0, -1>();
					}
				}
				obj->A = tmp;
				fmt::print("Signed Accumulator:  {}\n", tmp_b);
			}
			else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
			{
				BCT::UTryte A_u = obj->A;
				BCT::UTryte tmp = A_u + obj->unbalanced_memory_read(obj->PC, obj->CurrentMachine);
				if constexpr (carry_borrow == CarryBorrowType::Carry)
				{
					if ((obj->F & cb_flag_check) == cb_flag_check)
					{
						tmp += BCT::UTryte(1);
					}
				}
				uint32_t A_b = A_u;
				uint32_t tmp_b = tmp;
				if (tmp_b > 0)
				{
					obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
				}
				else
				{
					obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
				}
				if (tmp_b < A_b)
				{
					obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, +1, +1, -1>();
				}
				else
				{
					obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, 0, 0, -1>();
				}
				obj->A = tmp;
				// fmt::print("Unsigned Accumulator:  {}\n", tmp_b);
			}
			++obj->PC;
			obj->current_cycle_state = CycleState::Fetch;
		}

		template <typename T, CarryBorrowType carry_borrow, IndexRegisterType index_register_used> requires HasCurrentCycleState<T>
		void AddFromMemoryToAccumulator_AddressMode(T *obj)
		{
			constexpr BCT::Tryte cb_flag_check = BCT::GetTryteFromTritData<-1, -1, -1, +1, -1, -1>();
			constexpr BCT::Tryte bu_flag_check = BCT::GetTryteFromTritData<-1, +1, -1, -1, -1, -1>();
			obj->address += (static_cast<BCT::UWord>(obj->data_bus) << 6);
			if constexpr (index_register_used == IndexRegisterType::X)
			{
				obj->address += BCT::UWord(obj->X);
			}
			else if constexpr (index_register_used == IndexRegisterType::Y)
			{
				obj->address += BCT::UWord(obj->Y);
			}
			obj->data_bus = 0;
			if ((obj->F & bu_flag_check) == bu_flag_check)
			{
				BCT::Tryte tmp = obj->A + obj->balanced_memory_read(obj->address, obj->CurrentMachine);
				if constexpr (carry_borrow == CarryBorrowType::Carry)
				{
					if ((obj->F & cb_flag_check) == cb_flag_check)
					{
						tmp += BCT::Tryte(1);
					}
				}
				else if constexpr (carry_borrow == CarryBorrowType::Borrow)
				{
					if ((~(obj->F) & cb_flag_check) == cb_flag_check)
					{
						tmp -= BCT::Tryte(1);
					}
				}
				int32_t A_b = obj->A;
				int32_t tmp_b = tmp;
				if (tmp_b > 0)
				{
					obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
				}
				else if (tmp_b == 0)
				{
					obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
				}
				else
				{
					obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, +1, -1>();
				}
				if (A_b >= 0)
				{
					if (tmp_b < A_b)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, +1, +1, -1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, 0, 0, -1>();
					}
				}
				else
				{
					if (tmp_b > A_b)
					{
						obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, -1, -1, +1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, 0, 0, -1>();
					}
				}
				obj->A = tmp;
				// fmt::print("Signed Accumulator:  {}\n", tmp_b);
			}
			else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
			{
				BCT::UTryte A_u = obj->A;
				BCT::UTryte tmp = A_u + obj->unbalanced_memory_read(obj->address, obj->CurrentMachine);
				if constexpr (carry_borrow == CarryBorrowType::Carry)
				{
					if ((obj->F & cb_flag_check) == cb_flag_check)
					{
						tmp += BCT::UTryte(1);
					}
				}
				uint32_t A_b = A_u;
				uint32_t tmp_b = tmp;
				if (tmp_b > 0)
				{
					obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
				}
				else
				{
					obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
				}
				if (tmp_b < A_b)
				{
					obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, +1, +1, -1>();
				}
				else
				{
					obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, 0, 0, -1>();
				}
				obj->A = tmp;
				// fmt::print("Unsigned Accumulator:  {}\n", tmp_b);
			}
			obj->address = 0;
			obj->current_cycle_state = CycleState::Fetch;
		}

		template <typename T, CarryBorrowType carry_borrow> requires HasCurrentCycleState<T>
		void AddFromDataRegisterToAccumulator(T *obj)
		{
			constexpr BCT::Tryte cb_flag_check = BCT::GetTryteFromTritData<-1, -1, -1, +1, -1, -1>();
			constexpr BCT::Tryte bu_flag_check = BCT::GetTryteFromTritData<-1, +1, -1, -1, -1, -1>();
			if ((obj->F & bu_flag_check) == bu_flag_check)
			{
				BCT::Tryte tmp = obj->A + obj->D;
				if constexpr (carry_borrow == CarryBorrowType::Carry)
				{
					if ((obj->F & cb_flag_check) == cb_flag_check)
					{
						tmp += BCT::Tryte(1);
					}
				}
				else if constexpr (carry_borrow == CarryBorrowType::Borrow)
				{
					if ((~(obj->F) & cb_flag_check) == cb_flag_check)
					{
						tmp -= BCT::Tryte(-1);
					}
				}
				int32_t A_b = obj->A;
				int32_t tmp_b = tmp;
				if (tmp_b > 0)
				{
					obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
				}
				else if (tmp_b == 0)
				{
					obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
				}
				else
				{
					obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, +1, -1>();
				}
				if (A_b >= 0)
				{
					if (tmp_b < A_b)
					{
						obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, +1, +1, -1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, 0, 0, -1>();
					}
				}
				else
				{
					if (tmp_b > A_b)
					{
						obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, -1, -1, +1>();
					}
					else
					{
						obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, 0, 0, -1>();
					}
				}
				obj->A = tmp;
				// fmt::print("Signed Accumulator:  {}\n", tmp_b);
			}
			else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
			{
				BCT::UTryte A_u = obj->A;
				BCT::UTryte D_u = obj->D;
				BCT::UTryte tmp = A_u + D_u;
				if constexpr (carry_borrow == CarryBorrowType::Carry)
				{
					if ((obj->F & cb_flag_check) == cb_flag_check)
					{
						tmp += BCT::UTryte(1);
					}
				}
				uint32_t A_b = A_u;
				uint32_t tmp_b = tmp;
				if (tmp_b > 0)
				{
					obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
				}
				else
				{
					obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, 0>();
				}
				if (tmp_b < A_b)
				{
					obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, +1, +1, -1>();
				}
				else
				{
					obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, 0, 0, -1>();
				}
				obj->A = tmp;
				// fmt::print("Unsigned Accumulator:  {}\n", tmp_b);
			}
			obj->current_cycle_state = CycleState::Fetch;
		}

		template <typename T, RegisterType source_register, RegisterType destination_register>
		void TransferFromRegisterToRegister(T *obj)
		{
			if constexpr (destination_register == RegisterType::A)
			{
				if constexpr (source_register == RegisterType::C)
				{
					obj->A = obj->C;
				}
				else if constexpr (source_register == RegisterType::D)
				{
					obj->A = obj->D;
				}
				else if constexpr (source_register == RegisterType::X)
				{
					obj->A = obj->X;
				}
				else if constexpr (source_register == RegisterType::Y)
				{
					obj->A = obj->Y;
				}
			}
			else if constexpr (destination_register == RegisterType::C)
			{
				if constexpr (source_register == RegisterType::A)
				{
					obj->C = obj->A;
				}
				else if constexpr (source_register == RegisterType::D)
				{
					obj->C = obj->D;
				}
				else if constexpr (source_register == RegisterType::X)
				{
					obj->C = obj->X;
				}
				else if constexpr (source_register == RegisterType::Y)
				{
					obj->C = obj->Y;
				}
			}
			else if constexpr (destination_register == RegisterType::D)
			{
				if constexpr (source_register == RegisterType::A)
				{
					obj->D = obj->A;
				}
				else if constexpr (source_register == RegisterType::C)
				{
					obj->D = obj->C;
				}
				else if constexpr (source_register == RegisterType::X)
				{
					obj->D = obj->X;
				}
				else if constexpr (source_register == RegisterType::Y)
				{
					obj->D = obj->Y;
				}
			}
			else if constexpr (destination_register == RegisterType::X)
			{
				if constexpr (source_register == RegisterType::A)
				{
					obj->X = obj->A;
				}
				else if constexpr (source_register == RegisterType::C)
				{
					obj->X = obj->C;
				}
				else if constexpr (source_register == RegisterType::D)
				{
					obj->X = obj->D;
				}
				else if constexpr (source_register == RegisterType::Y)
				{
					obj->X = obj->Y;
				}
			}
			else if constexpr (destination_register == RegisterType::Y)
			{
				if constexpr (source_register == RegisterType::A)
				{
					obj->Y = obj->A;
				}
				else if constexpr (source_register == RegisterType::C)
				{
					obj->Y = obj->C;
				}
				else if constexpr (source_register == RegisterType::D)
				{
					obj->Y = obj->D;
				}
				else if constexpr (source_register == RegisterType::X)
				{
					obj->Y = obj->X;
				}
			}
			obj->current_cycle_state = CycleState::Fetch;
		}

		template <typename T, typename InstructionCallback, InstructionCallback next_callback>
		void TransferFromIndexRegisterXToStackPointerLowTryte(T *obj)
		{
			BCT::UTryte tmp = obj->X;
			obj->SP ^= BCT::GetValueInSeptemvigesimal<BCT::UWord, 'D', 'D', '0', '0'>();
			obj->SP += tmp;
			obj->instruction_callback = next_callback;
		}

		template <typename T> requires HasCurrentCycleState<T>
		void TransferFromIndexRegisterYToStackPointerHighTryte(T *obj)
		{
			BCT::UTryte tmp = obj->Y;
			obj->SP ^= BCT::GetValueInSeptemvigesimal<BCT::UWord, '0', '0', 'D', 'D'>();
			obj->SP += (static_cast<BCT::UWord>(tmp) << 6);
			obj->current_cycle_state = CycleState::Fetch;
		}

		template <typename T, typename InstructionCallback, InstructionCallback next_callback>
		void TransferFromStackPointerLowTryteToIndexRegisterX(T *obj)
		{
			BCT::UTryte tmp = obj->SP;
			obj->X = tmp;
			obj->instruction_callback = next_callback;
		}

		template <typename T> requires HasCurrentCycleState<T>
		void TransferFromStackPointerHighTryteToIndexRegisterY(T *obj)
		{
			BCT::UTryte tmp = (obj->SP >> 6);
			obj->Y = tmp;
			obj->current_cycle_state = CycleState::Fetch;
		}

		template <typename T> requires HasCurrentCycleState<T>
		void JumpToNewLocation(T *obj)
		{
			obj->address += (static_cast<BCT::UWord>(obj->data_bus) << 6);
			obj->data_bus = 0;
			obj->PC = obj->address;
			obj->address = 0;
			obj->current_cycle_state = CycleState::Fetch;
		}

		template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback, BranchTestType branch_test> requires HasCurrentCycleState<T>
		void LoadDataIntoAddressUpperTryteAndPerformBranchTest(T *obj)
		{
			obj->address += (BCT::UWord(obj->data_bus) << 6);
			obj->data_bus = 0;
			if constexpr (branch_test == BranchTestType::Positive || branch_test == BranchTestType::NotPositive)
			{
				constexpr BCT::Tryte pzn_flag_check = BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
				if constexpr (branch_test == BranchTestType::Positive)
				{
					if ((obj->F & pzn_flag_check) == pzn_flag_check)
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
				else if constexpr (branch_test == BranchTestType::NotPositive)
				{
					if (!((obj->F & pzn_flag_check) == pzn_flag_check))
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
			}
			else if constexpr (branch_test == BranchTestType::Zero || branch_test == BranchTestType::NotZero)
			{
				constexpr BCT::Tryte pzn_flag_check = BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
				if constexpr (branch_test == BranchTestType::Zero)
				{
					if ((obj->F & pzn_flag_check) != pzn_flag_check && (~(obj->F) & pzn_flag_check) != pzn_flag_check)
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
				else if constexpr (branch_test == BranchTestType::NotZero)
				{
					if (!((obj->F & pzn_flag_check) != pzn_flag_check && (~(obj->F) & pzn_flag_check) != pzn_flag_check))
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
			}
			else if constexpr (branch_test == BranchTestType::Negative || branch_test == BranchTestType::NotNegative)
			{
				constexpr BCT::Tryte pzn_flag_check = BCT::GetTryteFromTritData<-1, -1, -1, -1, -1, +1>();
				if constexpr (branch_test == BranchTestType::Negative)
				{
					if ((~(obj->F) & pzn_flag_check) == pzn_flag_check)
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
				else if constexpr (branch_test == BranchTestType::NotNegative)
				{
					if (!((~(obj->F) & pzn_flag_check) == pzn_flag_check))
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
			}
			else if constexpr (branch_test == BranchTestType::Overflow || branch_test == BranchTestType::NotOverflow)
			{
				constexpr BCT::Tryte ou_flag_check = BCT::GetTryteFromTritData<-1, -1, -1, -1, +1, -1>();
				if constexpr (branch_test == BranchTestType::Overflow)
				{
					if ((obj->F & ou_flag_check) == ou_flag_check)
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
				else if constexpr (branch_test == BranchTestType::NotOverflow)
				{
					if (!((obj->F & ou_flag_check) == ou_flag_check))
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
			}
			else if constexpr (branch_test == BranchTestType::Neutral || branch_test == BranchTestType::NotNeutral)
			{
				constexpr BCT::Tryte ou_flag_check = BCT::GetTryteFromTritData<-1, -1, -1, -1, +1, -1>();
				if constexpr (branch_test == BranchTestType::Neutral)
				{
					if ((obj->F & ou_flag_check) != ou_flag_check && (~(obj->F) & ou_flag_check) != ou_flag_check)
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
				else if constexpr (branch_test == BranchTestType::NotNeutral)
				{
					if (!((obj->F & ou_flag_check) != ou_flag_check && (~(obj->F & ou_flag_check) != ou_flag_check)))
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
			}
			else if constexpr (branch_test == BranchTestType::Underflow || branch_test == BranchTestType::NotUnderflow)
			{
				constexpr BCT::Tryte ou_flag_check = BCT::GetTryteFromTritData<-1, -1, -1, -1, +1, -1>();
				if constexpr (branch_test == BranchTestType::Underflow)
				{
					if ((~(obj->F) & ou_flag_check) == ou_flag_check)
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
				else if constexpr (branch_test == BranchTestType::NotUnderflow)
				{
					if (!((~(obj->F) & ou_flag_check) == ou_flag_check))
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
			}
			else if constexpr (branch_test == BranchTestType::Carry || branch_test == BranchTestType::NotCarry)
			{
				constexpr BCT::Tryte cb_flag_check = BCT::GetTryteFromTritData<-1, -1, -1, +1, -1, -1>();
				if constexpr (branch_test == BranchTestType::Carry)
				{
					if ((obj->F & cb_flag_check) == cb_flag_check)
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
				else if constexpr (branch_test == BranchTestType::NotCarry)
				{
					if (!((obj->F & cb_flag_check) == cb_flag_check))
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
			}
			else if constexpr (branch_test == BranchTestType::NoCarryBorrow || branch_test == BranchTestType::NotNoCarryBorrow)
			{
				constexpr BCT::Tryte cb_flag_check = BCT::GetTryteFromTritData<-1, -1, -1, +1, -1, -1>();
				if constexpr (branch_test == BranchTestType::NoCarryBorrow)
				{
					if ((obj->F & cb_flag_check) != cb_flag_check && (~(obj->F) & cb_flag_check) != cb_flag_check)
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
				else if constexpr (branch_test == BranchTestType::NotNoCarryBorrow)
				{
					if (!((obj->F & cb_flag_check) != cb_flag_check && (~(obj->F) & cb_flag_check) != cb_flag_check))
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}

			}
			else if constexpr (branch_test == BranchTestType::Borrow || branch_test == BranchTestType::NotBorrow)
			{
				constexpr BCT::Tryte cb_flag_check = BCT::GetTryteFromTritData<-1, -1, -1, -1, +1, -1>();
				if constexpr (branch_test == BranchTestType::Borrow)
				{
					if ((~(obj->F) & cb_flag_check) == cb_flag_check)
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
				else if constexpr (branch_test == BranchTestType::NotBorrow)
				{
					if (!((~(obj->F) & cb_flag_check) == cb_flag_check))
					{
						obj->instruction_callback = next_callback;
					}
					else
					{
						obj->address = 0;
						obj->current_cycle_state = CycleState::Fetch;
					}
				}
			}
		}

		template <typename T> requires HasCurrentCycleState<T>
		void BranchToNewLocation(T *obj)
		{
			obj->PC = obj->address;
			obj->address = 0;
			obj->current_cycle_state = CycleState::Fetch;
		}

		template <typename T, SetFlagType flag> requires HasCurrentCycleState<T>
		void SetFlagStateOperation(T *obj)
		{
			BCT::Tryte state = obj->balanced_memory_read(obj->PC, obj->CurrentMachine);
			++obj->PC;
			if constexpr (flag == SetFlagType::OverflowUnderflow)
			{
				if (static_cast<int16_t>(state) == +1)
				{
					obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, -1, +1, -1>();
				}
				else if (static_cast<int16_t>(state) == 0)
				{
					obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, -1, 0, -1>();
				}
				else
				{
					obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, +1, -1, +1>();
				}
			}
			else if constexpr (flag == SetFlagType::CarryBorrow)
			{
				if (state == +1)
				{
					obj->F |= BCT::GetTryteFromTritData<-1, -1, -1, +1, -1, -1>();
				}
				else if (state == 0)
				{
					obj->F ^= BCT::GetTryteFromTritData<-1, -1, -1, 0, -1, -1>();
				}
				else
				{
					obj->F &= BCT::GetTryteFromTritData<+1, +1, +1, -1, +1, +1>();
				}
			}
			else if constexpr (flag == SetFlagType::Interrupt)
			{
				if (state == +1)
				{
					obj->F |= BCT::GetTryteFromTritData<-1, -1, +1, -1, -1, -1>();
				}
				else if (state == -1)
				{
					obj->F &= BCT::GetTryteFromTritData<+1, +1, -1, +1, +1, +1>();
				}
			}
			else if constexpr (flag == SetFlagType::BalancedUnbalancedControl)
			{
				if (state == +1)
				{
					obj->F |= BCT::GetTryteFromTritData<-1, +1, -1, -1, -1, -1>();
				}
				else if (state == -1)
				{
					obj->F &= BCT::GetTryteFromTritData<+1, -1, +1, +1, +1, +1>();
				}
			}
			obj->current_cycle_state = CycleState::Fetch;
		}
	}

	template <typename T, typename InstructionCallbackType>
	consteval std::array<InstructionCallbackType, 729> ConstructInstructionTable()
	{
		std::array<InstructionCallbackType, 729> table;
		for (size_t i = 0; i < table.size(); ++i)
		{
			table[i] = Instruction::InvalidOperation<T>;
		}
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '0', '0'>())] = Instruction::NoOperation<T, InstructionCallbackType, nullptr>; // NOP
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', '0'>())] = Instruction::LoadFromMemoryToRegister_ImmediateValueMode<T, RegisterType::A>; // LDA #Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', '1'>())] = Instruction::LoadFromMemoryToRegister_ImmediateValueMode<T, RegisterType::C>; // LDC #Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', '2'>())] = Instruction::LoadFromMemoryToRegister_ImmediateValueMode<T, RegisterType::D>; // LDD #Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', '3'>())] = Instruction::LoadFromMemoryToRegister_ImmediateValueMode<T, RegisterType::X>; // LDX #Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', '4'>())] = Instruction::LoadFromMemoryToRegister_ImmediateValueMode<T, RegisterType::Y>; // LDY #Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', '5'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::A, IndexRegisterType::None>>>; // LDA Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', '6'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::C, IndexRegisterType::None>>>; // LDC Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', '7'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::D, IndexRegisterType::None>>>; // LDD Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', '8'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::X, IndexRegisterType::None>>>; // LDX Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', '9'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::Y, IndexRegisterType::None>>>; // LDY Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'A'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::A, IndexRegisterType::X>>>; // LDA Op, X
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'B'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::C, IndexRegisterType::X>>>; // LDC Op, X
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'C'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::D, IndexRegisterType::X>>>; // LDD Op, X
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'D'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::Y, IndexRegisterType::X>>>; // LDY Op, X
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'E'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::A, IndexRegisterType::Y>>>; // LDA Op, Y
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'F'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::C, IndexRegisterType::Y>>>; // LDC Op, Y
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'G'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::D, IndexRegisterType::Y>>>; // LDD Op, Y
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'H'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::X, IndexRegisterType::Y>>>; // LDX Op, Y
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'I'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndReadUnbalancedTryteFromAddress<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryteFromInitialAddress<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::A, IndexRegisterType::None>>>>>; // LDA [Op]
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'J'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndReadUnbalancedTryteFromAddress<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryteFromInitialAddress<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::D, IndexRegisterType::None>>>>>; // LDD [Op]
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'K'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndReadUnbalancedTryteFromAddress<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryteFromInitialAddress<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::A, IndexRegisterType::X>>>>>; // LDA [Op], X
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'L'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndReadUnbalancedTryteFromAddress<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryteFromInitialAddress<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::D, IndexRegisterType::X>>>>>; // LDD [Op], X
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'M'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndReadUnbalancedTryteFromAddress<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryteFromInitialAddress<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::A, IndexRegisterType::Y>>>>>; // LDA [Op], Y
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '2', 'N'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndReadUnbalancedTryteFromAddress<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryteFromInitialAddress<T, InstructionCallbackType, Instruction::LoadFromMemoryToRegister_AddressMode<T, RegisterType::D, IndexRegisterType::Y>>>>>; // LDD [Op], Y
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '3', '0'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::StoreFromRegisterToMemory_AddressMode<T, RegisterType::A, IndexRegisterType::None>>>; // STA Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '3', '1'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::StoreFromRegisterToMemory_AddressMode<T, RegisterType::C, IndexRegisterType::None>>>; // STC Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '3', '2'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::StoreFromRegisterToMemory_AddressMode<T, RegisterType::D, IndexRegisterType::None>>>; // STD Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '3', '3'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::StoreFromRegisterToMemory_AddressMode<T, RegisterType::X, IndexRegisterType::None>>>; // STX Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '3', '4'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::StoreFromRegisterToMemory_AddressMode<T, RegisterType::Y, IndexRegisterType::None>>>; // STY Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '3', '5'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::StoreFromRegisterToMemory_AddressMode<T, RegisterType::A, IndexRegisterType::X>>>; // STA Op, X
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '3', '6'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::StoreFromRegisterToMemory_AddressMode<T, RegisterType::C, IndexRegisterType::X>>>; // STC Op, X
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '3', '7'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::StoreFromRegisterToMemory_AddressMode<T, RegisterType::D, IndexRegisterType::X>>>; // STD Op, X
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '3', '8'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::StoreFromRegisterToMemory_AddressMode<T, RegisterType::Y, IndexRegisterType::X>>>; // STY Op, X
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '3', '9'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::StoreFromRegisterToMemory_AddressMode<T, RegisterType::A, IndexRegisterType::Y>>>; // STA Op, Y
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '3', 'A'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::StoreFromRegisterToMemory_AddressMode<T, RegisterType::C, IndexRegisterType::Y>>>; // STC Op, Y
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '3', 'B'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::StoreFromRegisterToMemory_AddressMode<T, RegisterType::D, IndexRegisterType::Y>>>; // STD Op, Y
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '3', 'C'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::StoreFromRegisterToMemory_AddressMode<T, RegisterType::X, IndexRegisterType::Y>>>; // STX Op, Y
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '0'>())] = Instruction::AddFromMemoryToAccumulator_ImmediateValueMode<T, CarryBorrowType::None>; // ADD #Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '1'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::AddFromMemoryToAccumulator_AddressMode<T, CarryBorrowType::None, IndexRegisterType::None>>>; // ADD Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '2'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::AddFromMemoryToAccumulator_AddressMode<T, CarryBorrowType::None, IndexRegisterType::X>>>; // ADD Op, X
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '3'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::AddFromMemoryToAccumulator_AddressMode<T, CarryBorrowType::None, IndexRegisterType::Y>>>; // ADD Op, Y
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '4'>())] = Instruction::AddFromDataRegisterToAccumulator<T, CarryBorrowType::None>; // ADDD
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '5'>())] = Instruction::AddFromMemoryToAccumulator_ImmediateValueMode<T, CarryBorrowType::Carry>; // ADDC #Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '6'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::AddFromMemoryToAccumulator_AddressMode<T, CarryBorrowType::Carry, IndexRegisterType::None>>>; // ADDC Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '7'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::AddFromMemoryToAccumulator_AddressMode<T, CarryBorrowType::Carry, IndexRegisterType::X>>>; // ADDC Op, X
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '8'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::AddFromMemoryToAccumulator_AddressMode<T, CarryBorrowType::Carry, IndexRegisterType::Y>>>; // ADDC Op, Y
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', '9'>())] = Instruction::AddFromDataRegisterToAccumulator<T, CarryBorrowType::Carry>; // ADDDC
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', 'A'>())] = Instruction::AddFromMemoryToAccumulator_ImmediateValueMode<T, CarryBorrowType::Borrow>; // ADDB #Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', 'B'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::AddFromMemoryToAccumulator_AddressMode<T, CarryBorrowType::Borrow, IndexRegisterType::None>>>; // ADDB Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', 'C'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::AddFromMemoryToAccumulator_AddressMode<T, CarryBorrowType::Borrow, IndexRegisterType::X>>>; // ADDB Op, X
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', 'D'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::AddFromMemoryToAccumulator_AddressMode<T, CarryBorrowType::Borrow, IndexRegisterType::Y>>>; // ADDB Op, Y
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '4', 'E'>())] = Instruction::AddFromDataRegisterToAccumulator<T, CarryBorrowType::Borrow>; // ADDDB
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', '0'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::C, RegisterType::A>; // TCA
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', '1'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::D, RegisterType::A>; // TDA
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', '2'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::X, RegisterType::A>; // TXA
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', '3'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::Y, RegisterType::A>; // TYA
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', '4'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::A, RegisterType::C>; // TAC
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', '5'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::D, RegisterType::C>; // TDC
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', '6'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::X, RegisterType::C>; // TXC
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', '7'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::Y, RegisterType::C>; // TYC
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', '8'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::A, RegisterType::D>; // TAD
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', '9'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::C, RegisterType::D>; // TCD
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'A'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::X, RegisterType::D>; // TXD
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'B'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::Y, RegisterType::D>; // TYD
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'C'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::A, RegisterType::X>; // TAX
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'D'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::C, RegisterType::X>; // TCX
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'E'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::D, RegisterType::X>; // TDX
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'F'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::Y, RegisterType::X>; // TYX
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'G'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::A, RegisterType::Y>; // TAY
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'H'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::C, RegisterType::Y>; // TCY
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'I'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::D, RegisterType::Y>; // TDY
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'J'>())] = Instruction::TransferFromRegisterToRegister<T, RegisterType::X, RegisterType::Y>; // TXY
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'K'>())] = Instruction::TransferFromIndexRegisterXToStackPointerLowTryte<T, InstructionCallbackType, Instruction::TransferFromIndexRegisterYToStackPointerHighTryte<T>>; // TYXSP
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'L'>())] = Instruction::TransferFromStackPointerLowTryteToIndexRegisterX<T, InstructionCallbackType, Instruction::TransferFromStackPointerHighTryteToIndexRegisterY<T>>; // TSPYX
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', '0'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::JumpToNewLocation<T>>>; // JMP Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', '1'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::Positive>>>; // BPOS Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', '2'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::Zero>>>; // BZERO Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', '3'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::Negative>>>; // BNEG Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', '4'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::NotPositive>>>; // BNPOS Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', '5'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::NotZero>>>; // BNZERO Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', '6'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::NotNegative>>>; // BNNEG Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', '7'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::Overflow>>>; // BOVRF Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', '8'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::Neutral>>>; // BNEUT Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', '9'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::Underflow>>>; // BUNDRF Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', 'A'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::NotOverflow>>>; // BNOVRF Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', 'B'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::NotNeutral>>>; // BNNEUT Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', 'C'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::NotUnderflow>>>; // BNUNDRF Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', 'D'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::Carry>>>; // BCRRY Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', 'E'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::NoCarryBorrow>>>; // BNCYBW Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', 'F'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::Borrow>>>; // BBRRW Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', 'G'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::NotCarry>>>; // BNCRRY Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', 'H'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::NotNoCarryBorrow>>>; // BNNCYBW Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'C', 'I'>())] = Instruction::ReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte<T, InstructionCallbackType, Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest<T, InstructionCallbackType, Instruction::BranchToNewLocation<T>, BranchTestType::NotBorrow>>>; // BNBBRW Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'P', '0'>())] = Instruction::SetFlagStateOperation<T, SetFlagType::OverflowUnderflow>; // SETOU #Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'P', '1'>())] = Instruction::SetFlagStateOperation<T, SetFlagType::CarryBorrow>; // SETCB #Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'P', '2'>())] = Instruction::SetFlagStateOperation<T, SetFlagType::Interrupt>; // SETI #Op
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, 'P', '3'>())] = Instruction::SetFlagStateOperation<T, SetFlagType::BalancedUnbalancedControl>; // SETBU #Op
		return table;
	}

	template <typename MachineType, typename BalancedMemoryReadFuncType, typename BalancedMemoryWriteFuncType, typename UnbalancedMemoryReadFuncType, typename UnbalancedMemoryWriteFuncType>
	class CPU
	{
		public:
			CPU(double cycles_per_second, MachineType *Machine, BalancedMemoryReadFuncType balanced_memory_read, BalancedMemoryWriteFuncType balanced_memory_write, UnbalancedMemoryReadFuncType unbalanced_memory_read, UnbalancedMemoryWriteFuncType unbalanced_memory_write) : A(0), C(0), D(0), X(0), Y(0), F(BCT::GetTryteFromTritData<0, +1, +1, 0, 0, 0>()), PC(BCT::GetValueInSeptemvigesimal<BCT::UWord, '0', '2', '0', '0'>()), SP(BCT::GetValueInSeptemvigesimal<BCT::UWord, '2', 'Q', 'Q', 'Q'>()), data_bus(0), address(0), current_cycle_state(CycleState::Fetch), cycles_per_second(cycles_per_second), CurrentMachine(Machine), instruction_callback(nullptr), balanced_memory_read(balanced_memory_read), balanced_memory_write(balanced_memory_write), unbalanced_memory_read(unbalanced_memory_read),  unbalanced_memory_write(unbalanced_memory_write), instruction_table(ConstructInstructionTable<CPU<MachineType, BalancedMemoryReadFuncType, BalancedMemoryWriteFuncType, UnbalancedMemoryReadFuncType, UnbalancedMemoryWriteFuncType>, void (*)(CPU<MachineType, BalancedMemoryReadFuncType, BalancedMemoryWriteFuncType, UnbalancedMemoryReadFuncType, UnbalancedMemoryWriteFuncType> *)>())
			{
			}

			~CPU() = default;
			
			inline double GetCyclesPerSecond() const
			{
				return cycles_per_second;
			}
			
			void operator()()
			{
				switch (current_cycle_state)
				{
					case CycleState::Fetch:
					{
						BCT::UTryte opcode = unbalanced_memory_read(PC, CurrentMachine);
						++PC;
						instruction_callback = instruction_table[static_cast<uint16_t>(opcode)];
						current_cycle_state = CycleState::Execute;
						break;
					}
					case CycleState::Execute:
					{
						instruction_callback(this);
						break;
					}
				}
			}

			template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback> requires HasCurrentCycleState<T>
			friend void Instruction::NoOperation(T *obj);

			template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback>
			friend void Instruction::ReadUnbalancedTryte(T *obj);

			template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback>
			friend void Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryte(T *obj);

			template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback>
			friend void Instruction::LoadDataIntoAddressLowerTryteAndReadUnbalancedTryteFromInitialAddress(T *obj);

			template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback>
			friend void Instruction::LoadDataIntoAddressUpperTryteAndReadUnbalancedTryteFromAddress(T *obj);

			template <typename T, RegisterType destination_register> requires HasCurrentCycleState<T>
			friend void Instruction::LoadFromMemoryToRegister_ImmediateValueMode(T *obj);

			template <typename T, RegisterType destination_register, IndexRegisterType index_register_used> requires HasCurrentCycleState<T>
			friend void Instruction::LoadFromMemoryToRegister_AddressMode(T *obj);

			template <typename T, RegisterType source_register, IndexRegisterType index_register_used> requires HasCurrentCycleState<T>
			friend void Instruction::StoreFromRegisterToMemory_AddressMode(T *obj);

			template <typename T, CarryBorrowType carry_borrow> requires HasCurrentCycleState<T>
			friend void Instruction::AddFromMemoryToAccumulator_ImmediateValueMode(T *obj);

			template <typename T, CarryBorrowType carry_borrow, IndexRegisterType index_register_used> requires HasCurrentCycleState<T>
			friend void Instruction::AddFromMemoryToAccumulator_AddressMode(T *obj);

			template <typename T, CarryBorrowType carry_borrow> requires HasCurrentCycleState<T>
			friend void Instruction::AddFromDataRegisterToAccumulator(T *obj);

			template <typename T, RegisterType source_register, RegisterType destination_register>
			friend void Instruction::TransferFromRegisterToRegister(T *obj);

			template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback>
			friend void Instruction::TransferFromIndexRegisterXToStackPointerLowTryte(T *obj);

			template <typename T> requires HasCurrentCycleState<T>
			friend void Instruction::TransferFromIndexRegisterYToStackPointerHighTryte(T *obj);

			template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback>
			friend void Instruction::TransferFromStackPointerLowTryteToIndexRegisterX(T *obj);

			template <typename T> requires HasCurrentCycleState<T>
			friend void Instruction::TransferFromStackPointerHighTryteToIndexRegisterY(T *obj);

			template <typename T> requires HasCurrentCycleState<T>
			friend void Instruction::JumpToNewLocation(T *obj);

			template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback, BranchTestType branch_test> requires HasCurrentCycleState<T>
			friend void Instruction::LoadDataIntoAddressUpperTryteAndPerformBranchTest(T *obj);

			template <typename T> requires HasCurrentCycleState<T>
			friend void Instruction::BranchToNewLocation(T *obj);

			template <typename T, SetFlagType flag> requires HasCurrentCycleState<T>
			friend void Instruction::SetFlagStateOperation(T *obj);
		private:
			BCT::Tryte A; // Accumulator Register
			BCT::Tryte C; // Counter Register
			BCT::Tryte D; // Data Register
			BCT::Tryte X; // Index Register X
			BCT::Tryte Y; // Index Register Y
			BCT::Tryte F; // Flag Register
			BCT::UWord PC; // Program Counter
			BCT::UWord SP; // Stack Pointer
			BCT::Tryte data_bus; // Data Bus
			BCT::UWord address; // Address (Used internally to store address data throughout different cycles.)
			CycleState current_cycle_state;
			double cycles_per_second;
			MachineType *CurrentMachine;

			void (*instruction_callback)(CPU<MachineType, BalancedMemoryReadFuncType, BalancedMemoryWriteFuncType, UnbalancedMemoryReadFuncType, UnbalancedMemoryWriteFuncType> *);

			BalancedMemoryReadFuncType balanced_memory_read;
			BalancedMemoryWriteFuncType balanced_memory_write;
			UnbalancedMemoryReadFuncType unbalanced_memory_read;
			UnbalancedMemoryWriteFuncType unbalanced_memory_write;

			const std::array<void (*)(CPU<MachineType, BalancedMemoryReadFuncType, BalancedMemoryWriteFuncType, UnbalancedMemoryReadFuncType, UnbalancedMemoryWriteFuncType> *), 729> instruction_table;
	};
}

#endif
