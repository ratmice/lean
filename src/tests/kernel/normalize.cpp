/*
Copyright (c) 2013 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#include <algorithm>
#include "normalize.h"
#include "trace.h"
#include "test.h"
#include "sets.h"
using namespace lean;

static void eval(expr const & e) { std::cout << e << " --> " << normalize(e) << "\n"; }
static expr t() { return constant("t"); }
static expr lam(expr const & e) { return lambda("_", t(), e); }
static expr lam(expr const & t, expr const & e) { return lambda("_", t, e); }
static expr v(unsigned i) { return var(i); }
static expr arrow(expr const & d, expr const & r) { return pi("_", d, r); }
static expr zero() {
    // fun (t : T) (s : t -> t) (z : t) z
    return lam(t(), lam(arrow(v(0), v(0)), lam(v(1), v(0))));
}
static expr one()  {
    // fun (t : T) (s : t -> t) s
    return lam(t(), lam(arrow(v(0), v(0)), v(0)));
}
static expr num() { return constant("num"); }
static expr plus() {
    //  fun (m n : numeral) (A : Type 0) (f : A -> A) (x : A) => m A f (n A f x).
    expr x = v(0), f = v(1), A = v(2), n = v(3), m = v(4);
    expr body = m(A, f, n(A, f, x));
    return lam(num(), lam(num(), lam(t(), lam(arrow(v(0), v(0)), lam(v(1), body)))));
}
static expr two() { return app(plus(), one(), one()); }
static expr three() { return app(plus(), two(), one()); }
static expr four() { return app(plus(), two(), two()); }
static expr times() {
    // fun (m n : numeral) (A : Type 0) (f : A -> A) (x : A) => m A (n A f) x.
    expr x = v(0), f = v(1), A = v(2), n = v(3), m = v(4);
    expr body = m(A, n(A, f), x);
    return lam(num(), lam(num(), lam(t(), lam(arrow(v(0), v(0)), lam(v(1), body)))));
}
static expr power() {
    // fun (m n : numeral) (A : Type 0) => m (A -> A) (n A).
    expr A = v(0), n = v(1), m = v(2);
    expr body = n(arrow(A, A), m(A));
    return lam(num(), lam(num(), lam(arrow(v(0), v(0)), body)));
}

unsigned count_core(expr const & a, expr_set & s) {
    if (s.find(a) != s.end())
        return 0;
    s.insert(a);
    switch (a.kind()) {
    case expr_kind::Var: case expr_kind::Constant: case expr_kind::Prop: case expr_kind::Type: case expr_kind::Numeral:
        return 1;
    case expr_kind::App:
        return std::accumulate(begin_args(a), end_args(a), 1,
                               [&](unsigned sum, expr const & arg){ return sum + count_core(arg, s); });
    case expr_kind::Lambda: case expr_kind::Pi:
        return count_core(abst_type(a), s) + count_core(abst_body(a), s) + 1;
    }
    return 0;
}

unsigned count(expr const & a) {
    expr_set s;
    return count_core(a, s);
}

static void tst_church_numbers() {
    expr N = constant("N");
    expr z = constant("z");
    expr s = constant("s");
    std::cout << normalize(app(zero(), N, s, z)) << "\n";
    std::cout << normalize(app(one(), N, s, z))  << "\n";
    std::cout << normalize(app(two(), N, s, z)) << "\n";
    std::cout << normalize(app(four(), N, s, z)) << "\n";
    std::cout << count(normalize(app(four(), N, s, z))) << "\n";
    lean_assert(count(normalize(app(four(), N, s, z))) == 4 + 2);
    std::cout << normalize(app(app(times(), four(), four()), N, s, z)) << "\n";
    std::cout << normalize(app(app(power(), two(), four()), N, s, z)) << "\n";
    lean_assert(count(normalize(app(app(power(), two(), four()), N, s, z))) == 16 + 2);
    std::cout << normalize(app(app(times(), two(), app(power(), two(), four())), N, s, z)) << "\n";
    std::cout << count(normalize(app(app(times(), two(), app(power(), two(), four())), N, s, z))) << "\n";
    std::cout << count(normalize(app(app(times(), four(), app(power(), two(), four())), N, s, z))) << "\n";
    lean_assert(count(normalize(app(app(times(), four(), app(power(), two(), four())), N, s, z))) == 64 + 2);
    expr big = normalize(app(app(power(), two(), app(power(), two(), three())), N, s, z));
    std::cout << count(big) << "\n";
    lean_assert(count(big) == 256 + 2);
    // expr three = app(plus(), two(), one());
    // lean_assert(count(normalize(app(app(power(), three, three), N, s, z))) == 27 + 2);
    // expr big = normalize(app(app(power(), two(), app(times(), app(plus(), four(), one()), four())), N, s, z));
    // std::cout << count(big) << "\n";
    std::cout << normalize(lam(lam(app(app(times(), four(), four()), N, var(0), z)))) << "\n";
}

static void tst1() {
    expr f = constant("f");
    expr a = constant("a");
    expr b = constant("b");
    expr x = var(0);
    expr y = var(1);
    expr t = prop();
    eval(app(lambda("x", t, x), a));
    eval(app(lambda("x", t, x), a, b));
    eval(lambda("x", t, f(x)));
    eval(lambda("y", t, lambda("x", t, f(y, x))));
    eval(app(lambda("x", t,
                    app(lambda("f", t,
                               app(var(0), b)),
                        lambda("g", t, f(var(1))))),
                 a));
    expr l01 = lam(v(0)(v(1)));
    expr l12 = lam(lam(v(1)(v(2))));
    eval(lam(l12(l01)));
    lean_assert(normalize(lam(l12(l01))) == lam(lam(v(1)(v(1)))));
}

int main() {
    continue_on_violation(true);
    tst1();
    tst_church_numbers();
    return has_violations() ? 1 : 0;
}
