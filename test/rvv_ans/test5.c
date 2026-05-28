#include <stdio.h>
#include <stdint.h>
#include <riscv_vector.h>

/**
 * @brief 使用RVV Intrinsic实现数组所有元素求和
 * @param a 输入数组
 * @param n 数组元素个数
 * @return 数组所有元素的和
 */
uint64_t rvv_sum_uint64(const uint64_t *a, size_t n) {
    uint64_t sum = 0;
    // ====================== 学生编写代码区域 ======================
    // 提示：使用vredsum_vs_u64m8_u64m1函数进行归约求和
    // 函数原型：vuint64m1_t vredsum_vs_u64m8_u64m1(vuint64m1_t dest, vuint64m8_t vs2, vuint64m1_t vs1, size_t vl)
    
    vuint64m1_t vsum = __riscv_vmv_v_x_u64m1(0, 1);  // 初始化为 0

    size_t vl = 0 ;

    for(size_t i = 0 ; i < n ; i+=vl){

        vl = __riscv_vsetvl_e64m8(n-i);
        // 创建向量寄存器
        // 将数据转化为向量寄存器
        vuint64m8_t  va = __riscv_vle64_v_u64m8(a+i, vl);
        // 向量寄存器计算
        vsum = __riscv_vredsum_vs_u64m8_u64m1(va,vsum,vl);
        
    }
    // 将数据转化为标量
    sum =  __riscv_vmv_x_s_u64m1_u64(vsum);
    
    // ====================== 学生编写代码区域结束 ======================
    return sum;
}

// 标量实现（用于验证结果）
uint64_t scalar_sum_uint64(const uint64_t *a, size_t n) {
    uint64_t sum = 0;
    for (size_t i = 0; i < n; i++) {
        sum += a[i];
    }
    return sum;
}

int main() {
    const size_t N = 1000;
    uint64_t a[N];

    // 初始化测试数据（1到100）
    for (size_t i = 0; i < N; i++) {
        a[i] = i + 1;
    }

    // 计算
    uint64_t sum_rvv = rvv_sum_uint64(a, N);
    uint64_t sum_scalar = scalar_sum_uint64(a, N);

    // 验证结果
    printf("\n题目5：数组求和（1到100）\n");
    printf("RVV计算结果：%lu\n", sum_rvv);
    printf("标量计算结果：%lu\n", sum_scalar);
    printf("数学公式结果：%lu\n", (100 * 101) / 2);
    printf("----------------------------------------\n");

    if (sum_rvv == sum_scalar) {
        printf("✅ 测试通过！\n");
    } else {
        printf("❌ 测试失败！\n");
    }

    return 0;
}

/* 预期输出：
题目5：数组求和（1到100）
RVV计算结果：5050
标量计算结果：5050
数学公式结果：5050
----------------------------------------
✅ 测试通过！
*/
