#pragma once

#include <string>
#include <vector>
using std::vector;
using std::string;
using std::wstring;


#pragma comment (lib, "iphlpapi.lib")
//ÍøËÙµ¥Î»
enum class SpeedUnit
{
	AUTO,		//×Ô¶¯
	KBPS,		//KB/s
	MBPS		//MB/s
};

class CCommon
{
public:
	static double StringSimilarDegree_LD(const string& srcString, const string& matchString);
	static wstring StrToUnicode(const char* str, bool utf8 = false);
	static string UnicodeToStr(const wchar_t* wstr, bool utf8 = false);
	//´Ó×ÊÔ´ÎÄ¼þÔØÈë×Ö·û´®¡£ÆäÖÐ£¬front_str¡¢back_strÎªÔØÈë×Ö·û´®Ê±ÐèÒªÔÚÇ°Ãæ»òºóÃæÌí¼ÓµÄ×Ö·û´®
	static CString LoadText(UINT id, LPCTSTR back_str = nullptr);
	static CString LoadText(LPCTSTR front_str, UINT id, LPCTSTR back_str = nullptr);
	static __int64 CompareFileTime2(FILETIME time1, FILETIME time2);
	static CString DataSizeToString(unsigned int size, SpeedUnit type = SpeedUnit::AUTO);
};



//±£´æÒ»¸öÍøÂçÁ¬½ÓÐÅÏ¢
struct NetWorkConection
{
	int index{};			//¸ÃÁ¬½ÓÔÚMIB_IFTABLEÖÐµÄË÷Òý
	string description;		//ÍøÂçÃèÊö£¨»ñÈ¡×ÔGetAdapterInfo£©
	string description_2;	//ÍøÂçÃèÊö£¨»ñÈ¡×ÔGetIfTable£©
	unsigned int in_bytes;	//³õÊ¼Ê±ÒÑ½ÓÊÕ×Ö½ÚÊý
	unsigned int out_bytes;	//³õÊ¼Ê±ÒÑ·¢ËÍ×Ö½ÚÊý
	wstring ip_address{ L"-.-.-.-" };	//IPµØÖ·
	wstring subnet_mask{ L"-.-.-.-" };	//×ÓÍøÑÚÂë
	wstring default_gateway{ L"-.-.-.-" };	//Ä¬ÈÏÍø¹Ø
};

class CAdapterCommon
{
public:
	CAdapterCommon();
	~CAdapterCommon();

	//»ñÈ¡ÍøÂçÁ¬½ÓÁÐ±í£¬Ìî³äÍøÂçÃèÊö¡¢IPµØÖ·¡¢×ÓÍøÑÚÂë¡¢Ä¬ÈÏÍø¹ØÐÅÏ¢
	static void GetAdapterInfo(vector<NetWorkConection>& adapters);

	//Ë¢ÐÂÍøÂçÁ¬½ÓÁÐ±íÖÐµÄIPµØÖ·¡¢×ÓÍøÑÚÂë¡¢Ä¬ÈÏÍø¹ØÐÅÏ¢
	static void RefreshIpAddress(vector<NetWorkConection>& adapters);

	//»ñÈ¡ÍøÂçÁÐ±íÖÐÃ¿¸öÍøÂçÁ¬½ÓµÄMIB_IFTABLEÖÐµÄË÷Òý¡¢³õÊ¼Ê±ÒÑ½ÓÊÕ/·¢ËÍ×Ö½ÚÊýµÄÐÅÏ¢
	static void GetIfTableInfo(vector<NetWorkConection>& adapters, MIB_IFTABLE* pIfTable);

	//Ö±½Ó½«MIB_IFTABLEÖÐµÄËùÓÐÁ¬½ÓÌí¼Óµ½adaptersÈÝÆ÷ÖÐ
	static void GetAllIfTableInfo(vector<NetWorkConection>& adapters, MIB_IFTABLE* pIfTable);
private:
	//¸ù¾ÝÒ»¸öÍøÂçÁ¬½ÓÃèÊöÅÐ¶ÏÊÇ·ñÔÚIfTableÁÐ±íÀï£¬·µ»ØË÷Òý£¬ÕÒ²»µ½Ôò·µ»Ø-1
	static int FindConnectionInIfTable(string connection, MIB_IFTABLE* pIfTable);

	//¸ù¾ÝÒ»¸öÍøÂçÁ¬½ÓÃèÊöÅÐ¶ÏÊÇ·ñÔÚIfTable½ÓÁÐ±íÀï£¬·µ»ØË÷Òý£¬ÕÒ²»µ½Ôò·µ»Ø-1¡£Ö»ÐèÒª²¿·ÖÆ¥Åä
	static int FindConnectionInIfTableFuzzy(string connection, MIB_IFTABLE* pIfTable);
};



class CSystemInfo
{
public:
	CSystemInfo();
	~CSystemInfo();

	void Init();

	int GetMemoryUsage();
	int GetCpuUsage();
	int GetNetworkTraffic();
	CString GetUpTraf() const;
	CString GetDownTraf() const;

	unsigned int m_in_speed{};		//ÏÂÔØËÙ¶È
	unsigned int m_out_speed{};		//ÉÏ´«ËÙ¶È
	int m_cpu_usage{};		//CPUÀûÓÃÂÊ
	int m_memory_usage{};	//ÄÚ´æÀûÓÃÂÊ
	int m_used_memory{};	//¿ÉÓÃÎïÀíÄÚ´æ£¨µ¥Î»ÎªKB£©
	int m_total_memory{};	//ÎïÀíÄÚ´æ×ÜÁ¿£¨µ¥Î»ÎªKB£©

	vector<NetWorkConection> m_connections;	//±£´æ»ñÈ¡µ½µÄÒªÏÔÊ¾µ½¡°
	MIB_IFTABLE* m_pIfTable;
	DWORD m_dwSize{};	//m_pIfTableµÄ´óÐ¡
	int m_connection_selected{ 0 };	//ÒªÏÔÊ¾Á÷Á¿µÄÁ¬½ÓµÄÐòºÅ
	unsigned __int64 m_in_bytes;		//µ±Ç°ÒÑ½ÓÊÕµÄ×Ö½ÚÊý
	unsigned __int64 m_out_bytes;	//µ±Ç°ÒÑ·¢ËÍµÄ×Ö½ÚÊý
	unsigned __int64 m_last_in_bytes{};	//ÉÏ´ÎÒÑ½ÓÊÕµÄ×Ö½ÚÊý
	unsigned __int64 m_last_out_bytes{};	//ÉÏ´ÎÒÑ·¢ËÍµÄ×Ö½ÚÊý
	bool m_connection_change_flag{ false };		//Èç¹ûÖ

	FILETIME m_preidleTime;
	FILETIME m_prekernelTime;
	FILETIME m_preuserTime;

};


extern CSystemInfo SYSINFO;