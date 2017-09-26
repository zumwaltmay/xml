/*
	Copyright(c) 2016 Christopher Zumwalt
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
		* Redistributions in binary form must reproduce the above copyright
		  notice, this list of conditions and the following disclaimer in the
		  documentation and / or other materials provided with the distribution.
		* Neither the name of the <organization> nor the
		  names of its contributors may be used to endorse or promote products
		  derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED.IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <cstring>
#include <fstream>
#include "xml.hpp"

namespace xml
{
	const int MAX_ESCAPE_LEN = 10;
	const int MAX_BOM_LEN = 4;
	const int MAX_OVERFLOW = 4;

	const char* GetElementName(EElement element)
	{
		switch (element)
		{
		case EElement::Header: return "header";
		case EElement::StartTag: return "start tag";
		case EElement::EndTag: return "end tag";
		case EElement::Attribute: return "attribute";
		case EElement::Value: return "value";
		case EElement::Content: return "content";
		};

		return nullptr;
	}

	class CFileStream : public IStream
	{
	private:
		std::ifstream _xml;
		char _overflow[MAX_OVERFLOW];
		std::size_t _extra;

	public:
		CFileStream(std::ifstream&& xml, char* overflow, std::size_t extra);

		std::size_t Read(char* buffer, std::size_t len);
	};

	enum class EClass
	{
		First,
		End,
		Name,
		Whitespace,
		Control,
		Other
	};

	EClass g_class[] = {
		EClass::Control, // 0 
		EClass::Control, // 1 
		EClass::Control, // 2 
		EClass::Control, // 3 
		EClass::Control, // 4 
		EClass::Control, // 5 
		EClass::Control, // 6 
		EClass::Control, // 7 
		EClass::Control, // 8 
		EClass::Whitespace, // 9 
		EClass::Whitespace, // 10 
		EClass::Whitespace, // 11 
		EClass::Whitespace, // 12 
		EClass::Whitespace, // 13 
		EClass::Control, // 14 
		EClass::Control, // 15 
		EClass::Control, // 16 
		EClass::Control, // 17 
		EClass::Control, // 18 
		EClass::Control, // 19 
		EClass::Control, // 20 
		EClass::Control, // 21 
		EClass::Control, // 22 
		EClass::Control, // 23 
		EClass::Control, // 24 
		EClass::Control, // 25 
		EClass::Control, // 26 
		EClass::Control, // 27 
		EClass::Control, // 28 
		EClass::Control, // 29 
		EClass::Control, // 30 
		EClass::Control, // 31 
		EClass::Whitespace, // 32 
		EClass::Other, // 33 
		EClass::Other, // 34 
		EClass::Other, // 35 
		EClass::Other, // 36 
		EClass::Other, // 37 
		EClass::Other, // 38 
		EClass::Other, // 39 
		EClass::Other, // 40 
		EClass::Other, // 41 
		EClass::Other, // 42 
		EClass::Other, // 43 
		EClass::Other, // 44 
		EClass::Other, // 45 
		EClass::Other, // 46 
		EClass::End, // 47 
		EClass::Name, // 48 
		EClass::Name, // 49 
		EClass::Name, // 50 
		EClass::Name, // 51 
		EClass::Name, // 52 
		EClass::Name, // 53 
		EClass::Name, // 54 
		EClass::Name, // 55 
		EClass::Name, // 56 
		EClass::Name, // 57 
		EClass::First, // 58 
		EClass::Other, // 59 
		EClass::Other, // 60 
		EClass::Other, // 61 
		EClass::End, // 62 
		EClass::Other, // 63 
		EClass::Other, // 64 
		EClass::First, // 65 
		EClass::First, // 66 
		EClass::First, // 67 
		EClass::First, // 68 
		EClass::First, // 69 
		EClass::First, // 70 
		EClass::First, // 71 
		EClass::First, // 72 
		EClass::First, // 73 
		EClass::First, // 74 
		EClass::First, // 75 
		EClass::First, // 76 
		EClass::First, // 77 
		EClass::First, // 78 
		EClass::First, // 79 
		EClass::First, // 80 
		EClass::First, // 81 
		EClass::First, // 82 
		EClass::First, // 83 
		EClass::First, // 84 
		EClass::First, // 85 
		EClass::First, // 86 
		EClass::First, // 87 
		EClass::First, // 88 
		EClass::First, // 89 
		EClass::First, // 90 
		EClass::Other, // 91 
		EClass::Other, // 92 
		EClass::Other, // 93 
		EClass::Other, // 94 
		EClass::First, // 95 
		EClass::Other, // 96 
		EClass::First, // 97 
		EClass::First, // 98 
		EClass::First, // 99 
		EClass::First, // 100 
		EClass::First, // 101 
		EClass::First, // 102 
		EClass::First, // 103 
		EClass::First, // 104 
		EClass::First, // 105 
		EClass::First, // 106 
		EClass::First, // 107 
		EClass::First, // 108 
		EClass::First, // 109 
		EClass::First, // 110 
		EClass::First, // 111 
		EClass::First, // 112 
		EClass::First, // 113 
		EClass::First, // 114 
		EClass::First, // 115 
		EClass::First, // 116 
		EClass::First, // 117 
		EClass::First, // 118 
		EClass::First, // 110 
		EClass::First, // 120 
		EClass::First, // 121 
		EClass::First, // 122 
		EClass::Other, // 123 
		EClass::Other, // 124 
		EClass::Other, // 125 
		EClass::Other, // 126 
		EClass::Other // 127 
	};

	
	CPosition::CPosition()
		: _line(0), _pos(0)
	{
	}

	CPosition::CPosition(const CPosition &other)
		: _line(other._line), _pos(other._pos)
	{
	}

	const CPosition& CPosition::operator=(const CPosition &other)
	{
		_line = other._line;
		_pos = other._pos;
		return *this;
	}

	CXmlReader::CXmlReader(std::unique_ptr<IStream> xml)
		: _xml(std::move(xml))
	{
		_len = _xml->Read((char*)&_buffer, MAX_BUFF);
		if (_len == 0)
		{
			throw XEof(_pos);
		}
		_read = 0;
	}

	void CXmlReader::Read(char &chr)
	{
		if (_read >= _len)
		{
			_len = _xml->Read((char*)&_buffer, MAX_BUFF);
			if (_len == 0)
			{
				throw XEof(_pos);
			}
			_read = 0;
		}

		chr = _buffer[_read++];
		
		if (chr == '\n')
		{
			_pos._line++;
			_pos._pos = 0;
		}
		else
		{
			_pos._pos++;
		}
	}

	void CXmlReader::SkipWhitespace(char &chr)
	{
		while (g_class[chr] == EClass::Whitespace)
		{
			Read(chr);
		}
	}

	void CXmlReader::GetName(char chr)
	{
		if (g_class[chr] != EClass::First)
		{
			throw XUnexceptedChar(chr, _pos);
		}

		do
		{
			_text.push_back(chr);
			Read(chr);
		} while (g_class[chr] == EClass::First || g_class[chr] == EClass::Name);

		_read--;
	}

	void CXmlReader::GetText(char chr, char end)
	{
		do
		{
			_text.push_back(chr);
			Read(chr);
		} while (chr != end);
	}

	EElement CXmlReader::Next()
	{
		char chr = _buffer[_read++];

		_text.clear();

		SkipWhitespace(chr);

		switch (_type)
		{
		case EElement::Header:
			if (chr != '<')
			{
				throw XUnexceptedChar(chr, _pos);
			}

			// Check if this file has a header
			Read(chr);

			if (chr == '?')
			{
				// Read the header
				while (chr != '>')
				{
					Read(chr);
				}
			}

			Read(chr);
			SkipWhitespace(chr);

			if (chr != '<')
			{
				throw XUnexceptedChar(chr, _pos);
			}

			Read(chr);
			GetName(chr);
			_type = EElement::StartTag;
			break;
		case EElement::Attribute:
			if (chr == '=')
			{
				Read(chr);
				SkipWhitespace(chr);
				if (chr != '\"')
				{
					throw XUnexceptedChar(chr, _pos);
				}

				Read(chr);
				GetText(chr, '\"');
				_type = EElement::Value;
				break;
			}
		case EElement::Value:
		case EElement::StartTag:
			if (chr == '/')
			{
				_type = EElement::EndTag;
				_text = _start;
			}
			else if (chr == '>')
			{
				Read(chr);
				SkipWhitespace(chr);

				if (chr == '<')
				{
					_type = EElement::StartTag;

					Read(chr);
					if (chr == '/')
					{
						Read(chr);
						_type = EElement::EndTag;
					}
					
					GetName(chr);
				}
				else 
				{
					GetText(chr, '<');
					_type = EElement::Content;
				}
			}
			else
			{
				GetName(chr);
				_type = EElement::Attribute;
			}

			_start = _text;
			break;
		case EElement::EndTag:
			if (chr != '>')
			{
				throw XUnexceptedChar(chr, _pos);
			}

			Read(chr);
			SkipWhitespace(chr);
			if (chr != '<')
			{
				throw XUnexceptedChar(chr, _pos);
			}

			Read(chr);
		case EElement::Content:
			_type = EElement::StartTag;

			if (chr == '/')
			{
				Read(chr);
				_type = EElement::EndTag;
			}

			GetName(chr);
			break;
		}

		return _type;
	}

	const std::string& CXmlReader::Next(EElement element)
	{
		if (Next() != element)
		{
			throw new xml::XUnexpectedType(*this, element);
		}

		return _text;
	}

	std::tuple<xml::EElement, const std::string&> CXmlReader::Read()
	{
		return std::make_tuple(Next(), GetText());
	}

	bool CXmlReader::operator==(EElement element)
	{
		return _type == element;
	}

	bool CXmlReader::operator==(const std::string &str)
	{
		return _text == str;
	}

	bool CXmlReader::operator==(const char* str)
	{
		return _text == str;
	}

	bool CXmlReader::operator!=(EElement element)
	{
		return _type != element;
	}

	bool CXmlReader::operator!=(const std::string &str)
	{
		return _text != str;
	}

	bool CXmlReader::operator!=(const char* str)
	{
		return _text != str;
	}

	std::unique_ptr<IStream> CreateStreamFromFile(const char* file)
	{
		std::ifstream xml(file);
		
		unsigned char bom[MAX_BOM_LEN];
		char overflow[MAX_OVERFLOW];
		std::size_t extra;

		xml.read((char*)&bom, 4);
		if (xml.good())
		{
			if (bom[0] == 0xEF && bom[1] == 0xBB && bom[2] == 0xBF)
			{
				overflow[0] = (char)bom[3];
				extra = 1;
			}
			else
			{
				throw std::runtime_error("unsupported encoding");
			}
		}
		else if (xml.eof())
		{
			throw std::ios_base::failure("failed to read from file");
		}

		return std::make_unique<CFileStream>(std::move(xml), (char*)&overflow, extra);
	}

	std::unique_ptr<IStream> CreateStreamFromFile(const std::string& file)
	{
		return CreateStreamFromFile(file.c_str());
	}

	CFileStream::CFileStream(std::ifstream&& xml, char* overflow, std::size_t extra)
		: _xml(std::move(xml)), _extra(extra)
	{
		std::size_t start = 0;

		switch (_extra)
		{
		case 3:
			_overflow[start] = overflow[start++];
		case 2:
			_overflow[start] = overflow[start++];
		case 1:
			_overflow[start] = overflow[start++];
		}
	}

	std::size_t CFileStream::Read(char* buffer, std::size_t len)
	{
		std::size_t start = 0;
		std::size_t read = _extra;

		switch (read)
		{
		case 3:
			buffer[start] = _overflow[start++];
		case 2:
			buffer[start] = _overflow[start++];
		case 1:
			buffer[start] = _overflow[start++];
		}

		if (!_xml.eof())
		{
			_xml.read((char*)&buffer[start], len - start);
			if (_xml.bad())
			{
				throw std::ios_base::failure("error reading from stream");
			}

			read += (std::size_t)_xml.gcount();
		}

		_extra = 0;
		return read;
	}

	XException::XException(std::string& msg, const CPosition& pos)
		: _msg(std::move(msg)), _pos(pos)
	{
	}

	XException::XException(const char* msg, const CPosition& pos)
		: _msg(msg), _pos(pos)
	{
	}

	XEof::XEof(const CPosition &pos)
		: XException("unexpected end of file reached.", pos)
	{
	}

	XUnexceptedChar::XUnexceptedChar(char chr, const CPosition& pos)
		: XException(GetMessage(chr), pos), _chr(chr)
	{
	}

	std::string XUnexceptedChar::GetMessage(char chr)
	{
		std::string str("unexcepted char (");
		str += chr;
		str += ')';
		return str;
	}

	XUnexpectedElement::XUnexpectedElement(CXmlReader &xml)
		: XException(GetMessage(xml, nullptr), xml.Location())
	{
	}

	XUnexpectedElement::XUnexpectedElement(CXmlReader &xml, const char* expected)
		: XException(GetMessage(xml, expected), xml.Location())
	{
	}

	XUnexpectedElement::XUnexpectedElement(CXmlReader &xml, const std::string &expected)
		: XException(GetMessage(xml, expected.c_str()), xml.Location())
	{
	}


	std::string XUnexpectedElement::GetMessage(CXmlReader &xml, const char* expected)
	{
		std::string err("unexpected ");

		switch (xml.GetType())
		{
		case EElement::StartTag:
			err += "start of ";
			err += xml.GetText();
			err += " tag";
			break;
		case EElement::EndTag:
			err += "end of ";
			err += xml.GetText();
			err += " tag";
			break;
		case EElement::Attribute:
			err += "attribute ";
			err += xml.GetText();
			break;
		case EElement::Value:
			err += "value ";
			err += xml.GetText();
			break;
		case EElement::Content:
			err += "content ";
			break;
		}

		if (expected)
		{
			err += " expected ";
			err += expected;
		}

		return err;
	}

	XUnexpectedType::XUnexpectedType(CXmlReader &xml)
		: XException(GetMessage(xml, nullptr), xml.Location())
	{
	}

	XUnexpectedType::XUnexpectedType(CXmlReader &xml, const char* expected)
		: XException(GetMessage(xml, expected), xml.Location())
	{
	}

	XUnexpectedType::XUnexpectedType(CXmlReader &xml, EElement expected)
		: XException(GetMessage(xml, GetElementName(expected)), xml.Location())
	{
	}

	std::string XUnexpectedType::GetMessage(CXmlReader &xml, const char* expected)
	{
		std::string err("unexpected ");
		err += GetElementName(xml.GetType());

		if (expected)
		{
			err += " expected ";
			err += expected;
		}

		return err;
	}
}