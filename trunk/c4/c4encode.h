#pragma once

#include <xstring>
#include <vector>
#include "c4policy.h"
#include "c4segment.h"

typedef wchar_t encode_type;

class CC4Encode;
class CC4EncodeAnsiBase;
class CC4EncodeUTF16;
class CC4EncodeUTF8;

class CC4Encode
{
public :
	static const wchar_t UNKNOWN_CHAR        = 0xFFFD;
	static const wchar_t LITTLEENDIAN_MARK   = 0xFEFF;
	static const wchar_t BIGENDIAN_MARK      = 0xFFFE;
	static const char    LITTLEENDIAN_BOM[2];
	static const char    BIGENDIAN_BOM[2];
	static const char    UTF_8_BOM[3];
	enum encodeType {
		typeBaseOnAnsi     = 0x01,  /* InputStream: Multibyte encoding. High byte is at first */
		typeBaseOnUnicode  = 0x02,  /* InputStream: Unicode encoding. (default) Low byte is at first */
		typeFixed          = 0x04,  /* InputStream: Fixed bytes of per character */
		typeVariable       = 0x08,  /* InputStream: Variable bytes of per character */
		typeResultAnsi     = 0x10,  /* OutputStream: multibyte string: std::string */
		typeResultUnicode  = 0x20,  /* OutputStream: Unicode string: std::wstring */
		typeExternal       = 0x40,  /* Load from external config file */
		typeInternal       = 0x80,  /* Build-in encoding */
		typeUTF16  = typeBaseOnUnicode|typeFixed|typeInternal,  /* UTF-16 */
		typeUTF8   = typeBaseOnUnicode|typeVariable|typeResultUnicode|typeInternal  /* UTF-8 */
	};

public:
	CC4Encode(const std::wstring& name, const std::wstring& version, const std::wstring& description, encode_type encode_features, bool is_auto_check);
	~CC4Encode();
	std::wstring virtual toString() const;
	bool virtual isAutoCheck() const;
	void virtual setAutoCheck(bool is_auto_check);
	std::wstring virtual getName() const;
	std::wstring virtual getVersion() const;
	std::wstring virtual getDescription() const;
	encode_type  virtual getEncodeType() const;

	/* Input Stream(Multibyte) matches this encode or not. */
	bool virtual match(const char *src, unsigned int src_length) const {return true;};

	/* Input Stream(Unicode) matches this encode or not */
	bool virtual wmatch(const wchar_t *src, unsigned int src_str_length) const {return true;};

	/* Convert input stream(Multibyte) to multibyte string */
	std::string  virtual convertText(const char *src, unsigned int src_length) const {return std::string();};
	std::string  virtual convertString(const char *src) const {return std::string();};

	/* Convert input stream(Multibyte) to Unicode string */
	std::wstring virtual wconvertText(const char *src, unsigned int src_length) const {return std::wstring();};
	std::wstring virtual wconvertString(const char *src) const {return std::wstring();};

	/* Convert input stream(Unicode) to multibyte string */
	std::string  virtual convertWideText(const wchar_t *src, unsigned int src_str_length) const {return std::string();};
	std::string  virtual convertWideString(const wchar_t *src) const {return std::string();};

	/* Convert input stream(Unicode) to Unicode string */
	std::wstring virtual wconvertWideText(const wchar_t *src, unsigned int src_str_length) const {return std::wstring();};
	std::wstring virtual wconvertWideString(const wchar_t *src) const {return std::wstring();};

private:
	std::wstring m_name;             // name of the encode, for example: Shift-JIS
	std::wstring m_version;          // version of the encode, for example: Microsoft CP932
	std::wstring m_description;      // description
	encode_type  m_encodeType;       // encode type
	bool         m_autoCheck;        // the encode is used in auto-_match mode or not
};

class CC4EncodeAnsiBase : CC4Encode
{
private:
	const unsigned char* m_mapBuffer;        // map buffer
	const unsigned int   m_mapBufferLength;  // map buffer length
	const CC4Policies*   m_policies;
	const CC4Segments*   m_segments;

public:
	CC4EncodeAnsiBase(const std::wstring& name, const std::wstring& version, const std::wstring& description, bool is_auto_check, const unsigned char *buffer, unsigned int buffer_length);
	// override
	std::wstring toString() const;
	bool isAutoCheck() const;
	void setAutoCheck(bool is_auto_check);
	std::wstring getName() const;
	std::wstring getVersion() const;
	std::wstring getDescription() const;
	encode_type  getEncodeType() const;
	bool match(const char *src, unsigned int src_length) const;
	bool wmatch(const wchar_t *src, unsigned int src_str_length) const {return false;};
	std::wstring wconvertText(const char *src, unsigned int src_length) const;
	std::wstring wconvertString(const char *src) const;
	std::string  convertWideText(const wchar_t *src, unsigned int src_str_length) const {return std::string();};
	std::string  convertWideString(const wchar_t *src) const {return std::string();};
	// implement
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

/*
class CC4EncodeUnicodeBase : CC4Encode
{
private:
	const unsigned char* m_mapBuffer;        // map buffer
	const unsigned int   m_mapBufferLength;  // map buffer length
	const CC4Policies*   m_policies;
	const CC4Segments*   m_segments;

public:
	CC4EncodeUnicodeBase(const std::wstring& name, const std::wstring& version, const std::wstring& description, bool is_auto_check, const unsigned char *buffer, unsigned int buffer_length);
	std::wstring toString() const;
	bool isAutoCheck() const;
	void setAutoCheck(bool is_auto_check);
	std::wstring getName() const;
	std::wstring getVersion() const;
	std::wstring getDescription() const;
	encode_type  getEncodeType() const;
	bool match(const char *src, unsigned int src_length) const;
	std::wstring wconvertText(const char *src, unsigned int src_length) const;
	std::wstring wconvertString(const char *src) const;
	wchar_t convertSingleChar(wchar_t chr) const;
	bool convert2unicode(const char *src, unsigned int src_length, wchar_t *dest, unsigned int dest_str_length, bool check_dest_length = false) const;
	bool setPolicies(const CC4Policies* ptr_policies);
	bool setSegments(const CC4Segments* ptr_segments);
	const CC4Policies* getPolicies() const;
	const CC4Segments* getSegments() const;
};*/

/************************************************************************/
/* CC4EncodeUTF16                                                       */
/* Notice: will treat all input as Unicode string                       */
/************************************************************************/
class CC4EncodeUTF16 : CC4Encode
{
public:
	static CC4EncodeUTF16* getInstance();
private:
	static CC4EncodeUTF16 *s_instance;  // Unicode instance. Singleton pattern
	CC4EncodeUTF16(const std::wstring& name, const std::wstring& version, const std::wstring& description, bool is_auto_check);
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
	std::wstring getDescription() const;
	encode_type  getEncodeType() const;
	static std::wstring _getName();
	static std::wstring _getVersion();
	static std::wstring _getDescription();
	static encode_type  _getEncodeType();
	bool match(const char *src, unsigned int src_length) const;
	bool wmatch(const wchar_t *src, unsigned int src_str_length) const;
	static bool _match(const char *src, unsigned int src_length);

	/* convert to utf-8 string */
	std::string convertText(const char *src, unsigned int src_length) const;
	std::string convertString(const char *src) const;

	/* simply build a Unicode string */
	std::wstring wconvertText(const char *src, unsigned int src_length) const;
	std::wstring wconvertString(const char *src) const;

	/* convert to utf-8 string */
	std::string convertWideText(const wchar_t *src, unsigned int src_str_length) const;
	std::string convertWideString(const wchar_t *src) const;

	/* simply build a Unicode string */
	std::wstring wconvertWideText(const wchar_t *src, unsigned int src_str_length) const;
	std::wstring wconvertWideString(const wchar_t *src) const;

	/* static converting functions */
	static std::string convert2utf8(const char *src, unsigned int src_length, bool is_little_endian = true);
	static std::string convert2utf8(const wchar_t *src, unsigned int src_str_length);
	static bool convert2utf8(const char *src, unsigned int src_length, char *dest, unsigned int dest_length, bool is_little_endian = true, bool check_dest_length = false);
	static unsigned int calcUtf8StringLength(const char *src, unsigned int src_length, bool is_little_endian = true);
};

/************************************************************************/
/* CC4EncodeUTF8                                                        */
/* Notice: will treat all input as UTF-8 string                         */
/************************************************************************/
class CC4EncodeUTF8 : CC4Encode
{
public:
	static CC4EncodeUTF8*   getInstance();
private:
	static CC4EncodeUTF8   *s_instance;    // Utf-8 instance. Singleton pattern
	CC4EncodeUTF8(const std::wstring& name, const std::wstring& version, const std::wstring& description, bool is_auto_check);
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
	std::wstring getDescription() const;
	encode_type  getEncodeType() const;
	static std::wstring _getName();
	static std::wstring _getVersion();
	static std::wstring _getDescription();
	static encode_type  _getEncodeType();
	bool match(const char *src, unsigned int src_length) const;
	static bool _match(const char *src, unsigned int src_length);

	/* simply build a utf-8 string */
	std::string convertText(const char *src, unsigned int src_length) const;
	std::string convertString(const char *src) const;

	/* convert to Unicode string */
	std::wstring wconvertText(const char *src, unsigned int src_length) const;
	std::wstring wconvertString(const char *src) const;

	/* Error input. Return empty string. */
	std::string convertWideText(const wchar_t *src, unsigned int src_str_length) const {return std::string();};
	std::string convertWideString(const wchar_t *src) const {return std::string();};

	/* Error input. Return empty string. */
	std::wstring wconvertWideText(const wchar_t *src, unsigned int src_str_length) const {return std::wstring();};
	std::wstring wconvertWideString(const wchar_t *src) const {return std::wstring();};

	/* static converting functions */
	static std::wstring convert2unicode(const char *src, unsigned int src_length);
	static bool convert2unicode(const char *src, unsigned int src_length, char *dest, unsigned int dest_length, bool check_dest_length = false);
	static bool convert2unicode(const char *src, unsigned int src_length, wchar_t *dest, unsigned int dest_str_length, bool check_dest_length = false);
	static unsigned int calcUnicodeStringLength(const char *src, unsigned int src_length);
};