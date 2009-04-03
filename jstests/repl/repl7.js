// Test persistence of list of dbs to add.

var baseName = "jstests_repl7test";

doTest = function( signal ) {

    ports = allocatePorts( 2 );
    
    // spec small oplog for fast startup on 64bit machines
    m = startMongod( "--port", ports[ 0 ], "--dbpath", "/data/db/" + baseName + "-master", "--master", "--oplogSize", "1", "--nohttpinterface" );

    for( n = "a"; n != "aaaaa"; n += "a" ) {
        m.getDB( n ).a.save( {x:1} );
    }

    s = startMongod( "--port", ports[ 1 ], "--dbpath", "/data/db/" + baseName + "-slave", "--slave", "--source", "127.0.0.1:" + ports[ 0 ], "--nohttpinterface" );
    
    assert.soon( function() {
                return -1 != s.getDBNames().indexOf( "aa" );
                } );
    
    stopMongod( ports[ 1 ], signal );
    
    s = startMongoProgram( "mongod", "--port", ports[ 1 ], "--dbpath", "/data/db/" + baseName + "-slave", "--slave", "--source", "127.0.0.1:" + ports[ 0 ], "--nohttpinterface" );    
    
    assert.soon( function() {
                for( n = "a"; n != "aaaaa"; n += "a" ) {
                    if ( -1 == s.getDBNames().indexOf( n ) )
                        return false;                    
                }
                return true;
                } );

    for( n = "a"; n != "aaaaa"; n += "a" ) {
        assert.eq( 1, m.getDB( n ).a.find().count() );
    }    

    ports.forEach( function( x ) { stopMongod( x ); } );

}

doTest( 15 ); // SIGTERM
doTest( 9 );  // SIGKILL