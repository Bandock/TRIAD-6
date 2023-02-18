#ifndef _BCT_HPP_
#define _BCT_HPP_

#include <concepts>
#include <type_traits>
#include <cstdint>
#include <cstring>
#include <array>
#include "utility.hpp"

namespace TRIAD_6
{
	namespace BCT // Binary Coded Ternary Namespace
	{
		struct UTryte;
		struct UWord;

		struct Tryte // Signed Tryte (for the balanced ternary system), use a 6-trit design for a tryte.
		{
			constexpr Tryte()
			{
				// memset(data.data(), 0, data.size());
				for (size_t i = 0; i < data.size(); ++i)
				{
					data[i] = 0;
				}
			}

			template <typename T> requires Utility::TypeCompatible<T, int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t, UTryte, UWord>
			constexpr Tryte(T value)
			{
				// memset(data.data(), 0, data.size());
				for (size_t i = 0; i < data.size(); ++i)
				{
					data[i] = 0;
				}
				if constexpr (Utility::CheckCompatibleType<T, int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t>())
				{
					int16_t s_value = value; // Force to use 16-bit signed integer for proper storage.
					if (s_value > 364)
					{
						s_value -= 729 * ((s_value + 365) / 729);
					}
					else if (s_value < -364)
					{
						s_value += 729 * (-(s_value - 365) / 729);
					}
					if (s_value >= 0)
					{
						size_t byte_offset = 0;
						size_t bit_offset = 0;
						for (size_t t_offset = 0; t_offset < 6; ++t_offset)
						{
							uint8_t c_data = (s_value + 1) % 3;
							data[byte_offset] |= (c_data << bit_offset);
							bit_offset += 2;
							if (bit_offset == 8)
							{
								bit_offset = 0;
								++byte_offset;
							}
							s_value = (s_value + 1) / 3;
						}
					}
					else
					{
						size_t byte_offset = 0;
						size_t bit_offset = 0;
						for (size_t t_offset = 0; t_offset < 6; ++t_offset)
						{
							uint8_t c_data = ((s_value - 1) % 3) + 2;
							data[byte_offset] |= (c_data << bit_offset);
							bit_offset += 2;
							if (bit_offset == 8)
							{
								bit_offset = 0;
								++byte_offset;
							}
							s_value = (s_value - 1) / 3;
						}
					}
				}
				else if constexpr(Utility::CheckCompatibleType<T, UTryte, UWord>())
				{
					Tryte result = value;
					data = result.data;
				}
			}

			constexpr Tryte(const Tryte &source)
			{
				data = source.data;
			}

			void operator++()
			{
				int16_t value = *this;
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				++value;
				if (value > 364)
				{
					value = -364;
				}
				memset(data.data(), 0, data.size());
				if (value >= 0)
				{
					for (size_t t_offset = 0; t_offset < 6; ++t_offset)
					{
						uint8_t c_data = (value + 1) % 3;
						data[byte_offset] |= (c_data << bit_offset);
						bit_offset += 2;
						if (bit_offset == 8)
						{
							bit_offset = 0;
							++byte_offset;
						}
						value = (value + 1) / 3;
					}
				}
				else
				{
					for (size_t t_offset = 0; t_offset < 6; ++t_offset)
					{
						uint8_t c_data = ((value - 1) % 3) + 2;
						data[byte_offset] |= (c_data << bit_offset);
						bit_offset += 2;
						if (bit_offset == 8)
						{
							bit_offset = 0;
							++byte_offset;
						}
						value = (value - 1) / 3;
					}
				}
			}

			void operator--()
			{
				int16_t value = *this;
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				--value;
				if (value < -364)
				{
					value = 364;
				}
				memset(data.data(), 0, data.size());
				if (value >= 0)
				{
					for (size_t t_offset = 0; t_offset < 6; ++t_offset)
					{
						uint8_t c_data = (value + 1) % 3;
						data[byte_offset] |= (c_data << bit_offset);
						bit_offset += 2;
						if (bit_offset == 8)
						{
							bit_offset = 0;
							++byte_offset;
						}
						value = (value + 1) / 3;
					}
				}
				else
				{
					for (size_t t_offset = 0; t_offset < 6; ++t_offset)
					{
						uint8_t c_data = ((value - 1) % 3) + 2;
						data[byte_offset] |= (c_data << bit_offset);
						bit_offset += 2;
						if (bit_offset == 8)
						{
							bit_offset = 0;
							++byte_offset;
						}
						value = (value - 1) / 3;
					}
				}
			}

			constexpr void operator+=(Tryte source) // Balanced Addition
			{
				int16_t value = *this + source;
				// memset(data.data(), 0, data.size());
				for (size_t i = 0; i < data.size(); ++i)
				{
					data[i] = 0;
				}
				if (value > 364)
				{
					value -= 729;
				}
				else if (value < -364)
				{
					value += 729;
				}
				if (value >= 0)
				{
					size_t byte_offset = 0;
					size_t bit_offset = 0;
					for (size_t t_offset = 0; t_offset < 6; ++t_offset)
					{
						uint8_t c_data = (value + 1) % 3;
						data[byte_offset] |= (c_data << bit_offset);
						bit_offset += 2;
						if (bit_offset == 8)
						{
							bit_offset = 0;
							++byte_offset;
						}
						value = (value + 1) / 3;
					}
				}
				else
				{
					size_t byte_offset = 0;
					size_t bit_offset = 0;
					for (size_t t_offset = 0; t_offset < 6; ++t_offset)
					{
						uint8_t c_data = ((value - 1) % 3) + 2;
						data[byte_offset] |= (c_data << bit_offset);
						bit_offset += 2;
						if (bit_offset == 8)
						{
							bit_offset = 0;
							++byte_offset;
						}
						value = (value - 1) / 3;
					}
				}
			}

			constexpr void operator-=(Tryte source) // Balanced Subtraction
			{
				int16_t value = *this - source;
				// memset(data.data(), 0, data.size());
				for (size_t i = 0; i < data.size(); ++i)
				{
					data[i] = 0;
				}
				if (value > 364)
				{
					value -= 729;
				}
				else if (value < -364)
				{
					value += 729;
				}
				if (value >= 0)
				{
					size_t byte_offset = 0;
					size_t bit_offset = 0;
					for (size_t t_offset = 0; t_offset < 6; ++t_offset)
					{
						uint8_t c_data = (value + 1) % 3;
						data[byte_offset] |= (c_data << bit_offset);
						bit_offset += 2;
						if (bit_offset == 8)
						{
							bit_offset = 0;
							++byte_offset;
						}
						value = (value + 1) / 3;
					}
				}
				else
				{
					size_t byte_offset = 0;
					size_t bit_offset = 0;
					for (size_t t_offset = 0; t_offset < 6; ++t_offset)
					{
						uint8_t c_data = ((value - 1) % 3) + 2;
						data[byte_offset] |= (c_data << bit_offset);
						bit_offset += 2;
						if (bit_offset == 8)
						{
							bit_offset = 0;
							++byte_offset;
						}
						value = (value - 1) / 3;
					}
				}
			}

			constexpr void operator<<=(int shift) // Balanced Tritwise Shift Left Operator
			{
				for (int i = 0; i < shift; ++i)
				{
					uint8_t c_data = (data[0] & 0xC0);
					data[0] <<= 2;
					data[0] |= 0x01;
					data[1] = (data[1] << 2) & 0x0F;
					data[1] |= (c_data >> 6);
				}
			}

			constexpr void operator>>=(int shift) // Balanced Tritwise Shift Right Operator
			{
				for (int i = 0; i < shift; ++i)
				{
					data[0] >>= 2;
					uint8_t c_data = (data[1] & 0x03);
					data[1] >>= 2;
					data[1] |= 0x04;
					data[0] |= (c_data << 6);
				}
			}

			constexpr void operator|=(Tryte source) // Balanced Tritwise OR Operator
			{
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					uint8_t src_data = ((source.data[byte_offset] >> bit_offset) & 0x03);
					uint8_t dest_data = ((data[byte_offset] >> bit_offset) & 0x03);
					data[byte_offset] &= ~(0x03 << bit_offset);
					switch (src_data)
					{
						case 0x0:
						{
							switch (dest_data)
							{
								case 0x1:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
								case 0x2:
								{
									data[byte_offset] |= (0x2 << bit_offset);
									break;
								}
							}
							break;
						}
						case 0x1:
						{
							switch (dest_data)
							{
								case 0x0:
								case 0x1:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
								case 0x2:
								{
									data[byte_offset] |= (0x2 << bit_offset);
									break;
								}
							}
							break;
						}
						case 0x2:
						{
							data[byte_offset] |= (0x2 << bit_offset);
							break;
						}
					}
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
			}

			constexpr void operator &=(Tryte source) // Balanced Tritwise AND Operator
			{
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					uint8_t src_data = ((source.data[byte_offset] >> bit_offset) & 0x03);
					uint8_t dest_data = ((data[byte_offset] >> bit_offset) & 0x03);
					data[byte_offset] &= ~(0x03 << bit_offset);
					switch (src_data)
					{
						case 0x1:
						{
							switch (dest_data)
							{
								case 0x1:
								case 0x2:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
							}
							break;
						}
						case 0x2:
						{
							switch (dest_data)
							{
								case 0x1:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
								case 0x2:
								{
									data[byte_offset] |= (0x2 << bit_offset);
									break;
								}
							}
							break;
						}
					}
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
			}

			constexpr void operator ^=(Tryte source) // Balanced Tritwise XOR Operator
			{
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					uint8_t src_data = ((source.data[byte_offset] >> bit_offset) & 0x03);
					uint8_t dest_data = ((data[byte_offset] >> bit_offset) & 0x03);
					data[byte_offset] &= ~(0x03 << bit_offset);
					switch (src_data)
					{
						case 0x0:
						{
							switch (dest_data)
							{
								case 0x1:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
								case 0x2:
								{
									data[byte_offset] |= (0x2 << bit_offset);
									break;
								}
							}
							break;
						}
						case 0x1:
						{
							data[byte_offset] |= (0x1 << bit_offset);
							break;
						}
						case 0x2:
						{
							switch (dest_data)
							{
								case 0x0:
								{
									data[byte_offset] |= (0x2 << bit_offset);
									break;
								}
								case 0x1:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
							}
							break;
						}
					}
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
			}

			constexpr operator int16_t() const
			{
				int16_t value = 0;
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				int16_t multiplier = 1;
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					value += (((data[byte_offset] >> bit_offset) & 0x03) - 1) * multiplier;
					multiplier *= 3;
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
				return value;
			}

			constexpr operator UTryte() const;
			constexpr operator UWord() const;

			std::array<uint8_t, 2> data;
		};

		struct UTryte // Unsigned Tryte (for the unbalanced ternary system), use a 6-trit design for a tryte.
		{
			constexpr UTryte()
			{
				// memset(data.data(), 0, data.size());
				for (size_t i = 0; i < data.size(); ++i)
				{
					data[i] = 0;
				}
			}

			template <typename T> requires Utility::TypeCompatible<T, int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t, Tryte, UWord>
			constexpr UTryte(T value)
			{
				// memset(data.data(), 0, data.size());
				for (size_t i = 0; i < data.size(); ++i)
				{
					data[i] = 0;
				}
				if constexpr (Utility::CheckCompatibleType<T, int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t>())
				{
					uint16_t u_value = value; // Force to use 16-bit unsigned integer for storage.
					if (u_value > 728)
					{
						u_value -= 729 * (u_value / 729);
					}
					size_t byte_offset = 0;
					size_t bit_offset = 0;
					for (size_t t_offset = 0; t_offset < 6; ++t_offset)
					{
						uint8_t c_data = u_value % 3;
						data[byte_offset] |= (c_data << bit_offset);
						bit_offset += 2;
						if (bit_offset == 8)
						{
							bit_offset = 0;
							++byte_offset;
						}
						u_value /= 3;
					}
				}
				else if constexpr (Utility::CheckCompatibleType<T, Tryte, UWord>())
				{
					UTryte result = value;
					data = result.data;
				}
			}

			constexpr UTryte(const UTryte &source)
			{
				data = source.data;
			}

			void operator++()
			{
				uint16_t value = *this;
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				++value;
				if (value > 728)
				{
					value = 0;
				}
				memset(data.data(), 0, data.size());
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					uint8_t c_data = value % 3;
					data[byte_offset] |= (c_data << bit_offset);
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
					value /= 3;
				}
			}

			void operator--()
			{
				uint16_t value = *this;
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				--value;
				if (value > 728)
				{
					value = 728;
				}
				memset(data.data(), 0, data.size());
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					uint8_t c_data = value % 3;
					data[byte_offset] |= (c_data << bit_offset);
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
					value /= 3;
				}
			}

			constexpr void operator+=(UTryte source) // Unbalanced Addition
			{
				uint16_t value = *this + source;
				// memset(data.data(), 0, data.size());
				for (size_t i = 0; i < data.size(); ++i)
				{
					data[i] = 0;
				}
				if (value > 728)
				{
					value -= 729;
				}
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					uint8_t c_data = value % 3;
					data[byte_offset] |= (c_data << bit_offset);
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
					value /= 3;
				}
			}

			constexpr void operator-=(UTryte source) // Unbalanced Subtraction
			{
				uint16_t value = *this - source;
				// memset(data.data(), 0, data.size());
				for (size_t i = 0; i < data.size(); ++i)
				{
					data[i] = 0;
				}
				if (value > 728)
				{
					uint16_t adjust = (65535 - value);
					value = 728 - adjust;
				}
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					uint8_t c_data = value % 3;
					data[byte_offset] |= (c_data << bit_offset);
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
					value /= 3;
				}
			}

			constexpr void operator<<=(int shift) // Unbalanced Tritwise Shift Left Operator
			{
				for (int i = 0; i < shift; ++i)
				{
					uint8_t c_data = (data[0] & 0xC0);
					data[0] <<= 2;
					data[1] = (data[1] << 2) & 0x0F;
					data[1] |= (c_data >> 6);
				}
			}

			constexpr void operator>>=(int shift) // Unbalanced Tritwise Shift Right Operator
			{
				for (int i = 0; i < shift; ++i)
				{
					data[0] >>= 2;
					uint8_t c_data = (data[1] & 0x03);
					data[1] >>= 2;
					data[0] |= (c_data << 6);
				}
			}

			constexpr void operator|=(UTryte source) // Unbalanced Tritwise OR Operator
			{
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					uint8_t src_data = ((source.data[byte_offset] >> bit_offset) & 0x03);
					uint8_t dest_data = ((data[byte_offset] >> bit_offset) & 0x03);
					data[byte_offset] &= ~(0x03 << bit_offset);
					switch (src_data)
					{
						case 0x0:
						{
							if (dest_data == 0x2)
							{
								data[byte_offset] |= (0x2 << bit_offset);
							}
							break;
						}
						case 0x1:
						{
							switch (dest_data)
							{
								case 0x1:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
								case 0x2:
								{
									data[byte_offset] |= (0x2 << bit_offset);
									break;
								}
							}
							break;
						}
						case 0x2:
						{
							data[byte_offset] |= (0x2 << bit_offset);
							break;
						}
					}
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
			}

			constexpr void operator &=(UTryte source) // Unbalanced Tritwise AND Operator
			{
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					uint8_t src_data = ((source.data[byte_offset] >> bit_offset) & 0x03);
					uint8_t dest_data = ((data[byte_offset] >> bit_offset) & 0x03);
					data[byte_offset] &= ~(0x03 << bit_offset);
					switch (src_data)
					{
						case 0x0:
						{
							if (dest_data == 0x1)
							{
								data[byte_offset] |= (0x1 << bit_offset);
							}
							break;
						}
						case 0x1:
						{
							data[byte_offset] |= (0x1 << bit_offset);
							break;
						}
						case 0x2:
						{
							switch (dest_data)
							{
								case 0x1:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
								case 0x2:
								{
									data[byte_offset] |= (0x2 << bit_offset);
									break;
								}
							}
							break;
						}
					}
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
			}

			constexpr void operator ^=(UTryte source) // Unbalanced Tritwise XOR Operator
			{
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					uint8_t src_data = ((source.data[byte_offset] >> bit_offset) & 0x03);
					uint8_t dest_data = ((data[byte_offset] >> bit_offset) & 0x03);
					data[byte_offset] &= ~(0x03 << bit_offset);
					switch (src_data)
					{
						case 0x1:
						{
							switch (dest_data)
							{
								case 0x1:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
								case 0x2:
								{
									data[byte_offset] |= (0x2 << bit_offset);
									break;
								}
							}
							break;
						}
						case 0x2:
						{
							switch (dest_data)
							{
								case 0x1:
								{
									data[byte_offset] |= (0x2 << bit_offset);
									break;
								}
								case 0x2:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
							}
							break;
						}
					}
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
			}

			constexpr operator uint16_t() const
			{
				uint16_t value = 0;
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				uint16_t multiplier = 1;
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					value += ((data[byte_offset] >> bit_offset) & 0x03) * multiplier;
					multiplier *= 3;
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
				return value;
			}

			constexpr operator Tryte() const // Unsigned Tryte to Signed Tryte Conversion
			{
				Tryte result = 0;
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				bool carry = false;
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					uint8_t c_data = ((data[byte_offset] >> bit_offset) & 0x03);
					uint8_t tmp = c_data + 1 + carry;
					if (tmp > 0x2)
					{
						carry = true;
						tmp -= 0x3;
					}
					else
					{
						carry = false;
					}
					result.data[byte_offset] &= ~(0x03 << bit_offset);
					result.data[byte_offset] |= (tmp << bit_offset);
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
				return result;
			}

			constexpr operator UWord() const;

			std::array<uint8_t, 2> data;
		};

		struct UWord // Unbalanced Word (for the unbalanced ternary system), use a 12-trit (2 trytes) design for a word.
		{
			constexpr UWord()
			{
				// memset(data.data(), 0, data.size());
				for (size_t i = 0; i < data.size(); ++i)
				{
					data[i] = 0;
				}
			}

			template <typename T> requires Utility::TypeCompatible<T, int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t, Tryte, UTryte>
			constexpr UWord(T value)
			{
				// memset(data.data(), 0, data.size());
				for (size_t i = 0; i < data.size(); ++i)
				{
					data[i] = 0;
				}
				if constexpr (Utility::CheckCompatibleType<T, int8_t, int16_t, int32_t, uint8_t, uint16_t, uint32_t>())
				{
					uint32_t u_value = value; // Force to use 32-bit unsigned integer for storage.
					if (u_value > 531440)
					{
						u_value -= 531441 * (u_value / 531441);
					}
					size_t byte_offset = 0;
					size_t bit_offset = 0;
					for (size_t t_offset = 0; t_offset < 12; ++t_offset)
					{
						uint8_t c_data = u_value % 3;
						data[byte_offset] |= (c_data << bit_offset);
						bit_offset += 2;
						if (bit_offset == 8)
						{
							bit_offset = 0;
							++byte_offset;
						}
						u_value /= 3;
					}
				}
				else if constexpr (Utility::CheckCompatibleType<T, Tryte, UTryte>())
				{
					UWord result = value;
					data = result.data;
				}
			}

			constexpr UWord(const UWord &source)
			{
				data = source.data;
			}

			void operator++()
			{
				uint32_t value = *this;
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				++value;
				if (value > 531440)
				{
					value = 0;
				}
				memset(data.data(), 0, data.size());
				for (size_t t_offset = 0; t_offset < 12; ++t_offset)
				{
					uint8_t c_data = value % 3;
					data[byte_offset] |= (c_data << bit_offset);
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
					value /= 3;
				}
			}

			void operator--()
			{
				uint32_t value = *this;
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				--value;
				if (value > 531440)
				{
					value = 531440;
				}
				memset(data.data(), 0, data.size());
				for (size_t t_offset = 0; t_offset < 12; ++t_offset)
				{
					uint8_t c_data = value % 3;
					data[byte_offset] |= (c_data << bit_offset);
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
					value /= 3;
				}
			}

			constexpr void operator+=(UWord source) // Unbalanced Word Addition
			{
				uint32_t value = *this + source;
				// memset(data.data(), 0, data.size());
				for (size_t i = 0; i < data.size(); ++i)
				{
					data[i] = 0;
				}
				if (value > 531440)
				{
					value -= 531441;
				}
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				for (size_t t_offset = 0; t_offset < 12; ++t_offset)
				{
					uint8_t c_data = value % 3;
					data[byte_offset] |= (c_data << bit_offset);
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
					value /= 3;
				}
			}

			constexpr void operator-=(UWord source) // Unbalanced Word Subtraction
			{
				uint32_t value = *this - source;
				memset(data.data(), 0, data.size());
				if (value > 531440)
				{
					uint32_t adjust = (0xFFFFFFFF - value);
					value = 531440 - adjust;
				}
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				for (size_t t_offset = 0; t_offset < 12; ++t_offset)
				{
					uint8_t c_data = value % 3;
					data[byte_offset] |= (c_data << bit_offset);
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
					value /= 3;
				}
			}

			constexpr void operator<<=(int shift) // Unbalance Tritwise Shift Left Operator (Word Variant)
			{
				for (int i = 0; i < shift; ++i)
				{
					uint8_t c_data = (data[0] & 0xC0);
					uint8_t c_data_2 = (data[1] & 0xC0);
					data[0] <<= 2;
					data[1] <<= 2;
					data[1] |= (c_data >> 6);
					data[2] <<= 2;
					data[2] |= (c_data_2 >> 6);

				}
			}

			constexpr void operator>>=(int shift) // Unbalanced Tritwise Shift Right Operator (Word Variant)
			{
				for (int i = 0; i < shift; ++i)
				{
					data[0] >>= 2;
					uint8_t c_data = (data[1] & 0x03);
					data[1] >>= 2;
					data[0] |= (c_data << 6);
					c_data = (data[2] & 0x03);
					data[2] >>= 2;
					data[1] |= (c_data << 6);
				}
			}

			constexpr void operator|=(UWord source) // Unbalanced Tritwise OR Operator (Word Variant)
			{
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				for (size_t t_offset = 0; t_offset < 12; ++t_offset)
				{
					uint8_t src_data = ((source.data[byte_offset] >> bit_offset) & 0x03);
					uint8_t dest_data = ((data[byte_offset] >> bit_offset) & 0x03);
					data[byte_offset] &= ~(0x03 << bit_offset);
					switch (src_data)
					{
						case 0x0:
						{
							if (dest_data == 0x2)
							{
								data[byte_offset] |= (0x2 << bit_offset);
							}
							break;
						}
						case 0x1:
						{
							switch (dest_data)
							{
								case 0x1:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
								case 0x2:
								{
									data[byte_offset] |= (0x2 << bit_offset);
									break;
								}
							}
							break;
						}
						case 0x2:
						{
							data[byte_offset] |= (0x2 << bit_offset);
							break;
						}
					}
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
			}

			constexpr void operator &=(UWord source) // Unbalanced Tritwise AND Operator (Word Variant)
			{
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				for (size_t t_offset = 0; t_offset < 12; ++t_offset)
				{
					uint8_t src_data = ((source.data[byte_offset] >> bit_offset) & 0x03);
					uint8_t dest_data = ((data[byte_offset] >> bit_offset) & 0x03);
					data[byte_offset] &= ~(0x03 << bit_offset);
					switch (src_data)
					{
						case 0x0:
						{
							if (dest_data == 0x1)
							{
								data[byte_offset] |= (0x1 << bit_offset);
							}
							break;
						}
						case 0x1:
						{
							data[byte_offset] |= (0x1 << bit_offset);
							break;
						}
						case 0x2:
						{
							switch (dest_data)
							{
								case 0x1:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
								case 0x2:
								{
									data[byte_offset] |= (0x2 << bit_offset);
									break;
								}
							}
							break;
						}
					}
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
			}

			constexpr void operator ^=(UWord source) // Unbalanced Tritwise XOR Operator (Word Variant)
			{
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				for (size_t t_offset = 0; t_offset < 12; ++t_offset)
				{
					uint8_t src_data = ((source.data[byte_offset] >> bit_offset) & 0x03);
					uint8_t dest_data = ((data[byte_offset] >> bit_offset) & 0x03);
					data[byte_offset] &= ~(0x03 << bit_offset);
					switch (src_data)
					{
						case 0x1:
						{
							switch (dest_data)
							{
								case 0x1:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
								case 0x2:
								{
									data[byte_offset] |= (0x2 << bit_offset);
									break;
								}
							}
							break;
						}
						case 0x2:
						{
							switch (dest_data)
							{
								case 0x1:
								{
									data[byte_offset] |= (0x2 << bit_offset);
									break;
								}
								case 0x2:
								{
									data[byte_offset] |= (0x1 << bit_offset);
									break;
								}
							}
							break;
						}
					}
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
			}

			constexpr operator uint32_t() const
			{
				uint32_t value = 0;
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				uint32_t multiplier = 1;
				for (size_t t_offset = 0; t_offset < 12; ++t_offset)
				{
					value += ((data[byte_offset] >> bit_offset) & 0x03) * multiplier;
					multiplier *= 3;
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
				return value;
			}

			constexpr operator Tryte() const// Unsigned Word to Signed Tryte Conversion
			{
				Tryte result = 0;
				size_t byte_offset = 0;
				size_t bit_offset = 0;
				bool carry = false;
				for (size_t t_offset = 0; t_offset < 6; ++t_offset)
				{
					uint8_t c_data = ((data[byte_offset] >> bit_offset) & 0x03);
					uint8_t tmp = c_data + 1 + carry;
					if (tmp > 0x2)
					{
						carry = true;
						tmp -= 0x3;
					}
					else
					{
						carry = false;
					}
					result.data[byte_offset] &= ~(0x03 << bit_offset);
					result.data[byte_offset] |= (tmp << bit_offset);
					bit_offset += 2;
					if (bit_offset == 8)
					{
						bit_offset = 0;
						++byte_offset;
					}
				}
				return result;
			}

			constexpr operator UTryte() const // Unsigned Word to Unsigned Tryte Conversion
			{
				UTryte result = 0;
				// memcpy(&result.data[0], &data[0], 2);
				result.data[0] = data[0];
				result.data[1] = (data[1] & 0xF);
				return result;
			}

			std::array<uint8_t, 3> data;
		};

		constexpr Tryte::operator UTryte() const // Signed Tryte to Unsigned Tryte Conversion
		{
			UTryte result = 0;
			size_t byte_offset = 0;
			size_t bit_offset = 0;
			bool borrow = false;
			for (size_t t_offset = 0; t_offset < 6; ++t_offset)
			{
				uint8_t c_data = ((data[byte_offset] >> bit_offset) & 0x03);
				uint8_t tmp = c_data - 1;
				if ((tmp & 0x03) > 0x2)
				{
					--tmp;
				}
				tmp -= borrow;
				if ((tmp & 0x03) > 0x2)
				{
					--tmp;
				}
				borrow = (tmp > c_data);
				c_data = (tmp & 0x03);
				result.data[byte_offset] |= (c_data << bit_offset);
				bit_offset += 2;
				if (bit_offset == 8)
				{
					bit_offset = 0;
					++byte_offset;
				}
			}
			return result;
		}

		constexpr Tryte::operator UWord() const // Signed Tryte to Unsigned Word Conversion
		{
			UWord result = 0;
			size_t byte_offset = 0;
			size_t bit_offset = 0;
			bool borrow = false;
			for (size_t t_offset = 0; t_offset < 12; ++t_offset)
			{
				uint8_t c_data = (t_offset < 6) ? ((data[byte_offset] >> bit_offset) & 0x03) : 0x01;
				uint8_t tmp = c_data - 1;
				if ((tmp & 0x03) > 0x2)
				{
					--tmp;
				}
				tmp -= borrow;
				if ((tmp & 0x03) > 0x2)
				{
					--tmp;
				}
				borrow = (tmp > c_data);
				c_data = (tmp & 0x03);
				result.data[byte_offset] |= (c_data << bit_offset);
				bit_offset += 2;
				if (bit_offset == 8)
				{
					bit_offset = 0;
					++byte_offset;
				}
			}
			return result;
		}

		constexpr UTryte::operator UWord() const // Unsigned Tryte To Unsigned Word Conversion
		{
			UWord result = 0;
			// memcpy(&result.data[0], &data[0], 2);
			result.data[0] = data[0];
			result.data[1] = data[1];
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, Tryte, UTryte, UWord>
		constexpr Tryte operator+(Tryte val1, T val2) // Balanced Tryte Addition (Compatible with Unbalanced)
		{
			Tryte result = val1;
			if constexpr (std::is_same<T, Tryte>())
			{
				result += val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, UTryte, UWord>())
			{
				result += BCT::Tryte(val2);
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, UTryte, Tryte, UWord>
		constexpr UTryte operator+(UTryte val1, T val2) // Unbalanced Tryte Addition (Compatible with Balanced)
		{
			UTryte result = val1;
			if constexpr (std::is_same<T, UTryte>())
			{
				result += val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, Tryte, UWord>())
			{
				result += BCT::UTryte(val2);
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, UWord, Tryte, UTryte>
		constexpr UWord operator+(UWord val1, T val2) // Unbalanced Tryte Addition (Word Variant, compatible with Balanced)
		{
			UWord result = val1;
			if constexpr (std::is_same<T, UWord>())
			{
				result += val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, Tryte, UTryte>())
			{
				result += BCT::UWord(val2);
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, Tryte, UTryte, UWord>
		constexpr Tryte operator-(Tryte val1, T val2) // Balanced Tryte Subtraction (Compatible with Unbalanced)
		{
			Tryte result = val1;
			if constexpr (std::is_same<T, Tryte>())
			{
				result -= val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, UTryte, UWord>())
			{
				result -= BCT::Tryte(val2);
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, UTryte, Tryte, UWord>
		constexpr UTryte operator-(UTryte val1, T val2) // Unbalanced Tryte Subtraction (Compatible with Balanced)
		{
			UTryte result = val1;
			if constexpr (std::is_same<T, UTryte>())
			{
				result -= val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, Tryte, UWord>())
			{
				result -= BCT::UTryte(val2);
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, UWord, Tryte, UTryte>
		constexpr UWord operator-(UWord val1, T val2) // Unbalanced Tryte Subtraction (Word Variant, compatible with Balanced)
		{
			UWord result = val1;
			if constexpr (std::is_same<T, UWord>())
			{
				result -= val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, Tryte, UTryte>())
			{
				result -= BCT::UWord(val2);
			}
			return result;
		}

		constexpr Tryte operator<<(Tryte val, int shift) // Balanced Tritwise Shift Left Operator
		{
			val <<= shift;
			return val;
		}

		constexpr UTryte operator<<(UTryte val, int shift) // Unbalanced Tritwise Shift Left Operator
		{
			val <<= shift;
			return val;
		}

		constexpr UWord operator<<(UWord val, int shift) // Unbalanced Tritwise Shift Left Operator (Word Variant)
		{
			val <<= shift;
			return val;
		}

		constexpr Tryte operator>>(Tryte val, int shift) // Balanced Tritwise Shift Right Operator
		{
			val >>= shift;
			return val;
		}

		constexpr UTryte operator>>(UTryte val, int shift) // Unbalanced Tritwise Shift Right Operator
		{
			val >>= shift;
			return val;
		}

		constexpr UWord operator>>(UWord val, int shift) // Unbalanced Tritwise Shift Right Operator (Word Variant)
		{
			val >>= shift;
			return val;
		}

		template <typename T> requires Utility::TypeCompatible<T, Tryte, UTryte, UWord>
		constexpr Tryte operator|(Tryte val1, T val2) // Balanced Tritwise OR Operator (Compatible with Unbalanced)
		{
			Tryte result = val1;
			if constexpr (std::is_same<T, Tryte>())
			{
				result |= val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, UTryte, UWord>())
			{
				result |= BCT::Tryte(val2);
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, UTryte, Tryte, UWord>
		constexpr UTryte operator|(UTryte val1, T val2) // Unbalanced Tritwise OR Operator (Compatible with Balanced)
		{
			UTryte result = val1;
			if constexpr (std::is_same<T, UTryte>())
			{
				result |= val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, Tryte, UWord>())
			{
				result |= BCT::UTryte(val2);
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, UWord, Tryte, UTryte>
		constexpr UWord operator|(UWord val1, T val2) // Unbalanced Tritwise OR Operator (Word Variant, compatible with Balanced)
		{
			UWord result = val1;
			if constexpr (std::is_same<T, UWord>())
			{
				result |= val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, Tryte, UTryte>())
			{
				result |= BCT::UWord(val2);
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, Tryte, UTryte, UWord>
		constexpr Tryte operator&(Tryte val1, T val2) // Balanced Tritwise AND Operator (Compatible with Unbalanced)
		{
			Tryte result = val1;
			if constexpr (std::is_same<T, Tryte>())
			{
				result &= val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, UTryte, UWord>())
			{
				result &= BCT::Tryte(val2);
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, UTryte, Tryte, UWord>
		constexpr UTryte operator&(UTryte val1, T val2) // Unbalanced Tritwise AND Operator (Compatible with Balanced)
		{
			UTryte result = val1;
			if constexpr (std::is_same<T, UTryte>())
			{
				result &= val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, Tryte, UWord>())
			{
				result &= BCT::UTryte(val2);
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, UWord, Tryte, UTryte>
		constexpr UWord operator&(UWord val1, T val2) // Unbalanced Tritwise AND Operator (Word Variant, compatible with Balanced)
		{
			UWord result = val1;
			if constexpr (std::is_same<T, UWord>())
			{
				result &= val2;
			}
			else if (Utility::CheckCompatibleType<T, Tryte, UTryte>())
			{
				result &= BCT::UWord(val2);
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, Tryte, UTryte, UWord>
		constexpr Tryte operator^(Tryte val1, T val2) // Balanced Tritwise XOR Operator (Compatible with Unbalanced)
		{
			Tryte result = val1;
			if constexpr (std::is_same<T, Tryte>())
			{
				result ^= val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, UTryte, UWord>())
			{
				result ^= BCT::Tryte(val2);
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, UTryte, Tryte, UWord>
		constexpr UTryte operator^(UTryte val1, T val2) // Unbalanced Tritwise XOR Operator (Compatible with Unbalanced)
		{
			UTryte result = val1;
			if constexpr (std::is_same<T, UTryte>())
			{
				result ^= val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, Tryte, UWord>())
			{
				result ^= BCT::UTryte(val2);
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, UWord, Tryte, UTryte>
		constexpr UWord operator^(UWord val1, T val2) // Unbalanced Tritwise XOR Operator (Word Variant, compatible with Unbalanced)
		{
			UWord result = val1;
			if constexpr (std::is_same<T, UWord>())
			{
				result ^= val2;
			}
			else if constexpr (Utility::CheckCompatibleType<T, Tryte, UTryte>())
			{
				result ^= BCT::UWord(val2);
			}
			return result;
		}

		constexpr Tryte operator~(Tryte source) // Balanced Tritwise NOT Operator
		{
			Tryte result = 0;
			size_t byte_offset = 0;
			size_t bit_offset = 0;
			for (size_t t_offset = 0; t_offset < 6; ++t_offset)
			{
				uint8_t c_data = ((source.data[byte_offset] & (0x03 << bit_offset)) >> bit_offset);
				switch (c_data)
				{
					case 0x0:
					{
						result.data[byte_offset] &= ~(0x1 << bit_offset);
						result.data[byte_offset] |= (0x2 << bit_offset);
						break;
					}
					case 0x1:
					{
						result.data[byte_offset] |= (0x1 << bit_offset);
						break;
					}
					case 0x2:
					{
						result.data[byte_offset] &= ~(0x3 << bit_offset);
						break;
					}
				}
				bit_offset += 2;
				if (bit_offset == 8)
				{
					bit_offset = 0;
					++byte_offset;
				}
			}
			return result;
		}

		constexpr UTryte operator~(UTryte source) // Unbalanced Tritwise NOT Operator
		{
			UTryte result = 0;
			size_t byte_offset = 0;
			size_t bit_offset = 0;
			for (size_t t_offset = 0; t_offset < 6; ++t_offset)
			{
				uint8_t c_data = ((source.data[byte_offset] & (0x03 << bit_offset)) >> bit_offset);
				switch (c_data)
				{
					case 0x1:
					{
						result.data[byte_offset] |= (0x2 << bit_offset);
						break;
					}
					case 0x2:
					{
						result.data[byte_offset] |= (0x1 << bit_offset);
						break;
					}
				}
				bit_offset += 2;
				if (bit_offset == 8)
				{
					bit_offset = 0;
					++byte_offset;
				}
			}
			return result;
		}

		constexpr UWord operator~(UWord source) // Unbalanced Tritwise NOT Operator (Word Variant)
		{
			UWord result = 0;
			size_t byte_offset = 0;
			size_t bit_offset = 0;
			for (size_t t_offset = 0; t_offset < 12; ++t_offset)
			{
				uint8_t c_data = ((source.data[byte_offset] & (0x03 << bit_offset)) >> bit_offset);
				switch (c_data)
				{
					case 0x1:
					{
						result.data[byte_offset] |= (0x2 << bit_offset);
						break;
					}
					case 0x2:
					{
						result.data[byte_offset] |= (0x1 << bit_offset);
						break;
					}
				}
				bit_offset += 2;
				if (bit_offset == 8)
				{
					bit_offset = 0;
					++byte_offset;
				}
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, Tryte, UTryte>
		inline T ReadTryte(const uint8_t *src, size_t tryte_address) // Read Tryte From Memory
		{
			T result;
			size_t byte_address = (tryte_address * 2 * 6);
			size_t tmp = byte_address / 8;
			size_t dest_byte_offset = 0;
			size_t src_bit_offset = byte_address % 8;
			size_t dest_bit_offset = 0;
			byte_address = tmp;
			for (size_t t_offset = 0; t_offset < 6; ++t_offset)
			{
				uint8_t c_data = ((src[byte_address] >> src_bit_offset) & 0x03);
				result.data[dest_byte_offset] |= (c_data << dest_bit_offset);
				src_bit_offset += 2;
				if (src_bit_offset == 8)
				{
					src_bit_offset = 0;
					++byte_address;
				}
				dest_bit_offset += 2;
				if (dest_bit_offset == 8)
				{
					dest_bit_offset = 0;
					++dest_byte_offset;
				}
			}
			return result;
		}

		template <typename T> requires Utility::TypeCompatible<T, Tryte, UTryte>
		inline void WriteTryte(uint8_t *dest, size_t tryte_address, T value) // Write Tryte To Memory
		{
			size_t byte_address = (tryte_address * 2 * 6);
			size_t tmp = byte_address / 8;
			size_t src_byte_offset = 0;
			size_t src_bit_offset = 0;
			size_t dest_bit_offset = byte_address % 8;
			byte_address = tmp;
			for (size_t t_offset = 0; t_offset < 6; ++t_offset)
			{
				dest[byte_address] &= ~(0x03 << dest_bit_offset);
				dest[byte_address] |= (((value.data[src_byte_offset] >> src_bit_offset) & 0x03) << dest_bit_offset);
				src_bit_offset += 2;
				if (src_bit_offset == 8)
				{
					src_bit_offset = 0;
					++src_byte_offset;
				}
				dest_bit_offset += 2;
				if (dest_bit_offset == 8)
				{
					dest_bit_offset = 0;
					++byte_address;
				}
			}
		}

		template <Tryte... Trits> requires (sizeof...(Trits) > 0 && sizeof...(Trits) <= 6)
		consteval Tryte GetTryteFromTritData()
		{
			std::array<Tryte, sizeof...(Trits)> trit_data = { Trits... };
			int shift = trit_data.size() - 1;
			Tryte result = 0;
			for (size_t i = 0; i < trit_data.size(); ++i)
			{
				if (static_cast<int16_t>(trit_data[i]) > 1)
				{
					trit_data[i] = 1;
				}
				else if (static_cast<int16_t>(trit_data[i]) < -1)
				{
					trit_data[i] = -1;
				}
				result += (trit_data[i] << shift);
				--shift;
			}
			return result;
		}

		template <typename T, char... Septemvigits> requires (sizeof...(Septemvigits) > 0) && Utility::TypeCompatible<T, Tryte, UTryte, UWord>
		consteval T GetValueInSeptemvigesimal()
		{
			T result = 0;
			int shift = 3 * (sizeof...(Septemvigits) - 1);
			std::array<char, sizeof...(Septemvigits)> septemvigits_list = { Septemvigits... };
			for (size_t i = 0; i < septemvigits_list.size(); ++i)
			{
				char current_septemvigit = septemvigits_list[i];
				switch (current_septemvigit)
				{
					case '0':
					case '1':
					case '2':
					case '3':
					case '4':
					case '5':
					case '6':
					case '7':
					case '8':
					case '9':
					{
						result += (T(current_septemvigit - '0') << shift);
						break;
					}
					case 'A':
					case 'B':
					case 'C':
					case 'D':
					case 'E':
					case 'F':
					case 'G':
					case 'H':
					case 'I':
					case 'J':
					case 'K':
					case 'L':
					case 'M':
					case 'N':
					case 'O':
					case 'P':
					case 'Q':
					{
						result += (T(10 + (current_septemvigit - 'A')) << shift);
						break;
					}
				}
				shift -= 3;
			}
			return result;
		}
	}
}

#endif
