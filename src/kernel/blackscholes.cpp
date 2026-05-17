#include "blackscholes.h"
#include <algorithm>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <random>

#define inv_sqrt_2xPI 0.39894228040143270286
#define p_val 0.2316419
#define coefficient_a1 0.319381530
#define coefficient_a2 -0.356563782
#define coefficient_a3 1.781477937
#define coefficient_a4 -1.821255978
#define coefficient_a5 1.330274429

constexpr static std::array<float, 256> table = {
    1.00000000e+00f,     9.39413063e-01f,     8.82496903e-01f,     8.29029118e-01f,     7.78800783e-01f,     7.31615629e-01f,     6.87289279e-01f,     6.45648526e-01f,
    6.06530660e-01f,     5.69782825e-01f,     5.35261429e-01f,     5.02831578e-01f,     4.72366553e-01f,     4.43747310e-01f,     4.16862020e-01f,     3.91605627e-01f,
    3.67879441e-01f,     3.45590753e-01f,     3.24652467e-01f,     3.04982769e-01f,     2.86504797e-01f,     2.69146349e-01f,     2.52839596e-01f,     2.37520819e-01f,
    2.23130160e-01f,     2.09611387e-01f,     1.96911675e-01f,     1.84981400e-01f,     1.73773943e-01f,     1.63245512e-01f,     1.53354967e-01f,     1.44063659e-01f,
    1.35335283e-01f,     1.27135733e-01f,     1.19432968e-01f,     1.12196891e-01f,     1.05399225e-01f,     9.90134084e-02f,     9.30144892e-02f,     8.73790262e-02f,
    8.20849986e-02f,     7.71117200e-02f,     7.24397570e-02f,     6.80508540e-02f,     6.39278612e-02f,     6.00546679e-02f,     5.64161395e-02f,     5.29980584e-02f,
    4.97870684e-02f,     4.67706224e-02f,     4.39369336e-02f,     4.12749294e-02f,     3.87742078e-02f,     3.64249973e-02f,     3.42181183e-02f,     3.21449473e-02f,
    3.01973834e-02f,     2.83678164e-02f,     2.66490973e-02f,     2.50345101e-02f,     2.35177459e-02f,     2.20928777e-02f,     2.07543379e-02f,     1.94968961e-02f,
    1.83156389e-02f,     1.72059504e-02f,     1.61634946e-02f,     1.51841980e-02f,     1.42642339e-02f,     1.34000077e-02f,     1.25881422e-02f,     1.18254653e-02f,
    1.11089965e-02f,     1.04359365e-02f,     9.80365504e-03f,     9.20968160e-03f,     8.65169520e-03f,     8.12751549e-03f,     7.63509422e-03f,     7.17250725e-03f,
    6.73794700e-03f,     6.32971543e-03f,     5.94621736e-03f,     5.58595426e-03f,     5.24751840e-03f,     4.92958733e-03f,     4.63091873e-03f,     4.35034555e-03f,
    4.08677144e-03f,     3.83916647e-03f,     3.60656314e-03f,     3.38805252e-03f,     3.18278080e-03f,     2.98994586e-03f,     2.80879419e-03f,     2.63861796e-03f,
    2.47875218e-03f,     2.32857217e-03f,     2.18749112e-03f,     2.05495773e-03f,     1.93045414e-03f,     1.81349383e-03f,     1.70361980e-03f,     1.60040269e-03f,
    1.50343919e-03f,     1.41235042e-03f,     1.32678043e-03f,     1.24639487e-03f,     1.17087962e-03f,     1.09993961e-03f,     1.03329764e-03f,     9.70693300e-04f,
    9.11881966e-04f,     8.56633830e-04f,     8.04733010e-04f,     7.55976702e-04f,     7.10174389e-04f,     6.67147098e-04f,     6.26726698e-04f,     5.88755247e-04f,
    5.53084370e-04f,     5.19574682e-04f,     4.88095244e-04f,     4.58523048e-04f,     4.30742541e-04f,     4.04645169e-04f,     3.80128958e-04f,     3.57098109e-04f,
    3.35462628e-04f,     3.15137975e-04f,     2.96044730e-04f,     2.78108287e-04f,     2.61258557e-04f,     2.45429702e-04f,     2.30559868e-04f,     2.16590951e-04f,
    2.03468369e-04f,     1.91140844e-04f,     1.79560205e-04f,     1.68681203e-04f,     1.58461325e-04f,     1.48860639e-04f,     1.39841629e-04f,     1.31369053e-04f,
    1.23409804e-04f,     1.15932782e-04f,     1.08908770e-04f,     1.02310321e-04f,     9.61116521e-05f,     9.02885414e-05f,     8.48182352e-05f,     7.96793582e-05f,
    7.48518299e-05f,     7.03167868e-05f,     6.60565080e-05f,     6.20543465e-05f,     5.82946637e-05f,     5.47627686e-05f,     5.14448602e-05f,     4.83279737e-05f,
    4.53999298e-05f,     4.26492871e-05f,     4.00652974e-05f,     3.76378637e-05f,     3.53575009e-05f,     3.32152982e-05f,     3.12028850e-05f,     2.93123978e-05f,
    2.75364493e-05f,     2.58681002e-05f,     2.43008313e-05f,     2.28285183e-05f,     2.14454083e-05f,     2.01460967e-05f,     1.89255064e-05f,     1.77788679e-05f,
    1.67017008e-05f,     1.56897959e-05f,     1.47391992e-05f,     1.38461963e-05f,     1.30072977e-05f,     1.22192253e-05f,     1.14788999e-05f,     1.07834285e-05f,
    1.01300936e-05f,     9.51634225e-06f,     8.93977622e-06f,     8.39814256e-06f,     7.88932483e-06f,     7.41133480e-06f,     6.96230472e-06f,     6.54048000e-06f,
    6.14421235e-06f,     5.77195335e-06f,     5.42224837e-06f,     5.09373095e-06f,     4.78511739e-06f,     4.49520179e-06f,     4.22285128e-06f,     3.96700165e-06f,
    3.72665317e-06f,     3.50086667e-06f,     3.28875988e-06f,     3.08950399e-06f,     2.90232041e-06f,     2.72647770e-06f,     2.56128877e-06f,     2.40610813e-06f,
    2.26032941e-06f,     2.12338297e-06f,     1.99473370e-06f,     1.87387890e-06f,     1.76034631e-06f,     1.65369232e-06f,     1.55350017e-06f,     1.45937835e-06f,
    1.37095909e-06f,     1.28789687e-06f,     1.20986715e-06f,     1.13656500e-06f,     1.06770401e-06f,     1.00301509e-06f,     9.42245482e-07f,     8.85157714e-07f,
    8.31528719e-07f,     7.81148941e-07f,     7.33821519e-07f,     6.89361521e-07f,     6.47595218e-07f,     6.08359407e-07f,     5.71500774e-07f,     5.36875292e-07f,
    5.04347663e-07f,     4.73790782e-07f,     4.45085250e-07f,     4.18118898e-07f,     3.92786355e-07f,     3.68988632e-07f,     3.46632741e-07f,     3.25631325e-07f,
    3.05902321e-07f,     2.87368636e-07f,     2.69957850e-07f,     2.53601931e-07f,     2.38236967e-07f,     2.23802919e-07f,     2.10243385e-07f,     1.97505382e-07f,
    1.85539136e-07f,     1.74297888e-07f,     1.63737713e-07f,     1.53817347e-07f,     1.44498025e-07f,     1.35743332e-07f,     1.27519059e-07f,     1.19793070e-07f,
};


void initialize_blackscholes(blackscholes_args &args,
                             std::size_t n,
                             std::uint32_t seed) {
    args.call_option_price.assign(n, 0.0f);
    args.put_option_price.assign(n, 0.0f);
    args.epsilon = 5e-3;

    args.spot_price.resize(n);
    args.strike.resize(n);
    args.rate.resize(n);
    args.volatility.resize(n);
    args.time.resize(n);

    std::mt19937 rng(seed);
    std::uniform_real_distribution<float> spot_dist(50.0f, 99.9f);
    std::uniform_real_distribution<float> strike_dist(50.0f, 99.9f);
    std::uniform_real_distribution<float> rate_dist(0.0275f, 0.1f);
    std::uniform_real_distribution<float> vol_dist(0.05f, 0.6f);
    std::uniform_real_distribution<float> time_dist(0.1f, 1.0f);

    for (std::size_t i = 0; i < n; ++i) {
        args.spot_price[i] = spot_dist(rng);
        args.strike[i] = strike_dist(rng);
        args.rate[i] = rate_dist(rng);
        args.volatility[i] = vol_dist(rng);
        args.time[i] = time_dist(rng);
    }
}

void CNDF(float &InputX, float &OutputX) {
    int sign = 0;
    float x = InputX;

    if (x < 0.0f) {
        x = -x;
        sign = 1;
    }

    const float xNPrimeofX = std::exp(-0.5f * x * x) * inv_sqrt_2xPI;
    const float k = 1.0f / (1.0f + p_val * x);
    const float k_2 = k * k;
    const float k_3 = k_2 * k;
    const float k_4 = k_3 * k;
    const float k_5 = k_4 * k;

    float local = k * coefficient_a1;
    local += k_2 * coefficient_a2;
    local += k_3 * coefficient_a3;
    local += k_4 * coefficient_a4;
    local += k_5 * coefficient_a5;
    local = 1.0f - local * xNPrimeofX;

    OutputX = sign ? (1.0f - local) : local;
}

static inline void naive_BlkSchls_one(float &CallOptionPrice,
                                      float &PutOptionPrice, float spotPrice,
                                      float strike, float rate,
                                      float volatility, float time) {
    const float xSqrtTime = std::sqrt(time);
    const float xLogTerm = std::log(spotPrice / strike);
    const float xPowerTerm = 0.5f * volatility * volatility;

    float xD1 = (rate + xPowerTerm) * time + xLogTerm;
    const float xDen = volatility * xSqrtTime;
    xD1 = xD1 / xDen;
    const float xD2 = xD1 - xDen;

    float d1 = xD1;
    float d2 = xD2;
    float NofXd1 = 0.0f;
    float NofXd2 = 0.0f;

    CNDF(d1, NofXd1);
    CNDF(d2, NofXd2);

    const float FutureValueX = strike * std::exp(-(rate) * (time));
    CallOptionPrice = (spotPrice * NofXd1) - (FutureValueX * NofXd2);

    const float NegNofXd1 = 1.0f - NofXd1;
    const float NegNofXd2 = 1.0f - NofXd2;
    PutOptionPrice = (FutureValueX * NegNofXd2) - (spotPrice * NegNofXd1);
}

void naive_BlkSchls(std::vector<float> &CallOptionPrice,
                    std::vector<float> &PutOptionPrice,
                    const std::vector<float> &spotPrice,
                    const std::vector<float> &strike,
                    const std::vector<float> &rate,
                    const std::vector<float> &volatility,
                    const std::vector<float> &time) {
    size_t n = spotPrice.size();
    for (size_t i = 0; i < n; ++i) {
        naive_BlkSchls_one(CallOptionPrice[i],
                           PutOptionPrice[i],
                           spotPrice[i],
                           strike[i],
                           rate[i],
                           volatility[i],
                           time[i]);
    }
}

static inline float stu_exp(float x) { // Domain: [-16, 0]
    if (x < -16.0f) return 0.0f; 
    const int k = std::floor(-x * 16.0f);
    const float r = -k * 0.0625f - x;

    return table[k] * (1.0f - r + 0.5f * r * r);
}

static inline float fast_sqrt_inverse(float x) {
    auto u = std::bit_cast<uint32_t>(x);
    u = 0x5F375A86-(u >> 1);
    auto y = std::bit_cast<float>(u);
    return y * (1.5f - 0.5f * x * y * y);
}

// perf annotate shows that the constant macro is recognized as double type
// and results in the generation of duplicate float-double converting. 
// Here we hard code the constants as float to avoid the cost
static inline void stu_CNDF(float &InputX, float &OutputX) {
    int sign = 0;
    float x = InputX;

    if (x < 0.0f) {
        x = -x;
        sign = 1;
    }

    const float xNPrimeofX = stu_exp(-0.5f * x * x) * 0.39894228040143270286f;
    const float k = 1.0f / (1.0f + 0.2316419f * x);
    const float k_2 = k * k;
    const float k_3 = k_2 * k;
    const float k_4 = k_3 * k;
    const float k_5 = k_4 * k;

    float local = k * 0.319381530f;
    local += k_2 * -0.356563782f;
    local += k_3 * 1.781477937f;
    local += k_4 * -1.821255978f;
    local += k_5 * 1.330274429f;
    local = 1.0f - local * xNPrimeofX;

    OutputX = sign ? (1.0f - local) : local;
}

void stu_BlkSchls(std::vector<float> &CallOptionPrice,
                  std::vector<float> &PutOptionPrice,
                  const std::vector<float> &spotPrice,
                  const std::vector<float> &strike,
                  const std::vector<float> &rate,
                  const std::vector<float> &volatility,
                  const std::vector<float> &time) {
    size_t n = spotPrice.size();
    for (size_t i = 0; i < n; ++i) {
        const float xSqrtTime = std::sqrt(time[i]);
        const float xLogTerm = std::log(spotPrice[i] / strike[i]);
        const float xPowerTerm = 0.5f * volatility[i] * volatility[i];

        float xD1 = (rate[i] + xPowerTerm) * time[i] + xLogTerm;
        const float xDen = volatility[i] * xSqrtTime;
        xD1 = xD1 / xDen;
        const float xD2 = xD1 - xDen;
    
        float d1 = xD1;
        float d2 = xD2;
        float NofXd1 = 0.0f;
        float NofXd2 = 0.0f;
    
        stu_CNDF(d1, NofXd1);
        stu_CNDF(d2, NofXd2);
    
        const float FutureValueX = strike[i] * stu_exp(-(rate[i]) * (time[i]));
        CallOptionPrice[i] = (spotPrice[i] * NofXd1) - (FutureValueX * NofXd2);
    
        const float NegNofXd1 = 1.0f - NofXd1;
        const float NegNofXd2 = 1.0f - NofXd2;
        PutOptionPrice[i] = (FutureValueX * NegNofXd2) - (spotPrice[i] * NegNofXd1);
    }
}

void naive_BlkSchls_wrapper(void *ctx) {
    auto &args = *static_cast<blackscholes_args *>(ctx);
    naive_BlkSchls(args.call_option_price,
                   args.put_option_price,
                   args.spot_price,
                   args.strike,
                   args.rate,
                   args.volatility,
                   args.time);
}

void stu_BlkSchls_wrapper(void *ctx) {
    auto &args = *static_cast<blackscholes_args *>(ctx);
    stu_BlkSchls(args.call_option_price,
                 args.put_option_price,
                 args.spot_price,
                 args.strike,
                 args.rate,
                 args.volatility,
                 args.time);
}

bool BlkSchls_check(void *stu_ctx, void *ref_ctx, lab_test_func naive_func) {
    naive_func(ref_ctx);
    auto &stu_args = *static_cast<blackscholes_args *>(stu_ctx);
    auto &ref_args = *static_cast<blackscholes_args *>(ref_ctx);
    const double eps = ref_args.epsilon; // relative tolerance

    if (ref_args.call_option_price.size() != stu_args.call_option_price.size() ||
        ref_args.put_option_price.size() != stu_args.put_option_price.size())
        return false;

    const double atol = 1e-5; // absolute tolerance for near-zero prices
    const size_t n = ref_args.call_option_price.size();
    double max_rel = 0.0, max_abs = 0.0;
    size_t max_idx = 0;
    const char *max_leg = "call";

    for (size_t i = 0; i < n; ++i) {
        const double rc = static_cast<double>(ref_args.call_option_price[i]);
        const double rp = static_cast<double>(ref_args.put_option_price[i]);
        const double sc = static_cast<double>(stu_args.call_option_price[i]);
        const double sp = static_cast<double>(stu_args.put_option_price[i]);

        const double err_c = std::abs(rc - sc);
        const double err_p = std::abs(rp - sp);
        const double rel_c = (err_c - atol) / std::abs(rc);
        const double rel_p = (err_p - atol) / std::abs(rp);

        const bool call_ok = err_c <= (atol + eps * std::abs(rc));
        const bool put_ok = err_p <= (atol + eps * std::abs(rp));

        if (rel_c > max_rel) {
            max_abs = err_c;
            max_rel = rel_c;
            max_idx = i;
            max_leg = "call";
        }
        if (rel_p > max_rel) {
            max_abs = err_p;
            max_rel = rel_p;
            max_idx = i;
            max_leg = "put";
        }

        if (!call_ok || !put_ok) {
            debug_log("\tDEBUG: fail idx={} | call ref={} stu={} err={} thr={} | put ref={} stu={} err={} thr={}\n",
                      i,
                      rc,
                      sc,
                      err_c,
                      (atol + eps * std::abs(rc)),
                      rp,
                      sp,
                      err_p,
                      (atol + eps * std::abs(rp)));
            return false;
        }
    }
    debug_log("\tBlkSchls_check passed: n={}, max_rel_err={}, max_abs_err={} at idx={} ({})\n",
              n,
              max_rel,
              max_abs,
              max_idx,
              max_leg);

    return true;
}
