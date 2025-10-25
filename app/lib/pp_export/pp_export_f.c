#include "pp_export/pp_export_f.h"
#include "pp_host/pp_host_errcode.h"
#include "rtthread.h"
#include "pp_assert/pp_assert.h"

static int32_t _prev0_fn_start(void * args)      { return 0; }
PP_EXPORT_F(_prev0_fn_start, 00_0_a);
static int32_t _prev0_fn_end(void * args)        { return 0; }
PP_EXPORT_F(_prev0_fn_end  , 00_0_c);
static int32_t _prev1_fn_start(void * args)      { return 0; }
PP_EXPORT_F(_prev1_fn_start, 00_1_a);
static int32_t _prev1_fn_end(void * args)        { return 0; }
PP_EXPORT_F(_prev1_fn_end  , 00_1_c);
static int32_t _prev2_fn_start(void * args)      { return 0; }
PP_EXPORT_F(_prev2_fn_start, 00_2_a);
static int32_t _prev2_fn_end(void * args)        { return 0; }
PP_EXPORT_F(_prev2_fn_end  , 00_2_c);

static int32_t _pulse0_fn_start(void * args)     { return 0; }
PP_EXPORT_F(_pulse0_fn_start, 01_0_a);
static int32_t _pulse0_fn_end(void * args)       { return 0; }
PP_EXPORT_F(_pulse0_fn_end  , 01_0_c);
static int32_t _pulse1_fn_start(void * args)     { return 0; }
PP_EXPORT_F(_pulse1_fn_start, 01_1_a);
static int32_t _pulse1_fn_end(void * args)       { return 0; }
PP_EXPORT_F(_pulse1_fn_end  , 01_1_c);
static int32_t _pulse2_fn_start(void * args)     { return 0; }
PP_EXPORT_F(_pulse2_fn_start, 01_2_a);
static int32_t _pulse2_fn_end(void * args)       { return 0; }
PP_EXPORT_F(_pulse2_fn_end  , 01_2_c);

static int32_t _app0_fn_start(void * args)      { return 0; }
PP_EXPORT_F(_app0_fn_start, 02_0_a);
static int32_t _app0_fn_end(void * args)        { return 0; }
PP_EXPORT_F(_app0_fn_end  , 02_0_c);
static int32_t _app1_fn_start(void * args)      { return 0; }
PP_EXPORT_F(_app1_fn_start, 02_1_a);
static int32_t _app1_fn_end(void * args)        { return 0; }
PP_EXPORT_F(_app1_fn_end  , 02_1_c);
static int32_t _app2_fn_start(void * args)      { return 0; }
PP_EXPORT_F(_app2_fn_start, 02_2_a);
static int32_t _app2_fn_end(void * args)        { return 0; }
PP_EXPORT_F(_app2_fn_end  , 02_2_c);

static int32_t _deinit_fn_start(void * args)    { return 0; }
PP_EXPORT_F(_deinit_fn_start, 03_a);
static int32_t _deinit_fn_end(void * args)      { return 0; }
PP_EXPORT_F(_deinit_fn_end  , 03_c);

static int32_t _hostpre_fn_start(void * args)   { return 0; }
PP_EXPORT_F(_hostpre_fn_start, 04_0_a);
static int32_t _hostpre_fn_end(void * args)     { return 0; }
PP_EXPORT_F(_hostpre_fn_end  , 04_0_c);
static int32_t _hostpost_fn_start(void * args)  { return 0; }
PP_EXPORT_F(_hostpost_fn_start, 04_1_a);
static int32_t _hostpost_fn_end(void * args)    { return 0; }
PP_EXPORT_F(_hostpost_fn_end  , 04_1_c);

static int32_t _hostcmd_fn_start(void * args)   { return PP_HOST_ERR_CMD_INDEX; }
PP_EXPORT_F(_hostcmd_fn_start, 05_a);
static int32_t _hostcmd_fn_end(void * args)     { return PP_HOST_ERR_CMD_INDEX; }
PP_EXPORT_F(_hostcmd_fn_end  , 05_c);

static int32_t _hostopr_fn_start(void * args)   { return PP_HOST_ERR_OPR; }
PP_EXPORT_F(_hostopr_fn_start, 06_a);
static int32_t _hostopr_fn_end(void * args)     { return PP_HOST_ERR_OPR; }
PP_EXPORT_F(_hostopr_fn_end  , 06_c);

static int32_t _run_all(const pp_fn_t * pp_fn_start, const pp_fn_t * pp_fn_end, void * args, uint8_t all_ok) {
    PP_ASSERT(pp_fn_start != RT_NULL);
    PP_ASSERT(pp_fn_end != RT_NULL);
    PP_ASSERT(pp_fn_end >= pp_fn_start);
    PP_ASSERT(all_ok <= 1);

    for (volatile const pp_fn_t * fn_ptr = pp_fn_start; fn_ptr <= pp_fn_end; fn_ptr++) {
        PP_ASSERT(*fn_ptr != RT_NULL);
        int32_t res = (*fn_ptr)(args);
        PP_ASSERT(all_ok == 0 || res == 0);
        if (res != 0) return res;
    }
    return 0;
}
static int32_t _run_one(const pp_fn_t * pp_fn_start, const pp_fn_t * pp_fn_end, void * args, int32_t ret_miss) {
    PP_ASSERT(pp_fn_start != RT_NULL);
    PP_ASSERT(pp_fn_end != RT_NULL);
    PP_ASSERT(pp_fn_end >= pp_fn_start);

    for (volatile const pp_fn_t * fn_ptr = pp_fn_start; fn_ptr <= pp_fn_end; fn_ptr++) {
        PP_ASSERT(*fn_ptr != RT_NULL);
        int32_t res = (*fn_ptr)(args);
        if (res == ret_miss) continue;
        return res;
    }
    return ret_miss;
}
int32_t pp_export_f_pulse(void * args) {
    _run_all(&__pp_fn__pulse0_fn_start, &__pp_fn__pulse0_fn_end, args, 1);
    _run_all(&__pp_fn__pulse1_fn_start, &__pp_fn__pulse1_fn_end, args, 1);
    _run_all(&__pp_fn__pulse2_fn_start, &__pp_fn__pulse2_fn_end, args, 1);
    return 0;
}

int32_t pp_export_f_deinit(void * args) {
    return _run_all(&__pp_fn__deinit_fn_start, &__pp_fn__deinit_fn_end, args, 1);
}

int32_t pp_export_f_hostpre(void * args) {
    return _run_all(&__pp_fn__hostpre_fn_start, &__pp_fn__hostpre_fn_end, args, 0);
}
int32_t pp_export_f_hostpost(void * args) {
    return _run_all(&__pp_fn__hostpost_fn_start, &__pp_fn__hostpost_fn_end, args, 0);
}

int32_t pp_export_f_hostcmd(void * args) {
    return _run_one(&__pp_fn__hostcmd_fn_start, &__pp_fn__hostcmd_fn_end, args, PP_HOST_ERR_CMD_INDEX);
}

int32_t pp_export_f_hostopr(void * args) {
    return _run_one(&__pp_fn__hostopr_fn_start, &__pp_fn__hostopr_fn_end, args, PP_HOST_ERR_OPR);
}

static int _appx_run(void) {
    _run_all(&__pp_fn__app0_fn_start, &__pp_fn__app0_fn_end, RT_NULL, 1);
    _run_all(&__pp_fn__app1_fn_start, &__pp_fn__app1_fn_end, RT_NULL, 1);
    _run_all(&__pp_fn__app2_fn_start, &__pp_fn__app2_fn_end, RT_NULL, 1);
    return 0;
}
INIT_APP_EXPORT(_appx_run);

static int _prevx_run(void) {
    _run_all(&__pp_fn__prev0_fn_start, &__pp_fn__prev0_fn_end, RT_NULL, 1);
    _run_all(&__pp_fn__prev1_fn_start, &__pp_fn__prev1_fn_end, RT_NULL, 1);
    _run_all(&__pp_fn__prev2_fn_start, &__pp_fn__prev2_fn_end, RT_NULL, 1);
    return 0;
}
INIT_PREV_EXPORT(_prevx_run);