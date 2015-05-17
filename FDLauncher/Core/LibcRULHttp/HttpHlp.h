/* -------------------------------------------------------------------------
//	File Name	:	HttpHlp.h
//	Author		:	Zhang Fan
//	Create Time	:	2012-3-20 16:15:30
//	Description	:
//
// -----------------------------------------------------------------------*/

#ifndef __HTTPHLP_H__
#define __HTTPHLP_H__

#include <set>
#include <map>
#include <string>
#include <cstring>
#include <cstdio>
#include <vector>
#include "../Misc/ErrorCheck.h"

// -------------------------------------------------------------------------

namespace HttpHlp {

    struct TraitsHttpHeaders {
        static char separator1() {
            return '\n';
        }
        static char separator2() {
            return ':';
        }
    };

    struct TraitsHttpParams {
        static char separator1() {
            return '&';
        }
        static char separator2() {
            return '=';
        }
    };

    struct TraitsHttpCookie {
        static char separator1() {
            return ';';
        }
        static char separator2() {
            return '=';
        }
    };

    struct TraitsNoEncoding {
        static char separator1() {
            return '|';
        }
        static char separator2() {
            return '=';
        }
    };

    struct UrlEncoding {

        static std::string& encode(const char* psrc, std::string& str)	{
            X_ASSERT(psrc != NULL && *psrc != 0);

            str.resize(::strlen(psrc) * 3 + 1, '\0');

            char* pdst = &(*str.begin());

            for (; *psrc != 0; ++psrc) {

                unsigned char* p = (unsigned char*)psrc;
                if (need_encoding(*p)) {

                    char a[3];
                    sprintf(a, "%02x", *p);
                    *pdst = '%';
                    *(pdst + 1) = a[0];
                    *(pdst + 2) = a[1];
                    pdst += 3;
                }
                else {
                    *pdst++ = *p;
                }
            }

            X_ASSERT(str.size() >= pdst - &(*str.begin()));
            str.resize(pdst - &(*str.begin()));

            return str;
        }

        static void decode(const char* psrc, char* pdst) {

            X_ASSERT(psrc != NULL && *psrc != 0);
            X_ASSERT(pdst != NULL);

            for (; *psrc != 0; ++psrc) {

                if (*psrc == '%') {

                    int code;
                    if (sscanf(psrc+1, "%02x", &code) != 1) 
                        code = '?';
                    *pdst++ = code;
                    psrc += 2;
                }
                else {
                    *pdst++ = *psrc;
                }
            }
        }

        static bool need_encoding(const char* pcsz) {
            X_ASSERT(pcsz != NULL);
            for (; *pcsz != 0; ++pcsz) {
                if (need_encoding(*pcsz))
                    return true;
            }
            return false;
        }

    private:
        static bool need_encoding(const char ch) {
            const unsigned char *p = (const unsigned char*)&ch;
            return (*p >  126 || *p == '&' || *p == ' ' || *p == '=' || *p == '%' || 
                *p == '.' || *p == '/' || *p == '+' || 
                *p == '`' || *p == '{' || *p == '}' || *p == '|' || *p == '[' ||
                *p == ']' || *p == '\"' || *p == '<' || *p == '>' || *p == '\\' ||
                *p == '^' || *p == ';' || *p == ':');
        }
    };

    template <class _Traits>
    class KeyValueParams {
	public:
		typedef std::pair<std::string, std::string> value_type;
    public:
        void add(const char* pcszKey, const char* pcszValue, bool fNeedCoding = true) {
            X_ASSERT(pcszKey != NULL && *pcszKey != 0);
			            
			if(fNeedCoding && pcszValue != NULL && *pcszValue != 0)
			{
				std::string str;				
				UrlEncoding::encode(pcszValue, str);
				m_items.push_back(std::make_pair(std::string(pcszKey), str));
			}
			else
			{
				m_items.push_back(std::make_pair(std::string(pcszKey), std::string(pcszValue)));
			}
        }

        void add(const char* pcszKeyValuePairs) {
            X_ASSERT(pcszKeyValuePairs != NULL && *pcszKeyValuePairs != 0);
            
			std::string str(pcszKeyValuePairs);
			size_t pos = str.find(_Traits::separator2());
			X_ASSERT(pos != std::string::npos);
			m_items.push_back(std::make_pair(str.substr(0, pos), str.substr(pos + 1)));			
        }

        std::string get() const {
			if(m_items.empty())
				return std::string();
			std::stringstream ss;
			for(std::vector<value_type>::const_iterator it = m_items.begin();it != m_items.end();++it)
			{
				ss << it->first;
				ss << _Traits::separator2();
				ss << it->second;
				ss << _Traits::separator1();
			}
			std::string str = ss.str();
			str.erase(str.size() - 1);
			return str;
        }

		const std::vector<value_type>& getItems() const {
			return m_items;
		}

        std::set<std::string> query(const std::string& key, bool needDecode = true) {
            std::set<std::string> values;

            for (std::vector<value_type>::const_iterator it = m_items.begin();it != m_items.end();++it)
            {
                if (it->first == key) {
                    std::string value;

                    if (needDecode) {
                        value.resize(it->second.size());
                        UrlEncoding::decode(it->second.c_str(), (char *)value.data());
                    }
                    else
                        value = it->second;

                    X_ASSERT(value.size());
                    values.insert(value);
                }
            }

            return values;
        }

        void parse(const std::string& header, bool needEncode = false) {

            std::string safeHeader = header;
            StringHelper::replace(safeHeader, "\r", "");

            StringHelper::SplitData rows;
            StringHelper::split(safeHeader.c_str(), _Traits::separator1(), rows);

            for (StringHelper::SplitData::iterator it = rows.begin(); it != rows.end(); ++it)
            {
                std::string::size_type find = (*it).find(_Traits::separator2());
                if (std::string::npos != find) {
                    std::string key = (*it).substr(0, find).c_str();
                    std::string value = (*it).substr(find + 1, (*it).length() - find - 1).c_str();

                    add(StringHelper::trimLeft(key).c_str(), StringHelper::trimLeft(value).c_str(), needEncode);
                }
            }
        }

        void clear() {
            m_items.clear();
        }

    private:        
		std::vector<value_type>  m_items;
    };

    typedef KeyValueParams<TraitsHttpHeaders>	Headers;
    typedef KeyValueParams<TraitsHttpParams>	Params;
    typedef KeyValueParams<TraitsHttpCookie>    Cookie;
}

//--------------------------------------------------------------------------
#endif /* __HTTPHLP_H__ */