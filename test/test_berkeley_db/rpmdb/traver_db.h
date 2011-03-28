#include "rpmmalloc.h"
/* structures */
struct _dbiIndexItem {
    unsigned int hdrNum;        /*!< header instance in db */
    unsigned int tagNum;        /*!< tag index in header */
    unsigned int fpNum;         /*!< finger print index */
};
struct _dbiIndexSet {
    struct _dbiIndexItem * recs; /*!< array of records */
    int count;                   /*!< number of records */
    size_t alloced;              /*< alloced size */
};
typedef struct _dbiIndexItem * dbiIndexItem; 
typedef struct _dbiIndexSet * dbiIndexSet;

/* dbswap */
union _dbswap {
	unsigned int ui;
	unsigned char uc[4];
};
#define _DBSWAP(_a) \
   { \
	unsigned char _b, *_c = (_a).uc; \
	_b = _c[3]; _c[3] = _c[0]; _c[0] = _b; \
	_b = _c[2]; _c[2] = _c[1]; _c[1] = _b; \
   }

/* set memory alloc */
static inline void dbiGrowSet(dbiIndexSet set, unsigned int nrecs)
{
    size_t need = (set->count + nrecs) * sizeof(*(set->recs));
    size_t alloced = set->alloced ? set->alloced : 1 << 4;

    while (alloced < need)
        alloced <<= 1;

    if (alloced != set->alloced) {
        set->recs = xrealloc(set->recs, alloced);
        set->alloced = alloced;
    }
}

/* DB的函数执行完成后，返回0代表成功，否则失败 */
#define print_error(ret) \
  { \
    if(ret != 0) \
      printf("ERROR: %s\n",db_strerror(ret)); \
  }
/* 数据结构DBT在使用前，应首先初始化，否则编译可通过但运行时报参数错误  */
static inline void init_DBT(DBT * key, DBT * data)
{
	memset(key, 0, sizeof(DBT));
	memset(data, 0, sizeof(DBT));
}
