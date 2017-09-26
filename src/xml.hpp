#pragma once

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

#include <tuple>
#include <string>
#include <memory>
#include <exception>

namespace xml
{
	const int MAX_BUFF = 1024;

	class CXmlReader;

	enum class EElement
	{
		Header,
		StartTag,
		EndTag,
		Attribute,
		Value,
		Content
	};

	class CPosition
	{
		std::size_t _line;
		std::size_t _pos;

		friend class CXmlReader;

	public:
		CPosition();
		CPosition(const CPosition &pos);

		const CPosition& operator=(const CPosition &pos);

		std::size_t Line() const noexcept { return _line; }
		std::size_t Pos() const noexcept { return _pos; }
	};

	struct IStream
	{
		virtual std::size_t Read(char* buffer, std::size_t len) = 0;
	};

	std::unique_ptr<IStream> CreateStreamFromFile(const char* file);
	std::unique_ptr<IStream> CreateStreamFromFile(const std::string &file);

	class CXmlReader
	{
		std::unique_ptr<IStream> _xml;
		unsigned char _buffer[MAX_BUFF];
		std::size_t _read;
		std::size_t _len;
		std::string _text;
		CPosition _pos;
		EElement _type;
		std::string _start;

		void Read(char &chr);
		void SkipWhitespace(char &chr);

		void GetName(char chr);
		void GetText(char chr, char end);

	public:
		CXmlReader(std::unique_ptr<IStream> xml);

		const CPosition& Location() const noexcept { return _pos; }
		const std::string& GetText() const noexcept { return _text; }
		EElement GetType() const noexcept { return _type; }

		EElement Next();
		const std::string& Next(EElement element);
		std::tuple<xml::EElement, const std::string&> Read();

		bool operator==(EElement element);
		bool operator==(const std::string& str);
		bool operator==(const char* str);
		bool operator!=(EElement element);
		bool operator!=(const std::string& str);
		bool operator!=(const char* str);
	};

	class XException : public std::exception
	{
		std::string _msg;
		CPosition _pos;

	public:
		XException(const char* msg, const CPosition &pos);
		XException(std::string& msg, const CPosition &pos);

		const std::string& Message() const noexcept { return _msg; }
		const CPosition& Position() const noexcept { return _pos; }

		const char* what() const { return _msg.c_str(); }
	};

	struct XEof : public XException
	{
		XEof(const CPosition &pos);
	};

	class XUnexceptedChar : public XException
	{
		char _chr;

		static std::string GetMessage(char chr);

	public:
		XUnexceptedChar(char chr, const CPosition &pos);

		char GetChar() const noexcept { return _chr; }
	};

	class XUnexpectedType : public XException
	{
		static std::string GetMessage(CXmlReader &xml, const char* expected);

	public:
		XUnexpectedType(CXmlReader &xml);
		XUnexpectedType(CXmlReader &xml, const char* expected);
		XUnexpectedType(CXmlReader &xml, EElement expected);
	};

	class XUnexpectedElement : public XException
	{
		std::string _expected;

		static std::string GetMessage(CXmlReader &xml, const char* expected);

	public:
		XUnexpectedElement(CXmlReader &xml);
		XUnexpectedElement(CXmlReader &xml, const char* expected);
		XUnexpectedElement(CXmlReader &xml, const std::string& expected);
	};


};
