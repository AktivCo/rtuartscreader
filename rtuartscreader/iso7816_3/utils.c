#include <rtuartscreader/iso7816_3/utils.h>

#include <math.h>

#include <rtuartscreader/iso7816_3/detail/error.h>

const f_freq_max_t* f_freq_max_by_index(uint8_t index) {
    static const f_freq_max_t f_freq_max_table[16] = {
        { 372, 4000000 },
        { 372, 5000000 },
        { 558, 6000000 },
        { 744, 8000000 },
        { 1116, 12000000 },
        { 1488, 16000000 },
        { 1860, 20000000 },
        { IS0_7816_3_RFU, IS0_7816_3_RFU },
        { IS0_7816_3_RFU, IS0_7816_3_RFU },
        { 512, 5000000 },
        { 768, 7500000 },
        { 1024, 10000000 },
        { 1536, 15000000 },
        { 2048, 20000000 },
        { IS0_7816_3_RFU, IS0_7816_3_RFU },
        { IS0_7816_3_RFU, IS0_7816_3_RFU }
    };

    return &f_freq_max_table[index];
}

uint32_t d_by_index(uint8_t index) {
    static const uint32_t d_table[16] = {
        IS0_7816_3_RFU, 1, 2, 4, 8, 16, 32, 64,
        12, 20, IS0_7816_3_RFU, IS0_7816_3_RFU, IS0_7816_3_RFU, IS0_7816_3_RFU, IS0_7816_3_RFU, IS0_7816_3_RFU
    };

    return d_table[index];
}

static void get_fi_di(const atr_info_t* atr_info, uint32_t* fi, uint32_t* di) {
    f_d_index_t f_d_index = f_d_index_default;
    if (atr_info->ta1.is_present) {
        f_d_index = atr_info->ta1.f_d;
    }

    *fi = f_freq_max_by_index(f_d_index.f_index)->f;
    *di = d_by_index(f_d_index.d_index);
}

iso7816_3_status_t compute_extra_gt(uint32_t f, uint32_t d, const atr_info_t* atr_info, uint32_t freq,
                                    uint32_t* extra_gt_us) {
    if (!atr_info->tc1.is_present || atr_info->tc1.n == 255 || atr_info->tc1.n == 0) {
        *extra_gt_us = 0;
        return iso7816_3_status_ok;
    }

    double r = ((double)f) / d;
    if (atr_info->explicit_protocols[15]) {
        uint32_t fi, di;
        get_fi_di(atr_info, &fi, &di);
        if (fi == IS0_7816_3_RFU || di == IS0_7816_3_RFU) {
            LOG_RETURN_ISO7816_3_ERROR_MSG(iso7816_3_status_invalid_params,
                                           "Card transmission parameters (F, D) are invalid");
        }

        r = ((double)fi) / di;
    }

    // calculating in double to avoid integer overflows during calculation
    *extra_gt_us = (uint32_t)(S_TO_US_MULTIPLIER_LF * r * atr_info->tc1.n / freq);

    return iso7816_3_status_ok;
}

iso7816_3_status_t compute_wt(const atr_info_t* atr_info, uint32_t freq, double* wt) {
    uint32_t fi, di;
    get_fi_di(atr_info, &fi, &di);
    if (fi == IS0_7816_3_RFU) {
        LOG_RETURN_ISO7816_3_ERROR_MSG(iso7816_3_status_invalid_params,
                                       "Card transmission parameters (F, D) are invalid");
    }

    double wi = 10;
    if (atr_info->tc2.is_present) {
        wi = atr_info->tc2.wi;
    }

    *wt = wi * 960 * fi / freq;

    return iso7816_3_status_ok;
}