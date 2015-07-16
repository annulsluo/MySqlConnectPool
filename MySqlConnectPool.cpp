#include "MySqlConnectPool.h"

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
                _vecConnPool.push_back( pItem );
                this->_nCurrSize++;
                // map[ dbConf.sHost ] = _vecConnPool;
            }
        }
    }
    catch( exception & e )
    {

    }
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
            _vecConnPool.push_back( pItem );
            this->_nCurrSize++;
            // map[ dbConf.sHost ] = _vecConnPool;
        }
    }
    catch( exception & e )
    {

    }
    pthread_mutex_unlock(&_lock);
}

CMySqlConnectPool::CMySqlConnectItem * CMySqlConnectPool::GetMysqlConnect()
{
    pthread_mutex_lock( &_lock );
    CMySqlConnectItem * pMysqlItem = NULL;
    for( vector< CMySqlConnectItem * >::iterator it = _vecConnPool.begin();
            it != _vecConnPool.end(); ++ it )
    {
        if( (*it)->_bUsed ) continue; 
        else 
        {
            (*it)->_bUsed = true;
            pMysqlItem = ( *it );
        }
    }
    pthread_mutex_unlock( &_lock );
    return pMysqlItem;
}

int CMySqlConnectPool::Size()
{
    return _nCurrSize;
}

void CMySqlConnectPool::DestoryConnPool()
{
    for( vector< CMySqlConnectItem* >::iterator it = _vecConnPool.begin(); 
        it != _vecConnPool.end(); ++ it )
    {
        if( (*it) != NULL && (*it)->_bUsed == false )
        {
            (*it)->DestoryConnectItem();
        }
    }
    _nCurrSize = 0;
    _vecConnPool.clear();
}

void CMySqlConnectPool::ReleaseConnect( CMySqlConnectItem * pMySqlItem )
{
    pthread_mutex_lock( &_lock );

    if( pMySqlItem != NULL )
        pMySqlItem->ReleaseConnectItem();

    pthread_mutex_unlock( &_lock );
}

CMySqlConnectPool::CMySqlConnectItem::CMySqlConnectItem( const TC_DBConf & dbConf )
{
    _mysql.Init( dbConf );

}

CMySqlConnectPool::CMySqlConnectItem::~CMySqlConnectItem()
{
}

void CMySqlConnectPool::CMySqlConnectItem::DestoryConnectItem()
{
    this->disconnect();
}

void CMySqlConnectPool::CMySqlConnectItem::ReleaseConnectItem()
{
    this->_bUsed = true;
}
