#include <binaryio/binaryreader.hpp>

using namespace binaryio;

BinaryReader::BinaryReader(const uint8_t *buffer, bool bigEndian)
{
	m_buffer = buffer;
	m_bigEndian = bigEndian;
	m_offset = 0;
}

BinaryReader BinaryReader::Copy() const
{
	auto fileStream = BinaryReader(m_buffer, m_bigEndian);
	fileStream.Seek(m_offset);
	return fileStream;
}

std::string BinaryReader::ReadString()
{
	std::string result;
	char c;
	while ((c = Read<char>()))
		result.push_back(c);

	return result;
}

std::string BinaryReader::ReadString(size_t size)
{
	std::string result;
	result.reserve(size);
	for (auto i = 0U; i < size; i++)
		result.push_back(Read<char>());

	return result;
}
