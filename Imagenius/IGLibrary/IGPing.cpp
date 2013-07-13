#include "StdAfx.h"
#include <string>

extern "C" {
#include "ipexport.h"
#include "icmpapi.h"
}

namespace IGPing
{
// An array of error message strings.
std::wstring ErrorStrings[] = {
	L"Error Base",
	L"Buffer too small.",
	L"Destination net unreachable.",
	L"Destination host unreachable.",
	L"Destination protocol unreachable.",
	L"Destination port unreachable.",
	L"Out of resources.",
	L"Bad option.",
	L"Hardware error.",
	L"Packet too large.",
	L"Request timed out.",
	L"Bad request.",
	L"Bad route.",
	L"TTL expired in transit.",
	L"TTL expired REASSEM.",
	L"Param problem.",
	L"Source quench.",
	L"Option too large.",
	L"Bad destination.",
	L"Address deleted.",
	L"Spec MNU change.",
	L"MTU change.",
	L"Unload"
};

bool Ping(std::wstring sServerIP)
{
	// Obtain an ICMP handle.
	HANDLE hIcmp = IcmpCreateFile();	
	if (!hIcmp) {
		_ASSERTE (0 && L" IGPing::Ping() Failed - ICMP error");
		return false;
	}
	// Parse the IP address in the Edit.
	size_t szNextDot = sServerIP.find (L'.');
	int addr1 = ::_wtoi (sServerIP.substr (0, szNextDot++).c_str());
	size_t szNextNum = szNextDot;
	szNextDot = sServerIP.find (L'.', szNextNum);
	int addr2 = ::_wtoi (sServerIP.substr (szNextNum, szNextDot++).c_str());
	szNextNum = szNextDot;
	szNextDot = sServerIP.find (L'.', szNextNum);
	int addr3 = ::_wtoi (sServerIP.substr (szNextNum, szNextDot++).c_str());
	szNextNum = szNextDot;
	szNextDot = sServerIP.find (L'.', szNextNum);
	int addr4 = ::_wtoi (sServerIP.substr (szNextNum, szNextDot).c_str());
	// Make an int out of the IP address.
	int addr = MAKELONG(
		MAKEWORD(addr1, addr2),
		MAKEWORD(addr3, addr4));
	// Allocate a buffer for the reply info.
	int size = sizeof(icmp_echo_reply) + 8;
	char* buff = new char[size];
	bool bSuccess = false;
	// Send the echo request three times to
	// emulate what the PING program does.
	for (int i=0;i<3;i++) {
		// Call IcmpSendEcho().
		DWORD res = IcmpSendEcho(hIcmp,
			addr, 0, 0, 0, buff, size, 1500);
		// Prepare to report the status.
		icmp_echo_reply reply;
		memcpy(&reply, buff, sizeof(reply));
		if (!res) {
			//sServerIP += ErrorStrings[reply.Status - 11000];
			_ASSERTE (0 && L" IGPing::Ping() Failed - ICMP error");
			continue;
		}		
		// If the status is non-zero then show the
		// corresponding error message from the
		// ErrorStrings array to the user.
		if (reply.Status > 0)
		{/*
			sServerIP += IGPING_ERROR;
			sServerIP += L" - ";
			sServerIP += ErrorStrings[reply.Status - 11000];*/
			_ASSERTE (0 && L" IGPing::Ping() Failed - ICMP error");
			continue;
		}
		else {
			// Build a string to report the results.
			/*
			std::string rtt = reply.RoundTripTime;
			std::string ttl = reply.Options.Ttl;
			std::string S = "Reply from " + IPEdit->Text +
			" time=" + rtt +"ms TTL=" + ttl + "ms";*/
			bSuccess = true;
			break;
		}
		// Pause a second and then loop.
		Sleep(1000);
	}
	// Close the ICMP handle.
	IcmpCloseHandle(hIcmp);
	return bSuccess;
}
}
