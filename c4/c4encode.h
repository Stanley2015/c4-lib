#pragma once

#include <xstring>
#include <vector>
#include "c4policy.h"
#include "c4segment.h"

class CC4Encode;
class CC4EncodeAnsi;
class CC4EncodeUnicode;
class CC4EncodeUTF_8;

class CC4Encode
{
public :
	static const wchar_t UNKNOWN_CHAR        = 0xFFFD;
	static const wchar_t LITTLEENDIAN_MARK   = 0xFEFF;
	static const wchar_t BIGENDIAN_MARK      = 0xFFFE;
	static const char    LITTLEENDIAN_BOM[2];
	static const char    BIGENDIAN_BOM[2];
	static const char    UTF_8_BOM[3];

public:
	CC4Encode(const std::wstring& name, const std::wstring& version, bool is_auto_check);
	~CC4Encode();
	std::wstring virtual toString() const;
	bool virtual isAutoCheck() const;
	void virtual setAutoCheck(bool is_auto_check);
	std::wstring virtual getName() const;
	std::wstring virtual getVersion() const;
	bool virtual match(const char *src, unsigned int src_length) const {return true;};
	std::wstring virtual convert(const char *src, unsigned int src_length) const {return std::wstring();};

private:
	std::wstring m_name;             // name of the encode, for example: Shift-JIS
	std::wstring m_version;          // version of the encode, for example: Microsoft CP932
	bool         m_autoCheck;        // the encode is used in auto-_match mode or not
};

class CC4EncodeAnsi : CC4Encode
{
private:
	const unsigned char* m_mapBuffer;        // map buffer
	const unsigned int   m_mapBufferLength;  // map buffer length
	const CC4Policies*   m_policies;
	const CC4Segments*   m_segments;

public:
	CC4EncodeAnsi(const std::wstring& name, const std::wstring& version, bool is_auto_check, const unsigned char *buffer, unsigned int buffer_length);
	std::wstring toString() const;
	bool isAutoCheck() const;
	void setAutoCheck(bool is_auto_check);
	std::wstring getName() const;
	std::wstring getVersion() const;
	bool match(const char *src, unsigned int src_length) const;
	std::wstring convert(const char *src, unsigned int src_length) const;
	wchar_t convertSingleChar(char high_byte, char low_byte) const;
	wchar_t convertSingleChar(wchar_t ansi_character) const;
	unsigned int calcUnicodeStringLength(const char *src, unsigned int src_length) const;
	bool convert2unicode(const char *src, unsigned int src_length, char *dest, unsigned int dest_length, bool check_dest_length = false) const;
	bool convert2unicode(const char *src, unsigned int src_length, wchar_t *dest, unsigned int dest_str_length, bool check_dest_length = false) const;
	bool setPolicies(const CC4Policies* ptr_policies);
	bool setSegments(const CC4Segments* ptr_segments);
	const CC4Policies* getPolicies() const;
	const CC4Segments* getSegments() const;
};

class CC4EncodeUnicode : CC4Encode
{
public:
	static CC4EncodeUnicode* getInstance();
private:
	static CC4EncodeUnicode *s_instance;  // Unicode instance. Singleton pattern
	CC4EncodeUnicode(const std::wstring& name, const std::wstring& version, bool is_auto_check);
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (s_instance)
				delete s_instance;
		}
	};
	static CGarbo garbo;
public:
	std::wstring toString() const;
	bool isAutoCheck() const;
	void setAutoCheck(bool is_auto_check);
	std::wstring getName() const;
	std::wstring getVersion() const;
	static std::wstring _getName();
	static std::wstring _getVersion();
	bool match(const char *src, unsigned int src_length) const;
	static bool _match(const char *src, unsigned int src_length);
	std::wstring convert(const char *src, unsigned int src_length) const;
	static std::string convert2utf8(const char *src, unsigned int src_length, bool is_little_endian = true);
	static std::string convert2utf8(const wchar_t *src, unsigned int src_str_length);
	static bool convert2utf8(const char *src, unsigned int src_length, char *dest, unsigned int dest_length, bool is_little_endian = true, bool check_dest_length = false);
	static unsigned int calcUtf8StringLength(const char *src, unsigned int src_length, bool is_little_endian = true);
};

class CC4EncodeUTF_8 : CC4Encode
{
public:
	static CC4EncodeUTF_8*   getInstance();
private:
	static CC4EncodeUTF_8   *s_instance;    // Utf-8 instance. Singleton pattern
	CC4EncodeUTF_8(const std::wstring& name, const std::wstring& version, bool is_auto_check);
	class CGarbo
	{
	public:
		~CGarbo()
		{
			if (s_instance)
				delete s_instance;
		}
	};
	static CGarbo garbo;
public:
	std::wstring toString() const;
	bool isAutoCheck() const;
	void setAutoCheck(bool is_auto_check);
	std::wstring getName() const;
	std::wstring getVersion() const;
	static std::wstring _getName();
	static std::wstring _getVersion();
	bool match(const char *src, unsigned int src_length) const;
	static bool _match(const char *src, unsigned int src_length);
	std::wstring convert(const char *src, unsigned int src_length) const;
	static std::wstring convert2unicode(const char *src, unsigned int src_length);
	static bool convert2unicode(const char *src, unsigned int src_length, char *dest, unsigned int dest_length, bool check_dest_length = false);
	static bool convert2unicode(const char *src, unsigned int src_length, wchar_t *dest, unsigned int dest_str_length, bool check_dest_length = false);
	static unsigned int calcUnicodeStringLength(const char *src, unsigned int src_length);
};