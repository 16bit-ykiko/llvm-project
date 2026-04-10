// RUN: %clang_cc1 -std=c++20 -ast-dump %s | FileCheck %s
// RUN: %clang_cc1 -std=c++20 -ast-dump %s | FileCheck %s --check-prefix=MEMBER

// Verify that explicit instantiation of function/variable/class templates
// preserves NNS and template argument source locations. Each explicit
// instantiation statement gets its own node in the redeclaration chain.

namespace test {
  template <typename T> void foo(T x, int y) {}
  template <typename T, typename U> void multi(T a, U b) {}
  template <typename T> T bar = T{};
  template <typename T> struct Baz {
    T value;
    void method() {}
    static T sval;
    struct Inner { T inner_val; };
    template <typename U> void tmpl_method(U u) {}
  };
  template <typename T> T Baz<T>::sval = T{};
}

// ==========================================================================
// Function template: single explicit instantiation definition with NNS
// ==========================================================================

template void test::foo<int>(int a, int b);
// CHECK: FunctionDecl {{.*}} prev {{.*}} <line:[[@LINE-1]]:1, col:21> col:21 foo 'void (int, int)' explicit_instantiation_definition
// CHECK: NestedNameSpecifier Namespace {{.*}} 'test'
// CHECK: TemplateArgument <col:25> type 'int'
// CHECK: ParmVarDecl {{.*}} <col:30, col:34> col:34 a 'int'
// CHECK: ParmVarDecl {{.*}} <col:37, col:41> col:41 b 'int'

// ==========================================================================
// Function template: extern declaration + definition in same file
// ==========================================================================

extern template void test::foo<float>(float c, int d);
// CHECK: FunctionDecl {{.*}} prev {{.*}} <line:[[@LINE-1]]:1, col:28> col:28 foo 'void (float, int)' explicit_instantiation_declaration
// CHECK: NestedNameSpecifier Namespace {{.*}} 'test'
// CHECK: TemplateArgument <col:32> type 'float'
// CHECK: ParmVarDecl {{.*}} <col:39, col:45> col:45 c 'float'
// CHECK: ParmVarDecl {{.*}} <col:48, col:52> col:52 d 'int'

template void test::foo<float>(float e, int f);
// CHECK: FunctionDecl {{.*}} prev {{.*}} <line:[[@LINE-1]]:1, col:21> col:21 foo 'void (float, int)' explicit_instantiation_definition
// CHECK: NestedNameSpecifier Namespace {{.*}} 'test'
// CHECK: TemplateArgument <col:25> type 'float'
// CHECK: ParmVarDecl {{.*}} <col:32, col:38> col:38 e 'float'
// CHECK: ParmVarDecl {{.*}} <col:41, col:45> col:45 f 'int'

// ==========================================================================
// Function template: implicit instantiation before explicit instantiation
// ==========================================================================

inline void use_foo() { test::foo<double>(1.0, 1); }
template void test::foo<double>(double g, int h);
// CHECK: FunctionDecl {{.*}} prev {{.*}} <line:[[@LINE-1]]:1, col:21> col:21 {{.*}}foo 'void (double, int)' explicit_instantiation_definition
// CHECK: NestedNameSpecifier Namespace {{.*}} 'test'
// CHECK: TemplateArgument <col:25> type 'double'
// CHECK: ParmVarDecl {{.*}} <col:33, col:40> col:40 g 'double'
// CHECK: ParmVarDecl {{.*}} <col:43, col:47> col:47 h 'int'

// ==========================================================================
// Function template: deduced template arguments (no explicit <T>)
// ==========================================================================

template void test::foo(short i, int j);
// CHECK: FunctionDecl {{.*}} prev {{.*}} <line:[[@LINE-1]]:1, col:21> col:21 foo 'void (short, int)' explicit_instantiation_definition
// CHECK: NestedNameSpecifier Namespace {{.*}} 'test'
// CHECK: ParmVarDecl {{.*}} <col:25, col:31> col:31 i 'short'
// CHECK: ParmVarDecl {{.*}} <col:34, col:38> col:38 j 'int'

// ==========================================================================
// Function template: multiple template parameters
// ==========================================================================

template void test::multi<int, double>(int k, double l);
// CHECK: FunctionDecl {{.*}} prev {{.*}} <line:[[@LINE-1]]:1, col:21> col:21 multi 'void (int, double)' explicit_instantiation_definition
// CHECK: NestedNameSpecifier Namespace {{.*}} 'test'
// CHECK: TemplateArgument <col:27> type 'int'
// CHECK: TemplateArgument <col:32> type 'double'
// CHECK: ParmVarDecl {{.*}} <col:40, col:44> col:44 k 'int'
// CHECK: ParmVarDecl {{.*}} <col:47, col:54> col:54 l 'double'

// ==========================================================================
// Function template: no NNS (same namespace)
// ==========================================================================

namespace test {
  template void foo<char>(char m, int n);
}
// CHECK: FunctionDecl {{.*}} prev {{.*}} <line:[[@LINE-2]]:3, col:17> col:17 foo 'void (char, int)' explicit_instantiation_definition
// CHECK: TemplateArgument <col:21> type 'char'
// CHECK: ParmVarDecl {{.*}} <col:27, col:32> col:32 m 'char'
// CHECK: ParmVarDecl {{.*}} <col:35, col:39> col:39 n 'int'

// ==========================================================================
// Variable template
// ==========================================================================

template int test::bar<int>;
// CHECK: VarTemplateSpecializationDecl {{.*}} <line:[[@LINE-1]]:1, {{.*}}> {{.*}} bar 'int' explicit_instantiation_definition
// CHECK: NestedNameSpecifier Namespace {{.*}} 'test'
// CHECK: TemplateArgument <line:[[@LINE-3]]:24> type 'int'

extern template float test::bar<float>;
// CHECK: VarTemplateSpecializationDecl {{.*}} <line:[[@LINE-1]]:1, {{.*}}> {{.*}} bar 'float' explicit_instantiation_declaration
// CHECK: NestedNameSpecifier Namespace {{.*}} 'test'
// CHECK: TemplateArgument <line:[[@LINE-3]]:33> type 'float'

// ==========================================================================
// Class template
// ==========================================================================

template struct test::Baz<int>;
// CHECK: ClassTemplateSpecializationDecl {{.*}} <line:[[@LINE-1]]:1, {{.*}}> {{.*}} struct Baz definition {{.*}} explicit_instantiation_definition
// CHECK: NestedNameSpecifier Namespace {{.*}} 'test'
// CHECK: TemplateArgument <col:27> type 'int'

extern template struct test::Baz<float>;
// CHECK: ClassTemplateSpecializationDecl {{.*}} <line:[[@LINE-1]]:1, {{.*}}> {{.*}} struct Baz definition {{.*}} explicit_instantiation_declaration
// CHECK: NestedNameSpecifier Namespace {{.*}} 'test'
// CHECK: TemplateArgument <col:34> type 'float'

// ==========================================================================
// Member function of class template specialization
// ==========================================================================

template void test::Baz<long>::method();
// MEMBER: CXXMethodDecl {{.*}} method 'void ()'{{.*}}explicit_instantiation_definition
// MEMBER: NestedNameSpecifier TypeSpec 'test::Baz<long>'

// ==========================================================================
// Static data member of class template specialization
// ==========================================================================

template long test::Baz<long>::sval;
// MEMBER: VarDecl {{.*}} sval 'long' explicit_instantiation_definition
// MEMBER: NestedNameSpecifier TypeSpec {{.*}}'test::Baz<long>'

// ==========================================================================
// Member function template of class template specialization
// ==========================================================================

template void test::Baz<long>::tmpl_method<double>(double);
// MEMBER: FunctionDecl {{.*}} prev {{.*}} <line:[[@LINE-1]]:1, col:32> col:32 tmpl_method 'void (double)' explicit_instantiation_definition
// MEMBER: NestedNameSpecifier TypeSpec 'test::Baz<long>'
// MEMBER: TemplateArgument <col:44> type 'double'

// ==========================================================================
// Nested class of class template specialization
// (clang does not yet create a dedicated AST node for this; see FIXME in
// SemaTemplate.cpp. We verify the code compiles without errors.)
// ==========================================================================

template struct test::Baz<short>::Inner;
