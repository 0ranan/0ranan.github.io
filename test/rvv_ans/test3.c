#include <stdio.h>
#include <stdint.h>
#include <riscv_vector.h>

/**
 * @brief 使用RVV Intrinsic实现32位浮点数组加法 c = a + b
 * @param c 输出数组，存储计算结果
 * @param a 输入数组1
 * @param b 输入数组2
 * @param n 数组元素个数
 */
void rvv_add_float32(float *c, const float *a, const float *b, size_t n) {
    // ====================== 学生编写代码区域 ======================
    
    
    
    // ====================== 学生编写代码区域结束 ======================
}

// 标量实现（用于验证结果）
void scalar_add_float32(float *c, const float *a, const float *b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

int main() {
    const size_t N = 12;
    float a[N], b[N], c_rvv[N], c_scalar[N];

    // 初始化测试数据
    for (size_t i = 0; i < N; i++) {
        a[i] = (float)i * 0.5f;
        b[i] = (float)i * 0.3f;
    }

    // 计算
    rvv_add_float32(c_rvv, a, b, N);
    scalar_add_float32(c_scalar, a, b, N);

    // 验证结果（浮点比较允许微小误差）
    int passed = 1;
    const float epsilon = 1e-6f;
    printf("\n题目3：32位浮点数组加法\n");
    printf("索引\t a[i]\t\t b[i]\t\t RVV结果\t 标量结果\n");
    printf("------------------------------------------------------------\n");
    for (size_t i = 0; i < N; i++) {
        printf("%zu\t %.6f\t %.6f\t %.6f\t %.6f\n", 
               i, a[i], b[i], c_rvv[i], c_scalar[i]);
        if (__builtin_fabsf(c_rvv[i] - c_scalar[i]) > epsilon) {
            passed = 0;
        }
    }

    printf("------------------------------------------------------------\n");
    if (passed) {
        printf("✅ 测试通过！\n");
    } else {
        printf("❌ 测试失败！\n");
    }

    return 0;
}

/* 预期输出：
索引    a[i]            b[i]            RVV结果         标量结果
------------------------------------------------------------
0       0.000000        0.000000        0.000000        0.000000
1       0.500000        0.300000        0.800000        0.800000
2       1.000000        0.600000        1.600000        1.600000
3       1.500000        0.900000        2.400000        2.400000
4       2.000000        1.200000        3.200000        3.200000
5       2.500000        1.500000        4.000000        4.000000
6       3.000000        1.800000        4.800000        4.800000
7       3.500000        2.100000        5.600000        5.600000
8       4.000000        2.400000        6.400000        6.400000
9       4.500000        2.700000        7.200000        7.200000
10      5.000000        3.000000        8.000000        8.000000
11      5.500000        3.300000        8.800000        8.800000
------------------------------------------------------------
✅ 测试通过！
*/
