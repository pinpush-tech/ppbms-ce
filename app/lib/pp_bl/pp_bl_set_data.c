#include "pp_bl/pp_bl_data.h"
#include "string.h"
#include "fal.h"
#include "rtthread.h"
#include "pp_assert/pp_assert.h"
#include "ulog.h"

void pp_bl_set_dl_offset(void) {
    const struct fal_partition  * dl_part = RT_NULL;
    dl_part = fal_partition_find("download");
    PP_ASSERT(dl_part != RT_NULL);
    pp_bl_data.dl_offset =  dl_part->offset;
}

void pp_bl_set_user_data(char *name) {
    PP_ASSERT(name != RT_NULL);
    rt_device_t bl_dev = rt_device_find(name);
    PP_ASSERT(bl_dev != RT_NULL);
    PP_ASSERT(bl_dev->user_data != RT_NULL);
    memcpy(pp_bl_data.user_data, bl_dev->user_data, sizeof(pp_bl_data.user_data));
}
