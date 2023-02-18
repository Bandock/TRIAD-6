#ifndef _UTILITY_HPP_
#define _UTILITY_HPP_

#include <type_traits>

namespace TRIAD_6
{
	namespace Utility
	{
		template <typename T, typename... Types>
		consteval bool CheckCompatibleType()
		{
			bool result = std::disjunction<std::is_same<T, Types>...>();
			return result;
		}

		consteval size_t KiloTrytesToTrytes(size_t kilotrytes) // KiloTrytes to Trytes Conversion Function (Compile-Time Only)
		{
			size_t result = kilotrytes;
			for (size_t i = 0; i < 10; ++i)
			{
				result *= 3;
			}
			return result;
		}
		
		constexpr size_t BCT_TrytesToBytes(size_t trytes) // Binary Coded Ternary Trytes To Bytes Conversion Function (Useable at Compile-Time and Run-Time)
		{
			size_t result = (trytes * (2 * 6));
			size_t tmp = result / 8;
			result = tmp + ((result % 8) ? 1 : 0);
			return result;
		}

		template <typename T, typename... Types>
		concept TypeCompatible = CheckCompatibleType<T, Types...>();
	}
}

#endif
