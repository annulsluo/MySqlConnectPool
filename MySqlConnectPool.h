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
                CMySqlConnectItem( const TC_DBConf & dbConf );
                ~CMySqlConnectItem();
                friend class CMySqlConnectPool;

            private:
                // ���ٵ������ݿ�����
                void DestoryConnectItem();          

                // �������ݿ�����
                void RecoverConnectItem();          

            public:

                bool _bUsed;
                int _nItemId;
                TC_Mysql _mysql;
        };

    public:
        CMySqlConnectPool();
        ~CMySqlConnectPool();
        // ��ʼ�����ӳ�(���ڴ������ӣ����Ҽ��뵽���ӳ���)
        void Init( const TC_DBConf & dbConf, int nSize );                                                 

        // ����db�����ӳش��� 
        int CreateConnection( const TC_DBConf & dbConf );        

        // ��ȡ����db����
        CMySqlConnectItem * GetMysqlConnect();    

        // ��ȡdb��������
        size_t Size();                               

        //�������ݿ����ӳ�
        void DestoryConnPool();                                              
        
        // ��������
        void RecoverConnect( CMySqlConnectItem * pMySqlItem );

        void DestoryConnect( CMySqlConnectItem * pMySqlItem );

        // TODO
        // �����߳̽��й���
        

        
    private:
        vector< CMySqlConnectItem * > _vecConnPool;
        int _nCurrSize;
        int _nMaxSize;
        int _nMinSize;
        pthread_mutex_t _lock;                                                // �߳���

 };

#endif
