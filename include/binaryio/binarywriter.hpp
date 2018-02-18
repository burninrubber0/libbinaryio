#pragma once
#include <sstream>
#include <fstream>
#include <queue>
#include <functional>
#include <cassert>
#include "util.hpp"

namespace binaryio
{
	class BinaryWriter
	{
	public:
		template<typename T>
		std::enable_if_t<std::is_arithmetic_v<typename SafeUnderlyingType<T>::type>> Write(T value)
		{
			m_outStream.write(reinterpret_cast<const char *>(&value), sizeof(T));

			assert(!m_outStream.fail());
		}

		template<typename T>
		std::enable_if_t<HasValueType<T>::value && !HasColType<T>::value> Write(T value)
		{
			for (auto i = 0U; i < sizeof(T) / sizeof(T::value_type); i++)
				m_outStream.write(reinterpret_cast<const char *>(&value[i]), sizeof(T::value_type));

			assert(!m_outStream.fail());
		}

		template<typename T>
		std::enable_if_t<HasColType<T>::value> Write(T value)
		{
			for (auto i = 0U; i < sizeof(T) / sizeof(T::col_type); i++)
				Write(value[i]);
		}

		void Write(const std::string &value, bool nullTerminate = true)
		{
			if (value.empty())
				return Write<uint32_t>(0);

			m_outStream.write(value.c_str(), value.length() + (nullTerminate ? 1 : 0));

			assert(!m_outStream.fail());
		}

		template<typename T>
		std::enable_if_t<std::is_arithmetic_v<typename SafeUnderlyingType<T>::type> || std::is_constructible_v<T, const std::string &>> VisitAndWrite(off_t &offset, T value)
		{
			const auto prevPos = GetOffset();
			Seek(offset);
			Write(value);

			offset = binaryio::Align(GetOffset(), 4);

			Seek(prevPos);
		}

		void Append(BinaryWriter &writer)
		{
			if (writer.GetSize() == 0)
				return;

			writer.m_outStream.seekg(0);
			Seek(0, std::ios::end);
			m_outStream << writer.m_outStream.rdbuf();

			assert(!m_outStream.fail());
		}

		void Defer(std::function<void(BinaryWriter &writer)> deferWriteFn)
		{
			m_deferredWrites.push(deferWriteFn);
		}

		void ProcessDeferQueue()
		{
			for (auto i = 0U; i < m_deferredWrites.size(); i++)
			{
				m_deferredWrites.front()(*this);
				m_deferredWrites.pop();
			}

			if (!m_deferredWrites.empty())
				ProcessDeferQueue();
		}

		void Seek(off_t offset, std::ios::seekdir seekdir = std::ios::beg)
		{
			auto absoluteOffset = offset;
			if (seekdir == std::ios::cur)
				absoluteOffset += GetOffset();
			else if (seekdir == std::ios::end)
				absoluteOffset += static_cast<off_t>(GetSize());

			if (absoluteOffset > static_cast<off_t>(GetSize()))
			{
				m_outStream.seekp(0, std::ios::end);
				const auto extensionSize = absoluteOffset - GetSize();
				const auto buffer = new char[extensionSize];
				m_outStream.write(buffer, extensionSize);
				delete[] buffer;
			}
			else
			{
				m_outStream.seekp(absoluteOffset);
			}

			assert(!m_outStream.fail());
		}

		void Align(uint_fast8_t alignment)
		{
			Seek(binaryio::Align(GetOffset(), alignment));
		}

		off_t GetOffset()
		{
			return static_cast<off_t>(m_outStream.tellp());
		}

		size_t GetSize()
		{
			m_outStream.seekg(0, std::ios::end);
			return m_outStream.tellg();
		}

		std::stringstream GetStream()
		{
			m_outStream.seekg(0);
			return std::move(m_outStream);
		}

	private:
		std::stringstream m_outStream;
		std::queue<std::function<void(BinaryWriter &writer)>> m_deferredWrites;
	};
}
