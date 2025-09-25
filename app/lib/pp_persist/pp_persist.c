#include "stdint.h"
#include "string.h"

#include "pp_serialize/pp_serialize.h"
#include "pp_persist/pp_persist.h"
#include "pp_cfg.h"

#include "cson.h"
#include "flashdb.h"
#include "fal.h"
#include "fal_def.h"
#include "pp_data/pp_data_sys.h"
#include <rtthread.h>
#include "pp_assert/pp_assert.h"
#include "pp_export/pp_export_f.h"

#define PAGE_LOG_NUM 64

static struct rt_mutex flash;
struct fdb_kvdb pp_kvdb;
struct fdb_tsdb pp_tsdb;

static void pp_flash_lock(fdb_db_t db) {
    rt_err_t ret = rt_mutex_take(&flash, RT_WAITING_FOREVER);
    PP_ASSERT(ret == RT_EOK);
}

static void pp_flash_unlock(fdb_db_t db) {
    rt_err_t ret = rt_mutex_release(&flash);
    PP_ASSERT(ret == RT_EOK);
}

void pp_del_env(char * name) {
    PP_ASSERT(name != RT_NULL);
    fdb_err_t ret = fdb_kv_del(&pp_kvdb, name);
    PP_ASSERT(ret == FDB_NO_ERR);
}

void pp_store_env(const pp_model_t * model, char *name) {
    PP_ASSERT(model != RT_NULL);
    PP_ASSERT(name != RT_NULL);

	struct fdb_blob blob;
    uint8_t length = pp_serialize_cal_length(model);
    uint8_t * buffer =(uint8_t *) rt_malloc(length);
    PP_ASSERT(buffer != RT_NULL);

    rt_memset(buffer, 0, length);
    /*序列化后的数据保存在buffer*/
    pp_serialize(model, buffer);
    /*buffer的数据存在内部flash*/
    fdb_err_t ret = fdb_kv_set_blob(&pp_kvdb, name, fdb_blob_make(&blob, buffer, length));
    PP_ASSERT(ret == FDB_NO_ERR);
    rt_free(buffer);
}

void pp_read_env(const pp_model_t * model, char *name) {
    PP_ASSERT(model != RT_NULL);
    PP_ASSERT(name != RT_NULL);

	struct fdb_blob blob;
    uint8_t length = pp_serialize_cal_length(model);
    uint8_t * buffer = (uint8_t *) rt_malloc(length);
    PP_ASSERT(buffer != RT_NULL);

    rt_memset(buffer, 0, length);
    /*读取内部flash的数据*/
    if(fdb_kv_get_blob(&pp_kvdb, name, fdb_blob_make(&blob, buffer, length))) {
        pp_deserialize(buffer, model);
    }
    rt_free(buffer);
}

void pp_store_log(char * log) {
    PP_ASSERT(log != RT_NULL);

	size_t length = 0;
	struct fdb_blob blob;
	length = strlen(log);
	fdb_tsl_append(&pp_tsdb, fdb_blob_make(&blob, log, length));
}

typedef struct {
    pp_persist_iter_log_cb_t cb;
    uint16_t log_cnt;
    uint16_t start_index;
    uint16_t current_index;
} fdb_tsl_iter_cb_arg_t;

/*读取日志*/
static bool read_log_by_blob(fdb_tsl_t tsl, void * cb_arg) {
    PP_ASSERT(tsl != RT_NULL);
    PP_ASSERT(cb_arg != RT_NULL);

    fdb_tsl_iter_cb_arg_t * cb_arg_p = (fdb_tsl_iter_cb_arg_t *)cb_arg;
    PP_ASSERT(cb_arg_p->cb != RT_NULL);

    if(cb_arg_p->current_index < cb_arg_p->start_index) {
        cb_arg_p->current_index++;
        return false;
    }
    uint8_t length = tsl->log_len;
    uint8_t * buffer =(uint8_t *) rt_malloc(length);
    PP_ASSERT(buffer != RT_NULL);

    rt_memset(buffer, 0, length);
    struct fdb_blob blob;

    fdb_blob_read((fdb_db_t)(&pp_tsdb), fdb_tsl_to_blob(tsl, fdb_blob_make(&blob, buffer, length)));
    cb_arg_p->cb(tsl->time, buffer, length);
    rt_free(buffer);
    cb_arg_p->current_index++;
    // log_cnt为0时，读取全部
    if (--cb_arg_p->log_cnt == 0) {
        return true;
    }
    return false;
}

/*flash日志输出*/
void pp_iter_log(pp_persist_iter_log_cb_t cb, uint16_t page_num) {
    PP_ASSERT(cb != RT_NULL);

    fdb_tsl_iter_cb_arg_t cb_arg = {0};
    cb_arg.cb = cb;
    cb_arg.log_cnt = PAGE_LOG_NUM;
    cb_arg.start_index = page_num * PAGE_LOG_NUM;
    cb_arg.current_index = 0;
    fdb_tsl_iter_reverse(&pp_tsdb, read_log_by_blob, (void *)&cb_arg);
}
/**
* 向分区中写入数据
* @param 分区名
* @param 偏移
* @param buffer
* @param size
*
* @return
*/
static void _pp_write_data(char *name, rt_uint32_t offset, uint8_t * buffer, rt_uint32_t size) {
    PP_ASSERT(name != RT_NULL);
    PP_ASSERT(buffer != RT_NULL);

    const struct fal_partition *  fal_part = RT_NULL;
    fal_part = fal_partition_find(name);
    PP_ASSERT(fal_part != RT_NULL);

    pp_flash_lock(RT_NULL);
    if(offset == 0) {
        int ret_erase = fal_partition_erase_all(fal_part);
        PP_ASSERT(ret_erase >= 0);
    }
    int ret_write = fal_partition_write(fal_part, offset, buffer, size);
    PP_ASSERT(ret_write >= 0);
    pp_flash_unlock(RT_NULL);
}

void pp_write_dl_data(rt_uint32_t offset, uint8_t * buffer, rt_uint32_t size) {
    PP_ASSERT(buffer != RT_NULL);
    _pp_write_data("download", offset, buffer, size);
}

void pp_write_bl_data(void) {
    _pp_write_data("bl_data", 0, (uint8_t*)&pp_bl_data, sizeof(pp_bl_data_t));
}

void pp_read_bn_data(uint8_t * buffer, rt_uint32_t size) {
    PP_ASSERT(buffer != RT_NULL);

    const struct fal_partition *  fal_part = RT_NULL;
    fal_part = fal_partition_find("bn_data");
    PP_ASSERT(fal_part != RT_NULL);

    rt_memcpy(buffer, (char *)fal_part->offset + PP_PCB_MCU_FLASH_START_ADRESS, size);
}

static int64_t get_ts_ms() {
    return _sys.timestamp;
}

static void flash_db_init() {
    /*pp_kvdb类型，命令（宏定义），函数*/
    fdb_kvdb_control(&pp_kvdb, FDB_KVDB_CTRL_SET_LOCK, (void *)pp_flash_lock);
    fdb_kvdb_control(&pp_kvdb, FDB_KVDB_CTRL_SET_UNLOCK, (void *)pp_flash_unlock);
    fdb_tsdb_control(&pp_tsdb, FDB_TSDB_CTRL_SET_LOCK, (void *)pp_flash_lock);
    fdb_tsdb_control(&pp_tsdb, FDB_TSDB_CTRL_SET_UNLOCK, (void *)pp_flash_unlock);
    fdb_err_t ret = fdb_kvdb_init(&pp_kvdb, "env", "kv_db", NULL, NULL);
    PP_ASSERT(ret == FDB_NO_ERR);
    ret = fdb_tsdb_init(&pp_tsdb, "log", "ts_db", get_ts_ms, 128, NULL);
    PP_ASSERT(ret == FDB_NO_ERR);
}

static int32_t _persist_init(void * args) {
    rt_err_t ret = rt_mutex_init(&flash, "flash", NULL);
    PP_ASSERT(ret == RT_EOK);
    ret = fal_init();
    PP_ASSERT(ret > 0);
    flash_db_init();
    return 0;
}
PP_PREV2_EXPORT_F(_persist_init);
