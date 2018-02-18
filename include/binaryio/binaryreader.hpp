#pragma once
#include "util.hpp"
#include <ios>
#include <tuple>

namespace binaryio
{
	class BinaryReader
	{
	public:
		BinaryReader(const uint8_t *buffer, bool bigEndian);
		BinaryReader Copy() const;

		template<typename T>
		std::enable_if_t<std::is_arithmetic_v<typename SafeUnderlyingType<T>::type>, T> Read()
		{
			const auto data = m_buffer + m_offset;
			uintmax_t result = 0;

			for (auto i = 0U; i < sizeof(T); i++)
			{
				if (m_bigEndian)
					result |= data[sizeof(T) - i - 1] << (i * 8);
				else
					result |= data[i] << (i * 8);
			}

			Skip<T>();
			return reinterpret_cast<T &>(result);
		}

		template<typename T>
		typename std::enable_if<HasValueType<T>::value, T>::type Read()
		{
			return ReadImpl<T>(std::make_index_sequence<sizeof(T) / sizeof(T::value_type)>{});
		}

		template<typename T>
		void Skip()
		{
			Seek(sizeof(T), std::ios::cur);
		}

		void Seek(off_t offset, std::ios::seekdir seekDir = std::ios::beg)
		{
			if (seekDir == std::ios::cur)
				m_offset += offset;
			else
				m_offset = offset;
		}

		const uint8_t *GetBuffer() const
		{
			return m_buffer;
		}

		bool IsBigEndian() const
		{
			return m_bigEndian;
		}

		off_t GetOffset() const
		{
			return m_offset;
		}

		std::string ReadString();
		std::string ReadString(size_t size);

	private:
		template<typename T, size_t... Is>
		typename std::enable_if<HasValueType<T>::value, T>::type ReadImpl(std::index_sequence<Is...>)
		{
			return {(static_cast<void>(Is), Read<typename T::value_type>())...};
		}

		const uint8_t *m_buffer;
		bool m_bigEndian;
		off_t m_offset;
	};
}
