#include <stdio.h>
#include <stdint.h>
#include <riscv_vector.h>

/**
 * @brief 使用RVV Intrinsic实现整数数组减法 c = a - b
 * @param c 输出数组，存储计算结果
 * @param a 输入数组1（被减数）
 * @param b 输入数组2（减数）
 * @param n 数组元素个数
 */
void rvv_sub_int32(int32_t *c, const int32_t *a, const int32_t *b, size_t n) {
    // ====================== 学生编写代码区域 ======================

    // c = a - b 
    // 创建向量寄存器
    while(n){

        size_t vl = __riscv_vsetvl_e32m8(n);
        // 将数据转化为向量寄存器
        vint32m8_t  va = __riscv_vle32_v_i32m8(a, vl);
        vint32m8_t  vb = __riscv_vle32_v_i32m8(b, vl);

        // 向量寄存器计算
        vint32m8_t vc = __riscv_vsub_vv_i32m8(va,vb,vl);

        // 将数据转化为标量
        __riscv_vse32_v_i32m8(c, vc, vl);
        a+=vl ; 
        b+=vl ; 
        c+=vl ; 
        n-=vl ;
        printf("vl = %zu\n",vl) ;
    }
    
    // ====================== 学生编写代码区域结束 ======================
}

// 标量实现（用于验证结果）
void scalar_sub_int32(int32_t *c, const int32_t *a, const int32_t *b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        c[i] = a[i] - b[i];
    }
}

int main() {
    const size_t N = 101; // 故意用非2的幂，测试剩余数据处理
    int32_t a[N], b[N], c_rvv[N], c_scalar[N];

    // 初始化测试数据
    for (size_t i = 0; i < N; i++) {
        a[i] = i * 10;
        b[i] = i;
    }

    // 计算
    rvv_sub_int32(c_rvv, a, b, N);
    scalar_sub_int32(c_scalar, a, b, N);

    // 验证结果
    int passed = 1;
    printf("题目1：整数数组减法\n");
    printf("索引\t a[i]\t b[i]\t RVV结果\t 标量结果\n");
    printf("----------------------------------------\n");
    for (size_t i = 0; i < N; i++) {
        printf("%zu\t %d\t %d\t %d\t\t %d\n", i, a[i], b[i], c_rvv[i], c_scalar[i]);
        if (c_rvv[i] != c_scalar[i]) {
            passed = 0;
        }
    }

    printf("----------------------------------------\n");
    if (passed) {
        printf("✅ 测试通过！\n");
    } else {
        printf("❌ 测试失败！\n");
    }

    return 0;
}

/* 预期输出：
索引    a[i]    b[i]    RVV结果         标量结果
----------------------------------------
0       0       0       0               0
1       10      1       9               9
2       20      2       18              18
3       30      3       27              27
4       40      4       36              36
5       50      5       45              45
6       60      6       54              54
7       70      7       63              63
8       80      8       72              72
9       90      9       81              81
10      100     10      90              90
11      110     11      99              99
12      120     12      108             108
13      130     13      117             117
14      140     14      126             126
----------------------------------------
✅ 测试通过！
*/
