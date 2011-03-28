#include <db.h> 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#include "traver_db.h"
#include "rpmmalloc.h"

static int dbt2set(DBT * data, dbiIndexSet * setp)
{
    int _dbbyteswapped = 0;	//set 0 for test
    int dbi_jlen = 8;		//set 8 for test
    const char * sdbir;
    dbiIndexSet set;
    int i;

    if (data == NULL || setp == NULL)
	return -1;

    if ((sdbir = data->data) == NULL) {
	*setp = NULL;
	return 0;
    }

    set = xcalloc(1, sizeof(*set));
    dbiGrowSet(set, data->size / dbi_jlen);
    set->count = data->size / dbi_jlen;

    switch (dbi_jlen) {
    default:
    case 2*sizeof(int):
	for (i = 0; i < set->count; i++) {
	    union _dbswap hdrNum, tagNum;

	    memcpy(&hdrNum.ui, sdbir, sizeof(hdrNum.ui));
	    sdbir += sizeof(hdrNum.ui);
	    memcpy(&tagNum.ui, sdbir, sizeof(tagNum.ui));
	    sdbir += sizeof(tagNum.ui);
	    if (_dbbyteswapped) {
		_DBSWAP(hdrNum);
		_DBSWAP(tagNum);
	    }
	    set->recs[i].hdrNum = hdrNum.ui;
	    set->recs[i].tagNum = tagNum.ui;
	    set->recs[i].fpNum = 0;
	}
	break;
    case 1*sizeof(int):
	for (i = 0; i < set->count; i++) {
	    union _dbswap hdrNum;

	    memcpy(&hdrNum.ui, sdbir, sizeof(hdrNum.ui));
	    sdbir += sizeof(hdrNum.ui);
	    if (_dbbyteswapped) {
		_DBSWAP(hdrNum);
	    }
	    set->recs[i].hdrNum = hdrNum.ui;
	    set->recs[i].tagNum = 0;
	    set->recs[i].fpNum = 0;
	}
	break;
    }
    *setp = set;
    return 0;
}
//buffer for data in key and data
char key_buf[800000];
char data_buf[800000];
void show_contant(DBT * key, DBT * data)
{
	dbiIndexSet setp = NULL;
	int i;

//	printf("key size:%d,ulen:%d,dlen:%d,doff:%d,flags:%d.  ", key->size, key->ulen, key->dlen, key->doff, key->flags);
//	printf("data size:%d,ulen:%d,dlen:%d,doff:%d,flags:%d\n", data->size, data->ulen, data->dlen, data->doff, data->flags);

	if(key->size>800000 || data->size>800000){
		printf("the key or data size is too large, continue\n");
		return ;
	}

	memcpy(key_buf, key->data, key->size);
	key_buf[key->size] = '\0';
	printf("%s\n", key_buf);

/*	memcpy(data_buf, data->data, data->size);
	data_buf[data->size] = '\0';
	printf("%s\n", data_buf);
*/
	dbt2set(data, &setp);

	for(i = 0; i < setp->count; i++){
//		if(setp->recs[i].hdrNum == 1621){
//			printf("hdrNum:%8d, tagNum:%8d, fpNum:%8d\n", setp->recs[i].hdrNum, setp->recs[i].tagNum, setp->recs[i].fpNum);
//		}
	}
}
int trave_db(DB *dbp)
{
	DBC *db_curse;
	DBT key, data;
	int ret;

	ret = dbp->cursor(dbp, NULL, &db_curse, 0);
	if(ret != 0){
		print_error(ret);
		return ret;
	}
	init_DBT(&key, &data);
	while(1){
		ret = db_curse->c_get(db_curse, &key, &data, DB_NEXT);
		if(ret == 0)
			show_contant(&key, &data);
		else if(ret == DB_NOTFOUND)
			break;
		else
			return ret;
	}
	db_curse->c_close(db_curse);
	return 0;
}

int main(int argv, char *args[])
{
	DB *dbp;           
	u_int32_t flags;
	int method;  
	int ret;

	if(argv < 3){
		printf("please input storage type and database path\n");
		return -1;
	}
	if(!strcmp(args[1], "btree"))
		method = DB_BTREE;
	else if(!strcmp(args[1], "hash"))
		method = DB_HASH;
	else{
		printf("storage type not recognized\n");
		return -1;
	}
	flags = DB_CREATE;    
	//create database object
	ret = db_create(&dbp, NULL, 0);
	if(ret != 0){
		print_error(ret);
		return -1;
	}
	//open a database
	ret = dbp->open(dbp, NULL, args[2], NULL, method, flags, 0);
	if(ret != 0){
		print_error(ret);
		return -1;
	}
	ret = trave_db(dbp);
	if(ret != 0){
		print_error(ret);
		return -1;
	}
	//close database
	if(dbp != NULL)
    		dbp->close(dbp, 0); 
	return 0;
}

