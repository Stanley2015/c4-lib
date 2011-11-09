#include "c4encode.h"

const char CC4Encode::LITTLEENDIAN_BOM[2] = {'\xFF', '\xFE'};
const char CC4Encode::BIGENDIAN_BOM[2]    = {'\xFE', '\xFF'};
const char CC4Encode::UTF_8_BOM[3]        = {'\xEF', '\xBB', '\xBF'};
CC4EncodeUnicode* CC4EncodeUnicode::s_instance = NULL;
CC4EncodeUTF_8*   CC4EncodeUTF_8::s_instance   = NULL;

CC4Encode::CC4Encode(const std::wstring& name, const std::wstring& version, bool is_auto_check)
	:m_name(name),m_version(version),m_autoCheck(is_auto_check)
{}

CC4Encode::~CC4Encode()
{}

std::wstring CC4Encode::toString() const
{
	std::wstring str;
	str.append(m_name);
	str.append(L"(");
	str.append(m_version);
	str.append(L")");
	return str;
}

bool CC4Encode::isAutoCheck() const
{
	return m_autoCheck;
}

void CC4Encode::setAutoCheck(bool is_auto_check)
{
	m_autoCheck = is_auto_check;
}

std::wstring CC4Encode::getName() const
{
	return m_name;
}

std::wstring CC4Encode::getVersion() const
{
	return m_version;
}

CC4EncodeAnsi::CC4EncodeAnsi(const std::wstring& name, const std::wstring& version, bool is_auto_check, const unsigned char * buffer, unsigned int buffer_length)
	:CC4Encode(name, version, is_auto_check),m_mapBufferLength(buffer_length)
{
	m_mapBuffer = buffer;
	m_policies  = NULL;
	m_segments  = NULL;
}

std::wstring CC4EncodeAnsi::toString() const
{
	return CC4Encode::toString();
}

bool CC4EncodeAnsi::isAutoCheck() const
{
	return CC4Encode::isAutoCheck();
}

void CC4EncodeAnsi::setAutoCheck(bool is_auto_check)
{
	CC4Encode::setAutoCheck(is_auto_check);
}

std::wstring CC4EncodeAnsi::getName() const
{
	return CC4Encode::getName();
}

std::wstring CC4EncodeAnsi::getVersion() const
{
	return CC4Encode::getVersion();
}

bool CC4EncodeAnsi::match(const char *src, unsigned int src_length) const
{
	if (NULL == src)
		return false;
	if (0 == src_length)
		return false;
	if (NULL == m_mapBuffer)
		return false;
	if (NULL == m_policies)
		return false;
	if (NULL == m_segments)
		return false;

	if (!m_policies->checkValid())
		return false;
	if (!m_segments->checkValid())
		return false;

	unsigned char low=0, high=0;
	wchar_t ansiChar = 0;
	for (unsigned int i=0; i<src_length;)
	{
		memcpy(&high, src+i, 1); // reading first byte
		i++;
		if (m_policies->isContinueReadNextChar(high))
		{
			memcpy(&low, src+i, 1);
			i++;
		} else {
			low = high;
			high = 0;
		}
		// ansiChar = low + high*256;
		ansiChar = high<<8 | low;
		if (UNKNOWN_CHAR == convertSingleChar(ansiChar))
			return false;
	}

	return true;
}

std::wstring CC4EncodeAnsi::convert(const char *src, unsigned int src_length) const
{
	if (NULL == src)
		return std::wstring();
	if (0 == src_length)
		return std::wstring();
	if (NULL == m_mapBuffer)
		return std::wstring();
	if (NULL == m_policies)
		return std::wstring();
	if (NULL == m_segments)
		return std::wstring();

	if (!m_policies->checkValid())
		return std::wstring();
	if (!m_segments->checkValid())
		return std::wstring();
	
	unsigned int need_length = calcUnicodeStringLength(src, src_length);
	std::wstring unicodeStr(need_length, 0);
	unsigned int offset = 0;

	unsigned char low=0, high=0;
	wchar_t ansiChar = 0;
	for (unsigned int i=0; i<src_length;)
	{
		memcpy(&high, src+i, 1); // reading first byte
		i++;
		if (m_policies->isContinueReadNextChar(high))
		{
			memcpy(&low, src+i, 1);
			i++;
		} else {
			low = high;
			high = 0;
		}
		ansiChar = high<<8 | low;
		unicodeStr[offset++] = convertSingleChar(ansiChar);
	}

	return unicodeStr;
}

wchar_t CC4EncodeAnsi::convertSingleChar(char high_byte, char low_byte) const
{
	wchar_t ansiChar;
	ansiChar = ((unsigned char)low_byte) + ((unsigned char)high_byte)*256;
	return convertSingleChar(ansiChar);
}

wchar_t CC4EncodeAnsi::convertSingleChar(wchar_t ansi_char) const
{
	const CC4Segment* seg = m_segments->findMatchedSegment(ansi_char);
	if (NULL == seg)
		return UNKNOWN_CHAR;

	if (CC4Segment::refASCII == seg->m_reference)
		return ansi_char&0x007F;
	else if (CC4Segment::refOxFFFD == seg->m_reference)
		return UNKNOWN_CHAR;
	else if ((CC4Segment::refBUFFER == seg->m_reference))
	{
		wchar_t unicodeChar = UNKNOWN_CHAR;
		int offset = ansi_char - seg->m_begin + seg->m_offset;
		memcpy((void*)&unicodeChar, m_mapBuffer+offset*sizeof(wchar_t), sizeof(wchar_t));
		return unicodeChar;
	} else
		return UNKNOWN_CHAR;
}

unsigned int CC4EncodeAnsi::calcUnicodeStringLength(const char *src, unsigned int src_length) const
{
	if (NULL == src)
		return 0;
	if (0 == src_length)
		return 0;
	if (NULL == m_mapBuffer)
		return 0;
	if (NULL == m_policies)
		return 0;
	if (NULL == m_segments)
		return 0;

	if (!m_policies->checkValid())
		return 0;
	if (!m_segments->checkValid())
		return 0;

	unsigned int dest_len = 0;
	for (unsigned int i=0; i<src_length;)
	{
		if (m_policies->isContinueReadNextChar((unsigned char)*(src+i)))
		{
			i+=2;
		} else {
			i++;
		}
		dest_len++;
	}

	return dest_len;
}

bool CC4EncodeAnsi::convert2unicode(const char *src, unsigned int src_length, char *dest, unsigned int dest_length, bool check_dest_length) const
{
	if (NULL == src || NULL == dest)
		return false;
	if (!(dest>src+src_length || src>dest+dest_length))
		return false;
	if ((dest_length&1) != 0)
		return false;
	if (check_dest_length)
	{
		if (dest_length < calcUnicodeStringLength(src, src_length)*sizeof(wchar_t))
			return false;
	}

	if (!m_policies->checkValid())
		return false;
	if (!m_segments->checkValid())
		return false;

	unsigned int offset = 0;
	unsigned char low=0, high=0;
	wchar_t ansiChar = 0;
	for (unsigned int i=0; i<src_length;)
	{
		memcpy(&high, src+i, 1); // reading first byte
		i++;
		if (m_policies->isContinueReadNextChar(high))
		{
			memcpy(&low, src+i, 1);
			i++;
		} else {
			low = high;
			high = 0;
		}
		ansiChar = high<<8 | low;
		*((wchar_t*)dest+offset) = convertSingleChar(ansiChar);
		offset++;
	}
	return true;
}

bool CC4EncodeAnsi::convert2unicode(const char *src, unsigned int src_length, wchar_t *dest, unsigned int dest_str_length, bool check_dest_length) const
{
	return convert2unicode(src, src_length, (char*)dest, dest_str_length*sizeof(wchar_t), check_dest_length);
}

bool CC4EncodeAnsi::setPolicies(const CC4Policies* ptr_policies)
{
	if (NULL == ptr_policies)
		return false;
	
	m_policies = ptr_policies;
	return true;
}

bool CC4EncodeAnsi::setSegments(const CC4Segments* ptr_segments)
{
	if (NULL == ptr_segments)
		return false;

	m_segments = ptr_segments;
	return true;
}

const CC4Policies* CC4EncodeAnsi::getPolicies() const
{
	return m_policies;
}

const CC4Segments* CC4EncodeAnsi::getSegments() const
{
	return m_segments;
}

CC4EncodeUnicode::CC4EncodeUnicode(const std::wstring& name, const std::wstring& version, bool is_auto_check)
	:CC4Encode(name, version, is_auto_check)
{}

std::wstring CC4EncodeUnicode::toString() const
{
	return CC4Encode::toString();
}

bool CC4EncodeUnicode::isAutoCheck() const
{
	return CC4Encode::isAutoCheck();
}

void CC4EncodeUnicode::setAutoCheck(bool is_auto_check)
{
	CC4Encode::setAutoCheck(is_auto_check);
}

std::wstring CC4EncodeUnicode::getName() const
{
	return CC4Encode::getName();
}

std::wstring CC4EncodeUnicode::getVersion() const
{
	return CC4Encode::getVersion();
}

std::wstring CC4EncodeUnicode::_getName()
{
	return L"UTF-16";
}

std::wstring CC4EncodeUnicode::_getVersion()
{
	return L"Unicode";
}

CC4EncodeUnicode* CC4EncodeUnicode::getInstance()
{
	if (NULL == s_instance)
		s_instance = new CC4EncodeUnicode(CC4EncodeUnicode::_getName(), CC4EncodeUnicode::_getVersion(), false);
	return s_instance;
}

bool CC4EncodeUnicode::match(const char *src, unsigned int src_length) const
{
	return CC4EncodeUnicode::_match(src, src_length);
}

std::wstring CC4EncodeUnicode::convert(const char *src, unsigned int src_length) const
{
	if (NULL == src)
		return std::wstring();
	if (src_length == 0)
		return std::wstring();
	// if src_length is an odd number
	if ((src_length&1) != 0)
		return std::wstring();

	const wchar_t *unicodeSrc;
	unicodeSrc = (wchar_t *)src;
	unsigned int need_length = src_length>>1;
	std::wstring unicodeStr(need_length, 0);
	for (unsigned int i=0; i<need_length; ++i)
		unicodeStr[i] = *(unicodeSrc+i);
	return unicodeStr;

	//return std::wstring((wchar_t*)src);
}

bool CC4EncodeUnicode::_match(const char *src, unsigned int src_length)
{
	if (NULL == src)
		return false;
	if (src_length == 0)
		return false;
	if ((src_length&1) != 0)
		return false;

	return true;
}

std::string CC4EncodeUnicode::convert2utf8(const char *src, unsigned int src_length, bool is_little_endian)
{
	if (NULL == src)
		return std::string();
	if (src_length == 0)
		return std::string();
	// if src_length is an odd number
	if ((src_length&1) != 0)
		return std::string();

	unsigned int need_length = calcUtf8StringLength(src, src_length, is_little_endian);
	std::string utf8str(need_length, 0);
	unsigned int offset = 0;

	const unsigned char *unsignedSrc = (unsigned char *)src;
	wchar_t chr = 0;
	char dest_chars[3];
	memset((void*)dest_chars, 0, 3);
	unsigned int dest_len;
	for (unsigned int i=0;i<src_length;)
	{
		dest_len = 0;
		if (is_little_endian)
			chr = *(unsignedSrc+i) + (*(unsignedSrc+i+1))*256;
		else
			chr = *(unsignedSrc+i+1) + (*(unsignedSrc+i))*256;
		i+=2;

		if (chr <= 0x007F)  // U-0000 - U-007F
		{
			dest_len = 1;
			dest_chars[0] = (char)chr;
		} else if (chr <= 0x07FF) {  // U-0080 - U-07FF
			dest_len = 2;
			dest_chars[0] = (char)(0xC0 | (chr>>6));
			dest_chars[1] = (char)(0x80 | (chr&0x003F));
		} else if (chr >= 0xD800 && chr <= 0xDFFF) {  // U-D800 - U-DFFF
			// ignore this unicode character
			dest_len = 0;
		} else {  // U-0800 - UD7FF and UE000 - UFFFF
			dest_len = 3;
			dest_chars[0] = (char)(0xE0 | (chr>>12));
			dest_chars[1] = (char)(0x80 | ((chr>>6) & 0x003F));
			dest_chars[2] = (char)(0x80 | (chr & 0x003F));
		}
		for (unsigned int j=0;j<dest_len;j++)
			utf8str[offset++] = dest_chars[j];
	}

	return utf8str;
}

std::string CC4EncodeUnicode::convert2utf8(const wchar_t *src, unsigned int src_str_length)
{
	return convert2utf8((char*)src, src_str_length*2, true);
}

bool CC4EncodeUnicode::convert2utf8(const char *src, unsigned int src_length, char *dest, unsigned int dest_length, bool is_little_endian, bool check_dest_length)
{
	if (NULL == src || NULL == dest)
		return false;
	if (0 == src_length || 0 == dest_length)
		return false;
	if (!(dest>src+src_length || src>dest+dest_length))
		return false;
	if ((src_length&1) != 0)
		return false;
	if (check_dest_length)
	{
		if (dest_length < calcUtf8StringLength(src, src_length, is_little_endian))
			return false;
	}

	unsigned int offset = 0;
	const unsigned char *unsignedSrc = (unsigned char *)src;
	wchar_t chr = 0;
	memset((void*)dest, 0, dest_length);
	for (unsigned int i=0;i<src_length;)
	{
		if (is_little_endian)
			chr = *(unsignedSrc+i) + (*(unsignedSrc+i+1))*256;
		else
			chr = *(unsignedSrc+i+1) + (*(unsignedSrc+i))*256;
		i+=2;

		if (chr <= 0x007F)  // U-0000 - U-007F
		{
			*(dest + offset++) = (char)chr;
		} else if (chr <= 0x07FF) {  // U-0080 - U-07FF
			*(dest + offset++) = (char)(0xC0 | (chr>>6));
			*(dest + offset++) = (char)(0x80 | (chr&0x003F));
		} else if (chr >= 0xD800 && chr <= 0xDFFF) {  // U-D800 - U-DFFF
			// ignore this unicode character
		} else {  // U-0800 - UD7FF and UE000 - UFFFF
			*(dest + offset++) = (char)(0xE0 | (chr>>12));
			*(dest + offset++) = (char)(0x80 | ((chr>>6) & 0x003F));
			*(dest + offset++) = (char)(0x80 | (chr & 0x003F));
		}
	}

	return true;
}

unsigned int CC4EncodeUnicode::calcUtf8StringLength(const char *src, unsigned int src_length, bool is_little_endian)
{
	if (NULL == src)
		return 0;
	if (src_length == 0)
		return 0;
	// if src_length is an odd number
	if ((src_length&1) != 0)
		return 0;

	const unsigned char *unsignedSrc = (unsigned char *)src;
	wchar_t chr = 0;
	unsigned int dest_len = 0;
	for (unsigned int i=0;i<src_length;)
	{
		if (is_little_endian)
			chr = *(unsignedSrc+i) + (*(unsignedSrc+i+1))*256;
		else
			chr = *(unsignedSrc+i+1) + (*(unsignedSrc+i))*256;
		i+=2;

		if (chr <= 0x007F)  // U-0000 - U-007F
		{
			dest_len += 1;
		} else if (chr <= 0x07FF) {  // U-0080 - U-07FF
			dest_len += 2;
		} else if (chr >= 0xD800 && chr <= 0xDFFF) {  // U-D800 - U-DFFF
			dest_len += 0;
		} else {  // U-0800 - UD7FF and UE000 - UFFFF
			dest_len += 3;
		}
	}

	return dest_len;
}

CC4EncodeUTF_8::CC4EncodeUTF_8(const std::wstring& name, const std::wstring& version, bool is_auto_check)
	:CC4Encode(name, version, is_auto_check)
{}

std::wstring CC4EncodeUTF_8::toString() const
{
	return CC4Encode::toString();
}

bool CC4EncodeUTF_8::isAutoCheck() const
{
	return CC4Encode::isAutoCheck();
}

void CC4EncodeUTF_8::setAutoCheck(bool is_auto_check)
{
	CC4Encode::setAutoCheck(is_auto_check);
}

std::wstring CC4EncodeUTF_8::getName() const
{
	return CC4Encode::getName();
}

std::wstring CC4EncodeUTF_8::getVersion() const
{
	return CC4Encode::getVersion();
}

std::wstring CC4EncodeUTF_8::_getName()
{
	return L"UTF-8";
}

std::wstring CC4EncodeUTF_8::_getVersion()
{
	return L"Unicode";
}

CC4EncodeUTF_8* CC4EncodeUTF_8::getInstance()
{
	if (NULL == s_instance)
		s_instance = new CC4EncodeUTF_8(CC4EncodeUTF_8::_getName(), CC4EncodeUTF_8::_getVersion(), true);
	return s_instance;
}

bool CC4EncodeUTF_8::match(const char *src, unsigned int src_length) const
{
	return CC4EncodeUTF_8::_match(src, src_length);
}

std::wstring CC4EncodeUTF_8::convert(const char *src, unsigned int src_length) const
{
	return CC4EncodeUTF_8::convert2unicode(src, src_length);
}

bool CC4EncodeUTF_8::_match(const char *src, unsigned int src_length)
{
	if (NULL == src)
		return false;
	if (src_length == 0)
		return false;

	unsigned int i = 0;
	bool isMatched = true;
	while (i < src_length)
	{
		if ((0x80 & *(src+i)) == 0) // ASCII
		{
			i++;
			continue;
		}
		else if((0xE0 & *(src+i)) == 0xC0) // 110xxxxx
		{
			if (*(src+i+1) == '\0')
			{
				isMatched=false;
				break;
			}
			if ((0xC0 & *(src+i+1)) == 0x80) // 10xxxxxx
			{
				i+=2;
				continue;
			}
			else
			{
				isMatched=false;
				break;
			}
		}
		else if((0xF0 & *(src+i)) == 0xE0) // 1110xxxx
		{
			if (*(src+i+1) == '\0')
			{
				isMatched=false;
				break;
			}
			if (*(src+i+2) == '\0')
			{
				isMatched=false;
				break;
			}
			if (((0xC0 & *(src+i+1)) == 0x80) && ((0xC0 & *(src+i+2)) == 0x80)) // 10xxxxxx 10xxxxxx
			{
				i+=3;
				continue;
			}
			else
			{
				isMatched=false;
				break;
			}
		}
		else // not matched
		{
			isMatched=false;
			break;
		}
	}
	return isMatched;
}

std::wstring CC4EncodeUTF_8::convert2unicode(const char *src, unsigned int src_length)
{
	if (NULL == src)
		return std::wstring();
	if (0 == src_length)
		return std::wstring();

	unsigned int need_length = calcUnicodeStringLength(src, src_length);
	if (0 == need_length)
		return std::wstring();

	unsigned int offset = 0;
	std::wstring unicodeStr(need_length, 0);
	wchar_t chr=0;
	for (unsigned int i=0;i<src_length;)
	{
		if ((0x80 & *(src+i)) == 0) // ASCII
		{
			chr=*(src+i);
			i++;
		}
		else if((0xE0 & *(src+i)) == 0xC0) // 110xxxxx 10xxxxxx
		{
			chr =(*(src+i) & 0x3F)<<6;
			chr|=(*(src+i+1) & 0x3F);
			i+=2;
		}
		else if((0xF0 & *(src+i)) == 0xE0) // 1110xxxx 10xxxxxx 10xxxxxx
		{
			chr =(*(src+i) & 0x1F)<<12;
			chr|=(*(src+i+1) & 0x3F)<<6;
			chr|=(*(src+i+2) & 0x3F);
			i+=3;
		}
		/*
		else if() // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		{}
		else if() // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx  10xxxxxx
		{}
		else if() // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx  10xxxxxx 10xxxxxx 
		{}
		*/
		else // not utf-8 string
		{
			return unicodeStr;
		}
		unicodeStr[offset++] = chr;
	}

	return unicodeStr;
}

bool CC4EncodeUTF_8::convert2unicode(const char *src, unsigned int src_length, char *dest, unsigned int dest_length, bool check_dest_length)
{
	if (NULL == src || NULL == dest)
		return false;
	if (!(dest>src+src_length || src>dest+dest_length))
		return false;
	if ((dest_length&1) != 0)
		return false;
	if (check_dest_length)
	{
		if (dest_length < calcUnicodeStringLength(src, src_length)*sizeof(wchar_t))
			return false;
	}

	unsigned int offset = 0;
	wchar_t chr=0;
	for (unsigned int i=0; i<src_length;)
	{
		if ((0x80 & *(src+i)) == 0) // ASCII
		{
			chr = *(src+i);
			i++;
		}
		else if((0xE0 & *(src+i)) == 0xC0) // 110xxxxx 10xxxxxx
		{
			chr =(*(src+i) & 0x3F)<<6;
			chr|=(*(src+i+1) & 0x3F);
			i+=2;
		}
		else if((0xF0 & *(src+i)) == 0xE0) // 1110xxxx 10xxxxxx 10xxxxxx
		{
			chr =(*(src+i) & 0x1F)<<12;
			chr|=(*(src+i+1) & 0x3F)<<6;
			chr|=(*(src+i+2) & 0x3F);
			i+=3;
		}
		/*
		else if() // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		{}
		else if() // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx  10xxxxxx
		{}
		else if() // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx  10xxxxxx 10xxxxxx 
		{}
		*/
		else // not utf-8 string
		{
			return false;
		}
		*((wchar_t*)dest+offset) = chr;
		offset++;
	}
	return true;
}

bool CC4EncodeUTF_8::convert2unicode(const char *src, unsigned int src_length, wchar_t *dest, unsigned int dest_str_length, bool check_dest_length)
{
	return convert2unicode(src, src_length, (char*)dest, dest_str_length*sizeof(wchar_t), check_dest_length);
}

unsigned int CC4EncodeUTF_8::calcUnicodeStringLength(const char *src, unsigned int src_length)
{
	if (NULL == src)
		return 0;
	if (0 == src_length)
		return 0;

	unsigned int dest_length=0;
	for (unsigned int i=0; i<src_length;)
	{
		// Only _match first byte of the utf-8 character
		if ((0x80 & *(src+i)) == 0) // ASCII
		{
			i++;
			dest_length++;
		}
		else if((0xE0 & *(src+i)) == 0xC0) // 110xxxxx 10xxxxxx
		{
			i+=2;
			dest_length++;
		}
		else if((0xF0 & *(src+i)) == 0xE0) // 1110xxxx 10xxxxxx 10xxxxxx
		{
			i+=3;
			dest_length++;
		}
		/*
		else if() // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
		{}
		else if() // 111110xx 10xxxxxx 10xxxxxx 10xxxxxx  10xxxxxx
		{}
		else if() // 1111110x 10xxxxxx 10xxxxxx 10xxxxxx  10xxxxxx 10xxxxxx 
		{}
		*/
		else // not utf-8 string
		{
			return 0;
		}
	}

	return dest_length;
}