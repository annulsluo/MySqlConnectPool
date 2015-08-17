#include "MySqlConnectPool.h"
using namespace taf;

CMySqlConnectPool::CMySqlConnectPool()
{
    pthread_mutex_init( &_lock, NULL );
}

CMySqlConnectPool::~CMySqlConnectPool()
{
    pthread_mutex_lock( &_lock );
    this->DestoryConnPool();

    pthread_mutex_unlock( &_lock );
    pthread_mutex_destroy( &_lock );

}

void CMySqlConnectPool::Init( const TC_DBConf & dbConf, int nSize )
{
    pthread_mutex_lock( &_lock);
    try
    { 
        _nMaxSize = nSize;
        while( nSize -- )
        {
            CMySqlConnectItem * pItem = new CMySqlConnectItem( dbConf );
            
            if( pItem != NULL )
            {
                pItem->_mysql.connect();

                this->_nCurrSize++;
                pItem->_nItemId = this->_nCurrSize;
                _vecConnPool.push_back( pItem );
                printf( "create connect %d\n", this->_nCurrSize );
            }
        }
    }
    catch( exception & e )
    {
        printf( "fun %s line %d exception %s", __func__, __LINE__, e.what() );
    }
    printf( "Init MysqlConnectPool Succ! \n" );
    pthread_mutex_unlock(&_lock);
}

int CMySqlConnectPool::CreateConnection( const TC_DBConf & dbConf )
{
    pthread_mutex_lock( &_lock );
    try
    { 
        if( _nCurrSize == _nMaxSize )
        {
            return -1;
        }
        CMySqlConnectItem * pItem = new CMySqlConnectItem( dbConf );

        if( pItem != NULL )
        {
            pItem->_mysql.connect();

            this->_nCurrSize++;
            pItem->_nItemId = this->_nCurrSize;
            _vecConnPool.push_back( pItem );
        }
    }
    catch( exception & e )
    {
        printf( "fun %s line %d exception %s", __func__, __LINE__, e.what() );
    }
    pthread_mutex_unlock(&_lock);
    
    return 0;
}

CMySqlConnectPool::CMySqlConnectItem * CMySqlConnectPool::GetMysqlConnect()
{
    pthread_mutex_lock( &_lock );
    CMySqlConnectItem * pMysqlItem = NULL;

    for( vector< CMySqlConnectItem * >::iterator it = _vecConnPool.begin();
            it != _vecConnPool.end(); ++ it )
    {
        printf( "func %s line %d item id %d \n", __func__, __LINE__, (*it)->_nItemId );
        if( (*it)->_bUsed ) continue; 
        else 
        {
            (*it)->_bUsed = true;
            pMysqlItem = ( *it );
            break;
        }
    }
    pthread_mutex_unlock( &_lock );
    return pMysqlItem;
}

size_t CMySqlConnectPool::Size()
{
    return _nCurrSize;
}

void CMySqlConnectPool::DestoryConnPool()
{
    printf( "func %s line %d \n", __func__, __LINE__ ); 
    for( vector< CMySqlConnectItem* >::iterator it = _vecConnPool.begin(); 
        it != _vecConnPool.end(); ++ it )
    {
        if( (*it) != NULL /*&& (*it)->_bUsed == false*/ )
        {
            (*it)->DestoryConnectItem();
            delete (*it);
            (*it) = NULL;
        }
    }
    _nCurrSize = 0;
    _vecConnPool.clear();
}

void CMySqlConnectPool::RecoverConnect( CMySqlConnectItem * pMysqlItem )
{
    pthread_mutex_lock( &_lock );
    
    if( pMysqlItem != NULL && pMysqlItem->_bUsed )
    {
        pMysqlItem->RecoverConnectItem();     
        _nCurrSize ++;
    }
    
    pthread_mutex_unlock( &_lock );
}

void CMySqlConnectPool::DestoryConnect( CMySqlConnectItem * pMySqlItem )
{
    pthread_mutex_lock( &_lock );

    if( pMySqlItem != NULL )
    {
        for( vector< CMySqlConnectItem * >::iterator it = _vecConnPool.begin();
            it != _vecConnPool.end(); ++ it )
        {
            if( (*it)->_nItemId == pMySqlItem->_nItemId )
            {
                pMySqlItem->DestoryConnectItem();
                _nCurrSize--;
                _vecConnPool.erase( it );
                break;
            }
        }
    }

    pthread_mutex_unlock( &_lock );
}

CMySqlConnectPool::CMySqlConnectItem::CMySqlConnectItem( const TC_DBConf & dbConf ): _bUsed( false )
{
    printf( "func %s line %d init db\n", __func__, __LINE__ );
    _mysql.init( dbConf );
}

CMySqlConnectPool::CMySqlConnectItem::~CMySqlConnectItem()
{
    printf( "delete connect\n" );
}

void CMySqlConnectPool::CMySqlConnectItem::DestoryConnectItem()
{
    this->disconnect();
}

void CMySqlConnectPool::CMySqlConnectItem::RecoverConnectItem()
{
    this->_bUsed = false;
}
