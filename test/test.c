#include "../cum.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static int ntests = 0;
static int npass  = 0;

#define TEST(name) do { printf("  %-54s ", name); ntests++; } while (0)
#define PASS()     do { printf("PASSED\n"); npass++; } while (0)

/* ======================== Join ======================== */

static void test_join(void)
{
    TEST("Join(+) multiple ints");
    assert(Join(+, 1, 2, 3, 4, 5) == 15);
    PASS();

    TEST("Join(+) single arg");
    assert(Join(+, 42) == 42);
    PASS();

    TEST("Join(*) multiple ints");
    assert(Join(*, 1, 2, 3, 4) == 24);
    PASS();

    TEST("Join(-)");
    assert(Join(-, 10, 3, 2) == 5);
    PASS();

    TEST("Join(&&)");
    assert((Join(&&, 1, 1, 0)) == 0);
    PASS();
}

/* ======================== Unused ======================== */

static void test_unused(void)
{
    TEST("Unused single var");
    { int a; Unused(a); }
    PASS();

    TEST("Unused multiple vars");
    { int a, b, c; Unused(a, b, c); }
    PASS();
}

/* ======================== Min / Max ======================== */

static void test_min_max(void)
{
    TEST("Min int");
    assert(Min(3, 7) == 3);
    PASS();

    TEST("Max int");
    assert(Max(3, 7) == 7);
    PASS();

    TEST("Min double");
    assert(Min(3.14, 2.71) == 2.71);
    PASS();

    TEST("Max double");
    assert(Max(3.14, 2.71) == 3.14);
    PASS();

    TEST("Min unsigned");
    assert(Min(5u, 3u) == 3u);
    PASS();

    TEST("Side-effects evaluated once");
    { int a = 5, b = 3; int r = Min(++a, ++b); assert(a == 6 && b == 4 && r == 4); }
    PASS();
}

/* ======================== Size literals ======================== */

static void test_sizes(void)
{
    TEST("KB(1)");
    assert(KB(1) == 1024ULL);
    PASS();

    TEST("MB(1)");
    assert(MB(1) == 1024ULL * 1024);
    PASS();

    TEST("GB(1)");
    assert(GB(1) == 1024ULL * 1024 * 1024);
    PASS();

    TEST("KB(4)");
    assert(KB(4) == 4096ULL);
    PASS();

    TEST("Composition KB(MB(1))");
    assert(KB(MB(1)) == 1024ULL * 1024 * 1024);
    PASS();

    TEST("TB(1)");
    assert(TB(1) == 1024ULL * 1024 * 1024 * 1024);
    PASS();
}

/* ======================== Bitfields ======================== */

static void test_bitfields(void)
{
    enum { A = 1, B = 2, C = 4, D = 8 };

    TEST("Bf init and has");
    { unsigned f; Bf(f); assert(!Bf_has(f, A) && !Bf_has(f, B)); }
    PASS();

    TEST("Bf_set single");
    { unsigned f; Bf(f); Bf_set(f, A); assert(Bf_has(f, A)); assert(!Bf_has(f, B)); }
    PASS();

    TEST("Bf_clr");
    { unsigned f; Bf(f); Bf_set(f, A|B); Bf_clr(f, A); assert(!Bf_has(f, A)); assert(Bf_has(f, B)); }
    PASS();

    TEST("Bf_has multiple bits");
    { unsigned f; Bf(f); Bf_set(f, A|B); assert(Bf_has(f, A|B)); }
    PASS();

    TEST("Bf_has requires all bits");
    { unsigned f; Bf(f); Bf_set(f, A|B); assert(!Bf_has(f, A|B|C)); }
    PASS();

    TEST("Bf_set multiple");
    { unsigned f; Bf(f); Bf_set(f, A|C); assert(Bf_has(f, A|C)); assert(!Bf_has(f, B)); }
    PASS();

    TEST("Bf_clr removes all if mask covers all");
    { unsigned f; Bf(f); Bf_set(f, A|B|C); Bf_clr(f, A|B|C); assert(!Bf_has(f, A|B|C)); }
    PASS();
}

/* ======================== Tostr ======================== */

static void test_tostr(void)
{
    TEST("Tostr");
    assert(strcmp(Tostr(hello), "hello") == 0);
    PASS();

    TEST("Tostr with numbers");
    assert(strcmp(Tostr(42), "42") == 0);
    PASS();

    TEST("Tostr with identifier");
    assert(strcmp(Tostr(my_var_name), "my_var_name") == 0);
    PASS();
}

/* ======================== Memdup / Memzero ======================== */

static void test_memdup(void)
{
    TEST("Memdup int");
    { int x = 42; int *p = Memdup(x); assert(*p == 42); free(p); }
    PASS();

    TEST("Memdup independence");
    { int x = 42; int *p = Memdup(x); *p = 99; assert(x == 42); free(p); }
    PASS();

    TEST("Memdup struct");
    { struct P { int x; double y; } pt = {10, 3.14}; struct P *cp = Memdup(pt); assert(cp->x == 10 && cp->y == 3.14); free(cp); }
    PASS();
}

static void test_memzero(void)
{
    TEST("Memzero");
    { struct S { int a; double b; char buf[8]; } s = {1, 2.0, "hello"}; Memzero(s); assert(s.a == 0 && s.b == 0.0 && s.buf[0] == '\0'); }
    PASS();
}

/* ======================== Dynamic Array ======================== */

typedef struct { int x, y; } Point;

static void test_da_basic(void)
{
    TEST("Da_append");
    { Da(int) da = {0}; Da_append(&da, 10); assert(da.count == 1 && da.items[0] == 10); Da_destroy(&da); }
    PASS();

    TEST("Da_append multiple");
    { Da(int) da = {0}; Da_append(&da, 1); Da_append(&da, 2); Da_append(&da, 3); assert(da.count == 3 && da.items[2] == 3); Da_destroy(&da); }
    PASS();

    TEST("Da_append triggers growth");
    { Da(int) da = {0}; for (int i = 0; i < 100; i++) Da_append(&da, i); assert(da.count == 100); for (int i = 0; i < 100; i++) assert(da.items[i] == i); Da_destroy(&da); }
    PASS();

    TEST("Da_destroy sets to zero");
    { Da(int) da = {0}; Da_append(&da, 1); Da_destroy(&da); assert(da.count == 0 && da.capacity == 0 && da.items == NULL); }
    PASS();

}

static void test_da_foreach(void)
{
    TEST("Da_foreach sum");
    { Da(int) da = {0}; Da_append(&da, 1); Da_append(&da, 2); Da_append(&da, 3); int s = 0; Da_foreach(it, da) s += *it; assert(s == 6); Da_destroy(&da); }
    PASS();

    TEST("Da_foreach empty");
    { Da(int) da = {0}; int c = 0; Da_foreach(it, da) c++; assert(c == 0); }
    PASS();

    TEST("Da_foreach modify elements");
    { Da(int) da = {0}; Da_append(&da, 1); Da_append(&da, 2); Da_foreach(it, da) *it *= 10; assert(da.items[0] == 10 && da.items[1] == 20); Da_destroy(&da); }
    PASS();
}

static void test_da_index(void)
{
    TEST("Da_index");
    { Da(int) da = {0}; Da_append(&da, 100); Da_append(&da, 200); Da_append(&da, 300); assert(Da_index(&da.items[1], &da) == 1); Da_destroy(&da); }
    PASS();

    TEST("Da_index first");
    { Da(int) da = {0}; Da_append(&da, 42); assert(Da_index(&da.items[0], &da) == 0); Da_destroy(&da); }
    PASS();
}

static void test_da_remove(void)
{
    TEST("Da_remove middle");
    { Da(int) da = {0}; Da_append(&da, 1); Da_append(&da, 2); Da_append(&da, 3); Da_remove(&da, 1); assert(da.count == 2 && da.items[0] == 1 && da.items[1] == 3); Da_destroy(&da); }
    PASS();

    TEST("Da_remove first");
    { Da(int) da = {0}; Da_append(&da, 1); Da_append(&da, 2); Da_remove(&da, 0); assert(da.count == 1 && da.items[0] == 2); Da_destroy(&da); }
    PASS();

    TEST("Da_remove last via index");
    { Da(int) da = {0}; Da_append(&da, 1); Da_append(&da, 2); Da_remove(&da, 1); assert(da.count == 1 && da.items[0] == 1); Da_destroy(&da); }
    PASS();

    TEST("Da_remove out-of-bounds (negative)");
    { Da(int) da = {0}; Da_append(&da, 1); Da_remove(&da, -1); assert(da.count == 1); Da_destroy(&da); }
    PASS();

    TEST("Da_remove out-of-bounds (too large)");
    { Da(int) da = {0}; Da_append(&da, 1); Da_remove(&da, 5); assert(da.count == 1); Da_destroy(&da); }
    PASS();

    TEST("Da_remove shrinks count");
    { Da(int) da = {0}; Da_append(&da, 10); Da_append(&da, 20); Da_remove(&da, 0); assert(da.count == 1); Da_destroy(&da); }
    PASS();
}

static void test_da_remove_last(void)
{
    TEST("Da_remove_last");
    { Da(int) da = {0}; Da_append(&da, 1); Da_append(&da, 2); Da_append(&da, 3); Da_remove_last(&da); assert(da.count == 2 && da.items[0] == 1 && da.items[1] == 2); Da_destroy(&da); }
    PASS();

    TEST("Da_remove_last until empty");
    { Da(int) da = {0}; Da_append(&da, 1); Da_remove_last(&da); assert(da.count == 0); Da_remove_last(&da); assert(da.count == 0); Da_destroy(&da); }
    PASS();
}

static void test_da_insert(void)
{
    TEST("Da_insert at beginning");
    { Da(int) da = {0}; Da_append(&da, 2); Da_append(&da, 3); Da_insert(&da, 1, 0); assert(da.count == 3 && da.items[0]==1 && da.items[1]==2 && da.items[2]==3); Da_destroy(&da); }
    PASS();

    TEST("Da_insert at end");
    { Da(int) da = {0}; Da_append(&da, 1); Da_append(&da, 2); Da_insert(&da, 3, 2); assert(da.count == 3 && da.items[0]==1 && da.items[1]==2 && da.items[2]==3); Da_destroy(&da); }
    PASS();

    TEST("Da_insert in middle");
    { Da(int) da = {0}; Da_append(&da, 1); Da_append(&da, 3); Da_insert(&da, 2, 1); assert(da.count == 3 && da.items[0]==1 && da.items[1]==2 && da.items[2]==3); Da_destroy(&da); }
    PASS();

    TEST("Da_insert into empty DA");
    { Da(int) da = {0}; Da_insert(&da, 42, 0); assert(da.count == 1 && da.items[0] == 42); Da_destroy(&da); }
    PASS();
}

static void test_da_dup(void)
{
    TEST("Da_dup values match");
    { Da(int) da = {0}; Da_append(&da, 10); Da_append(&da, 20); Typeof(da) c; c = Da_dup(&da); assert(c.count == 2 && c.items[0]==10 && c.items[1]==20); Da_destroy(&da); Da_destroy(&c); }
    PASS();

    TEST("Da_dup is independent copy");
    { Da(int) da = {0}; Da_append(&da, 1); Typeof(da) c; c = Da_dup(&da); da.items[0] = 99; assert(c.items[0] == 1); Da_destroy(&da); Da_destroy(&c); }
    PASS();

    TEST("Da_dup float");
    { Da(float) da = {0}; Da_append(&da, 1.5f); Da_append(&da, 2.5f); Typeof(da) c; c = Da_dup(&da); assert(c.items[0]==1.5f && c.items[1]==2.5f); Da_destroy(&da); Da_destroy(&c); }
    PASS();
}

static void test_da_struct(void)
{
    TEST("Da with struct type");
    { Da(Point) da = {0}; Da_append(&da, ((Point){1, 2})); Da_append(&da, ((Point){3, 4})); assert(da.items[0].x==1 && da.items[0].y==2 && da.items[1].x==3 && da.items[1].y==4); Da_destroy(&da); }
    PASS();
}

/* ======================== Stack (Ss) ======================== */

static void test_ss(void)
{
    TEST("Ss_push and Ss_top");
    { Ss(int) st = {0}; Ss_push(&st, 10); assert(Ss_top(&st) == 10); Ss_push(&st, 20); assert(Ss_top(&st) == 20); Da_destroy(&st); }
    PASS();

    TEST("Ss_pop");
    { Ss(int) st = {0}; Ss_push(&st, 1); Ss_push(&st, 2); Ss_pop(&st); assert(Ss_top(&st) == 1); Ss_pop(&st); assert(st.count == 0); Ss_pop(&st); assert(st.count == 0); Da_destroy(&st); }
    PASS();

    TEST("Ss_foreach");
    { Ss(int) st = {0}; Ss_push(&st, 100); Ss_push(&st, 200); Ss_push(&st, 300); int s = 0; Ss_foreach(it, st) s += *it; assert(s == 600); Da_destroy(&st); }
    PASS();
}

/* ======================== main ======================== */

int main(void)
{
    printf("=== cum.h C tests ===\n\n");

    printf("Join:\n");             test_join();
    printf("Unused:\n");           test_unused();
    printf("Min/Max:\n");          test_min_max();
    printf("Sizes:\n");            test_sizes();
    printf("Bitfields:\n");        test_bitfields();
    printf("Tostr:\n");            test_tostr();
    printf("Memdup:\n");           test_memdup();
    printf("Memzero:\n");          test_memzero();

    printf("\n-- Dynamic Array --\n");
    printf("Basic:\n");            test_da_basic();
    printf("Foreach:\n");          test_da_foreach();
    printf("Index:\n");            test_da_index();
    printf("Remove:\n");           test_da_remove();
    printf("Remove last:\n");      test_da_remove_last();
    printf("Insert:\n");           test_da_insert();
    printf("Dup:\n");              test_da_dup();
    printf("Struct:\n");           test_da_struct();

    printf("\n-- Stack --\n");
    test_ss();

    printf("\n%d/%d tests passed\n", npass, ntests);
    return npass == ntests ? 0 : 1;
}
