#include <stdio.h>
#include <stdint.h>
#include <riscv_vector.h>

/**
 * @brief 使用RVV Intrinsic实现16位整数数组乘法 c = a * b
 * @param c 输出数组，存储计算结果（32位，防止溢出）
 * @param a 输入数组1（16位）
 * @param b 输入数组2（16位）
 * @param n 数组元素个数
 */
void rvv_mul_int16_to_int32(int32_t *c, const int16_t *a, const int16_t *b, size_t n) {
    // ====================== 学生编写代码区域 ======================
    // 提示：使用vsetvl_e16m4配置，使用vwmul_vv_i16i32m4进行乘法运算
    
    while(n){

        size_t vl = __riscv_vsetvl_e16m4(n);
        // 将数据转化为向量寄存器
        vint16m4_t  va = __riscv_vle16_v_i16m4(a, vl);
        vint16m4_t  vb = __riscv_vle16_v_i16m4(b, vl);
        
        // 位宽翻倍，LMUL 也会随之翻倍
        // 向量寄存器计算：16位拓宽乘法至32位，输出类型为vint32m8_t，函数名后缀为_i32m8
        vint32m8_t vc = __riscv_vwmul_vv_i32m8(va,vb,vl);

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
void scalar_mul_int16_to_int32(int32_t *c, const int16_t *a, const int16_t *b, size_t n) {
    for (size_t i = 0; i < n; i++) {
        c[i] = (int32_t)a[i] * (int32_t)b[i];
    }
}

int main() {
    const size_t N = 101;
    int16_t a[N], b[N];
    int32_t c_rvv[N], c_scalar[N];

    // 初始化测试数据
    for (size_t i = 0; i < N; i++) {
        a[i] = (int16_t)(i + 1);
        b[i] = (int16_t)(i + 2);
    }

    // 计算
    rvv_mul_int16_to_int32(c_rvv, a, b, N);
    scalar_mul_int16_to_int32(c_scalar, a, b, N);

    // 验证结果
    int passed = 1;
    printf("\n题目2：16位整数数组乘法（结果32位）\n");
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

/* 预期输出（前5行和最后5行）：
索引    a[i]    b[i]    RVV结果         标量结果
----------------------------------------
0       1       2       2               2
1       2       3       6               6
2       3       4       12              12
3       4       5       20              20
4       5       6       30              30
...
15      16      17      272             272
16      17      18      306             306
17      18      19      342             342
18      19      20      380             380
19      20      21      420             420
----------------------------------------
✅ 测试通过！
*/
