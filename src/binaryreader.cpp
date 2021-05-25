#include <binaryio/binaryreader.hpp>

using namespace binaryio;

BinaryReader::BinaryReader(std::shared_ptr<std::vector<uint8_t>> buffer, bool bigEndian)
{
	m_buffer = buffer;
	m_bigEndian = bigEndian;
	m_64BitMode = false;
	m_offset = 0;
}

BinaryReader BinaryReader::Copy() const
{
	auto fileStream = BinaryReader(m_buffer, m_bigEndian);
	fileStream.Set64BitMode(m_64BitMode);
	fileStream.Seek(m_offset);
	return fileStream;
}

void BinaryReader::Align()
{
	Align(m_64BitMode ? 8 : 4);
}

void BinaryReader::Align(uint_fast32_t byteAlignment)
{
	m_offset = binaryio::Align(m_offset, byteAlignment);
}

void BinaryReader::SkipPointer()
{
	if (m_64BitMode)
		return Skip<uint64_t>();

	Skip<uint32_t>();
}

uint64_t BinaryReader::ReadPointer()
{
	if (m_64BitMode)
		return Read<uint64_t>();

	return Read<uint32_t>();
}

std::string BinaryReader::ReadString()
{
	std::string result;
	char c;
	while ((c = Read<char>()))
		result.push_back(c);

	Align();

	return result;
}

std::string BinaryReader::ReadString(size_t size)
{
	std::string result(m_buffer->begin() + m_offset, m_buffer->begin() + m_offset + size);
	Seek(static_cast<off_t>(size), std::ios::cur);
	return result;
}
