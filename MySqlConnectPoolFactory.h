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
        CMySqlConnectPoolFactory();
        ~CMySqlConnectPoolFactory();
        // ��ʼ�����ӳ�(���ڴ������ӣ����Ҽ��뵽���ӳ���)
        void Init( const TC_DBConf & dbConf, int nSize, T Index );                                                 

        // ����db�����ӳش��� 
        int CreateConnection( const TC_DBConf & dbConf, T Index );        

        // ��ȡ����db����
        CMySqlConnectItem * GetMysqlConnect( T Index );    

        // ��ȡdb��������
        size_t Size();                               

        //�������ݿ����ӳ�
        void DestoryConnPool();                                              
        
        // ��������
        void RecoverConnect( CMySqlConnectItem * pMySqlItem, T Index );

        void DestoryConnect( CMySqlConnectItem * pMySqlItem );

        // TODO
        // �����߳̽��й���
        

        
    private:
        map< T, vector< CMySqlConnectItem * > > _mapConnPool;
        int _nCurrSize;
        int _nMaxSize;
        int _nMinSize;
        pthread_mutex_t _lock;                                                // �߳���

 };

 #endif 
