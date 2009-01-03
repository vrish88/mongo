#include "stdafx.h"
#include <stdio.h>
#include "../util/goodies.h"
#include <fcntl.h>

// logstream defines these, we don't want that:
#undef cout
#undef endl

int main(int argc, char* argv[], char *envp[] ) {
	cout << "hello" << endl;

	FILE *f = fopen("/data/db/temptest", "a");

	if( f == 0 ) {
		cout << "can't open file\n";
		return 1;
	}

	{
		Timer t;
		for( int i = 0; i < 50000; i++ )
			fwrite("abc", 3, 1, f);
		cout << "small writes: " << t.millis() << "ms" << endl;
	}
    
	{
		Timer t;
		for( int i = 0; i < 10000; i++ ) {
			fwrite("abc", 3, 1, f);
			fflush(f);
            fsync( fileno( f ) );
		}
		int ms = t.millis();
		cout << "flush: " << ms << "ms, " << ms / 10000.0 << "ms/request" << endl;
	}

	{
		Timer t;
		for( int i = 0; i < 500; i++ ) {
			fwrite("abc", 3, 1, f);
			fflush(f);
            fsync( fileno( f ) );
			sleepmillis(10);
		}
		int ms = t.millis();
		cout << "flush with sleeps intermixed: " << ms << "ms, " << (ms-5000) / 500.0 << "ms/request" << endl;
	}

	char buf[8192];
	for( int pass = 0; pass < 2; pass++ ) {    
		cout << "pass " << pass << endl;
	{
		Timer t;
		int n = 500;
		for( int i = 0; i < n; i++ ) {
			if( pass == 0 )
				fwrite("abc", 3, 1, f);
			else
				fwrite(buf, 8192, 1, f);
			buf[0]++;
			fflush(f);
			fcntl( fileno(f), F_FULLFSYNC );
		}
		int ms = t.millis();
		cout << "fullsync: " << ms << "ms, " << ms / ((double) n) << "ms/request" << endl;
	}

	{
		Timer t;
		for( int i = 0; i < 500; i++ ) {
			if( pass == 0 )
				fwrite("abc", 3, 1, f);
			else
				fwrite(buf, 8192, 1, f);
			buf[0]++;
			fflush(f);
			fcntl( fileno(f), F_FULLFSYNC );
			sleepmillis(10);
		}
		int ms = t.millis();
		cout << "fullsync with sleeps intermixed: " << ms << "ms, " << (ms-5000) / 500.0 << "ms/request" << endl;
	}
	}
    
	return 0;
}