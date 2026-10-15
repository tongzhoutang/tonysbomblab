#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;

    // TODO: 根据 phase_01 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t lo = (uint32_t)challenge;
    uint32_t hi = (uint32_t)(challenge >> 32);
    uint32_t mid = (uint32_t)(challenge >> 16);

    uint32_t a = (lo + 2u * hi) ^ 0x9809u;
    uint32_t b = (5u * hi) ^ (mid + 0xf369u);
    uint32_t d = (3u * b + lo) ^ 0xd128u;

    values[0] = d ^ a;
    count = 1;

    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    // TODO: 根据 phase_02 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    values[0] =(((uint32_t)(challenge >> 28) * 2u) & 0xffu) + 0x11u;

for (uint32_t i = 1; i < 6; ++i) {
    uint32_t part = (uint32_t)((challenge >> (2u * i)) & 0x1fu);
    values[i] = 3u * values[i - 1] + (part ^ (i + 6u));
}

    count = 6;

    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    // TODO: 根据 phase_03 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t index = (uint32_t)challenge & 7u;
    uint32_t x = (uint32_t)(challenge >> 32) ^ (uint32_t)(challenge >> 3);
    uint32_t answer = 0;

    switch (index) {
        case 0:
            answer = (4u * x) ^ 0x4e30u;
            break;
        case 1:
            answer = (x ^ 0xccc4u) + (x >> 1);
            break;
        case 2:
            answer = 5u * x + 0x100ccu;
            break;
        case 3:
            answer = (x ^ 0x7334u) + (x >> 2);
            break;
        case 4:
            answer = 5u * x + 0x5bb9u;
            break;
        case 5:
            answer = 3u * x + 0xa259u;
            break;
        case 6:
            answer = (2u * x) ^ 0x862fu;
            break;
        case 7:
            answer = 5u * x + 0x3d91eu;
            break;
    }

    values[0] = index;
    values[1] = answer;
    count = 2;

    return write_answer(out, cap, values, count);
}

static uint32_t phase4_walk(uint32_t target, uint32_t low,
                            uint32_t high, uint32_t key) {
    uint32_t mid = low + ((high - low) >> 1);

    if (target == mid) {
        return (mid + 18u) ^ key;
    }

    if (target < mid) {
        uint32_t child = phase4_walk(target, low, mid - 1u, key);
        return (5u * child) ^ (mid + key) ^ 3u;
    }

    uint32_t child = phase4_walk(target, mid + 1u, high, key);
    return (5u * child) ^ (mid + key) ^ 11u;
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    // TODO: 根据 phase_04 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t target = (uint32_t)challenge & 0x7fu;
    uint32_t key =
        (uint32_t)(challenge >> 7) ^
        (uint32_t)(challenge >> 32);

    values[0] = phase4_walk(target, 0u, 127u, key);

    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge, char *out, size_t cap) {
    static const uint32_t ids[] = {
        0x064eu, 0x074au, 0x0915u, 0x0011u,
        0x0887u, 0x0279u, 0x0a30u, 0x0b25u,
        0x046au, 0x031bu, 0x0552u, 0x011du
    };
    static const uint32_t weights[] = {
        0xba9du, 0x11c7u, 0xe729u, 0x4729u,
        0x10edu, 0xe585u, 0xb677u, 0x9ab6u,
        0xd934u, 0x4fc8u, 0xf847u, 0x4001u
    };

    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 2;
    // TODO: 根据 phase_05 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。

    uint32_t start = (uint32_t)challenge & 7u;
    uint32_t hops = (uint32_t)(challenge >> 3) & 3u;
    uint32_t acc =
        (uint32_t)(challenge >> 6) ^
        (uint32_t)(challenge >> 32) ^
        0x2c37u;

    for (uint32_t i = 0; i <= hops; ++i) {
        uint32_t pos = start + i;
        acc = (5u * (acc + weights[pos])) ^ ids[pos];
    }

    values[0] = acc;
    values[1] = ids[start + hops];

    return write_answer(out, cap, values, count);
}


static uint32_t phase6_apply(uint32_t index, uint32_t v) {
    switch (index) {
    case 0:
    case 3:
        return (v ^ (v >> 2)) + 0x9024u;

    case 1:
    case 2:
        return ((v << 7) | (v >> 25)) ^ 0x8191u;

    case 4:
        return (v ^ 0x53c4u) + (v << 7);

    case 5:
        return (v + 0x4e78u) ^ (v >> 5);

    case 6:
        return (2u * v + 0x5732u) ^ (7u * v);

    case 7:
        return (v >> 5) | (v << 27);
    }

    return 0;  // 调用处保证 index 在 0～7 内
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 1;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t first = (uint32_t)challenge & 7u;
    uint32_t second = (uint32_t)(challenge >> 3) & 7u;
    uint32_t x =
        (uint32_t)(challenge >> 6) ^
        (uint32_t)(challenge >> 32);

    uint32_t a = phase6_apply(first, x);
    uint32_t b = a ^ 0xd429u;
    uint32_t d = phase6_apply(second, b);

    values[0] = d + 0xc54du;

    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    uint32_t c =
    (uint32_t)challenge ^ (uint32_t)(challenge >> 32);

    uint32_t t =
    (uint32_t)context ^ (uint32_t)(context >> 32);
    uint32_t first = (uint32_t)context & 3u;
    uint32_t v = c ^ t;
    uint32_t rotated = (v << 1) | (v >> 31);
    uint32_t mixed = (rotated + 0x6941u) ^ 0x6b5u;
    uint32_t second =
    mixed ^ (first * 0x6941u) ^ (uint32_t)(context >> 16);
    uint32_t third =
    second ^ (t + first + 0x6b5u + (c >> 1));
    
     /* 四组各 8 个 A，共 32 字节；紧接 OPEN 和三个整数。 */
    int size = snprintf(
        out, cap,
        "AAAAAAAA" "AAAAAAAA" "AAAAAAAA" "AAAAAAAA"
        "OPEN"
        "%" PRIu32 " %" PRIu32 " %" PRIu32,
        first, second, third
    );

    if (size < 0 || (size_t)size >= cap) {
        return 1;
    }
    return 0;
}
