/* Test program for pow_*.c */
#include "common.h"
#include "implcontext.h"

#if defined(ENABLE_AVX)
extern ImplContext PoWAVX_Context;
#elif defined(ENABLE_SSE)
extern ImplContext PoWSSE_Context;
#else
extern ImplContext PoWC_Context;
#endif

#if defined(ENABLE_OPENCL)
extern ImplContext PoWCL_Context;
#endif

#if defined(ENABLE_FPGA_ACCEL)
extern ImplContext PoWFPGAAccel_Context;
#endif

const char *description[ ] = {
#if defined(ENABLE_AVX)
    "CPU - AVX",
#elif defined(ENABLE_SSE)
    "CPU - SSE",
#else
    "CPU - pure C",
#endif

#if defined(ENABLE_OPENCL)
    "GPU - OpenCL",
#endif

#if defined(ENABLE_FPGA_ACCEL)
    "FPGA",
#endif
};

int main()
{
    char *trytes =
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "99999999999999999A9RGRKVGWMWMKOLVMDFWJUHNUNYWZTJADGGPZGXNLERLXYWJE9WQH"
        "WWBMCPZMVVMJUMWWBLZLNMLDCGDJ999999999999999999999999999999999999999999"
        "999999999999YGYQIVD99999999999999999999TXEFLKNPJRBYZPORHZU9CEMFIFVVQBU"
        "STDGSJCZMBTZCDTTJVUFPTCCVHHORPMGCURKTH9VGJIXUQJVHK99999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999999999999999999999999999999999999999999999999999999999999"
        "9999999999999";

    int mwm = 14;

    ImplContext ImplContextArr[ ] = {
#if defined(ENABLE_AVX)
        PoWAVX_Context,
#elif defined(ENABLE_SSE)
        PoWSSE_Context,
#else
        PoWC_Context,
#endif

#if defined(ENABLE_OPENCL)
        PoWCL_Context,
#endif

#if defined(ENABLE_FPGA_ACCEL)
        PoWFPGAAccel_Context,
#endif
    };
    PoW_Info pow_info; 

    for (int idx = 0; idx < sizeof(ImplContextArr) / sizeof(ImplContext); idx++) {
        printf("%s\n",description[idx]);

        ImplContext *PoW_Context_ptr = &ImplContextArr[idx];

        /* test implementation with mwm = 14 */
        initializeImplContext(PoW_Context_ptr);
        void *pow_ctx = getPoWContext(PoW_Context_ptr, (int8_t *) trytes, mwm);
        assert(pow_ctx);
        doThePoW(PoW_Context_ptr, pow_ctx);
        int8_t *ret_trytes = getPoWResult(PoW_Context_ptr, pow_ctx);
        assert(ret_trytes);
        PoW_Info *info = (PoW_Info *) getPoWInfo(PoW_Context_ptr, pow_ctx);
        assert(info);
        pow_info.time = info->time;
        pow_info.hash_count = info->hash_count;
        freePoWContext(PoW_Context_ptr, pow_ctx);
        destroyImplContext(PoW_Context_ptr);

        Trytes_t *trytes_t = initTrytes(ret_trytes, TRANSACTION_TRYTES_LENGTH);
        assert(trytes_t);
        Trytes_t *hash_trytes = hashTrytes(trytes_t);
        assert(hash_trytes);
        Trits_t *ret_trits = trits_from_trytes(hash_trytes);
        assert(ret_trits);

        /* Validation */
        for (int i = HASH_TRITS_LENGTH - 1; i >= HASH_TRITS_LENGTH - mwm; i--) {
            assert(ret_trits->data[i] == 0);
        }

        free(ret_trytes);
        freeTrobject(trytes_t);
        freeTrobject(hash_trytes);
        freeTrobject(ret_trits);

#if defined(ENABLE_STAT)
        printf("Hash count: %"PRIu64"\n", pow_info.hash_count);
        printf("PoW execution time: %.3f sec\n", pow_info.time);
        printf("Hash rate: %.3lf kH/sec\n", pow_info.hash_count / pow_info.time / 1000);
#endif
        printf("Success.\n");
    }

    return 0;
}