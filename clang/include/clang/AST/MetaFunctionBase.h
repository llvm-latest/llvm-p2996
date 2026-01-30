//===-- MetaFunctionBase.h - Prereqs for defining MetaFunction - *- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_AST_METAFUNCTION_BASE_H
#define LLVM_CLANG_AST_METAFUNCTION_BASE_H

#include "clang/AST/APValue.h"

namespace clang {

// Must match the values in `<meta>` header.
// This enum is mainly used for easier debugging.
enum class MetaFunctionID : std::uint8_t {
  invalid = 0,

  // non-exposed metafunctions
  get_begin_enumerator_decl_of = 1,
  get_next_enumerator_decl_of = 2,
  get_ith_base_of = 3,
  get_ith_template_argument_of = 4,
  get_begin_member_decl_of = 5,
  get_next_member_decl_of = 6,
  is_structural_type = 7,
  map_decl_to_entity = 8,

  // P2996 metafunctions
  identifier_of = 11,
  has_identifier = 12,
  operator_of = 13,
  source_location_of = 14,
  type_of = 15,
  parent_of = 16,
  underlying_entity_of = 17,
  proxied_entity_of = 18,
  object_of = 19,
  constant_of = 20,
  template_of = 21,
  substitute = 22,
  extract = 23,
  is_public = 24,
  is_protected = 25,
  is_private = 26,
  is_virtual = 27,
  is_pure_virtual = 28,
  is_override = 29,
  is_deleted = 30,
  is_defaulted = 31,
  is_explicit = 32,
  is_noexcept = 33,
  is_bit_field = 34,
  is_enumerator = 35,
  is_final = 36,
  is_const = 37,
  is_volatile = 38,
  is_mutable_member = 39,
  is_lvalue_reference_qualified = 40,
  is_rvalue_reference_qualified = 41,
  has_static_storage_duration = 42,
  has_thread_storage_duration = 43,
  has_automatic_storage_duration = 44,
  has_internal_linkage = 45,
  has_module_linkage = 46,
  has_external_linkage = 47,
  has_linkage = 48,
  is_class_member = 49,
  is_namespace_member = 50,
  is_nonstatic_data_member = 51,
  is_static_member = 52,
  is_base = 53,
  is_data_member_spec = 54,
  is_namespace = 55,
  is_function = 56,
  is_variable = 57,
  is_type = 58,
  is_alias = 59,
  is_entity_proxy = 60,
  is_complete_type = 61,
  has_complete_definition = 62,
  is_enumerable_type = 63,
  is_template = 64,
  is_function_template = 65,
  is_variable_template = 66,
  is_class_template = 67,
  is_alias_template = 68,
  is_conversion_function_template = 69,
  is_operator_function_template = 70,
  is_literal_operator_template = 71,
  is_constructor_template = 72,
  is_concept = 73,
  is_structured_binding = 74,
  is_value = 75,
  is_object = 76,
  has_template_arguments = 77,
  has_default_member_initializer = 78,
  is_conversion_function = 79,
  is_operator_function = 80,
  is_literal_operator = 81,
  is_constructor = 82,
  is_default_constructor = 83,
  is_copy_constructor = 84,
  is_move_constructor = 85,
  is_assignment = 86,
  is_copy_assignment = 87,
  is_move_assignment = 88,
  is_destructor = 89,
  is_special_member_function = 90,
  is_user_provided = 91,
  is_user_declared = 92,
  reflect_result = 93,
  data_member_spec = 94,
  define_aggregate = 95,
  offset_of = 96,
  size_of = 97,
  bit_offset_of = 98,
  bit_size_of = 99,
  alignment_of = 100,

  // P3096 parameter reflection metafunctions
  get_ith_parameter_of = 111,
  has_ellipsis_parameter = 112,
  has_default_argument = 113,
  is_explicit_object_parameter = 114,
  is_function_parameter = 115,
  return_type_of = 116,
  variable_of = 117,

  // P3394 annotation metafunctions
  get_ith_annotation_of = 121,
  is_annotation = 122,
  annotate = 123,

  // P3493 accessibility metafunctions
  current_access_context = 126,
  is_accessible = 127,

  // Other bespoke functions (not proposed at this time)
  is_access_specified = 131,
  reflect_invoke = 132,

  sentinel = 133,
};

class MetaActions;
class Expr;
class PartialDiagnostic;

// Type of callback provided to executing metafunctions to help evaluate an
// expression in the current constant evaluation context.
using MetaFunctionEvaluateFn =
    llvm::function_ref<bool(APValue &, const Expr *, bool ConvertToRValue)>;

// Type of callback provided to report a diagnostic to the evaluation context.
using MetaFunctionDiagnoseFn =
    llvm::function_ref<PartialDiagnostic &(SourceLocation, unsigned)>;

struct MetaFunctionEvalContext {
  APValue *Result = nullptr;
  ASTContext *C = nullptr;
  MetaActions *Meta = nullptr;
  Decl *ContainingDecl = nullptr;
  MetaFunctionEvaluateFn Evaluator;
  MetaFunctionDiagnoseFn Diagnoser;
  QualType ResultTy;
  SourceRange Range;
  ArrayRef<Expr *> Args;
  bool AllowInjection = false;

  MetaFunctionEvalContext(APValue *Result, ASTContext *C, MetaActions *Meta,
                          MetaFunctionEvaluateFn Evaluator,
                          MetaFunctionDiagnoseFn Diagnoser, bool AllowInjection,
                          QualType ResultTy, SourceRange Range,
                          ArrayRef<Expr *> Args, Decl *ContainingDecl)
      : Result(Result), C(C), Meta(Meta), ContainingDecl(ContainingDecl),
        Evaluator(std::move(Evaluator)), Diagnoser(std::move(Diagnoser)),
        ResultTy(std::move(ResultTy)), Range(std::move(Range)),
        Args(std::move(Args)), AllowInjection(AllowInjection) {}

  auto getScratchContextCreator() const {
    return [this, ScratchValue = APValue(),
            SwallowDiags = MetaFunctionDiagnoseFn()]() mutable {
      return MetaFunctionEvalContext(&ScratchValue, C, Meta, Evaluator,
                                     SwallowDiags, AllowInjection, ResultTy,
                                     Range, Args, ContainingDecl);
    };
  }
};

bool MetaFunctionImplPrototype(const MetaFunctionEvalContext &EvalCtx);
using MetaFunctionImplSignature = decltype(MetaFunctionImplPrototype);

} // namespace clang

#endif
