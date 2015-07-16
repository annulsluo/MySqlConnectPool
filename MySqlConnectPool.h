#ifndef  MYSQL_CONNECT_POOL_H
#define MYSQL_CONNECT_POOL_H
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

class CMySqlConnectPool: public taf::TC_Singleton< CMySqlConnectPool,  CreateStatic, DefaultLifetime >
{
    public:
        class CMySqlConnectItem: public TC_Mysql 
        {
            public:
                CMySqlConnectItem( const TC_DBConf & dbConf ): TC_Mysql( dbConf ), _bUsed( false ){};
                ~CMySqlConnectItem();
                // 销毁单个数据库链接
                void DestoryConnectItem();          

                // 回收数据库链接
                void ReleaseConnectItem();          

                bool _bUsed;

                TC_Mysql _mysql;
        };

    public:
        CMySqlConnectPool();
        ~CMySqlConnectPool();
        // 初始化链接池(用于创建链接，并且加入到链接池中)
        void Init( const TC_DBConf & dbConf, int nSize );                                                 

        // 单个db的链接池创建 
        int CreateConnection( const TC_DBConf & dbConf );        

        // 获取单个db链接
        CMySqlConnectItem * GetMysqlConnect();    

        // 获取db的链接数
        int Size();                               

        //销毁数据库连接池
        void DestoryConnPool();                                              

        void ReleaseConnect( CMySqlConnectItem * pMySqlItem );

        
    private:
        vector< CMySqlConnectItem * > _vecConnPool;
        int _nCurrSize;
        int _nMaxSize;
        pthread_mutex_t _lock;                                                // 线程锁

 };

#endif
