#ifndef  MYSQL_CONNECT_POOL_FACTORY_H
#define MYSQL_CONNECT_POOL_FACTORY_H
#include "util/tc_mysql.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <map>
#include <vector>
#include "util/tc_config.h"
#include "util/tc_singleton.h"

using namespace std;
using namespace taf;
template < class T >
class CMySqlConnectPoolFactory: public taf::TC_Singleton< CMySqlConnectPoolFactory<T>,  CreateStatic, DefaultLifetime >
{
    public:
        class CMySqlConnectItem: public TC_Mysql 
        {
            public:
                CMySqlConnectItem( const TC_DBConf & dbConf );
                ~CMySqlConnectItem();
                friend class CMySqlConnectPoolFactory;

            private:
                // 销毁单个数据库链接
                void DestoryConnectItem();          

                // 回收数据库链接
                void RecoverConnectItem();          

            public:

                bool _bUsed;
                int _nItemId;
                TC_Mysql _mysql;
        };

    public:
        CMySqlConnectPoolFactory();
        ~CMySqlConnectPoolFactory();
        // 初始化链接池(用于创建链接，并且加入到链接池中)
        void Init( const TC_DBConf & dbConf, int nSize, T Index );                                                 

        // 单个db的链接池创建 
        int CreateConnection( const TC_DBConf & dbConf, T Index );        

        // 获取单个db链接
        CMySqlConnectItem * GetMysqlConnect( T Index );    

        // 获取db的链接数
        size_t Size();                               

        //销毁数据库连接池
        void DestoryConnPool();                                              
        
        // 回收链接
        void RecoverConnect( CMySqlConnectItem * pMySqlItem, T Index );

        void DestoryConnect( CMySqlConnectItem * pMySqlItem );

        // TODO
        // 开启线程进行管理
        

        
    private:
        map< T, vector< CMySqlConnectItem * > > _mapConnPool;
        int _nCurrSize;
        int _nMaxSize;
        int _nMinSize;
        pthread_mutex_t _lock;                                                // 线程锁

 };

 #endif 
