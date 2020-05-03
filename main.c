#include <stdio.h>
#include <stdlib.h>
#include <winscard.h>

const DWORD SHARE_SHARED = 2;
const DWORD PROTOCOL_T0 = 1;
const DWORD PROTOCOL_T1 = 2;
const DWORD DISPOSITION_LEAVE_CARD = 0;

void release_context(SCARDCONTEXT ctx)
{
	SCardReleaseContext(ctx);
}

void disconnect(SCARDCONTEXT ctx)
{
	SCardDisconnect(ctx, DISPOSITION_LEAVE_CARD);
}

int main()
{
	SCARDCONTEXT ctx;
	LPCVOID reserved1, reserved2;
	DWORD scope = 2;
	int r = SCardEstablishContext(scope, reserved1, reserved2, &ctx);
	printf("context error: %s %ld\n", pcsc_stringify_error(r & 0xFFFFFFFF), ctx);

	if (r == 0) {
		DWORD bufLen = 0;
		r = SCardListReaders(ctx, NULL, NULL, &bufLen);
		if (r !=0) {
			release_context(ctx);
			printf("reader access error: %s\n", pcsc_stringify_error(r & 0xFFFFFFFF));
			return -r;
		}
		printf("Found %ld readers", bufLen);

		char *readerlist = malloc(sizeof(char)*bufLen);

		r = SCardListReaders(ctx, NULL, readerlist, &bufLen);
		printf("Got %s", readerlist);

		// Take only the first reader, it should be 0-terminated so the string
		// is longer than necessary if you have more than one reader. Tough life.

		DWORD proto;
		SCARDHANDLE handle;
		r = SCardConnect(ctx, readerlist, SHARE_SHARED, PROTOCOL_T0 | PROTOCOL_T1, &handle, &proto);
		if (r !=0) {
			release_context(ctx);
			printf("card connection error: %s\n", pcsc_stringify_error(r & 0xFFFFFFFF));
			return -r;
		}

		

		disconnect(ctx);
	} else {
		release_context(ctx);
		return -r;
	}
}
