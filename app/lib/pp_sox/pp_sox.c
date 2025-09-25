#include "pp_sox/pp_sox.h"
#include "pp_persist/pp_persist.h"
#include "rtthread.h"
#include "pp_cfg.h"
#include "pp_continued/pp_continued.h"
#include "ulog.h"
#include "pp_log/pp_log.h"
#include "stdio.h"
#include "pp_data/pp_data_afe.h"
#include "pp_data/pp_data_afe_cfg.h"
#include "pp_data/pp_data_sys.h"
#include "pp_assert/pp_assert.h"

typedef struct {
    uint8_t     st;                 //电池状态          0:闲置 1:放电 2:充电
    uint8_t     cf;                 //满充
    uint8_t     ov;                 //过压
    uint8_t     uv;                 //欠压
    uint8_t     js;                 //第一次启动
    int32_t     tv;                 //电池总压
    uint16_t    nv;                 //电池最小电压
    uint16_t    xv;                 //电池最大电压
    int16_t     nt;                 //电池最小温度
    int16_t     xt;                 //电池最大温度
    int64_t     cr;                 //电池电流
    uint8_t     ts;                 //次数
    int16_t     ct;                 //循环时间
} pp_sox_in_t;

static pp_sox_in_t sox_in;

static void store_fmah_log(uint8_t type) {
    pp_log_F_sox("FMAH fix f%hhu time: %llu cyc: %hu fmah: %f", type, _sys.timestamp, _sox.cyc_cnt, _sox.fmah);
}

static void store_rmah_log(uint8_t type, int32_t calc, int32_t to) {
    pp_log_F_sox("RMAH fix r%hhu time: %llu cyc: %hu fmah: %f calc: %d to: %d", type, _sys.timestamp, _sox.cyc_cnt, _sox.fmah, calc, to);
}

static uint8_t chk_vol_diff(void) {
    return  (sox_in.xv - sox_in.nv < 500) ? 1 : 0 ;
}

static uint8_t chk_over_vol(void) {
    return (sox_in.xv - _afe_cfg.ov1_th < 200 && _afe_cfg.uv1_th - sox_in.nv < 200) ? 1 : 0;
}

static uint8_t chk_tmpr(void) {
    return  (sox_in.nt >= 30 && sox_in.xt <= 55) ? 1 : 0;
}

static void update_fmah_careful(float predict_fmah) {
    PP_ASSERT(_sox.fmah > 0.0f);
    if(predict_fmah > _sox.fmah * 1.01f) {
        _sox.fmah = _sox.fmah * 1.01f;
    } else if(predict_fmah < _sox.fmah * 0.99f) {
        _sox.fmah = _sox.fmah * 0.99f;
    } else {
        _sox.fmah = predict_fmah;
    }
}

#ifdef PP_BAT_OCV_SHEET
static uint8_t ocv2_pct(void) {
    PP_ASSERT(_afe_cfg.cell_count > 0);
    uint8_t i = 0;
    for(; i <= 99; i++) {
        if(sox_in.tv < _sox_cfg.ocv_sheet[99 - i] * _afe_cfg.cell_count) {
            return i;
        }
    }
    return i;
}
#endif

static void update_fmah(float predict_fmah, uint8_t precision, uint8_t tmpr_sensitive) {
    PP_ASSERT(_afe_cfg.fmah > 0.0f);
    if(predict_fmah >= _afe_cfg.fmah * 0.6f) {
        if(precision && (!tmpr_sensitive || chk_tmpr())) {
            _sox.fmah = predict_fmah;     // 设计容量的60%以上
        } else {
            update_fmah_careful(predict_fmah);               // 当前容量的99% - 101%
        }
        if(_sox.fmah > _afe_cfg.fmah * 1.01f) {
            _sox.fmah = _afe_cfg.fmah * 1.01f;     // 设计容量的101%以下
        }
    }
}

static float tmpr_decrease_rate_calc(uint8_t index) {
    PP_ASSERT(index >= 0 && index <= 3);
    PP_ASSERT((_sox_cfg.tf_curve_tmpr[index + 1] - _sox_cfg.tf_curve_tmpr[index]) != 0);

    float tmpr_fmah_rate = 0;
    float fmah_now = 0;
    int8_t tmpr_diff = 0;

    tmpr_fmah_rate = (_sox_cfg.tf_curve_fmah[index + 1] - _sox_cfg.tf_curve_fmah[index]) / 100.0f / (_sox_cfg.tf_curve_tmpr[index + 1] - _sox_cfg.tf_curve_tmpr[index]);
    tmpr_diff = sox_in.nt - _sox_cfg.tf_curve_tmpr[index];
    fmah_now = _sox_cfg.tf_curve_fmah[index] / 100.0f + tmpr_diff * tmpr_fmah_rate;

    return 1 - fmah_now;
}

static float tmpr_decrease_rate_(void) {
    if(sox_in.nt < 100 && sox_in.nt > -50) {
        if(sox_in.nt >= _sox_cfg.tf_curve_tmpr[3]) {
            //  10到25  用  10到25  斜率
            return  tmpr_decrease_rate_calc(3);
        } else if(sox_in.nt >= _sox_cfg.tf_curve_tmpr[2]) {
            //   0到10  用   0到10  斜率
            return  tmpr_decrease_rate_calc(2);
        } else if(sox_in.nt >= _sox_cfg.tf_curve_tmpr[1]) {
            // -10到0   用 -10到0   斜率
            return tmpr_decrease_rate_calc(1);
        } else {
            // -40到-10 用 -20到-10 斜率
            return tmpr_decrease_rate_calc(0);
        }
    } else {
        return 0;
    }
}

static int64_t comp_crt_calc(float actual_dsg_chg_mah, float still_secrease_cap, int64_t dsg_chg_crt) {
    int64_t comp_crt = 0;
    if(actual_dsg_chg_mah > 1) {
        comp_crt = still_secrease_cap / actual_dsg_chg_mah * dsg_chg_crt;
        if(comp_crt > dsg_chg_crt * 4.0f) {
            comp_crt = dsg_chg_crt * 4.0f;
        } else if(comp_crt < dsg_chg_crt * -0.9f) {
            comp_crt = dsg_chg_crt * -0.9;
        }
    }
    return comp_crt;
}

static void rmah_fmah_fix(void) {
    PP_ASSERT(_afe_cfg.fmah > 0.0f);

    uint8_t rmah_pct = 0;
    float comp_mah = 0;
    float predict_fmah = 0;

    if(_sox.fmah < 100) {
        // 纠错 # 容量修正.x-x.修正
        update_fmah(_afe_cfg.fmah, 1, 0);
        store_fmah_log(3);
        _sox.rmah_pct = 0XFF;
    }
    if (_sox.rmah_pct == 0XFF) {
        // 启动 # 电量修正.x
#ifdef PP_BAT_OCV_SHEET
        rmah_pct = ocv2_pct();
#else
        rmah_pct = 50;
#endif
        store_rmah_log(0, _sox.rmah, _sox.fmah * rmah_pct / 100.0f);
        _sox.rmah_pct = rmah_pct;
        _sox.rmah = _sox.fmah * rmah_pct / 100.0f;
    }
    if(sox_in.js) {
        if(chk_vol_diff() && chk_over_vol() && sox_in.nv <= _afe_cfg.uv1_th) {
            // 启动 # 容量修正.0-100.标记
            _sox.ff_x_100_x = 0;
            _sox.ff_x_100_type = 2;
            _sox.ff_x_100_dsg_mah = 0;
            _sox.ff_x_100_chg_mah = 0;
        }
    }
    if(_sox.cyc_dsg_mah > _sox.fmah * 0.9f) {
        // 循环 # 容量修正.x-x.修正
        _sox.cyc_cnt++;
        PP_ASSERT(_afe_cfg.max_cyc > 0);
        update_fmah(_sox.fmah - _afe_cfg.fmah * 0.2 / _afe_cfg.max_cyc , 1, 0);
        _sox.cyc_dsg_mah = 0;
        store_fmah_log(0);
        _sox.soh = 100 - (float)(_sox.cyc_cnt / _afe_cfg.max_cyc);
    }
#ifdef PP_BAT_OCV_SHEET
    if(PP_CONTINUED(chk_vol_diff() && chk_over_vol() && sox_in.st == 0, 60 * 60 * 1000)) { // 每闲置一小时做一次修正

        rmah_pct = ocv2_pct();
        store_rmah_log(2, _sox.rmah, _sox.fmah * rmah_pct / 100.0f);
        if(_sox.ff_x_100_type != 2 && rmah_pct < 40) {
            // 闲置 # 容量修正.x-100.标记
            _sox.ff_x_100_x = rmah_pct;
            _sox.ff_x_100_type = 1;
            _sox.ff_x_100_dsg_mah = 0;
            _sox.ff_x_100_chg_mah = 0;
        }
        if(_afe_cfg.ov1_th <= 4000 && _sox.rmah_pct - rmah_pct > 1) {
            // 闲置 # 电量修正.x.铁锂
            _sox.rmah_pct -= 1;
            _sox.rmah = _sox.rmah - _sox.fmah * 0.01f;
            _sox.cyc_dsg_mah += _sox.fmah * 0.01f;
        }
        if(_afe_cfg.ov1_th > 4000) {
            // 闲置 # 电量修正.x.三元
            comp_mah = _sox.fmah * rmah_pct / 100.0f - _sox.rmah;
            _sox.comp_mah = comp_mah;
        }
    }
#endif
    // 闲置 # 电量修正.自损耗
    // 暂时不做
    if(PP_CONTINUED(sox_in.st == 1 && _sox.ff_100_0_cf_flag == 1 && _sox.ff_100_0_dsg_flag == 0, 10 * 1000)) {
        // 放电 # 容量修正.100-0.标记
        _sox.ff_100_0_dsg_flag = 1;
    }
    if(PP_CONTINUED(sox_in.st != 1 && _sox.ff_100_0_dsg_flag == 1, 10 * 1000)) {
        // !放电 # 容量修正.100-0.标记
        _sox.ff_100_0_cf_flag = 2;
        _sox.ff_100_0_dsg_flag = 0;
    }
    if(PP_CONTINUED(sox_in.st != 2 && _sox.ff_x_100_type == 2 && _sox.ff_x_100_chg_mah > 100, 300 * 1000)) {
        // !充电 # 容量修正.0-100.清标
        _sox.ff_x_100_x = 0;
        _sox.ff_x_100_type = 0;
        _sox.ff_x_100_dsg_mah = 0;
        _sox.ff_x_100_chg_mah = 0;
    }
    if(!(sox_in.ov && sox_in.uv)) {
        if(chk_vol_diff() && chk_over_vol() && (sox_in.ov || sox_in.cf)) {
            // 充满 # 容量修正.100-0.标记
            _sox.ff_100_0_cf_flag = 1;
            _sox.ff_100_0_dsg_flag = 0;

            if(_sox.ff_x_100_type != 0) {
                // 充满 # 容量修正.0-100.修正
                // 充满 # 容量修正.x-100.修正
                if(_sox.ff_x_100_x <= 40 && _sox.ff_x_100_chg_mah > _sox.ff_x_100_dsg_mah) {
                    predict_fmah = (_sox.ff_x_100_chg_mah - _sox.ff_x_100_dsg_mah) * 98.5f / (100 - _sox.ff_x_100_x); // 充电损耗1.5%
                } else {
                    predict_fmah = 0;
                }
                update_fmah(predict_fmah, _sox.ff_x_100_type == 2, 0);
                store_fmah_log(1);
                // 充满 # 容量修正.0-100.清标
                _sox.ff_x_100_dsg_mah = 0;
                _sox.ff_x_100_chg_mah = 0;
                _sox.ff_x_100_type = 0;
                _sox.ff_x_100_x = 0;
            }
            // 充满 # 电量修正.充>100;满<100
            store_rmah_log(3, _sox.rmah, _sox.fmah);
            _sox.rmah = _sox.fmah;
            _sox.rmah_pct = 100;
            _sox.comp_mah = 0;
        }
        if(chk_vol_diff() && chk_over_vol() && (sox_in.uv)) {
            if (chk_tmpr()) {
                if(_sox.ff_x_100_type != 1) {
                    // 放空 # 容量修正.0-100.标记
                    _sox.ff_x_100_x = 0;
                    _sox.ff_x_100_type = 2;
                    _sox.ff_x_100_dsg_mah = 0;
                    _sox.ff_x_100_chg_mah = 0;
                }
            }
            if(_sox.ff_100_0_cf_flag != 0) {
                if(_sox.rmah_pct < 25) {
                    // 放空 # 容量修正.100-0.修正
                    predict_fmah = _sox.fmah - _sox.rmah;
                    if (_sox.fmah_lrn_flag == 0) {
                        _sox.fmah_lrn_flag = 1;
                        update_fmah(predict_fmah, 1, 0);
                    } else {
                        update_fmah(predict_fmah, _sox.ff_100_0_cf_flag == 1, 1);
                    }
                    store_fmah_log(2);
                }
                // 放空 # 容量修正.100-0.清标
                _sox.ff_100_0_cf_flag = 0;
                _sox.ff_100_0_dsg_flag = 0;
            }
            if(_sox.rmah_pct < 25) {
                // 放空 # 电量修正.放<0;空>0
                store_rmah_log(4, _sox.rmah, 0);
                _sox.rmah = 0;
                _sox.rmah_pct = 0;
            }
        }
    }
}

void mah_cmlt(void) {
    float tmpr_decrease_rate = 0;
    int64_t comp_dsg_chg_crt = 0;
    int64_t dsg_chg_crt = 0;
    float decrease_mah = 0;
    float still_decrease_mah = 0;
    float actual_dsg_chg_mah = 0;
    int64_t after_comp_dsg_chg_crt = 0;

    static float dsg_mah_float = 0;
    static float chg_mah_float = 0;
    int16_t dsg_chg_mah_uint = 0;

    if(sox_in.st == 1) {
        // 放电 # 电量修正.补偿
        tmpr_decrease_rate = tmpr_decrease_rate_();
        comp_dsg_chg_crt = 0;
        dsg_chg_crt = sox_in.cr * -1;
        if(_sox.rmah_pct <= 60) {
            decrease_mah = _sox.fmah * tmpr_decrease_rate;
            still_decrease_mah = decrease_mah - _sox.comp_mah;
            actual_dsg_chg_mah = _sox.rmah - still_decrease_mah;
            comp_dsg_chg_crt = comp_crt_calc(actual_dsg_chg_mah, still_decrease_mah, dsg_chg_crt);
        }
        after_comp_dsg_chg_crt = dsg_chg_crt + comp_dsg_chg_crt;
        _sox.comp_mah += comp_dsg_chg_crt * sox_in.ts / 4.0f / 3600.0f;
        // 为精确保留小数点后数字
        dsg_mah_float += after_comp_dsg_chg_crt * sox_in.ts * sox_in.ct / 1000.0f / 3600.0f;
        dsg_chg_mah_uint = (int16_t) dsg_mah_float;
        if(dsg_chg_mah_uint >= 1) {
            _sox.mah_t2 -= dsg_chg_mah_uint;
            dsg_mah_float -= dsg_chg_mah_uint;
        }
        if(_sox.ff_x_100_type != 0) {
            // 放电 # 容量修正.0-100.记录
            // 放电 # 容量修正.x-100.记录
            _sox.ff_x_100_dsg_mah += dsg_chg_crt * sox_in.ts * sox_in.ct / 1000.0f / 3600.0f;
        }
    }
    if(sox_in.st == 2) {
        // 充电 # 电量修正.补偿
        comp_dsg_chg_crt = 0;
        dsg_chg_crt = sox_in.cr;
        still_decrease_mah = _sox.comp_mah;
        actual_dsg_chg_mah = _sox.fmah - _sox.rmah - still_decrease_mah;
        comp_dsg_chg_crt = comp_crt_calc(actual_dsg_chg_mah, still_decrease_mah, dsg_chg_crt);
        after_comp_dsg_chg_crt = dsg_chg_crt + comp_dsg_chg_crt;
        _sox.comp_mah -= comp_dsg_chg_crt * sox_in.ts * sox_in.ct / 1000.0f / 3600.0f;
        // 为精确保留小数点后数字
        chg_mah_float += after_comp_dsg_chg_crt * sox_in.ts * sox_in.ct / 1000.0f / 3600.0f;
        dsg_chg_mah_uint = (int16_t)chg_mah_float;
        if(dsg_chg_mah_uint >= 1) {
            _sox.mah_t2 += dsg_chg_mah_uint;
            chg_mah_float -= dsg_chg_mah_uint;
        }
        if(_sox.ff_x_100_type != 0) {
            // 充电 # 容量修正.0-100.记录
            // 充电 # 容量修正.x-100.记录
            _sox.ff_x_100_chg_mah += dsg_chg_crt * sox_in.ts * sox_in.ct / 1000.0f / 3600.0f;
        }
    }
}

void mah_calc(void) {
    float mah_diff = 0;
    // 结束 # 计算电量.差
    mah_diff = _sox.mah_t2 - _sox.mah_t1;
    _sox.mah_t1  = _sox.mah_t2;
    if(mah_diff > _sox.fmah / 10 || mah_diff < _sox.fmah / -10) {
        mah_diff = 0;
    }
    if(sox_in.st == 0) {
        _sox.mah_t1 = 0;
        _sox.mah_t2 = 0;
    }
    // 循环 # 容量修正.x-x.记录
    if(mah_diff < 0) {
        _sox.cyc_dsg_mah += mah_diff * -1;
    }
    // 结束 # 计算电量.值
    if(sox_in.st == 1) {
        if(_sox.rmah > _sox.fmah) {
            // 结束 # 电量修正.充>100(实容大)
            if (_sox.rmah - _sox.fmah > _sox.fmah * 0.01f)
                store_rmah_log(5, _sox.rmah, _sox.fmah);
            _sox.rmah = _sox.fmah;
        }
        _sox.rmah += mah_diff;  // 存在：放<0(实容大);空>0(实容小)
    }
    if(sox_in.st == 2) {
        if(_sox.rmah < 0) {
            // 结束 # 电量修正.放<0(实容大)
            if (_sox.rmah < _sox.fmah * -0.01f)
                store_rmah_log(6, _sox.rmah, 0);
            _sox.rmah = 0;
        }
        _sox.rmah += mah_diff;  // 存在：充>100(实容大);满<100(实容小)
    }
    // 结束 # 计算电量.比
    PP_ASSERT(_sox.fmah > 0.0f);
    if(_sox.rmah > 0) {
        _sox.rmah_pct = (uint8_t)(_sox.rmah / _sox.fmah * 100 + 0.99f);  //存在>100
    } else {
        _sox.rmah_pct = 0;  // 解决<0
    }
    if(_sox.rmah_pct >= 100) {
        if(sox_in.cf) {
            _sox.rmah_pct = 100; // 解决>100
        } else if(sox_in.st == 2) {
            _sox.rmah_pct = 99;  // 解决>100
        } else {
            _sox.rmah_pct = 100; // 解决>100
        }
    }
}

void pp_sox_cal(void) {
    static uint8_t ov_status = 0;
    static uint8_t uv_status = 0;
    sox_in.js = _sys.first_cycle_flag;
    sox_in.ts = _afe.afe_times;
    sox_in.ct = _afe.afe_cyc_time;
    sox_in.cf = _afe.eoc_status;
    sox_in.ov = (_afe.ov1_status == 1 && ov_status == 0) ? 1 : 0;
    sox_in.uv = (_afe.uv1_status == 1 && uv_status == 0) ? 1 : 0;
    sox_in.xt = _afe.cell_max_temp;
    sox_in.nt = _afe.cell_min_temp;
    sox_in.cr = _afe.crt > _afe_cfg.zero_th ? _afe.crt : _afe.crt < -_afe_cfg.zero_th ? _afe.crt : 0;
    sox_in.tv = _afe.bat_vol;
    sox_in.xv = _afe.cell_max_vol;
    sox_in.nv = _afe.cell_min_vol;
    if(_afe.crt < 0) {
        sox_in.st = 1;
    } else if(_afe.crt > 0) {
        sox_in.st = 2;
    } else {
        sox_in.st = 0;
    }

    rmah_fmah_fix();
    mah_cmlt();
    mah_calc();
    ov_status = _afe.ov1_status;
    uv_status = _afe.uv1_status;
}