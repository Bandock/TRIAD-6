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

	enum class SetFlagType
	{
		OverflowUnderflow, CarryBorrow, Interrupt, BalancedUnbalancedControl
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

		template <typename T, RegisterType destination_register> requires HasCurrentCycleState<T>
		void LoadFromMemoryToRegister_ImmediateValueMode(T *obj)
		{
			if constexpr (destination_register == RegisterType::A)
			{
				BCT::Tryte bu_flag_check = BCT::GetTryteFromTritData<-1, +1, -1, -1, -1, -1>();
				if ((obj->F & bu_flag_check) == bu_flag_check)
				{
					obj->A = obj->balanced_memory_read(obj->PC, obj->CurrentMachine);
					fmt::print("Signed Accumulator:  {}\n", static_cast<int16_t>(obj->A));
				}
				else if ((~(obj->F) & bu_flag_check) == bu_flag_check)
				{
					obj->A = obj->unbalanced_memory_read(obj->PC, obj->CurrentMachine);
					fmt::print("Unsigned Accumulator:  {}\n", static_cast<uint16_t>(obj->A));
				}
				++obj->PC;
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
		void TransferFromIndexRegisterXToStackPointerLowByte(T *obj)
		{
			BCT::UTryte tmp = obj->X;
			obj->SP ^= BCT::GetValueInSeptemvigesimal<BCT::UWord, 'D', 'D', '0', '0'>();
			obj->SP += tmp;
			obj->instruction_callback = next_callback;
		}

		template <typename T> requires HasCurrentCycleState<T>
		void TransferFromIndexRegisterYToStackPointerHighByte(T *obj)
		{
			BCT::UTryte tmp = obj->Y;
			obj->SP ^= BCT::GetValueInSeptemvigesimal<BCT::UWord, '0', '0', 'D', 'D'>();
			obj->SP += (static_cast<BCT::UWord>(tmp) << 6);
			obj->current_cycle_state = CycleState::Fetch;
		}

		template <typename T, typename InstructionCallback, InstructionCallback next_callback>
		void TransferFromStackPointerLowByteToIndexRegisterX(T *obj)
		{
			BCT::UTryte tmp = obj->SP;
			obj->X = tmp;
			obj->instruction_callback = next_callback;
		}

		template <typename T> requires HasCurrentCycleState<T>
		void TransferFromStackPointerHighByteToIndexRegisterY(T *obj)
		{
			BCT::UTryte tmp = (obj->SP >> 6);
			obj->Y = tmp;
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
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'K'>())] = Instruction::TransferFromIndexRegisterXToStackPointerLowByte<T, InstructionCallbackType, Instruction::TransferFromIndexRegisterYToStackPointerHighByte<T>>; // TYXSP
		table[static_cast<uint16_t>(BCT::GetValueInSeptemvigesimal<BCT::UTryte, '8', 'L'>())] = Instruction::TransferFromStackPointerLowByteToIndexRegisterX<T, InstructionCallbackType, Instruction::TransferFromStackPointerHighByteToIndexRegisterY<T>>; // TSPYX
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
			CPU(double cycles_per_second, MachineType *Machine, BalancedMemoryReadFuncType balanced_memory_read, BalancedMemoryWriteFuncType balanced_memory_write, UnbalancedMemoryReadFuncType unbalanced_memory_read, UnbalancedMemoryWriteFuncType unbalanced_memory_write) : A(0), C(0), D(0), X(0), Y(0), F(0), PC(1458), SP(59048), data(0), address(0), current_cycle_state(CycleState::Fetch), cycles_per_second(cycles_per_second), CurrentMachine(Machine), instruction_callback(nullptr), balanced_memory_read(balanced_memory_read), balanced_memory_write(balanced_memory_write), unbalanced_memory_read(unbalanced_memory_read),  unbalanced_memory_write(unbalanced_memory_write), instruction_table(ConstructInstructionTable<CPU<MachineType, BalancedMemoryReadFuncType, BalancedMemoryWriteFuncType, UnbalancedMemoryReadFuncType, UnbalancedMemoryWriteFuncType>, void (*)(CPU<MachineType, BalancedMemoryReadFuncType, BalancedMemoryWriteFuncType, UnbalancedMemoryReadFuncType, UnbalancedMemoryWriteFuncType> *)>())
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

			template <typename T, RegisterType destination_register> requires HasCurrentCycleState<T>
			friend void Instruction::LoadFromMemoryToRegister_ImmediateValueMode(T *obj);

			template <typename T, RegisterType source_register, RegisterType destination_register>
			friend void Instruction::TransferFromRegisterToRegister(T *obj);

			template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback>
			friend void Instruction::TransferFromIndexRegisterXToStackPointerLowByte(T *obj);

			template <typename T> requires HasCurrentCycleState<T>
			friend void Instruction::TransferFromIndexRegisterYToStackPointerHighByte(T *obj);

			template <typename T, typename InstructionCallbackType, InstructionCallbackType next_callback>
			friend void Instruction::TransferFromStackPointerLowByteToIndexRegisterX(T *obj);

			template <typename T> requires HasCurrentCycleState<T>
			friend void Instruction::TransferFromStackPointerHighByteToIndexRegisterY(T *obj);

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
			BCT::Tryte data; // Data Bus
			BCT::UWord address; // Address Bus
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
