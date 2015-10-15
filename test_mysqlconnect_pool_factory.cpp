#include "MySqlConnectPoolFactory.h"
#include <map>
#include <pthread.h>
#include <string>
using namespace std;
using namespace taf;
pthread_t main_tid;
char toStr( int nNum )
{
    string sIndex;
    if( nNum <= 9 && nNum >=0 )
    {   
        return '0' + nNum;
    }   
    else
    {   
        return 'a' + ( nNum - 10 );
    }   
}

void dothing( string sStr )
{
    for( size_t i = 0; i < 10; ++ i )
    {
        string sDBName = string( "callbacknotify_db_" ) + toStr( i%2 );
        CMySqlConnectPoolFactory<string>::CMySqlConnectItem * pItem = CMySqlConnectPoolFactory<string>::getInstance()->GetMysqlConnect( sDBName );
        if( pItem != NULL )
        {
            printf( "str %s used %d itemid %d\n ", sStr.c_str(), pItem->_bUsed, pItem->_nItemId );
            string sSql = "select * from callbacknotify_table_01";
            TC_Mysql::MysqlData data = pItem->_mysql.queryRecord( sSql.c_str() );
            printf( "str %s data size %d itemid %d\n", sStr.c_str(), data.size(), pItem->_nItemId );
            CMySqlConnectPoolFactory<string>::getInstance()->RecoverConnect( pItem, sDBName );
        }
        else
        {
            printf( "str %s get fail. itemid %d\n", sStr.c_str(), i%2+1  );
        }
    }

}
void * func( void * arg )
{
    while(1)
    {
        dothing( "child thread" );
    }
}

int main( int argc, char * argv[] )
{
    int nSize = 2;
    TC_DBConf dbConf;
    map<string, string>dbmap;
    dbmap["dbhost"] = "10.240.64.140";
    dbmap["dbuser"] = "com_yaq_callback";
    dbmap["dbpass"] = "tggv@T_d@q_aM3e";
    dbmap["charset"] = "gbk";
    dbmap["dbport"] = "3734";


    dbmap["dbname"] = "callbacknotify_db_0";
    dbConf.loadFromMap(dbmap);
    CMySqlConnectPoolFactory<string>::getInstance()->Init( dbConf, nSize, dbmap["dbname"] );

    dbmap["dbname"] = "callbacknotify_db_1";
    dbConf.loadFromMap(dbmap);
    CMySqlConnectPoolFactory<string>::getInstance()->Init( dbConf, nSize, dbmap["dbname"] );

    printf( "create mysqlconnectpoolex succ!\n" );

    size_t size = CMySqlConnectPoolFactory<string>::getInstance()->Size();
    printf( "size %d\n", size );

    int iErr = pthread_create( &main_tid, NULL, func, NULL );
    if ( iErr != 0 )
    {
        printf( "create pthread error %d\n", iErr );
        return 0;
    }

    while(1)
    {
        dothing( "parent thread" );
    }

    return 0;
}
