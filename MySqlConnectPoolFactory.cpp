#include "MySqlConnectPoolFactory.h"
using namespace taf;

template< class T >
CMySqlConnectPoolFactory< T >::CMySqlConnectPoolFactory()
{
    pthread_mutex_init( &_lock, NULL );
}

template< class T >
CMySqlConnectPoolFactory< T >::~CMySqlConnectPoolFactory()
{
    pthread_mutex_lock( &_lock );
    this->DestoryConnPool();

    pthread_mutex_unlock( &_lock );
    pthread_mutex_destroy( &_lock );

}

template< class T >
void CMySqlConnectPoolFactory< T >::Init( const TC_DBConf & dbConf, int nSize, T Index )
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
                _mapConnPool[ Index ].push_back( pItem );
                printf( "create connect %d\n", this->_nCurrSize );
            }
            else
            {
                printf( "create connect fail\n" );
            }
        }

    }
    catch( exception & e )
    {
        printf( "fun %s line %d exception %s", __func__, __LINE__, e.what() );
    }
    printf( "Init MysqlConnectPoolEx Succ! \n" );
    pthread_mutex_unlock(&_lock);
}

template< class T >
int CMySqlConnectPoolFactory< T >::CreateConnection( const TC_DBConf & dbConf, T Index )
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
            _mapConnPool[ Index ].push_back( pItem );
        }
    }
    catch( exception & e )
    {
        printf( "fun %s line %d exception %s", __func__, __LINE__, e.what() );
    }
    pthread_mutex_unlock(&_lock);

    return 0;
}

template< class T >
typename CMySqlConnectPoolFactory<T>::CMySqlConnectItem * CMySqlConnectPoolFactory<T>::GetMysqlConnect( T Index )
{
    pthread_mutex_lock( &_lock );
    CMySqlConnectItem * pMysqlItem = NULL;

    for( typename vector< CMySqlConnectItem * >::iterator it = _mapConnPool[ Index ].begin();
            it != _mapConnPool[ Index ].end(); ++ it )
    {
        printf( "func %s line %d item id %d \n", __func__, __LINE__, (*it)->_nItemId );
        if( (*it)->_bUsed ) continue; 
        else 
        {
            (*it)->_bUsed = true;
            pMysqlItem = ( *it );
            this->_nCurrSize--;
            break;
        }
    }
    pthread_mutex_unlock( &_lock );
    return pMysqlItem;
}

template< class T >
size_t CMySqlConnectPoolFactory<T>::Size()
{
    return _nCurrSize;
}

template< class T >
void CMySqlConnectPoolFactory<T>::DestoryConnPool()
{
    printf( "func %s line %d \n", __func__, __LINE__ ); 
    for( typename map< T, vector< CMySqlConnectItem* > >::iterator mIt = _mapConnPool.begin();
            mIt != _mapConnPool.end(); ++mIt )
    {
        for( typename vector< CMySqlConnectItem* >::iterator it = mIt->second.begin(); 
                it != mIt->second.end(); ++ it )
        {
            if( (*it) != NULL /*&& (*it)->_bUsed == false*/ )
            {
                (*it)->DestoryConnectItem();
                delete (*it);
                (*it) = NULL;
            }
        }
        mIt->second.clear();
    }
    _nCurrSize = 0;
}

template< class T >
void CMySqlConnectPoolFactory<T>::RecoverConnect( CMySqlConnectItem * pMysqlItem, T Index )
{
    pthread_mutex_lock( &_lock );

    if( pMysqlItem != NULL && pMysqlItem->_bUsed )
    {
        pMysqlItem->RecoverConnectItem();     
        _mapConnPool[Index].push_back( pMysqlItem );
        _nCurrSize ++;
    }

    pthread_mutex_unlock( &_lock );
}

template< class T >
void CMySqlConnectPoolFactory<T>::DestoryConnect( CMySqlConnectItem * pMySqlItem )
{
    pthread_mutex_lock( &_lock );

    if( pMySqlItem != NULL )
    {
        for( typename map< T, vector< CMySqlConnectItem * > >::iterator mIt = _mapConnPool.begin();
                mIt != _mapConnPool.end(); ++mIt )
        {
            for( typename vector< CMySqlConnectItem * >::iterator it = mIt->second.begin();
                    it != mIt->second.end(); ++ it )
            {
                if( (*it)->_nItemId == pMySqlItem->_nItemId )
                {
                    pMySqlItem->DestoryConnectItem();
                    _nCurrSize--;
                    mIt->second.erase( it );
                    break;
                }
            }
        }
    }

    pthread_mutex_unlock( &_lock );
}

template< class T >
CMySqlConnectPoolFactory<T>::CMySqlConnectItem::CMySqlConnectItem( const TC_DBConf & dbConf ): _bUsed( false )
{
    printf( "func %s line %d init db\n", __func__, __LINE__ );
    _mysql.init( dbConf );
}

template< class T >
CMySqlConnectPoolFactory<T>::CMySqlConnectItem::~CMySqlConnectItem()
{
    printf( "delete connect\n" );
}

template< class T >
void CMySqlConnectPoolFactory<T>::CMySqlConnectItem::DestoryConnectItem()
{
    this->disconnect();
}

template< class T >
void CMySqlConnectPoolFactory<T>::CMySqlConnectItem::RecoverConnectItem()
{
    this->_bUsed = false;
}

template class CMySqlConnectPoolFactory<string>;
