#include "stdafx.h"
#include "resource.h"
#include "CSystemInfo.h"
#include <TlHelp32.h>

CString CCommon::LoadText(UINT id, LPCTSTR back_str)
{
	CString str;
	str.LoadString(id);
	if (back_str != nullptr)
		str += back_str;
	return str;
}

CString CCommon::LoadText(LPCTSTR front_str, UINT id, LPCTSTR back_str)
{
	CString str;
	str.LoadString(id);
	if (back_str != nullptr)
		str += back_str;
	if (front_str != nullptr)
		str = front_str + str;
	return str;
}

wstring CCommon::StrToUnicode(const char* str, bool utf8)
{
	wstring result;
	int size;
	size = MultiByteToWideChar((utf8 ? CP_UTF8 : CP_ACP), 0, str, -1, NULL, 0);
	if (size <= 0) return wstring();
	wchar_t* str_unicode = new wchar_t[size + 1];
	MultiByteToWideChar((utf8 ? CP_UTF8 : CP_ACP), 0, str, -1, str_unicode, size);
	result.assign(str_unicode);
	delete[] str_unicode;
	return result;
}
string CCommon::UnicodeToStr(const wchar_t * wstr, bool utf8)
{
	string result;
	int size{ 0 };
	size = WideCharToMultiByte((utf8 ? CP_UTF8 : CP_ACP), 0, wstr, -1, NULL, 0, NULL, NULL);
	if (size <= 0) return string();
	char* str = new char[size + 1];
	WideCharToMultiByte((utf8 ? CP_UTF8 : CP_ACP), 0, wstr, -1, str, size, NULL, NULL);
	result.assign(str);
	delete[] str;
	return result;
}

__int64 CCommon::CompareFileTime2(FILETIME time1, FILETIME time2)
{
	__int64 a = static_cast<__int64>(time1.dwHighDateTime) << 32 | time1.dwLowDateTime;
	__int64 b = static_cast<__int64>(time2.dwHighDateTime) << 32 | time2.dwLowDateTime;
	return b - a;
}
double CCommon::StringSimilarDegree_LD(const string & srcString, const string & matchString)
{
	int n = srcString.size();
	int m = matchString.size();
	//int[, ] d = new int[n + 1, m + 1]; // matrix
	vector<vector<int>> d(n + 1, vector<int>(m + 1));
	int cost; // cost
			  // Step 1£¨Èç¹ûÆäÖÐÒ»¸ö×Ö·û´®³¤¶ÈÎª0£¬ÔòÏàËÆ¶ÈÎª1£©£¿
			  //if (n == 0) return (double)m / max(srcString.size(), matchString.size());
			  //if (m == 0) return (double)n / max(srcString.size(), matchString.size());
	if (n == 0 || m == 0) return 0.0;	//Èç¹ûÆäÖÐÒ»¸ö×Ö·û´®³¤¶ÈÎª0£¬ÔòÏàËÆ¶ÈÎª0
										// Step 2
	for (int i = 0; i <= n; d[i][0] = i++);
	for (int j = 0; j <= m; d[0][j] = j++);
	// Step 3
	for (int i = 1; i <= n; i++)
	{
		//Step 4
		for (int j = 1; j <= m; j++)
		{
			// Step 5
			cost = (matchString.substr(j - 1, 1) == srcString.substr(i - 1, 1) ? 0 : 1);
			// Step 6
			d[i][j] = min(min(d[i - 1][j] + 1, d[i][j - 1] + 1), d[i - 1][j - 1] + cost);
		}
	}

	// Step 7
	double ds = 1 - (double)d[n][m] / max(srcString.size(), matchString.size());

	return ds;
}

CString CCommon::DataSizeToString(unsigned int size, SpeedUnit type)
{
	CString value_str, unit_str;
	BOOL speed_short_mode = FALSE;
	BOOL hide_unit = FALSE;
	BOOL unit_byte = TRUE;

	switch (type)
	{
	case SpeedUnit::AUTO:
		if (speed_short_mode)
		{
			if (size < 1024 * 10)					//10KBÒÔÏÂÒÔKBÎªµ¥Î»£¬±£Áô1Î»Ð¡Êý
			{
				value_str.Format(_T("%.1f"), size / 1024.0f);
				unit_str = _T("K");
			}
			else if (size < 1024 * 1000)			//1000KBÒÔÏÂÒÔKBÎªµ¥Î»£¬±£ÁôÕûÊý
			{
				value_str.Format(_T("%.0f"), size / 1024.0f);
				unit_str = _T("K");
			}
			else if (size < 1024 * 1024 * 1000)		//1000MBÒÔÏÂÒÔMBÎªµ¥Î»£¬±£Áô1Î»Ð¡Êý
			{
				value_str.Format(_T("%.1f"), size / 1024.0f / 1024.0f);
				unit_str = _T("M");
			}
			else
			{
				value_str.Format(_T("%.2f"), size / 1024.0f / 1024.0f / 1024.0f);
				unit_str = _T("G");
			}
		}
		else
		{
			if (size < 1024 * 10)					//10KBÒÔÏÂÒÔKBÎªµ¥Î»£¬±£Áô2Î»Ð¡Êý
			{
				value_str.Format(_T("%.2f"), size / 1024.0f);
				unit_str = _T("KB");
			}
			else if (size < 1024 * 1000)			//1000KBÒÔÏÂÒÔKBÎªµ¥Î»£¬±£Áô1Î»Ð¡Êý
			{
				value_str.Format(_T("%.1f"), size / 1024.0f);
				unit_str = _T("KB");
			}
			else if (size < 1024 * 1024 * 1000)		//1000MBÒÔÏÂÒÔMBÎªµ¥Î»£¬±£Áô2Î»Ð¡Êý
			{
				value_str.Format(_T("%.2f"), size / 1024.0f / 1024.0f);
				unit_str = _T("MB");
			}
			else
			{
				value_str.Format(_T("%.2f"), size / 1024.0f / 1024.0f / 1024.0f);
				unit_str = _T("GB");
			}
		}
		break;
	case SpeedUnit::KBPS:
		if (speed_short_mode)
		{
			if (size < 1024 * 10)					//10KBÒÔÏÂ±£Áô1Î»Ð¡Êý
				value_str.Format(_T("%.1f"), size / 1024.0f);
			else					//10KBÒÔÉÏ±£ÁôÕûÊý
				value_str.Format(_T("%.0f"), size / 1024.0f);
			if (!hide_unit)
				unit_str = _T("K");
		}
		else
		{
			if (size < 1024 * 10)					//10KBÒÔÏÂ±£Áô2Î»Ð¡Êý
				value_str.Format(_T("%.2f"), size / 1024.0f);
			else			//10KBÒÔÉÏ±£Áô1Î»Ð¡Êý
				value_str.Format(_T("%.1f"), size / 1024.0f);
			if (!hide_unit)
				unit_str = _T("KB");
		}
		break;
	case SpeedUnit::MBPS:
		if (speed_short_mode)
		{
			value_str.Format(_T("%.1f"), size / 1024.0f / 1024.0f);
			if (!hide_unit)
				unit_str = _T("M");
		}
		else
		{
			value_str.Format(_T("%.2f"), size / 1024.0f / 1024.0f);
			if (!hide_unit)
				unit_str = _T("MB");
		}
		break;
	}
	CString str;
	if (!hide_unit)
		str = value_str + _T(' ') + unit_str;
	else
		str = value_str + unit_str;
	if (!unit_byte)
	{
		if (speed_short_mode && !hide_unit)
			str += _T('b');		//Èç¹ûÊ¹ÓÃ±ÈÌØ(bit)Îªµ¥Î»£¬¼´Ê¹ÉèÖÃÁËÍøËÙ¼ò½àÄ£Ê½£¬Ò²½«¡°b¡±ÏÔÊ¾³öÀ´
		else
			str.Replace(_T('B'), _T('b'));	//Èç¹ûÊ¹ÓÃ±ÈÌØ(bit)Îªµ¥Î»£¬½«BÌæ»»³Éb
	}
	return str;
}


CSystemInfo::CSystemInfo()
{
	Init();
}


CSystemInfo::~CSystemInfo()
{
	if (m_pIfTable) {
		free(m_pIfTable);
	}
}

void CSystemInfo::Init()
{
	//Îªm_pIfTable¿ª±ÙËùÐè´óÐ¡µÄÄÚ´æ
	
	m_dwSize = sizeof(MIB_IFTABLE);
	m_pIfTable = (MIB_IFTABLE *)malloc(m_dwSize);
	int rtn;
	rtn = GetIfTable(m_pIfTable, &m_dwSize, FALSE);
	if (rtn == ERROR_INSUFFICIENT_BUFFER)	//Èç¹ûº¯Êý·µ»ØÖµÎªERROR_INSUFFICIENT_BUFFER£¬ËµÃ÷m_pIfTableµÄ´óÐ¡²»¹»
	{
		free(m_pIfTable);
		m_pIfTable = (MIB_IFTABLE *)malloc(m_dwSize);	//ÓÃÐÂµÄ´óÐ¡ÖØÐÂ¿ª±ÙÒ»¿éÄÚ´æ
	}
	//»ñÈ¡µ±Ç°ËùÓÐµÄÁ¬½Ó£¬²¢±£´æµ½m_connectionsÈÝÆ÷ÖÐ
	GetIfTable(m_pIfTable, &m_dwSize, FALSE);
	if (TRUE)
	{
		CAdapterCommon::GetAdapterInfo(m_connections);
		CAdapterCommon::GetIfTableInfo(m_connections, m_pIfTable);
	}
	else
	{
		CAdapterCommon::GetAllIfTableInfo(m_connections, m_pIfTable);
	}

}

int CSystemInfo::GetMemoryUsage()
{
	//»ñÈ¡ÄÚ´æÀûÓÃÂÊ
	MEMORYSTATUSEX statex;
	statex.dwLength = sizeof(statex);
	GlobalMemoryStatusEx(&statex);
	m_memory_usage = statex.dwMemoryLoad;
	m_used_memory = static_cast<int>((statex.ullTotalPhys - statex.ullAvailPhys) / 1024);
	m_total_memory = static_cast<int>(statex.ullTotalPhys / 1024);

	return m_memory_usage;
}

int CSystemInfo::GetCpuUsage()
{
	FILETIME idleTime;
	FILETIME kernelTime;
	FILETIME userTime;
	GetSystemTimes(&idleTime, &kernelTime, &userTime);

	__int64 idle = CCommon::CompareFileTime2(m_preidleTime, idleTime);
	__int64 kernel = CCommon::CompareFileTime2(m_prekernelTime, kernelTime);
	__int64 user = CCommon::CompareFileTime2(m_preuserTime, userTime);

	if (kernel + user == 0)
	{
		m_cpu_usage = 0;
	}
	else
	{
		//£¨×ÜµÄÊ±¼ä-¿ÕÏÐÊ±¼ä£©/×ÜµÄÊ±¼ä=Õ¼ÓÃcpuµÄÊ±¼ä¾ÍÊÇÊ¹ÓÃÂÊ
		m_cpu_usage = static_cast<int>(abs((kernel + user - idle) * 100 / (kernel + user)));
	}
	m_preidleTime = idleTime;
	m_prekernelTime = kernelTime;
	m_preuserTime = userTime;
	return m_cpu_usage;
}

int CSystemInfo::GetNetworkTraffic()
{
	int rtn = GetIfTable(m_pIfTable, &m_dwSize, FALSE);
	m_in_bytes = 0;
	m_out_bytes = 0;
	for (size_t i{}; i < m_connections.size(); i++)
	{
		m_in_bytes += m_pIfTable->table[m_connections[i].index].dwInOctets;
		m_out_bytes += m_pIfTable->table[m_connections[i].index].dwOutOctets;
	}

	if ((m_in_bytes == 0 && m_out_bytes == 0) || (m_last_in_bytes == 0 && m_last_out_bytes) || m_connection_change_flag)
	{
		m_in_speed = 0;
		m_out_speed = 0;
	}
	else
	{
		m_in_speed = static_cast<unsigned int>(m_in_bytes - m_last_in_bytes);
		m_out_speed = static_cast<unsigned int>(m_out_bytes - m_last_out_bytes);
	}
	//Èç¹û´óÓÚ1GB/s£¬ËµÃ÷¿ÉÄÜ²úÉúÁËÒì³££¬ÍøËÙÎÞÐ§
	if (m_in_speed > 1073741824)
		m_in_speed = 0;
	if (m_out_speed > 1073741824)
		m_out_speed = 0;

	m_connection_change_flag = false;	//Çå³ýÁ¬½Ó·¢Éú±ä»¯µÄ±êÖ¾

	m_last_in_bytes = m_in_bytes;
	m_last_out_bytes = m_out_bytes;
	return 0;
}

CString CSystemInfo::GetUpTraf() const
{
	return CCommon::DataSizeToString(m_in_speed);
}

CString CSystemInfo::GetDownTraf() const
{
	return CCommon::DataSizeToString(m_out_speed);
}


CSystemInfo SYSINFO;



CAdapterCommon::CAdapterCommon()
{
}


CAdapterCommon::~CAdapterCommon()
{
}

void CAdapterCommon::GetAdapterInfo(vector<NetWorkConection>& adapters)
{
	adapters.clear();
	PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();		//PIP_ADAPTER_INFO½á¹¹ÌåÖ¸Õë´æ´¢±¾»úÍø¿¨ÐÅÏ¢
	unsigned long stSize = sizeof(IP_ADAPTER_INFO);		//µÃµ½½á¹¹Ìå´óÐ¡,ÓÃÓÚGetAdaptersInfo²ÎÊý
	int nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);	//µ÷ÓÃGetAdaptersInfoº¯Êý,Ìî³äpIpAdapterInfoÖ¸Õë±äÁ¿;ÆäÖÐstSize²ÎÊý¼ÈÊÇÒ»¸öÊäÈëÁ¿Ò²ÊÇÒ»¸öÊä³öÁ¿

	if (ERROR_BUFFER_OVERFLOW == nRel)
	{
		//Èç¹ûº¯Êý·µ»ØµÄÊÇERROR_BUFFER_OVERFLOW
		//ÔòËµÃ÷GetAdaptersInfo²ÎÊý´«µÝµÄÄÚ´æ¿Õ¼ä²»¹»,Í¬Ê±Æä´«³östSize,±íÊ¾ÐèÒªµÄ¿Õ¼ä´óÐ¡
		//ÕâÒ²ÊÇËµÃ÷ÎªÊ²Ã´stSize¼ÈÊÇÒ»¸öÊäÈëÁ¿Ò²ÊÇÒ»¸öÊä³öÁ¿
		delete pIpAdapterInfo;	//ÊÍ·ÅÔ­À´µÄÄÚ´æ¿Õ¼ä
		pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];	//ÖØÐÂÉêÇëÄÚ´æ¿Õ¼äÓÃÀ´´æ´¢ËùÓÐÍø¿¨ÐÅÏ¢
		nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);		//ÔÙ´Îµ÷ÓÃGetAdaptersInfoº¯Êý,Ìî³äpIpAdapterInfoÖ¸Õë±äÁ¿
	}

	PIP_ADAPTER_INFO pIpAdapterInfoHead = pIpAdapterInfo;	//±£´æpIpAdapterInfoÁ´±íÖÐµÚÒ»¸öÔªËØµÄµØÖ·
	if (ERROR_SUCCESS == nRel)
	{
		while (pIpAdapterInfo)
		{
			NetWorkConection connection;
			connection.description = pIpAdapterInfo->Description;
			connection.ip_address = CCommon::StrToUnicode(pIpAdapterInfo->IpAddressList.IpAddress.String);
			connection.subnet_mask = CCommon::StrToUnicode(pIpAdapterInfo->IpAddressList.IpMask.String);
			connection.default_gateway = CCommon::StrToUnicode(pIpAdapterInfo->GatewayList.IpAddress.String);

			adapters.push_back(connection);
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
	}
	//ÊÍ·ÅÄÚ´æ¿Õ¼ä
	if (pIpAdapterInfoHead)
	{
		delete pIpAdapterInfoHead;
	}
	if (adapters.empty())
	{
		NetWorkConection connection{};
		connection.description = CCommon::UnicodeToStr(CCommon::LoadText(L"<", IDS_NO_CONNECTION, L">"));
		adapters.push_back(connection);
	}
}

void CAdapterCommon::RefreshIpAddress(vector<NetWorkConection>& adapters)
{
	vector<NetWorkConection> adapters_tmp;
	GetAdapterInfo(adapters_tmp);
	for (const auto& adapter_tmp : adapters_tmp)
	{
		for (auto& adapter : adapters)
		{
			if (adapter_tmp.description == adapter.description)
			{
				adapter.ip_address = adapter_tmp.ip_address;
				adapter.subnet_mask = adapter_tmp.subnet_mask;
				adapter.default_gateway = adapter_tmp.default_gateway;
			}
		}
	}
}

void CAdapterCommon::GetIfTableInfo(vector<NetWorkConection>& adapters, MIB_IFTABLE* pIfTable)
{
	//ÒÀ´ÎÔÚIfTableÀï²éÕÒÃ¿¸öÁ¬½Ó
	for (size_t i{}; i < adapters.size(); i++)
	{
		if (adapters[i].description.empty())
			continue;
		int index;
		index = FindConnectionInIfTable(adapters[i].description, pIfTable);
		if (index == -1)		//Èç¹ûÊ¹ÓÃ¾«È·Æ¥ÅäµÄ·½Ê½Ã»ÓÐÕÒµ½£¬Ôò²ÉÓÃÄ£ºýÆ¥ÅäµÄ·½Ê½ÔÙ²éÕÒÒ»´Î
			index = FindConnectionInIfTableFuzzy(adapters[i].description, pIfTable);
		//if (index != -1)
		//{
		adapters[i].index = index;
		adapters[i].in_bytes = pIfTable->table[index].dwInOctets;
		adapters[i].out_bytes = pIfTable->table[index].dwOutOctets;
		adapters[i].description_2 = (const char*)pIfTable->table[index].bDescr;
		//}
	}
}

void CAdapterCommon::GetAllIfTableInfo(vector<NetWorkConection>& adapters, MIB_IFTABLE * pIfTable)
{
	vector<NetWorkConection> adapters_tmp;
	GetAdapterInfo(adapters_tmp);		//»ñÈ¡IPµØÖ·
	adapters.clear();
	for (size_t i{}; i < pIfTable->dwNumEntries; i++)
	{
		NetWorkConection connection;
		connection.description = connection.description_2 = (const char*)pIfTable->table[i].bDescr;
		connection.index = i;
		connection.in_bytes = pIfTable->table[i].dwInOctets;
		connection.out_bytes = pIfTable->table[i].dwOutOctets;
		for (size_t j{}; j < adapters_tmp.size(); j++)
		{
			if (connection.description.find(adapters_tmp[j].description) != string::npos)
			{
				connection.ip_address = adapters_tmp[j].ip_address;
				connection.subnet_mask = adapters_tmp[j].subnet_mask;
				connection.default_gateway = adapters_tmp[j].default_gateway;
				break;
			}
		}
		adapters.push_back(connection);
	}
}

int CAdapterCommon::FindConnectionInIfTable(string connection, MIB_IFTABLE* pIfTable)
{
	for (size_t i{}; i < pIfTable->dwNumEntries; i++)
	{
		string descr = (const char*)pIfTable->table[i].bDescr;
		if (descr == connection)
			return i;
	}
	return -1;
}

int CAdapterCommon::FindConnectionInIfTableFuzzy(string connection, MIB_IFTABLE* pIfTable)
{
	for (size_t i{}; i < pIfTable->dwNumEntries; i++)
	{
		string descr = (const char*)pIfTable->table[i].bDescr;
		size_t index;
		//ÔÚ½Ï³¤µÄ×Ö·û´®Àï²éÕÒ½Ï¶ÌµÄ×Ö·û´®
		if (descr.size() >= connection.size())
			index = descr.find(connection);
		else
			index = connection.find(descr);
		if (index != wstring::npos)
			return i;
	}
	//Èç¹û»¹ÊÇÃ»ÓÐÕÒµ½£¬ÔòÊ¹ÓÃ×Ö·û´®Æ¥ÅäËã·¨²éÕÒ
	double max_degree{};
	int best_index{};
	for (size_t i{}; i < pIfTable->dwNumEntries; i++)
	{
		string descr = (const char*)pIfTable->table[i].bDescr;
		double degree = CCommon::StringSimilarDegree_LD(descr, connection);
		if (degree > max_degree)
		{
			max_degree = degree;
			best_index = i;
		}
	}
	return best_index;
}


typedef struct tagWNDINFO
{
	DWORD dwProcessId;
	HWND hWnd;
} WNDINFO, *LPWNDINFO;

BOOL CALLBACK MyEnumProc(HWND hWnd, LPARAM lParam)//枚举所有进程
{
	DWORD dwProcId;
	GetWindowThreadProcessId(hWnd, &dwProcId);
	LPWNDINFO pInfo = (LPWNDINFO)lParam;
	if (dwProcId == pInfo->dwProcessId)
	{
		// 在此添加更多限制条件
		CString strTmp;

		GetClassName(hWnd, strTmp.GetBuffer(200), 200);
		strTmp.ReleaseBuffer();

		if (!strTmp.Compare(L"WinDbgFrameClass"))	// 具体可使用Spy++查看
		{
			pInfo->hWnd = hWnd;
			return FALSE;
		}

		//if (IsWindowVisible(hWnd)) // 当前窗口是否可见
		//{
		//	pInfo->hWnd = hWnd;		// 获取到第一个窗口句柄
		//	return FALSE;
		//}
	}
	return TRUE;
}

HWND GetProcessHwnd(DWORD proccessId)
{
	WNDINFO wi;
	wi.dwProcessId = proccessId;
	wi.hWnd = NULL;
	EnumWindows(MyEnumProc, (LPARAM)&wi);
	// EnumChildWindows(hWnPar, MyEnumProc, (LPARAM)&wi); // 枚举窗口的子窗口句柄, MFC中的控作等
	return wi.hWnd;
}

HWND GetProcessHwndByPID(DWORD proccessId)
{
	DWORD dwPID = 0;
	HWND hwndRet = NULL;
	HWND hwndWindow = ::GetTopWindow(0);
	while (hwndWindow)
	{
		dwPID = 0;
		// 通过窗口句柄取得进程ID
		DWORD dwTheardID = ::GetWindowThreadProcessId(hwndWindow, &dwPID);
		if (dwTheardID != 0)
		{
			// 判断和参数传入的进程ID是否相等
			if (dwPID == proccessId)
			{
				// 进程ID相等，则记录窗口句柄
				hwndRet = hwndWindow;
				//break;
			}
		}
		// 取得下一个窗口句柄
		hwndWindow = ::GetNextWindow(hwndWindow, GW_HWNDNEXT);
	}
	return hwndRet;
}

vector<HWND> GetProcessInfo(CString processName)
{
	//创建进程快照(TH32CS_SNAPPROCESS表示创建所有进程的快照)
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	//PROCESSENTRY32进程快照的结构体       
	PROCESSENTRY32 pe;
	//实例化后使用Process32First获取第一个快照的进程前必做的初始化操作
	pe.dwSize = sizeof(PROCESSENTRY32);
	//下面的IF效果同:       
	//if(hProcessSnap == INVALID_HANDLE_VALUE)   无效的句柄  
	if (!Process32First(hSnapShot, &pe))
	{
		return vector<HWND>();
	}
	processName.MakeLower();
	BOOL clearprocess = FALSE;
	vector<HWND> vechWnd;
	//如果句柄有效  则一直获取下一个句柄循环下去 
	while (Process32Next(hSnapShot, &pe))
	{
		//pe.szExeFile获取当前进程的可执行文件名称 
		CString scTmp = pe.szExeFile;
		scTmp.MakeLower();
		char modPath[MAX_PATH] = { 0 };
		if (!scTmp.Compare(processName))
		{
			DWORD dwProcessID = pe.th32ProcessID;

			HWND hWnd = GetProcessHwnd(dwProcessID);

			vechWnd.push_back(hWnd);

		}
	}
	::CloseHandle(hSnapShot);
	return vechWnd;
}
