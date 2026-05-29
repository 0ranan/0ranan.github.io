#include <stdio.h>
#include <stdint.h>
#include <riscv_vector.h>

/**
 * @brief 使用RVV Intrinsic实现数组每个元素乘以一个标量 c = a * scalar
 * @param c 输出数组，存储计算结果
 * @param a 输入数组
 * @param scalar 标量乘数
 * @param n 数组元素个数
 */
void rvv_mul_scalar_uint64(uint64_t *c, const uint64_t *a, uint64_t scalar, size_t n) {
    // ====================== 学生编写代码区域 ======================
    // 提示：使用vmul_vx_u64m8函数（vx表示向量×标量）
    
    
    
    // ====================== 学生编写代码区域结束 ======================
}

// 标量实现（用于验证结果）
void scalar_mul_scalar_uint64(uint64_t *c, const uint64_t *a, uint64_t scalar, size_t n) {
    for (size_t i = 0; i < n; i++) {
        c[i] = a[i] * scalar;
    }
}

int main() {
    const size_t N = 101;
    uint64_t a[N], c_rvv[N], c_scalar[N];
    const uint64_t scalar = 7; // 测试标量

    // 初始化测试数据
    for (size_t i = 0; i < N; i++) {
        a[i] = i + 1;
    }

    // 计算
    rvv_mul_scalar_uint64(c_rvv, a, scalar, N);
    scalar_mul_scalar_uint64(c_scalar, a, scalar, N);

    // 验证结果
    int passed = 1;
    printf("\n题目4：数组元素乘以标量（×%lu）\n", scalar);
    printf("索引\t a[i]\t RVV结果\t 标量结果\n");
    printf("----------------------------------------\n");
    for (size_t i = 0; i < N; i++) {
        printf("%zu\t %lu\t %lu\t\t %lu\n", i, a[i], c_rvv[i], c_scalar[i]);
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
索引    a[i]    RVV结果         标量结果
----------------------------------------
0       1       7               7
1       2       14              14
2       3       21              21
3       4       28              28
4       5       35              35
5       6       42              42
6       7       49              49
7       8       56              56
8       9       63              63
9       10      70              70
10      11      77              77
11      12      84              84
12      13      91              91
13      14      98              98
14      15      105             105
15      16      112             112
16      17      119             119
17      18      126             126
----------------------------------------
✅ 测试通过！
*/
