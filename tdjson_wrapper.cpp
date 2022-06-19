/*
 * tdjson_wrapper.cpp
 *
 * A wrapper for the TDjson library for MetaTrader 5.
 */

#pragma once
#define DLLFUNC extern "C" __declspec(dllexport)

#include <windows.h>
#include <stdio.h>
#include <td/telegram/td_json_client.h>

BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

#define DATA_BUFFER_SIZE	4096
#define RCV_DATA_BUFFER_SIZE	(DATA_BUFFER_SIZE * 10)
static char			mbstr[DATA_BUFFER_SIZE];
static WCHAR			wcstr[RCV_DATA_BUFFER_SIZE];

/**
 * Returns an opaque identifier of a new TDLib instance.
 * The TDLib instance will not send updates until the first request
 * is sent to it.
 *
 * \return Opaque identifier of a new TDLib instance.
 */
DLLFUNC int TDCreateClientId(void)
{
	return td_create_client_id();
}

/**
 * Sends request to the TDLib client. May be called from any thread.
 * \param[in] client_id TDLib client identifier.
 * \param[in] request JSON-serialized null-terminated request to TDLib.
 */
DLLFUNC void TDSend(int client_id, const LPCWSTR request)
{
	size_t length;

	wcstombs_s(&length, mbstr, request, wcslen(request));
	td_send(client_id, mbstr);
}

/**
 * Receives incoming updates and request responses. Must not be called
 * simultaneously from two different threads.
 * The returned pointer can be used until the next call to td_receive or
 * td_execute, after which it will be deallocated by TDLib.
 *
 * \param[in] timeout The maximum number of seconds allowed for this
 *                    function to wait for new data.
 *
 * \return JSON-serialized null-terminated incoming update or request
 *         response. May be NULL if the timeout expires.
 */
DLLFUNC const LPCWSTR TDReceive(double timeout)
{
	const char *resp = td_receive(timeout);
	size_t rcv_len = resp != NULL ? strlen(resp) : 0;

	memset(wcstr, 0, RCV_DATA_BUFFER_SIZE * sizeof(wcstr[0]));

	if (resp && rcv_len <= RCV_DATA_BUFFER_SIZE - 1) {
		size_t length;
		mbstowcs_s(&length, wcstr, RCV_DATA_BUFFER_SIZE - 1, resp, rcv_len);
	}
	else
	if (resp && rcv_len > RCV_DATA_BUFFER_SIZE - 1) {
		_snwprintf_s(wcstr, RCV_DATA_BUFFER_SIZE, rcv_len - 1,
			   L"{\"@type\":\"error\",\"code\":-1,\"message\":\"Insufficient buffer: %zd\"}",
			   rcv_len);
	}
	else {
		_snwprintf_s(wcstr, RCV_DATA_BUFFER_SIZE, 1, L"");
	}

	return wcstr;
}

/**
 * Synchronously executes a TDLib request.
 * A request can be executed synchronously, only if it is documented
 * with "Can be called synchronously".
 * The returned pointer can be used until the next call to td_receive
 * or td_execute, after which it will be deallocated by TDLib.
 *
 * \param[in] request JSON-serialized null-terminated request to TDLib.
 * \return JSON-serialized null-terminated request response.
 */
DLLFUNC const LPCWSTR TDExecute(const LPCWSTR request)
{
	size_t length;
	const char *resp;

	wcstombs_s(&length, mbstr, request, wcslen(request));
	memset(wcstr, 0, RCV_DATA_BUFFER_SIZE * sizeof(wcstr[0]));

	resp = td_execute(mbstr);
	if (resp) {
		size_t length;
		mbstowcs_s(&length, wcstr, RCV_DATA_BUFFER_SIZE - 1, resp, strlen(resp));
	}

	return wcstr;
}
