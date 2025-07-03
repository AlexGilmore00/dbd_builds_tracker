CC=gcc
CCWin32=x86_64-w64-mingw32-gcc
DBGR=gdb

all: dbd

dbd: dbd_builds.c utilities.c read_file.c write_to_file.c
	$(CC) -Wall -g dbd_builds.c utilities.c read_file.c write_to_file.c -o dbd_builds

win32: dbd_builds.c utilities.c read_file.c write_to_file.c
	$(CCWin32) -Wall -g -mwin32 dbd_builds.c utilities.c read_file.c write_to_file.c -o dbd_builds_win32.exe

clean:
	rm dbd_builds

cleanwin32:
	rm dbd_builds_win32.exe

cleanall:
	rm dbd_builds && rm dbd_builds_win32.exe

run:
	./dbd_builds

debug:
	$(DBGR) ./dbd_builds

refreshperkfiles:
	cd files && ./.venv/bin/python ./perk_scraper.py && cd ..