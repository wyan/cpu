#include "vendor/unity.h"
#include "../src/cpu.h"

#define MEM_SIZE 512

void setUp(void) {}

void tearDown(void) {}

void test_load_program_into_memory(void)
{
    const uint16_t program[] = {0xDEAD, 0xBEEF, 0xA5A5};

    CPU cpu(MEM_SIZE);

    cpu.loadmem(program, sizeof(program), 0x0);
    TEST_ASSERT_EQUAL_UINT16(program[0], cpu.getmem_at(0x0));
    TEST_ASSERT_EQUAL_UINT16(program[1], cpu.getmem_at(0x1));
    TEST_ASSERT_EQUAL_UINT16(program[2], cpu.getmem_at(0x2));

    cpu.loadmem(program, sizeof(program), 0x99);
    TEST_ASSERT_EQUAL_UINT16(program[0], cpu.getmem_at(0x99));
    TEST_ASSERT_EQUAL_UINT16(program[1], cpu.getmem_at(0x9A));
    TEST_ASSERT_EQUAL_UINT16(program[2], cpu.getmem_at(0x9B));
}

void test_instruction_jabsi_unconditional(void) {
    CPU cpu(MEM_SIZE);

    const uint16_t program[] = {0x5100, 0xD00B};

    cpu.loadmem(program, sizeof(program), 0x0100);
    cpu.reset();
    cpu.run_once();

    TEST_ASSERT_EQUAL_UINT16(0xD00B, cpu.getPC());
}

void test_instruction_jabsi_eq_taken(void) {
    CPU cpu(MEM_SIZE);

    const uint16_t program_eq[] = {0x0100, 0x0110, 0x4001, 0x5101, 0x01FE, 0xF800};
    const uint16_t program_branch[] = {0xF800};

    cpu.loadmem(program_eq, sizeof(program_eq), 0x0100);
    cpu.loadmem(program_branch, sizeof(program_branch), 0x01FE);
    cpu.reset();
    while(!cpu.halted()) cpu.run_once();

    TEST_ASSERT_EQUAL_UINT16(0x01FF, cpu.getPC());
}

void test_instruction_jabsi_eq_not_taken(void) {
    CPU cpu(MEM_SIZE);

    const uint16_t program_neq[] = {0x0100, 0x011F, 0x4001, 0x5101, 0xD00B, 0xF800};

    cpu.loadmem(program_neq, sizeof(program_neq), 0x0100);
    cpu.reset();
    while(!cpu.halted()) cpu.run_once();

    TEST_ASSERT_EQUAL_UINT16(0x0106, cpu.getPC());
}

void test_instruction_jabsi_neg_taken(void) {
    CPU cpu(MEM_SIZE);

    const uint16_t program_neq[] = {0x0100, 0x011F, 0x4001, 0x5106, 0x01FE, 0xF800};
    const uint16_t program_branch[] = {0xF800};

    cpu.loadmem(program_neq, sizeof(program_neq), 0x0100);
    cpu.loadmem(program_branch, sizeof(program_branch), 0x01FE);
    cpu.reset();
    while(!cpu.halted()) cpu.run_once();

    TEST_ASSERT_EQUAL_UINT16(0x01FF, cpu.getPC());
}

void test_instruction_jabsi_pos_not_taken(void) {
    CPU cpu(MEM_SIZE);

    const uint16_t program_neq[] = {0x0100, 0x011F, 0x4001, 0x510E, 0x01FE, 0xF800};
    const uint16_t program_branch[] = {0xF800};

    cpu.loadmem(program_neq, sizeof(program_neq), 0x0100);
    cpu.loadmem(program_branch, sizeof(program_branch), 0x01FE);
    cpu.reset();
    while(!cpu.halted()) cpu.run_once();

    TEST_ASSERT_EQUAL_UINT16(0x0106, cpu.getPC());
}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_load_program_into_memory);
    RUN_TEST(test_instruction_jabsi_unconditional);
    RUN_TEST(test_instruction_jabsi_eq_taken);
    RUN_TEST(test_instruction_jabsi_eq_not_taken);
    RUN_TEST(test_instruction_jabsi_neg_taken);
    RUN_TEST(test_instruction_jabsi_pos_not_taken);
    return UNITY_END();
}
