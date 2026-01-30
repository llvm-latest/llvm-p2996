//===-- ExprConstantMeta.cpp - Functions targeting reflections --*- C++ -*-===//
//
// Copyright 2025 Bloomberg Finance L.P.
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
//  This file implements all metafunctions from the <experimental/meta> header.
//
//===----------------------------------------------------------------------===//

#include "clang/AST/APValue.h"
#include "clang/AST/Attr.h"
#include "clang/AST/CXXInheritance.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/DeclGroup.h"
#include "clang/AST/DeclTemplate.h"
#include "clang/AST/Expr.h"
#include "clang/AST/MetaFunction.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/Reflection.h"
#include "clang/Basic/DiagnosticMetafn.h"
#include "clang/Basic/DiagnosticSema.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/Preprocessor.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/ADT/TypeSwitch.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"

namespace clang {
namespace {

using EvalFn = MetaFunctionEvaluateFn;
using DiagFn = MetaFunctionDiagnoseFn;

// -----------------------------------------------------------------------------
// P2996 Metafunction declarations
// -----------------------------------------------------------------------------

#pragma region Metafunction Declarations

bool get_begin_enumerator_decl_of(const MetaFunctionEvalContext &EvalCtx);
bool get_next_enumerator_decl_of(const MetaFunctionEvalContext &EvalCtx);
bool get_ith_base_of(const MetaFunctionEvalContext &EvalCtx);
bool get_ith_template_argument_of(const MetaFunctionEvalContext &EvalCtx);
bool get_begin_member_decl_of(const MetaFunctionEvalContext &EvalCtx);
bool get_next_member_decl_of(const MetaFunctionEvalContext &EvalCtx);
bool is_structural_type(const MetaFunctionEvalContext &EvalCtx);
bool map_decl_to_entity(const MetaFunctionEvalContext &EvalCtx);
bool identifier_of(const MetaFunctionEvalContext &EvalCtx);
bool has_identifier(const MetaFunctionEvalContext &EvalCtx);
bool operator_of(const MetaFunctionEvalContext &EvalCtx);
bool source_location_of(const MetaFunctionEvalContext &EvalCtx);
bool type_of(const MetaFunctionEvalContext &EvalCtx);
bool parent_of(const MetaFunctionEvalContext &EvalCtx);
bool underlying_entity_of(const MetaFunctionEvalContext &EvalCtx);
bool proxied_entity_of(const MetaFunctionEvalContext &EvalCtx);
bool constant_of(const MetaFunctionEvalContext &EvalCtx);
bool object_of(const MetaFunctionEvalContext &EvalCtx);
bool template_of(const MetaFunctionEvalContext &EvalCtx);
bool substitute(const MetaFunctionEvalContext &EvalCtx);
bool extract(const MetaFunctionEvalContext &EvalCtx);
bool is_public(const MetaFunctionEvalContext &EvalCtx);
bool is_protected(const MetaFunctionEvalContext &EvalCtx);
bool is_private(const MetaFunctionEvalContext &EvalCtx);
bool is_virtual(const MetaFunctionEvalContext &EvalCtx);
bool is_pure_virtual(const MetaFunctionEvalContext &EvalCtx);
bool is_override(const MetaFunctionEvalContext &EvalCtx);
bool is_deleted(const MetaFunctionEvalContext &EvalCtx);
bool is_defaulted(const MetaFunctionEvalContext &EvalCtx);
bool is_explicit(const MetaFunctionEvalContext &EvalCtx);
bool is_noexcept(const MetaFunctionEvalContext &EvalCtx);
bool is_bit_field(const MetaFunctionEvalContext &EvalCtx);
bool is_enumerator(const MetaFunctionEvalContext &EvalCtx);
bool is_final(const MetaFunctionEvalContext &EvalCtx);
bool is_const(const MetaFunctionEvalContext &EvalCtx);
bool is_volatile(const MetaFunctionEvalContext &EvalCtx);
bool is_mutable_member(const MetaFunctionEvalContext &EvalCtx);
bool is_lvalue_reference_qualified(const MetaFunctionEvalContext &EvalCtx);
bool is_rvalue_reference_qualified(const MetaFunctionEvalContext &EvalCtx);
bool has_static_storage_duration(const MetaFunctionEvalContext &EvalCtx);
bool has_thread_storage_duration(const MetaFunctionEvalContext &EvalCtx);
bool has_automatic_storage_duration(const MetaFunctionEvalContext &EvalCtx);
bool has_internal_linkage(const MetaFunctionEvalContext &EvalCtx);
bool has_module_linkage(const MetaFunctionEvalContext &EvalCtx);
bool has_external_linkage(const MetaFunctionEvalContext &EvalCtx);
bool has_linkage(const MetaFunctionEvalContext &EvalCtx);
bool is_class_member(const MetaFunctionEvalContext &EvalCtx);
bool is_namespace_member(const MetaFunctionEvalContext &EvalCtx);
bool is_nonstatic_data_member(const MetaFunctionEvalContext &EvalCtx);
bool is_static_member(const MetaFunctionEvalContext &EvalCtx);
bool is_base(const MetaFunctionEvalContext &EvalCtx);
bool is_data_member_spec(const MetaFunctionEvalContext &EvalCtx);
bool is_namespace(const MetaFunctionEvalContext &EvalCtx);
bool is_function(const MetaFunctionEvalContext &EvalCtx);
bool is_variable(const MetaFunctionEvalContext &EvalCtx);
bool is_type(const MetaFunctionEvalContext &EvalCtx);
bool is_alias(const MetaFunctionEvalContext &EvalCtx);
bool is_entity_proxy(const MetaFunctionEvalContext &EvalCtx);
bool is_complete_type(const MetaFunctionEvalContext &EvalCtx);
bool has_complete_definition(const MetaFunctionEvalContext &EvalCtx);
bool is_enumerable_type(const MetaFunctionEvalContext &EvalCtx);
bool is_template(const MetaFunctionEvalContext &EvalCtx);
bool is_function_template(const MetaFunctionEvalContext &EvalCtx);
bool is_variable_template(const MetaFunctionEvalContext &EvalCtx);
bool is_class_template(const MetaFunctionEvalContext &EvalCtx);
bool is_alias_template(const MetaFunctionEvalContext &EvalCtx);
bool is_conversion_function_template(const MetaFunctionEvalContext &EvalCtx);
bool is_operator_function_template(const MetaFunctionEvalContext &EvalCtx);
bool is_literal_operator_template(const MetaFunctionEvalContext &EvalCtx);
bool is_constructor_template(const MetaFunctionEvalContext &EvalCtx);
bool is_concept(const MetaFunctionEvalContext &EvalCtx);
bool is_structured_binding(const MetaFunctionEvalContext &EvalCtx);
bool is_value(const MetaFunctionEvalContext &EvalCtx);
bool is_object(const MetaFunctionEvalContext &EvalCtx);
bool has_template_arguments(const MetaFunctionEvalContext &EvalCtx);
bool has_default_member_initializer(const MetaFunctionEvalContext &EvalCtx);
bool is_conversion_function(const MetaFunctionEvalContext &EvalCtx);
bool is_operator_function(const MetaFunctionEvalContext &EvalCtx);
bool is_literal_operator(const MetaFunctionEvalContext &EvalCtx);
bool is_constructor(const MetaFunctionEvalContext &EvalCtx);
bool is_default_constructor(const MetaFunctionEvalContext &EvalCtx);
bool is_copy_constructor(const MetaFunctionEvalContext &EvalCtx);
bool is_move_constructor(const MetaFunctionEvalContext &EvalCtx);
bool is_assignment(const MetaFunctionEvalContext &EvalCtx);
bool is_copy_assignment(const MetaFunctionEvalContext &EvalCtx);
bool is_move_assignment(const MetaFunctionEvalContext &EvalCtx);
bool is_destructor(const MetaFunctionEvalContext &EvalCtx);
bool is_special_member_function(const MetaFunctionEvalContext &EvalCtx);
bool is_user_provided(const MetaFunctionEvalContext &EvalCtx);
bool is_user_declared(const MetaFunctionEvalContext &EvalCtx);
bool reflect_result(const MetaFunctionEvalContext &EvalCtx);
bool data_member_spec(const MetaFunctionEvalContext &EvalCtx);
bool define_aggregate(const MetaFunctionEvalContext &EvalCtx);
bool offset_of(const MetaFunctionEvalContext &EvalCtx);
bool size_of(const MetaFunctionEvalContext &EvalCtx);
bool bit_offset_of(const MetaFunctionEvalContext &EvalCtx);
bool bit_size_of(const MetaFunctionEvalContext &EvalCtx);
bool alignment_of(const MetaFunctionEvalContext &EvalCtx);

// -----------------------------------------------------------------------------
// P3096 Metafunction declarations
// -----------------------------------------------------------------------------

bool get_ith_parameter_of(const MetaFunctionEvalContext &EvalCtx);
bool has_ellipsis_parameter(const MetaFunctionEvalContext &EvalCtx);
bool has_default_argument(const MetaFunctionEvalContext &EvalCtx);
bool is_explicit_object_parameter(const MetaFunctionEvalContext &EvalCtx);
bool is_function_parameter(const MetaFunctionEvalContext &EvalCtx);
bool return_type_of(const MetaFunctionEvalContext &EvalCtx);
bool variable_of(const MetaFunctionEvalContext &EvalCtx);
bool get_ith_annotation_of(const MetaFunctionEvalContext &EvalCtx);
bool is_annotation(const MetaFunctionEvalContext &EvalCtx);
bool annotate(const MetaFunctionEvalContext &EvalCtx);

// =========================
// Accessibility API (P3493)
// =========================

bool current_access_context(const MetaFunctionEvalContext &EvalCtx);
bool is_accessible(const MetaFunctionEvalContext &EvalCtx);

// ===================================================
// Other bespoke functions (not proposed at this time)
// ===================================================

bool is_access_specified(const MetaFunctionEvalContext &EvalCtx);
bool reflect_invoke(const MetaFunctionEvalContext &EvalCtx);

// -----------------------------------------------------------------------------
// Metafunction table
//
// Order of entries MUST be kept in sync with order of declarations in the
//   <experimental/meta>
// header file.
// -----------------------------------------------------------------------------

#define SEMA_METAFUNCTION(Kind, MinArgs, MaxArgs, Impl)                        \
  {MetaFunction::ResultKind::Kind, MinArgs, MaxArgs, MetaFunctionID::Impl, Impl}
#define SEMA_METAFUNCTION_PLACEHOLDER(Id)                                      \
  {MetaFunction::ResultKind::MaxNum, 0, 0, MetaFunctionID::invalid, nullptr}

constexpr MetaFunction Metafunctions[] = {
    // Kind, MinArgs, MaxArgs, Impl

    SEMA_METAFUNCTION_PLACEHOLDER(0),

    // non-exposed metafunctions
    SEMA_METAFUNCTION(MetaInfo, 2, 2, get_begin_enumerator_decl_of),
    SEMA_METAFUNCTION(MetaInfo, 2, 2, get_next_enumerator_decl_of),
    SEMA_METAFUNCTION(MetaInfo, 3, 3, get_ith_base_of),
    SEMA_METAFUNCTION(MetaInfo, 3, 3, get_ith_template_argument_of),
    SEMA_METAFUNCTION(MetaInfo, 2, 2, get_begin_member_decl_of),
    SEMA_METAFUNCTION(MetaInfo, 2, 2, get_next_member_decl_of),
    SEMA_METAFUNCTION(Bool, 1, 1, is_structural_type),
    SEMA_METAFUNCTION(MetaInfo, 1, 1, map_decl_to_entity),

    SEMA_METAFUNCTION_PLACEHOLDER(9),
    SEMA_METAFUNCTION_PLACEHOLDER(10),

    // exposed metafunctions
    SEMA_METAFUNCTION(SpliceFromArg, 4, 4, identifier_of),
    SEMA_METAFUNCTION(Bool, 1, 1, has_identifier),
    SEMA_METAFUNCTION(Size, 1, 1, operator_of),
    SEMA_METAFUNCTION(SourceLoc, 1, 1, source_location_of),
    SEMA_METAFUNCTION(MetaInfo, 1, 1, type_of),
    SEMA_METAFUNCTION(MetaInfo, 1, 1, parent_of),
    SEMA_METAFUNCTION(MetaInfo, 1, 1, underlying_entity_of),
    SEMA_METAFUNCTION(MetaInfo, 1, 1, proxied_entity_of),
    SEMA_METAFUNCTION(MetaInfo, 1, 1, object_of),
    SEMA_METAFUNCTION(MetaInfo, 1, 1, constant_of),
    SEMA_METAFUNCTION(MetaInfo, 1, 1, template_of),
    SEMA_METAFUNCTION(MetaInfo, 4, 4, substitute),
    SEMA_METAFUNCTION(SpliceFromArg, 2, 2, extract),
    SEMA_METAFUNCTION(Bool, 1, 1, is_public),
    SEMA_METAFUNCTION(Bool, 1, 1, is_protected),
    SEMA_METAFUNCTION(Bool, 1, 1, is_private),
    SEMA_METAFUNCTION(Bool, 1, 1, is_virtual),
    SEMA_METAFUNCTION(Bool, 1, 1, is_pure_virtual),
    SEMA_METAFUNCTION(Bool, 1, 1, is_override),
    SEMA_METAFUNCTION(Bool, 1, 1, is_deleted),
    SEMA_METAFUNCTION(Bool, 1, 1, is_defaulted),
    SEMA_METAFUNCTION(Bool, 1, 1, is_explicit),
    SEMA_METAFUNCTION(Bool, 1, 1, is_noexcept),
    SEMA_METAFUNCTION(Bool, 1, 1, is_bit_field),
    SEMA_METAFUNCTION(Bool, 1, 1, is_enumerator),
    SEMA_METAFUNCTION(Bool, 1, 1, is_final),
    SEMA_METAFUNCTION(Bool, 1, 1, is_const),
    SEMA_METAFUNCTION(Bool, 1, 1, is_volatile),
    SEMA_METAFUNCTION(Bool, 1, 1, is_mutable_member),
    SEMA_METAFUNCTION(Bool, 1, 1, is_lvalue_reference_qualified),
    SEMA_METAFUNCTION(Bool, 1, 1, is_rvalue_reference_qualified),
    SEMA_METAFUNCTION(Bool, 1, 1, has_static_storage_duration),
    SEMA_METAFUNCTION(Bool, 1, 1, has_thread_storage_duration),
    SEMA_METAFUNCTION(Bool, 1, 1, has_automatic_storage_duration),
    SEMA_METAFUNCTION(Bool, 1, 1, has_internal_linkage),
    SEMA_METAFUNCTION(Bool, 1, 1, has_module_linkage),
    SEMA_METAFUNCTION(Bool, 1, 1, has_external_linkage),
    SEMA_METAFUNCTION(Bool, 1, 1, has_linkage),
    SEMA_METAFUNCTION(Bool, 1, 1, is_class_member),
    SEMA_METAFUNCTION(Bool, 1, 1, is_namespace_member),
    SEMA_METAFUNCTION(Bool, 1, 1, is_nonstatic_data_member),
    SEMA_METAFUNCTION(Bool, 1, 1, is_static_member),
    SEMA_METAFUNCTION(Bool, 1, 1, is_base),
    SEMA_METAFUNCTION(Bool, 1, 1, is_data_member_spec),
    SEMA_METAFUNCTION(Bool, 1, 1, is_namespace),
    SEMA_METAFUNCTION(Bool, 1, 1, is_function),
    SEMA_METAFUNCTION(Bool, 1, 1, is_variable),
    SEMA_METAFUNCTION(Bool, 1, 1, is_type),
    SEMA_METAFUNCTION(Bool, 1, 1, is_alias),
    SEMA_METAFUNCTION(Bool, 1, 1, is_entity_proxy),
    SEMA_METAFUNCTION(Bool, 1, 1, is_complete_type),
    SEMA_METAFUNCTION(Bool, 1, 1, has_complete_definition),
    SEMA_METAFUNCTION(Bool, 1, 1, is_enumerable_type),
    SEMA_METAFUNCTION(Bool, 1, 1, is_template),
    SEMA_METAFUNCTION(Bool, 1, 1, is_function_template),
    SEMA_METAFUNCTION(Bool, 1, 1, is_variable_template),
    SEMA_METAFUNCTION(Bool, 1, 1, is_class_template),
    SEMA_METAFUNCTION(Bool, 1, 1, is_alias_template),
    SEMA_METAFUNCTION(Bool, 1, 1, is_conversion_function_template),
    SEMA_METAFUNCTION(Bool, 1, 1, is_operator_function_template),
    SEMA_METAFUNCTION(Bool, 1, 1, is_literal_operator_template),
    SEMA_METAFUNCTION(Bool, 1, 1, is_constructor_template),
    SEMA_METAFUNCTION(Bool, 1, 1, is_concept),
    SEMA_METAFUNCTION(Bool, 1, 1, is_structured_binding),
    SEMA_METAFUNCTION(Bool, 1, 1, is_value),
    SEMA_METAFUNCTION(Bool, 1, 1, is_object),
    SEMA_METAFUNCTION(Bool, 1, 1, has_template_arguments),
    SEMA_METAFUNCTION(Bool, 1, 1, has_default_member_initializer),
    SEMA_METAFUNCTION(Bool, 1, 1, is_conversion_function),
    SEMA_METAFUNCTION(Bool, 1, 1, is_operator_function),
    SEMA_METAFUNCTION(Bool, 1, 1, is_literal_operator),
    SEMA_METAFUNCTION(Bool, 1, 1, is_constructor),
    SEMA_METAFUNCTION(Bool, 1, 1, is_default_constructor),
    SEMA_METAFUNCTION(Bool, 1, 1, is_copy_constructor),
    SEMA_METAFUNCTION(Bool, 1, 1, is_move_constructor),
    SEMA_METAFUNCTION(Bool, 1, 1, is_assignment),
    SEMA_METAFUNCTION(Bool, 1, 1, is_copy_assignment),
    SEMA_METAFUNCTION(Bool, 1, 1, is_move_assignment),
    SEMA_METAFUNCTION(Bool, 1, 1, is_destructor),
    SEMA_METAFUNCTION(Bool, 1, 1, is_special_member_function),
    SEMA_METAFUNCTION(Bool, 1, 1, is_user_provided),
    SEMA_METAFUNCTION(Bool, 1, 1, is_user_declared),
    SEMA_METAFUNCTION(MetaInfo, 2, 2, reflect_result),
    SEMA_METAFUNCTION(MetaInfo, 10, 10, data_member_spec),
    SEMA_METAFUNCTION(MetaInfo, 3, 3, define_aggregate),
    SEMA_METAFUNCTION(SpliceFromArg, 2, 2, offset_of),
    SEMA_METAFUNCTION(Size, 1, 1, size_of),
    SEMA_METAFUNCTION(SpliceFromArg, 2, 2, bit_offset_of),
    SEMA_METAFUNCTION(Size, 1, 1, bit_size_of),
    SEMA_METAFUNCTION(Size, 1, 1, alignment_of),

    SEMA_METAFUNCTION_PLACEHOLDER(101),
    SEMA_METAFUNCTION_PLACEHOLDER(102),
    SEMA_METAFUNCTION_PLACEHOLDER(103),
    SEMA_METAFUNCTION_PLACEHOLDER(104),
    SEMA_METAFUNCTION_PLACEHOLDER(105),
    SEMA_METAFUNCTION_PLACEHOLDER(106),
    SEMA_METAFUNCTION_PLACEHOLDER(107),
    SEMA_METAFUNCTION_PLACEHOLDER(108),
    SEMA_METAFUNCTION_PLACEHOLDER(109),
    SEMA_METAFUNCTION_PLACEHOLDER(110),

    // P3096 metafunction extensions
    SEMA_METAFUNCTION(MetaInfo, 3, 3, get_ith_parameter_of),
    SEMA_METAFUNCTION(Bool, 1, 1, has_ellipsis_parameter),
    SEMA_METAFUNCTION(Bool, 1, 1, has_default_argument),
    SEMA_METAFUNCTION(Bool, 1, 1, is_explicit_object_parameter),
    SEMA_METAFUNCTION(Bool, 1, 1, is_function_parameter),
    SEMA_METAFUNCTION(MetaInfo, 1, 1, return_type_of),
    SEMA_METAFUNCTION(MetaInfo, 1, 1, variable_of),

    SEMA_METAFUNCTION_PLACEHOLDER(118),
    SEMA_METAFUNCTION_PLACEHOLDER(119),
    SEMA_METAFUNCTION_PLACEHOLDER(120),

    // P3394 annotation metafunction extensions
    SEMA_METAFUNCTION(MetaInfo, 3, 3, get_ith_annotation_of),
    SEMA_METAFUNCTION(Bool, 1, 1, is_annotation),
    SEMA_METAFUNCTION(MetaInfo, 2, 2, annotate),

    SEMA_METAFUNCTION_PLACEHOLDER(124),
    SEMA_METAFUNCTION_PLACEHOLDER(125),

    // P3493 accessibility extensions
    SEMA_METAFUNCTION(MetaInfo, 0, 0, current_access_context),
    SEMA_METAFUNCTION(Bool, 3, 3, is_accessible),

    SEMA_METAFUNCTION_PLACEHOLDER(128),
    SEMA_METAFUNCTION_PLACEHOLDER(129),
    SEMA_METAFUNCTION_PLACEHOLDER(130),

    // Other bespoke functions (not proposed at this time)
    SEMA_METAFUNCTION(Bool, 1, 1, is_access_specified),
    SEMA_METAFUNCTION(MetaInfo, 5, 5, reflect_invoke),
};

#undef SEMA_METAFUNCTION_PLACEHOLDER
#undef SEMA_METAFUNCTION

constexpr std::size_t NumMetafunctions =
    static_cast<std::size_t>(MetaFunctionID::sentinel);
static_assert(NumMetafunctions == sizeof(Metafunctions) / sizeof(MetaFunction));
#pragma endregion

// -----------------------------------------------------------------------------
// Metafunction helper functions
// -----------------------------------------------------------------------------

#pragma region Metafunction Helper Functions
APValue makeBool(ASTContext &C, bool B) {
  return APValue(C.MakeIntValue(B, C.BoolTy));
}

APValue makeReflection(std::nullptr_t) {
  return {ReflectionKind::Null, nullptr};
}

APValue makeReflection(QualType QT) {
  return {ReflectionKind::Type, QT.getAsOpaquePtr()};
}

APValue makeReflection(Decl *D) {
  if (isa<NamespaceDecl>(D) || isa<NamespaceAliasDecl>(D) ||
      isa<TranslationUnitDecl>(D))
    return {ReflectionKind::Namespace, D};
  else if (isa<TemplateDecl>(D))
    return {ReflectionKind::Template, D};
  else if (isa<UsingShadowDecl>(D))
    return {ReflectionKind::EntityProxy, D};
  else if (isa<ParmVarDecl>(D))
    return {ReflectionKind::Parameter, D};

  return {ReflectionKind::Declaration, D};
}

APValue makeReflection(TemplateName TName) {
  return {ReflectionKind::Template, TName.getAsVoidPointer()};
}

APValue makeReflection(CXXBaseSpecifier *Base) {
  return {ReflectionKind::BaseSpecifier, Base};
}

APValue makeReflection(TagDataMemberSpec *TDMS) {
  return {ReflectionKind::DataMemberSpec, TDMS};
}

APValue makeReflection(CXX26AnnotationAttr *A) {
  return {ReflectionKind::Annotation, A};
}

Expr *makeStrLiteral(StringRef Str, ASTContext &C, bool Utf8) {
  QualType ConstCharTy = (Utf8 ? C.Char8Ty : C.CharTy).withConst();

  // Get the type for 'const char[Str.size()]'.
  QualType StrLitTy =
      C.getConstantArrayType(ConstCharTy, llvm::APInt(32, Str.size() + 1),
                             nullptr, ArraySizeModifier::Normal, 0);

  // Create a string literal having type 'const char [Str.size()]'.
  StringLiteralKind SLK =
      Utf8 ? StringLiteralKind::UTF8 : StringLiteralKind::Ordinary;
  return StringLiteral::Create(C, Str, SLK, false, StrLitTy, SourceLocation{});
}

const Type *getTypeForDecl(const Decl *D) {
  assert(D && "declaration is nullptr");
  return llvm::TypeSwitch<const Decl *, const Type *>(D)
      // this also covers CXXRecordDecl and ClassTemplateSpecializationDecl
      .Case<TagDecl>([&](const TagDecl *TD) {
        return TD->getASTContext().getCanonicalTagType(TD).getTypePtrOrNull();
      })
      .Case<TypeDecl>([](const TypeDecl *TD) { return TD->getTypeForDecl(); })
      .Case<VarDecl>(
          [](const VarDecl *VD) { return VD->getType().getTypePtrOrNull(); })
      .Default([](const Decl *) -> const Type * {
        llvm_unreachable("unhandled decl type");
      });
}

bool SetAndSucceed(APValue &Out, const APValue &Result) {
  Out = Result;
  return false;
}

bool SetBoolAndSucceed(const MetaFunctionEvalContext &EvalCtx, bool Result) {
  return SetAndSucceed(*EvalCtx.Result, makeBool(*EvalCtx.C, Result));
}

template <typename R>
bool SetReflectionAndSucceed(const MetaFunctionEvalContext &EvalCtx, R &&Refl) {
  return SetAndSucceed(*EvalCtx.Result, makeReflection(std::forward<R>(Refl)));
}
#pragma endregion

TemplateName findTemplateOfDecl(const Decl *D) {
  TemplateDecl *TDecl = nullptr;
  if (const auto *FD = dyn_cast<FunctionDecl>(D)) {
    if (FunctionTemplateSpecializationInfo *Info =
            FD->getTemplateSpecializationInfo())
      TDecl = Info->getTemplate();
  } else if (const auto *VD = dyn_cast<VarDecl>(D)) {
    if (const auto *P = VD->getTemplateInstantiationPattern())
      VD = P;
    TDecl = VD->getDescribedVarTemplate();
  }
  assert(!isa<ClassTemplateSpecializationDecl>(D) &&
         "use findTemplateOfType instead");
  return TDecl ? TemplateName(TDecl) : TemplateName();
}

TemplateName findTemplateOfType(QualType QT) {
  if (auto *TST = dyn_cast<TemplateSpecializationType>(QT)) {
    TemplateName TName = TST->getTemplateName();
    if (TName.getKind() == TemplateName::QualifiedTemplate)
      TName = TName.getAsQualifiedTemplateName()->getUnderlyingTemplate();
    return TName;
  }

  if (auto *CXXRD = QT->getAsCXXRecordDecl())
    if (auto *CTSD = dyn_cast<ClassTemplateSpecializationDecl>(CXXRD))
      return TemplateName(CTSD->getSpecializedTemplate());

  return {};
}

void getTemplateName(std::string &Result, ASTContext &C, TemplateName TName) {
  PrintingPolicy PP = C.getPrintingPolicy();
  {
    llvm::raw_string_ostream NameOut(Result);
    TName.print(NameOut, PP, TemplateName::Qualified::None);
  }
}

void getDeclName(std::string &Result, ASTContext &C, Decl *D) {
  if (TemplateName TName = findTemplateOfDecl(D); !TName.isNull())
    return getTemplateName(Result, C, TName);

  PrintingPolicy PP = C.getPrintingPolicy();
  {
    llvm::raw_string_ostream NameOut(Result);
    if (auto *ND = dyn_cast<NamedDecl>(D);
        ND && !isa<TemplateParamObjectDecl>(D))
      ND->printName(NameOut, PP);
  }
}

bool getParameterName(ParmVarDecl *PVD, std::string &Out) {
  // Parameters instantiated from function parameter packs are not considered
  // to have identifiers.
  if (auto STTPT = dyn_cast<SubstTemplateTypeParmType>(PVD->getType());
      STTPT && STTPT->getPackIndex())
    return true;

  unsigned ParamIdx = PVD->getFunctionScopeIndex();

  // TODO(P2996): This will crash if we're in the trailing requires-clause of
  // a function declaration, since the DeclContext is not the function but the
  // TranslationUnitDecl.
  FunctionDecl *FD = cast<FunctionDecl>(PVD->getDeclContext());
  FD = FD->getMostRecentDecl();
  PVD = FD->getParamDecl(ParamIdx);

  bool Consistent = true;
  StringRef FirstNameSeen = PVD->getName();

  while (PVD) {
    FD = cast<FunctionDecl>(PVD->getDeclContext());
    FD = FD->getPreviousDecl();
    if (!FD) {
      Out = FirstNameSeen;
      return true;
    }

    PVD = FD->getParamDecl(ParamIdx);
    assert(PVD);
    if (IdentifierInfo *II = PVD->getIdentifier()) {
      if (FirstNameSeen.empty()) {
        FirstNameSeen = II->getName();
      } else if (II->getName() != FirstNameSeen) {
        Consistent = false;
        break;
      }
    }
  }
  Out = FirstNameSeen;
  return Consistent;
}

ParmVarDecl *getMostRecentParmVarDecl(ParmVarDecl *PVD) {
  // TODO(P2996): This will crash if we're in the trailing requires-clause of
  // a function declaration, since the DeclContext is not the function but the
  // TranslationUnitDecl.
  FunctionDecl *FD = cast<FunctionDecl>(PVD->getDeclContext());
  FD = FD->getMostRecentDecl();
  return FD->getParamDecl(PVD->getFunctionScopeIndex());
}

#pragma region FindDecl Helpers
NamedDecl *findTypeDecl(QualType QT) {
  return llvm::TypeSwitch<const Type *, NamedDecl *>(QT.getTypePtr())
      .Case<TypedefType>([](auto *T) { return T->getDecl(); })
      .Case<UsingType>([](auto *T) { return T->getDecl(); })
      .Case<TagType>([](auto *T) { return T->getOriginalDecl(); })
      .Case<UnresolvedUsingType>([](auto *T) { return T->getDecl(); })
      .Case<SubstTemplateTypeParmType>(
          [](auto *T) { return findTypeDecl(T->getReplacementType()); })
      .Case<DecltypeType>(
          [](auto *T) { return findTypeDecl(T->getUnderlyingType()); })
      .Case<TemplateSpecializationType>([](auto *TST) -> NamedDecl * {
        auto *CTD = dyn_cast_or_null<ClassTemplateDecl>(
            TST->getTemplateName().getAsTemplateDecl());
        if (!CTD)
          return nullptr;
        void *InsertPos;
        return CTD->findSpecialization(TST->template_arguments(), InsertPos);
      })
      .Default([&](const Type *) -> NamedDecl * { return QT->getAsTagDecl(); });
}

bool findWhateverDeclLocWithLoc(const MetaFunctionEvalContext &EvalCtx,
                                SourceLocation BLoc, SourceLocation RParenLoc,
                                DeclContext *Context) {
  SourceLocExpr *SLE = new (*EvalCtx.C)
      SourceLocExpr(*EvalCtx.C, SourceLocIdentKind::SourceLocStruct,
                    EvalCtx.ResultTy, BLoc, RParenLoc, Context);
  return !EvalCtx.Evaluator(*EvalCtx.Result, SLE, true);
}

bool findWhateverDeclLoc(const MetaFunctionEvalContext &EvalCtx, Decl *D) {
  return findWhateverDeclLocWithLoc(
      EvalCtx, D ? D->getLocation() : SourceLocation(), SourceLocation(),
      D ? D->getDeclContext() : nullptr);
}

bool findTypeDeclLoc(const MetaFunctionEvalContext &EvalCtx, QualType QT) {
  // Get the type's declaration.
  NamedDecl *D = findTypeDecl(QT);
  return findWhateverDeclLoc(EvalCtx, D);
}

bool findDeclLoc(const MetaFunctionEvalContext &EvalCtx, Decl *D) {
  return findWhateverDeclLoc(EvalCtx, D);
}

bool findBaseSpecLoc(const MetaFunctionEvalContext &EvalCtx,
                     CXXBaseSpecifier *B) {
  return findWhateverDeclLocWithLoc(EvalCtx, B->getBeginLoc(), SourceLocation(),
                                    B->getDerived()->getDeclContext());
}

bool findAnnotLoc(const MetaFunctionEvalContext &EvalCtx,
                  CXX26AnnotationAttr *A) {
  return findWhateverDeclLocWithLoc(EvalCtx, A->getEqLoc(), SourceLocation(),
                                    nullptr);
}
#pragma endregion

#pragma region Elaborated Types & Type Aliases
QualType desugarType(QualType QT, bool UnwrapAliases, bool DropCV,
                     bool DropRefs) {
  bool IsConst = QT.isConstQualified();
  bool IsVolatile = QT.isVolatileQualified();

  while (true) {
    QT = QualType(QT.getTypePtr(), 0);
    if (auto *TDT = dyn_cast<TypedefType>(QT); TDT && UnwrapAliases)
      QT = TDT->desugar();
    else if (auto *UT = dyn_cast<UsingType>(QT); TDT && UnwrapAliases)
      QT = UT->desugar();
    else if (auto *TST = dyn_cast<TemplateSpecializationType>(QT);
             TST && UnwrapAliases && TST->isTypeAlias())
      QT = TST->getAliasedType();
    else if (auto *AT = dyn_cast<AutoType>(QT))
      QT = AT->desugar();
    else if (auto *RT = dyn_cast<ReferenceType>(QT); RT && DropRefs)
      QT = RT->getPointeeType();
    else if (auto *STTP = dyn_cast<SubstTemplateTypeParmType>(QT))
      QT = STTP->getReplacementType();
    else if (auto *RST = dyn_cast<ReflectionSpliceType>(QT))
      QT = RST->desugar();
    else
      break;
  }

  if (!DropCV) {
    if (IsConst)
      QT.addConst();
    if (IsVolatile)
      QT.addVolatile();
  }
  return QT;
}

bool isTypeAlias(QualType QT) {
  // If it's a TypedefType, it's an alias.
  return QT->isTypedefNameType();
}
#pragma endregion

#pragma region Template Helpers
void expandTemplateArgPacks(ArrayRef<TemplateArgument> Args,
                            SmallVectorImpl<TemplateArgument> &Out) {
  for (const TemplateArgument &Arg : Args)
    if (Arg.getKind() == TemplateArgument::Pack)
      for (const TemplateArgument &TA : Arg.getPackAsArray())
        Out.push_back(TA);
    else
      Out.push_back(Arg);
}

bool getTemplateArgumentsFromType(QualType QT,
                                  SmallVectorImpl<TemplateArgument> &Out) {
  // Obtain the template arguments from the Type* representation
  if (auto asTmplSpecialization = QT->getAs<TemplateSpecializationType>())
    expandTemplateArgPacks(asTmplSpecialization->template_arguments(), Out);
  else if (auto *CTSD = dyn_cast_or_null<ClassTemplateSpecializationDecl>(
               QT->getAsRecordDecl()))
    expandTemplateArgPacks(CTSD->getTemplateArgs().asArray(), Out);
  else
    return true;

  return false;
}

bool getTemplateArgumentsFromDecl(Decl *D,
                                  SmallVectorImpl<TemplateArgument> &Out) {
  if (auto FD = dyn_cast<FunctionDecl>(D)) {
    if (auto templArgs = FD->getTemplateSpecializationArgs()) {
      expandTemplateArgPacks(templArgs->asArray(), Out);
      return false;
    }
  } else if (auto VTSD = dyn_cast<VarTemplateSpecializationDecl>(D)) {
    expandTemplateArgPacks(VTSD->getTemplateArgs().asArray(), Out);
    return false;
  }
  return true;
}

APValue getNthTemplateArgument(ASTContext &C,
                               ArrayRef<TemplateArgument> templateArgs,
                               EvalFn Evaluator, APValue Sentinel, size_t Idx) {
  if (Idx >= templateArgs.size())
    return Sentinel;

  const auto &templArgument = templateArgs[Idx];
  switch (templArgument.getKind()) {
  case TemplateArgument::Type:
    return makeReflection(templArgument.getAsType());
  case TemplateArgument::Expression: {
    Expr *TExpr = templArgument.getAsExpr();

    APValue ArgResult;
    bool success = Evaluator(ArgResult, TExpr, !TExpr->isLValue());
    assert(success);

    return ArgResult.Lift(TExpr->getType());
  }
  case TemplateArgument::Template: {
    TemplateName TName = templArgument.getAsTemplate();
    if (TName.getKind() == TemplateName::QualifiedTemplate)
      TName = TName.getAsQualifiedTemplateName()->getUnderlyingTemplate();
    return makeReflection(TName);
  }
  case TemplateArgument::Declaration:
    return makeReflection(templArgument.getAsDecl());
  case TemplateArgument::NullPtr: {
    APValue NullPtrValue((ValueDecl *)nullptr,
                         CharUnits::fromQuantity(C.getTargetNullPointerValue(
                             templArgument.getNullPtrType())),
                         APValue::NoLValuePath(),
                         /*IsNullPtr=*/true);
    return NullPtrValue.Lift(templArgument.getNullPtrType());
  }
  case TemplateArgument::StructuralValue: {
    APValue SV = templArgument.getAsStructuralValue();
    return SV.Lift(templArgument.getStructuralValueType());
  }
  case TemplateArgument::Integral: {
    APValue IV(templArgument.getAsIntegral());
    return IV.Lift(templArgument.getIntegralType());
  }
  case TemplateArgument::Pack:
    llvm_unreachable("Packs should be expanded before calling this");

  // Could not get a test case to hit one of the below
  case TemplateArgument::Null:
    llvm_unreachable("TemplateArgument::Null not supported");
  case TemplateArgument::TemplateExpansion:
    llvm_unreachable("TemplateArgument::TemplateExpansion not supported");
  }
  llvm_unreachable("Unknown template argument type");
}

bool isTemplateSpecialization(QualType QT) {
  if (isa<UsingType>(QT) || isa<TypedefType>(QT))
    return false;

  return isa<TemplateSpecializationType>(QT) ||
         isa_and_nonnull<ClassTemplateSpecializationDecl>(
             QT->getAsCXXRecordDecl());
}
#pragma endregion

#pragma region Offset Helpers
size_t getBitOffsetOfField(ASTContext &C, const FieldDecl *FD) {
  const RecordDecl *Parent = FD->getParent();
  assert(Parent && "no parent for field!");

  const ASTRecordLayout &Layout = C.getASTRecordLayout(Parent);
  return Layout.getFieldOffset(FD->getFieldIndex());
}

size_t getOffsetOfBase(ASTContext &C, const CXXBaseSpecifier *Base) {
  // todo [merge:yukino:maybe-revert]
  const CXXRecordDecl *Derived = Base->getDerived();
  assert(Derived && "no parent for field!");

  const ASTRecordLayout &Layout = C.getASTRecordLayout(Derived);

  QualType BaseQT = Base->getType();
  BaseQT = desugarType(BaseQT, /*UnwrapAliases=*/true, /*DropCV=*/false,
                       /*DropRefs=*/false);
  CXXRecordDecl *RD = BaseQT->getAsCXXRecordDecl();
  assert(RD && "base isn't a record type?");

  if (Base->isVirtual())
    return Layout.getVBaseClassOffset(RD).getQuantity();
  else
    return Layout.getBaseClassOffset(RD).getQuantity();
}
#pragma endregion

bool ensureDeclared(ASTContext &C, QualType QT, SourceLocation SpecLoc) {
  // Get the type's declaration.
  if (auto *TS = dyn_cast<TemplateSpecializationType>(QT)) {
    if (auto *CTD = dyn_cast<ClassTemplateDecl>(
            TS->getTemplateName().getAsTemplateDecl())) {
      void *InsertPos;
      if (!CTD->findSpecialization(TS->template_arguments(), InsertPos)) {
        ClassTemplateSpecializationDecl *D =
            ClassTemplateSpecializationDecl::Create(
                C, CTD->getTemplatedDecl()->getTagKind(), CTD->getDeclContext(),
                SpecLoc, SpecLoc, CTD, TS->template_arguments(), false,
                nullptr);
        if (!D)
          return false;

        CTD->AddSpecialization(D, InsertPos);
      }
    }
  }
  return true;
}

#pragma region Reflectable Decls & Members
bool isReflectableDecl(const MetaFunctionEvalContext &EvalCtx, Decl *D) {
  assert(D && "null declaration");

  if (D != D->getCanonicalDecl()) {
    Decl *First = nullptr;
    for (Decl *I = D->getMostRecentDecl(); I; I = I->getPreviousDecl())
      if (I->getLexicalDeclContext() == D->getLexicalDeclContext())
        First = I;
    if (D != First)
      return false;
  }

  if (D->isLocalExternDecl())
    return false;

  if (isa<NamespaceAliasDecl>(D))
    return true;

  if (!isa<VarDecl, FunctionDecl, TypeDecl, FieldDecl, TemplateDecl,
           NamespaceDecl, NamespaceAliasDecl, TranslationUnitDecl,
           UsingShadowDecl>(D))
    return false;

  if (isa<UsingShadowDecl>(D) &&
      !EvalCtx.C->getLangOpts().EntityProxyReflection)
    return false;

  if (auto *Class = dyn_cast<CXXRecordDecl>(D))
    if (Class->isInjectedClassName() || Class->isLambda())
      return false;

  if (auto *FD = dyn_cast<FunctionDecl>(D)) {
    for (auto *R = FD->getMostRecentDecl(); R; R = R->getPreviousDecl()) {
      if (!R->getReturnType()->isUndeducedType() &&
          EvalCtx.Meta->HasSatisfiedConstraints(R))
        return true;
    }
    return false;
  }

  if (isa<ClassTemplateSpecializationDecl, VarTemplateSpecializationDecl>(D))
    return false;

  return D->getCanonicalDecl() == D;
}

/// Filter non-reflectable members.
Decl *findIterableMember(const MetaFunctionEvalContext &EvalCtx, Decl *D,
                         bool Inclusive) {
  if (!D)
    return D;

  if (Inclusive) {
    if (isReflectableDecl(EvalCtx, D))
      return D;

    // Handle the case where the first Decl is a LinkageSpecDecl.
    if (auto *LSDecl = dyn_cast_or_null<LinkageSpecDecl>(D)) {
      Decl *RecD = findIterableMember(EvalCtx, *LSDecl->decls_begin(), true);
      if (RecD)
        return RecD;
    }
  }

  do {
    DeclContext *DC = D->getDeclContext(); // note: SemanticDC

    if (D->getLexicalDeclContext() == DC) {
      // Get the next declaration in the DeclContext.
      //
      // Explicit specializations of templates are created with the DeclContext
      // of the template from which they're instantiated, but they end up in the
      // DeclContext within which they're declared. We therefore skip over any
      // declarations whose DeclContext is different from the previous Decl;
      // otherwise, we may inadvertently break the chain of redeclarations in
      // difficult to predit ways.
      do {
        D = D->getNextDeclInContext();
      } while (D && D->getDeclContext() != DC);

      // In the case of namespaces, walk the redeclaration chain.
      if (auto *NSDecl = dyn_cast<NamespaceDecl>(DC)) {
        while (!D && NSDecl) {
          NSDecl = NSDecl->getPreviousDecl();
          D = NSDecl ? *NSDecl->decls_begin() : nullptr;
        }

        if (!D) {
          auto *Canonical = cast<NamespaceDecl>(DC->getPrimaryContext());
          D = Canonical->getLastMultDCSemaDecl();
        }
      }
    } else {
      D = D->getPrevMultDCDeclInSemaContext();
    }

    // We need to recursively descend into LinkageSpecDecls to iterate over the
    // members declared therein (e.g., `extern "C"` blocks).
    if (auto *LSDecl = dyn_cast_or_null<LinkageSpecDecl>(D)) {
      Decl *RecD = findIterableMember(EvalCtx, *LSDecl->decls_begin(), true);
      if (RecD)
        return RecD;
    }

    // Pop back out of a recursively entered LinkageSpecDecl.
    if (!D && isa<LinkageSpecDecl>(DC))
      return findIterableMember(EvalCtx, cast<Decl>(DC), false);
  } while (D && !isReflectableDecl(EvalCtx, D));

  return D;
}
#pragma endregion

unsigned parentOf(APValue &Result, Decl *D) {
  if (!D)
    return diag::metafn_parent_of_undeclared;

  if (auto *FD = dyn_cast<FunctionDecl>(D); FD && FD->isExternC())
    return diag::metafn_parent_of_extern_c;
  else if (auto *VD = dyn_cast<VarDecl>(D); VD && VD->isExternC())
    return diag::metafn_parent_of_extern_c;

  auto *DC = D->getDeclContext();
  while (DC && !isa<NamespaceDecl>(DC) && !isa<RecordDecl>(DC) &&
         !isa<FunctionDecl>(DC) && !isa<TranslationUnitDecl>(DC) &&
         !isa<EnumDecl>(DC))
    DC = DC->getParent();

  assert(DC);
  if (auto *RD = dyn_cast<TagDecl>(DC))
    // todo [merge:yukino:maybe-revert]
    return SetAndSucceed(Result,
                         makeReflection(QualType(getTypeForDecl(RD), 0)));

  return SetAndSucceed(Result, makeReflection(cast<Decl>(DC)));
}

bool isSpecialMember(FunctionDecl *FD) {
  bool IsSpecial = false;
  if (const auto *MD = dyn_cast<CXXMethodDecl>(FD)) {
    IsSpecial = (isa<CXXDestructorDecl>(MD) || MD->isCopyAssignmentOperator() ||
                 MD->isMoveAssignmentOperator());

    if (auto *CtorD = dyn_cast<CXXConstructorDecl>(MD))
      IsSpecial = IsSpecial ||
                  (CtorD->isDefaultConstructor() ||
                   CtorD->isCopyConstructor() || CtorD->isMoveConstructor());
  }
  return IsSpecial;
}

bool isFunctionOrMethodNoexcept(const QualType QT) {
  const Type *T = QT.getTypePtr();

  if (T->isFunctionProtoType()) {
    // This covers (virtual) methods & functions
    const auto *FPT = T->getAs<FunctionProtoType>();
    switch (FPT->getExceptionSpecType()) {
    case EST_BasicNoexcept:
    case EST_NoexceptTrue:
      return true;
    default:
      return false;
    }
  }

  return false;
}

bool isConstQualifiedType(QualType QT) {
  bool result = QT.isConstQualified();
  if (auto *FPT = dyn_cast<FunctionProtoType>(QT))
    result |= FPT->isConst();

  return result;
}

bool isVolatileQualifiedType(QualType QT) {
  bool result = QT.isVolatileQualified();
  if (auto *FPT = dyn_cast<FunctionProtoType>(QT))
    result |= FPT->isVolatile();

  return result;
}

QualType ComputeResultType(QualType ExprTy, const APValue &V) {
  SplitQualType SQT;

  if (V.isLValue() && !ExprTy->isPointerType() && !V.getLValueBase().isNull()) {
    SQT = V.getLValueBase().getType().split();

    for (auto p = V.getLValuePath().begin(); p != V.getLValuePath().end();
         ++p) {
      const Decl *D = V.getLValuePath().back().getAsBaseOrMember().getPointer();
      if (D) { // base or member case
        if (auto *VD = dyn_cast<FieldDecl>(D)) {
          QualType QT = VD->getType();
          SQT.Ty = QT.getTypePtr();

          if (QT.isConstQualified())
            SQT.Quals.addConst();
          if (QT.isVolatileQualified())
            SQT.Quals.addVolatile();

          continue;
        } else if (auto *TD = dyn_cast<CXXRecordDecl>(D)) {
          // todo [merge:yukino:maybe-revert]
          SQT.Ty = getTypeForDecl(TD);
          continue;
        }

        llvm_unreachable("unknown lvalue path kind");
      } else { // array case
        QualType QT = cast<ArrayType>(SQT.Ty)->getElementType();
        SQT.Ty = QT.getTypePtr();
        if (QT.isConstQualified())
          SQT.Quals.addConst();
        if (QT.isVolatileQualified())
          SQT.Quals.addVolatile();
      }
    }
    return QualType(SQT.Ty, SQT.Quals.getAsOpaqueValue());
  }
  return desugarType(ExprTy, /*UnwrapAliases=*/true,
                     /*DropCV=*/!ExprTy->isRecordType(),
                     /*DropRefs=*/true);
}

APValue MaybeUnproxy(ASTContext &C, APValue RV, bool Dealias = true) {
  assert(RV.isReflection());

  if (!RV.isReflectedEntityProxy())
    return RV;

  NamedDecl *ND = RV.getReflectedEntityProxy()->getTargetDecl();
  ND = cast<NamedDecl>(ND->getCanonicalDecl());

  if (auto *T = dyn_cast<TypeDecl>(ND)) {
    QualType QT = C.getTypeDeclType(T);
    if (Dealias)
      QT = desugarType(QT, /*UnwrapAlias=*/true, /*DropCV=*/false,
                       /*DropRefs=*/false);

    return {ReflectionKind::Type, QT.getAsOpaquePtr()};
  } else if (auto *T = dyn_cast<TemplateDecl>(ND)) {
    return {ReflectionKind::Template, T};
  }

  return {ReflectionKind::Declaration, ND};
}

// -----------------------------------------------------------------------------
// Diagnostic helper function
// -----------------------------------------------------------------------------

#pragma region Diagnostic Helpers
StringRef GetSpecialMemberName(DeclarationName Name, bool IsTemplate) {
  // Helper to append " template" if needed, avoiding manual duplication.
  // Note: Since we return StringRef, we must return string literals.
  // We handle the boolean branching inside the cases for safety.
  switch (Name.getNameKind()) {
  case DeclarationName::CXXConstructorName:
    return IsTemplate ? "a constructor template" : "a constructor";
  case DeclarationName::CXXDestructorName:
    return IsTemplate ? "a destructor template" : "a destructor";
  case DeclarationName::CXXConversionFunctionName:
    return IsTemplate ? "a conversion function template"
                      : "a conversion function";
  case DeclarationName::CXXOperatorName:
    return IsTemplate ? "an operator function template"
                      : "an operator function";
  case DeclarationName::CXXLiteralOperatorName:
    return IsTemplate ? "a literal operator template" : "a literal operator";
  default:
    return "";
  }
}

StringRef DescriptionOf(APValue RV, bool Granular = true) {
  // 1. Handle non-AST constructs first (Simple Kinds)
  switch (RV.getReflectionKind()) {
  case ReflectionKind::Null:
    return "a null reflection";
  case ReflectionKind::Object:
    return "an object";
  case ReflectionKind::Value:
    return "a value";
  case ReflectionKind::EntityProxy:
    return "an entity proxy";
  case ReflectionKind::BaseSpecifier:
    return "a base class specifier";
  case ReflectionKind::Parameter:
    return "a parameter";
  case ReflectionKind::DataMemberSpec:
    return "a description of a non-static data member";
  case ReflectionKind::Annotation:
    return "an annotation";
  case ReflectionKind::Type:
    return isTypeAlias(RV.getReflectedType()) ? "type alias" : "a type";
  default:
    break; // Fallthrough to AST handling
  }

  // 2. Handle AST Declarations (Declarations and Templates)
  // We use TypeSwitch to flatten the hierarchy check.
  if (RV.getReflectionKind() == ReflectionKind::Declaration) {
    ValueDecl *D = RV.getReflectedDecl();

    // Check for special names first (Constructor, Destructor, etc.)
    if (StringRef Special =
            GetSpecialMemberName(D->getDeclName(), /*IsTemplate=*/false);
        !Special.empty())
      return Special;

    return llvm::TypeSwitch<ValueDecl *, StringRef>(D)
        .Case<FieldDecl>([](FieldDecl *FD) {
          if (FD->isUnnamedBitField())
            return "an unnamed bit-field";
          return FD->isBitField() ? "a bit-field" : "a non-static data member";
        })
        .Case<ParmVarDecl>([](auto) { return "function parameter"; })
        .Case<VarDecl>([](auto) { return "a variable"; })
        .Case<BindingDecl>([](auto) { return "a structured binding"; })
        .Case<FunctionDecl>([](auto) { return "a function"; })
        .Case<EnumConstantDecl>([](auto) { return "a enumerator"; })
        .Default([] {
          llvm_unreachable("unhandled declaration kind");
          return StringRef();
        }());
  }

  if (RV.getReflectionKind() == ReflectionKind::Template) {
    TemplateDecl *TD = RV.getReflectedTemplate().getAsTemplateDecl();

    if (StringRef Special =
            GetSpecialMemberName(TD->getDeclName(), /*IsTemplate=*/true);
        !Special.empty())
      return Special;

    return llvm::TypeSwitch<TemplateDecl *, StringRef>(TD)
        .Case<FunctionTemplateDecl>([](auto) { return "a function template"; })
        .Case<ClassTemplateDecl>([](auto) { return "a class template"; })
        .Case<TypeAliasTemplateDecl>([](auto) { return "an alias template"; })
        .Case<VarTemplateDecl>([](auto) { return "a variable template"; })
        .Case<ConceptDecl>([](auto) { return "a concept"; })
        .Default([]() {
          llvm_unreachable("unhandled template kind");
          return StringRef();
        }());
  }

  if (RV.getReflectionKind() == ReflectionKind::Namespace) {
    return llvm::TypeSwitch<Decl *, StringRef>(RV.getReflectedNamespace())
        .Case<TranslationUnitDecl>([](auto) { return "the global namespace"; })
        .Case<NamespaceAliasDecl>([](auto) { return "a namespace alias"; })
        .Case<NamespaceDecl>([](auto) { return "a namespace"; })
        .Default([] {
          llvm_unreachable("unhandled namespace kind");
          return StringRef();
        }());
  }

  llvm_unreachable("Unknown ReflectionKind");
}

bool DiagnoseReflectionKind(DiagFn Diagnoser, SourceRange Range,
                            StringRef Expected, StringRef Instead = "") {
  if (!Instead.empty())
    Diagnoser(Range.getBegin(), diag::metafn_expected_reflection_of_but_got)
        << Expected << Instead << Range;
  else
    Diagnoser(Range.getBegin(), diag::metafn_expected_reflection_of)
        << Expected << Range;

  return true;
}

void CheckReflectionArg(const MetaFunctionEvalContext &EvalCtx, size_t I) {
  if (!EvalCtx.Args[I]->getType()->isReflectionType())
    llvm_unreachable("argument supposed to be a reflection");
}

void CheckReflectionArrayArg(const MetaFunctionEvalContext &EvalCtx, size_t I) {
  if (!EvalCtx.Args[I]
           ->getType()
           ->getPointeeOrArrayElementType()
           ->isReflectionType())
    llvm_unreachable("argument supposed to be an array of reflections");
}

void CheckIntegralArg(const MetaFunctionEvalContext &EvalCtx, size_t I) {
  if (!EvalCtx.Args[I]->getType()->isIntegerType())
    llvm_unreachable("argument supposed to be an integer");
}

void CheckResultTyIsMetaInfo(const MetaFunctionEvalContext &EvalCtx) {
  if (EvalCtx.ResultTy != EvalCtx.C->MetaInfoTy)
    llvm_unreachable("this metafunction should return a reflection");
}

void CheckResultTyIsBool(const MetaFunctionEvalContext &EvalCtx) {
  if (EvalCtx.ResultTy != EvalCtx.C->BoolTy)
    llvm_unreachable("this metafunction should return a boolean value");
}

void CheckResultTyIsSizeT(const MetaFunctionEvalContext &EvalCtx) {
  if (EvalCtx.ResultTy != EvalCtx.C->getSizeType())
    llvm_unreachable("this metafunction should return a size_t");
}
#pragma endregion

// -----------------------------------------------------------------------------
// Yukino's extension helper functions
// -----------------------------------------------------------------------------

// #pragma region Yukino's extension helpers

// #pragma endregion

// -----------------------------------------------------------------------------
// Metafunction implementations
// -----------------------------------------------------------------------------

#pragma region Metafunctions for Enumerables
bool get_begin_enumerator_decl_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsMetaInfo(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  APValue Sentinel;
  if (!EvalCtx.Evaluator(Sentinel, EvalCtx.Args[1], true))
    return true;
  assert(Sentinel.isReflectedType());

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    Decl *D = findTypeDecl(RV.getReflectedType());

    if (auto enumDecl = dyn_cast_or_null<EnumDecl>(D)) {
      if (auto itr = enumDecl->enumerator_begin();
          itr != enumDecl->enumerator_end()) {
        return SetReflectionAndSucceed(EvalCtx, *itr);
      }
      return SetAndSucceed(*EvalCtx.Result, Sentinel);
    }
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "an enum type");
  }
  case ReflectionKind::Null:
  case ReflectionKind::Declaration:
  case ReflectionKind::Template:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation: {
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "an enum type", DescriptionOf(RV));
  }
  }
  llvm_unreachable("unknown reflection kind");
}

bool get_next_enumerator_decl_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsMetaInfo(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  APValue Sentinel;
  if (!EvalCtx.Evaluator(Sentinel, EvalCtx.Args[1], true))
    return true;
  assert(Sentinel.isReflectedType());

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Declaration: {
    Decl *currEnumConstDecl = RV.getReflectedDecl();
    if (auto nextEnumConstDecl = currEnumConstDecl->getNextDeclInContext()) {
      return SetReflectionAndSucceed(EvalCtx, nextEnumConstDecl);
    }
    return SetAndSucceed(*EvalCtx.Result, Sentinel);
  }
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Template:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::Parameter:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation: {
    llvm_unreachable("should have failed in 'get_begin_enumerator_decl_of'");
  }
  }
  llvm_unreachable("unknown reflection kind");
}

bool get_ith_base_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsMetaInfo(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  APValue Sentinel;
  if (!EvalCtx.Evaluator(Sentinel, EvalCtx.Args[1], true))
    return true;
  assert(Sentinel.isReflectedType());

  APValue Idx;
  if (!EvalCtx.Evaluator(Idx, EvalCtx.Args[2], true))
    return true;
  size_t idx = Idx.getInt().getExtValue();

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    QualType QT = RV.getReflectedType();
    QT = desugarType(QT, /*UnwrapAliases=*/true, /*DropCV=*/false,
                     /*DropRefs=*/false);

    Decl *typeDecl = findTypeDecl(QT);

    if (auto cxxRecordDecl = dyn_cast_or_null<CXXRecordDecl>(typeDecl)) {
      EvalCtx.Meta->EnsureInstantiated(typeDecl, EvalCtx.Range);
      if (RV.getReflectedType()->isIncompleteType())
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_cannot_introspect_type)
               << 0 << 0 << EvalCtx.Range;

      auto numBases = cxxRecordDecl->getNumBases();
      if (idx >= numBases)
        return SetAndSucceed(*EvalCtx.Result, Sentinel);

      // the unqualified base class
      CXXBaseSpecifier *baseClassItr = cxxRecordDecl->bases_begin() + idx;
      return SetReflectionAndSucceed(EvalCtx, baseClassItr);
    }
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_introspect_type)
           << 0 << 1 << EvalCtx.Range;
  }
  case ReflectionKind::Null:
  case ReflectionKind::Declaration:
  case ReflectionKind::Template:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::Parameter:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "a class type", DescriptionOf(RV));
  }
  llvm_unreachable("unknown reflection kind");
}

bool get_ith_template_argument_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsMetaInfo(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  APValue Sentinel;
  if (!EvalCtx.Evaluator(Sentinel, EvalCtx.Args[1], true))
    return true;
  assert(Sentinel.isReflectedType());

  APValue Idx;
  if (!EvalCtx.Evaluator(Idx, EvalCtx.Args[2], true))
    return true;
  size_t idx = Idx.getInt().getExtValue();

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    SmallVector<TemplateArgument, 4> TArgs;
    if (getTemplateArgumentsFromType(RV.getReflectedType(), TArgs))
      return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                    "a template specialization");

    APValue R = getNthTemplateArgument(*EvalCtx.C, TArgs, EvalCtx.Evaluator,
                                       Sentinel, idx);
    if (R.isReflectedDecl())
      R = APValue(APValue::LValueBase{R.getReflectedDecl()}, CharUnits::Zero(),
                  {}, false, false)
              .Lift(QualType{});
    return SetAndSucceed(*EvalCtx.Result, R);
  }
  case ReflectionKind::Declaration: {
    SmallVector<TemplateArgument, 4> TArgs;
    if (getTemplateArgumentsFromDecl(RV.getReflectedDecl(), TArgs))
      return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                    "a template specialization");
    APValue R = getNthTemplateArgument(*EvalCtx.C, TArgs, EvalCtx.Evaluator,
                                       Sentinel, idx);
    if (R.isReflectedDecl() && !isa<FunctionDecl>(R.getReflectedDecl()))
      R = APValue(APValue::LValueBase{R.getReflectedDecl()}, CharUnits::Zero(),
                  {}, false, false)
              .Lift(QualType{});
    return SetAndSucceed(*EvalCtx.Result, R);
  }
  case ReflectionKind::Null:
  case ReflectionKind::Template:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::Parameter:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "a template specialization",
                                  DescriptionOf(RV));
  }
  llvm_unreachable("unknown reflection kind");
}

bool get_begin_member_decl_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckResultTyIsMetaInfo(EvalCtx);
  
  CheckReflectionArg(EvalCtx, 0);
  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true)) {
    return true;
  }

  CheckReflectionArg(EvalCtx, 1);
  APValue Sentinel;
  if (!EvalCtx.Evaluator(Sentinel, EvalCtx.Args[1], true))
    return true;
  assert(Sentinel.isReflectedType());

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    QualType QT = RV.getReflectedType();
    if (isTypeAlias(QT))
      QT = desugarType(QT, /*UnwrapAliases=*/true, /*DropCV=*/false,
                       /*DropRefs=*/false);

    if (isa<EnumType>(QT)) {
      EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                        diag::metafn_cannot_introspect_type)
          << 1 << 1 << EvalCtx.Range;
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_members_of_enum)
             << EvalCtx.Range;
    }

    ensureDeclared(*EvalCtx.C, QT, EvalCtx.Range.getBegin());
    Decl *typeDecl = findTypeDecl(QT);
    if (!typeDecl)
      return true;

    if (!EvalCtx.Meta->EnsureInstantiated(typeDecl, EvalCtx.Range))
      return true;

    if (QT->isIncompleteType())
      return true;
    // NOTE(P2996): Uncomment to allow 'members_of' within member
    // specification.
    /*
    if (auto *TD = dyn_cast<TagDecl>(typeDecl); !TD || !TD->isBeingDefined())
      return true;
    */

    if (auto *CXXRD = dyn_cast<CXXRecordDecl>(typeDecl))
      EvalCtx.Meta->EnsureDeclarationOfImplicitMembers(CXXRD);

    DeclContext *declContext = dyn_cast<DeclContext>(typeDecl);
    assert(declContext && "no DeclContext?");

    Decl *beginMember =
        findIterableMember(EvalCtx, *declContext->decls_begin(), true);
    if (!beginMember)
      return SetAndSucceed(*EvalCtx.Result, Sentinel);
    return SetAndSucceed(*EvalCtx.Result,
                         APValue(ReflectionKind::Declaration, beginMember));
  }
  case ReflectionKind::Namespace: {
    Decl *NS = RV.getReflectedNamespace();
    if (auto *A = dyn_cast<NamespaceAliasDecl>(NS))
      NS = A->getNamespace();

    DeclContext *DC = cast<DeclContext>(NS->getMostRecentDecl());

    Decl *beginMember = findIterableMember(EvalCtx, *DC->decls_begin(), true);
    if (!beginMember)
      return SetAndSucceed(*EvalCtx.Result, Sentinel);
    return SetAndSucceed(*EvalCtx.Result,
                         APValue(ReflectionKind::Declaration, beginMember));
  }
  case ReflectionKind::Null:
  case ReflectionKind::Declaration:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::Parameter:
  case ReflectionKind::Template:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return true;
  }
  llvm_unreachable("unknown reflection kind");
}

bool get_next_member_decl_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckResultTyIsMetaInfo(EvalCtx);
  CheckReflectionArg(EvalCtx, 0);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;
  CheckReflectionArg(EvalCtx, 1);

  APValue Sentinel;
  if (!EvalCtx.Evaluator(Sentinel, EvalCtx.Args[1], true))
    return true;
  assert(Sentinel.isReflectedType());

  if (Decl *Next = findIterableMember(EvalCtx, RV.getReflectedDecl(), false))
    return SetAndSucceed(*EvalCtx.Result,
                         APValue(ReflectionKind::Declaration, Next));
  return SetAndSucceed(*EvalCtx.Result, Sentinel);
}

bool is_structural_type(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  auto result = false;
  if (RV.isReflectedType()) {
    // If this is a declared type with a reachable definition, ensure that the
    // type is instantiated.
    if (Decl *typeDecl = findTypeDecl(RV.getReflectedType()))
      EvalCtx.Meta->EnsureInstantiated(typeDecl, EvalCtx.Range);

    const QualType QT = RV.getReflectedType();
    const Type *T = QT.getTypePtr();

    result = T->isStructuralType();
  }

  return SetBoolAndSucceed(EvalCtx, result);
}
#pragma endregion

bool map_decl_to_entity(const MetaFunctionEvalContext &EvalCtx) {
  CheckResultTyIsMetaInfo(EvalCtx);
  CheckReflectionArg(EvalCtx, 0);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;
  Decl *D = RV.getReflectedDecl();

  if (auto *TyDecl = dyn_cast<TypeDecl>(D)) {
    QualType QT = EvalCtx.C->getTypeDeclType(TyDecl);
    return SetReflectionAndSucceed(EvalCtx, QT);
  }
  return SetReflectionAndSucceed(EvalCtx, D);
}

bool identifier_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[1], true))
    return true;

  bool IsUtf8;
  {
    APValue Scratch;
    if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[2], true))
      return true;
    IsUtf8 = Scratch.getInt().getBoolValue();
  }

  bool EnforceConsistent;
  {
    APValue Scratch;
    if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[3], true))
      return true;
    EnforceConsistent = Scratch.getInt().getBoolValue();
  }

  RV = MaybeUnproxy(*EvalCtx.C, RV, /*Dealias=*/false);

  std::string Name;
  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    QualType QT = RV.getReflectedType();
    if (isTemplateSpecialization(QT))
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_name_is_not_identifier)
             << 0 << EvalCtx.Range;

    if (auto *D = findTypeDecl(QT))
      if (auto *ND = dyn_cast<NamedDecl>(D); ND && ND->getIdentifier())
        Name = ND->getIdentifier()->getName();

    break;
  }
  case ReflectionKind::Declaration: {
    if (auto *ND = dyn_cast<NamedDecl>(RV.getReflectedDecl())) {
      if (!findTemplateOfDecl(ND).isNull())
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_name_is_not_identifier)
               << 0 << EvalCtx.Range;
      if (isa<CXXConstructorDecl>(ND))
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_name_is_not_identifier)
               << 1 << EvalCtx.Range;
      if (isa<CXXDestructorDecl>(ND))
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_name_is_not_identifier)
               << 2 << EvalCtx.Range;
      if (ND->getDeclName().getNameKind() == DeclarationName::CXXOperatorName)
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_name_is_not_identifier)
               << 3 << EvalCtx.Range;
      if (ND->getDeclName().getNameKind() ==
          DeclarationName::CXXConversionFunctionName)
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_name_is_not_identifier)
               << 4 << EvalCtx.Range;

      if (auto *II = ND->getIdentifier())
        Name = II->getName();
      else if (auto *II = ND->getDeclName().getCXXLiteralIdentifier())
        Name = II->getName();
    }

    break;
  }
  case ReflectionKind::Parameter: {
    bool ConsistentName = getParameterName(RV.getReflectedParameter(), Name);
    if (EnforceConsistent && !ConsistentName) {
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_inconsistent_name)
             << DescriptionOf(RV) << EvalCtx.Range;
    }
    break;
  }
  case ReflectionKind::Template: {
    const TemplateDecl *TD = RV.getReflectedTemplate().getAsTemplateDecl();
    if (auto *FTD = dyn_cast<FunctionTemplateDecl>(TD)) {
      if (isa<CXXConstructorDecl>(FTD->getTemplatedDecl()))
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_name_is_not_identifier)
               << 5 << EvalCtx.Range;
      else if (FTD->getDeclName().getNameKind() ==
               DeclarationName::CXXOperatorName)
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_name_is_not_identifier)
               << 6 << EvalCtx.Range;
      else if (FTD->getDeclName().getNameKind() ==
               DeclarationName::CXXConversionFunctionName)
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_name_is_not_identifier)
               << 7 << EvalCtx.Range;
    }

    if (auto *II = TD->getIdentifier())
      Name = II->getName();
    else if (auto *II = TD->getDeclName().getCXXLiteralIdentifier())
      Name = II->getName();

    break;
  }
  case ReflectionKind::Namespace: {
    if (isa<TranslationUnitDecl>(RV.getReflectedNamespace()))
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_name_of_unnamed_singleton)
             << 1 << EvalCtx.Range;
    getDeclName(Name, *EvalCtx.C, RV.getReflectedNamespace());
    break;
  }
  case ReflectionKind::DataMemberSpec: {
    TagDataMemberSpec *TDMS = RV.getReflectedDataMemberSpec();
    if (TDMS->Name)
      Name = *TDMS->Name;
    break;
  }
  case ReflectionKind::BaseSpecifier: {
    CXXBaseSpecifier *Base = RV.getReflectedBaseSpecifier();
    QualType QT = Base->getType();
    if (!QT.isNull() && QT.getBaseTypeIdentifier()) {
      Name = QT.getBaseTypeIdentifier()->getName();
    }
    break;
  }
  case ReflectionKind::Null:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_name_of_unnamed_singleton)
           << 0 << EvalCtx.Range;
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Annotation:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_have_name)
           << DescriptionOf(RV) << EvalCtx.Range;
  case ReflectionKind::EntityProxy:
    llvm_unreachable("proxies should already have been unwrapped");
  }
  if (Name.empty())
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_anonymous_entity)
           << DescriptionOf(RV) << EvalCtx.Range;

  Expr *StrLit = makeStrLiteral(Name, *EvalCtx.C, IsUtf8);

  APValue::LValuePathEntry Path[1] = {APValue::LValuePathEntry::ArrayIndex(0)};
  return SetAndSucceed(*EvalCtx.Result,
                       APValue(StrLit, CharUnits::Zero(), Path, false));
}

bool has_identifier(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  RV = MaybeUnproxy(*EvalCtx.C, RV, /*Dealias=*/false);

  bool HasIdentifier = false;
  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    QualType QT = RV.getReflectedType();
    if (isTemplateSpecialization(QT))
      break;

    if (auto *D = findTypeDecl(QT))
      if (auto *ND = dyn_cast<NamedDecl>(D); ND && ND->getIdentifier())
        HasIdentifier = (ND->getIdentifier() != nullptr);

    break;
  }
  case ReflectionKind::Parameter: {
    auto *PVD = RV.getReflectedParameter();

    std::string Name;
    bool Consistent = getParameterName(PVD, Name);

    HasIdentifier = Consistent && !Name.empty();
    break;
  }
  case ReflectionKind::Declaration: {
    auto *D = RV.getReflectedDecl();

    if (auto *FD = dyn_cast<FunctionDecl>(D);
        FD && FD->getTemplateSpecializationArgs())
      break;
    else if (isa<VarTemplateSpecializationDecl>(D))
      break;
    else if (auto *PVD = dyn_cast<ParmVarDecl>(D)) {
      std::string Name;
      (void)getParameterName(PVD, Name);
      HasIdentifier = !Name.empty();
    } else if (auto *ND = dyn_cast<NamedDecl>(D))
      HasIdentifier = (ND->getIdentifier() != nullptr);

    break;
  }
  case ReflectionKind::Template: {
    const TemplateDecl *TD = RV.getReflectedTemplate().getAsTemplateDecl();
    if (auto *FTD = dyn_cast<FunctionTemplateDecl>(TD))
      if (isa<CXXConstructorDecl>(FTD->getTemplatedDecl()))
        break;

    HasIdentifier = (TD->getIdentifier() != nullptr);
    break;
  }
  case ReflectionKind::Namespace: {
    if (auto *ND = dyn_cast<NamedDecl>(RV.getReflectedNamespace()))
      HasIdentifier = (ND->getIdentifier() != nullptr);
    break;
  }
  case ReflectionKind::DataMemberSpec: {
    TagDataMemberSpec *TDMS = RV.getReflectedDataMemberSpec();
    HasIdentifier = TDMS->Name && !TDMS->Name->empty();
    break;
  }
  case ReflectionKind::Null:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Annotation:
    break;
  case ReflectionKind::EntityProxy:
    llvm_unreachable("proxies should already have been unwrapped");
  }

  return SetBoolAndSucceed(EvalCtx, HasIdentifier);
}

bool operator_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsSizeT(EvalCtx);

  static constexpr OverloadedOperatorKind OperatorIndices[] = {
      OO_None,          OO_New,
      OO_Delete,        OO_Array_New,
      OO_Array_Delete,  OO_Coawait,
      OO_Call,          OO_Subscript,
      OO_Arrow,         OO_ArrowStar,
      OO_Tilde,         OO_Exclaim,
      OO_Plus,          OO_Minus,
      OO_Star,          OO_Slash,
      OO_Percent,       OO_Caret,
      OO_Amp,           OO_Pipe,
      OO_Equal,         OO_PlusEqual,
      OO_MinusEqual,    OO_StarEqual,
      OO_SlashEqual,    OO_PercentEqual,
      OO_CaretEqual,    OO_AmpEqual,
      OO_PipeEqual,     OO_EqualEqual,
      OO_ExclaimEqual,  OO_Less,
      OO_Greater,       OO_LessEqual,
      OO_GreaterEqual,  OO_Spaceship,
      OO_AmpAmp,        OO_PipePipe,
      OO_LessLess,      OO_GreaterGreater,
      OO_LessLessEqual, OO_GreaterGreaterEqual,
      OO_PlusPlus,      OO_MinusMinus,
      OO_Comma,
  };

  auto findOperatorOf = [](FunctionDecl *FD) -> size_t {
    OverloadedOperatorKind OO = FD->getOverloadedOperator();
    if (OO == OO_None)
      return 0;

    auto *OpPtr =
        std::find(std::begin(OperatorIndices), std::end(OperatorIndices), OO);
    assert(OpPtr < std::end(OperatorIndices));

    return (OpPtr - OperatorIndices);
  };

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  RV = MaybeUnproxy(*EvalCtx.C, RV);

  size_t OperatorId = 0;
  if (RV.isReflectedTemplate()) {
    const TemplateDecl *TD = RV.getReflectedTemplate().getAsTemplateDecl();
    if (auto *FTD = dyn_cast<FunctionTemplateDecl>(TD))
      OperatorId = findOperatorOf(FTD->getTemplatedDecl());
  } else if (RV.isReflectedDecl()) {
    if (auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl()))
      OperatorId = findOperatorOf(FD);
  }

  if (OperatorId == 0)
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_not_an_operator)
           << DescriptionOf(RV) << EvalCtx.Range;

  return SetAndSucceed(
      *EvalCtx.Result,
      APValue(EvalCtx.C->MakeIntValue(OperatorId, EvalCtx.C->getSizeType())));
}

bool source_location_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type:
    return findTypeDeclLoc(EvalCtx, RV.getReflectedType());
  case ReflectionKind::Declaration:
    return findDeclLoc(EvalCtx, RV.getReflectedDecl());
  case ReflectionKind::Template: {
    TemplateName TName = RV.getReflectedTemplate();
    return findDeclLoc(EvalCtx, TName.getAsTemplateDecl());
  }
  case ReflectionKind::Namespace:
    return findDeclLoc(EvalCtx, RV.getReflectedNamespace());
  case ReflectionKind::EntityProxy:
    return findDeclLoc(EvalCtx, RV.getReflectedEntityProxy());
  case ReflectionKind::Parameter:
    return findDeclLoc(EvalCtx, RV.getReflectedParameter());
  case ReflectionKind::BaseSpecifier:
    return findBaseSpecLoc(EvalCtx, RV.getReflectedBaseSpecifier());
  case ReflectionKind::Annotation:
    return findAnnotLoc(EvalCtx, RV.getReflectedAnnotation());
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Null:
  case ReflectionKind::DataMemberSpec:
    return findDeclLoc(EvalCtx, nullptr);
  }
  llvm_unreachable("unknown reflection kind");
}

bool type_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_no_associated_property)
           << DescriptionOf(RV) << 0 << EvalCtx.Range;
  case ReflectionKind::Object:
  case ReflectionKind::Value: {
    QualType QT = desugarType(RV.getTypeOfReflectedResult(*EvalCtx.C),
                              /*UnwrapAliases=*/true, /*DropCV=*/false,
                              /*DropRefs=*/false);
    return SetReflectionAndSucceed(EvalCtx, QT);
  }
  case ReflectionKind::Declaration: {
    ValueDecl *VD = cast<ValueDecl>(RV.getReflectedDecl());
    if (isa<CXXConstructorDecl, CXXDestructorDecl, BindingDecl>(VD))
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_cannot_query_property)
             << 0 << DescriptionOf(RV) << EvalCtx.Range;

    if (auto *FD = dyn_cast<FunctionDecl>(VD))
      EvalCtx.Meta->EnsureInstantiationOfExceptionSpec(EvalCtx.Range.getBegin(),
                                                       FD);

    QualType QT = desugarType(VD->getType(),
                              /*UnwrapAliases=*/true, /*DropCV=*/false,
                              /*DropRefs=*/false);
    return SetReflectionAndSucceed(EvalCtx, QT);
  }
  case ReflectionKind::Parameter: {
    ParmVarDecl *PVD = RV.getReflectedParameter();
    QualType QT = desugarType(PVD->getType(),
                              /*UnwrapAliases=*/true, /*DropCV=*/true,
                              /*DropRefs=*/false);
    return SetReflectionAndSucceed(EvalCtx, QT);
  }
  case ReflectionKind::BaseSpecifier: {
    QualType QT = RV.getReflectedBaseSpecifier()->getType();
    QT = desugarType(QT, /*UnwrapAliases=*/true, /*DropCV=*/false,
                     /*DropRefs=*/false);
    return SetReflectionAndSucceed(EvalCtx, QT);
  }
  case ReflectionKind::DataMemberSpec: {
    QualType QT = RV.getReflectedDataMemberSpec()->Ty;
    QT = desugarType(QT, /*UnwrapAliases=*/true, /*DropCV=*/false,
                     /*DropRefs=*/false);
    return SetReflectionAndSucceed(EvalCtx, QT);
  }
  case ReflectionKind::Annotation: {
    QualType QT = RV.getReflectedAnnotation()->getArg()->getType();
    QT = desugarType(QT, /*UnwrapAliases=*/true, /*DropCV=*/true,
                     /*DropRefs=*/false);
    return SetReflectionAndSucceed(EvalCtx, QT);
  }
  }
  llvm_unreachable("unknown reflection kind");
}

bool parent_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  auto DiagWrapper = [&](unsigned DiagId) {
    if (DiagId && EvalCtx.Diagnoser)
      return bool(EvalCtx.Diagnoser(EvalCtx.Range.getBegin(), DiagId)
                  << DescriptionOf(RV) << EvalCtx.Range);

    return DiagId > 0;
  };

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    if (EvalCtx.Diagnoser)
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_no_associated_property)
             << DescriptionOf(RV) << 1 << EvalCtx.Range;
    return true;
  case ReflectionKind::Type: {
    if (TemplateName TName = findTemplateOfType(RV.getReflectedType());
        !TName.isNull())
      return DiagWrapper(parentOf(*EvalCtx.Result, TName.getAsTemplateDecl()));

    return DiagWrapper(
        parentOf(*EvalCtx.Result, findTypeDecl(RV.getReflectedType())));
  }
  case ReflectionKind::Declaration: {
    if (TemplateName TName = findTemplateOfDecl(RV.getReflectedDecl());
        !TName.isNull())
      return DiagWrapper(parentOf(*EvalCtx.Result, TName.getAsTemplateDecl()));

    return DiagWrapper(parentOf(*EvalCtx.Result, RV.getReflectedDecl()));
  }
  case ReflectionKind::Template: {
    return DiagWrapper(parentOf(*EvalCtx.Result,
                                RV.getReflectedTemplate().getAsTemplateDecl()));
  }
  case ReflectionKind::Parameter: {
    return DiagWrapper(parentOf(*EvalCtx.Result, RV.getReflectedParameter()));
  }
  case ReflectionKind::Namespace:
    if (isa<TranslationUnitDecl>(RV.getReflectedNamespace())) {
      if (EvalCtx.Diagnoser)
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_no_associated_property)
               << DescriptionOf(RV) << 1 << EvalCtx.Range;
      return true;
    }
    return DiagWrapper(parentOf(*EvalCtx.Result, RV.getReflectedNamespace()));
  case ReflectionKind::EntityProxy:
    return DiagWrapper(parentOf(*EvalCtx.Result, RV.getReflectedEntityProxy()));
  case ReflectionKind::BaseSpecifier: {
    CXXRecordDecl *RD = RV.getReflectedBaseSpecifier()->getDerived();
    // todo [merge:yukino:maybe-revert]
    QualType QT = desugarType(QualType(getTypeForDecl(RD), 0),
                              /*UnwrapAliases=*/true, /*DropCV=*/false,
                              /*DropRefs=*/false);
    return SetReflectionAndSucceed(EvalCtx, QT);
  }
  }
  llvm_unreachable("unknown reflection kind");
}

bool underlying_entity_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsMetaInfo(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Declaration:
  case ReflectionKind::Template:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return SetAndSucceed(*EvalCtx.Result, RV);
  case ReflectionKind::Type: {
    QualType QT = RV.getReflectedType();
    QT = desugarType(QT, /*UnwrapAliases=*/true, /*DropCV=*/false,
                     /*DropRefs=*/false);
    return SetReflectionAndSucceed(EvalCtx, QT);
  }
  case ReflectionKind::Namespace: {
    Decl *NS = RV.getReflectedNamespace();
    if (auto *A = dyn_cast<NamespaceAliasDecl>(NS))
      NS = A->getNamespace();
    return SetReflectionAndSucceed(EvalCtx, NS);
  }
  case ReflectionKind::EntityProxy:
    return SetAndSucceed(*EvalCtx.Result, MaybeUnproxy(*EvalCtx.C, RV));
  }
  llvm_unreachable("unknown reflection kind");
}

bool proxied_entity_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsMetaInfo(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Declaration:
  case ReflectionKind::Namespace:
  case ReflectionKind::Template:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "an entity proxy");
  case ReflectionKind::EntityProxy:
    return SetAndSucceed(*EvalCtx.Result, MaybeUnproxy(*EvalCtx.C, RV, false));
  }
  llvm_unreachable("unknown reflection kind");
}

bool object_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsMetaInfo(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Object:
    return SetAndSucceed(*EvalCtx.Result, RV);
  case ReflectionKind::Declaration: {
    VarDecl *VD = dyn_cast<VarDecl>(RV.getReflectedDecl());
    if (!VD)
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_cannot_query_property)
             << 1 << DescriptionOf(RV) << EvalCtx.Range;

    EvalCtx.Meta->EnsureInstantiated(VD, EvalCtx.Args[0]->getSourceRange());

    QualType QT = VD->getType();
    if (auto *LVRT = dyn_cast<LValueReferenceType>(QT)) {
      QT = LVRT->getPointeeType();
    }

    Expr *Synthesized = DeclRefExpr::Create(
        *EvalCtx.C, NestedNameSpecifierLoc(), SourceLocation(), VD, false,
        EvalCtx.Range.getBegin(), QT, VK_LValue, VD, nullptr);
    APValue Value;
    if (!EvalCtx.Evaluator(Value, Synthesized, false) || !Value.isLValue())
      return true;

    APValue OV = Value.Lift(QualType{});
    return SetAndSucceed(*EvalCtx.Result, OV);
  }
  case ReflectionKind::Null:
  case ReflectionKind::Value:
  case ReflectionKind::Type:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::Parameter:
  case ReflectionKind::Template:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_query_property)
           << 1 << DescriptionOf(RV) << EvalCtx.Range;
  }
  llvm_unreachable("unknown reflection kind");
}

bool constant_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsMetaInfo(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Value:
    return SetAndSucceed(*EvalCtx.Result, RV);
  case ReflectionKind::Object: {
    if (!RV.getTypeOfReflectedResult(*EvalCtx.C)->isStructuralType())
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_cannot_query_property)
             << 2 << "an object of non-structural type" << EvalCtx.Range;

    QualType ObjectTy = RV.getTypeOfReflectedResult(*EvalCtx.C);
    Expr *OVE = new (*EvalCtx.C)
        OpaqueValueExpr(EvalCtx.Range.getBegin(), ObjectTy, VK_LValue);
    Expr *CE = ConstantExpr::Create(*EvalCtx.C, OVE, RV.getReflectedObject());

    Expr::EvalResult ER;
    if (!CE->EvaluateAsRValue(ER, *EvalCtx.C, true))
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_cannot_query_property)
             << 2 << "an object not usable in constant expressions"
             << EvalCtx.Range;

    APValue Constant = ER.Val;
    QualType ConstantTy =
        ComputeResultType(RV.getTypeOfReflectedResult(*EvalCtx.C), Constant);
    if (ConstantTy->isRecordType()) {
      auto *TPO = EvalCtx.C->getTemplateParamObjectDecl(ConstantTy, Constant);
      Constant = APValue(APValue::LValueBase{TPO}, CharUnits::Zero(), {}, false,
                         false);
      ConstantTy = QualType{};
    }
    return SetAndSucceed(*EvalCtx.Result, Constant.Lift(ConstantTy));
  }
  case ReflectionKind::Declaration: {
    ValueDecl *Decl = RV.getReflectedDecl();

    APValue Constant;
    QualType QT;
    if (auto *VD = dyn_cast<VarDecl>(Decl)) {
      if (!VD->isUsableInConstantExpressions(*EvalCtx.C))
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_cannot_query_property)
               << 2 << "a variable not usable in constant expressions"
               << EvalCtx.Range;

      QT = VD->getType();
      if (auto *LVRT = dyn_cast<LValueReferenceType>(QT))
        QT = LVRT->getPointeeType();

      Expr *Synthesized = DeclRefExpr::Create(
          *EvalCtx.C, NestedNameSpecifierLoc(), SourceLocation(), VD, false,
          EvalCtx.Range.getBegin(), QT, VK_LValue, Decl, nullptr);
      if (!EvalCtx.Evaluator(Constant, Synthesized, true))
        llvm_unreachable("failed to evaluate variable usable in constant "
                         "expressions");
    } else if (isa<EnumConstantDecl>(Decl)) {
      Expr *Synthesized = DeclRefExpr::Create(
          *EvalCtx.C, NestedNameSpecifierLoc(), SourceLocation(), Decl, false,
          EvalCtx.Range.getBegin(), Decl->getType(), VK_PRValue, Decl, nullptr);
      QT = Synthesized->getType();

      Expr::EvalResult ER;
      if (!Synthesized->EvaluateAsConstantExpr(ER, *EvalCtx.C))
        llvm_unreachable("failed to evaluate enumerator constant");
      Constant = ER.Val;
    } else if (auto *TPOD = dyn_cast<TemplateParamObjectDecl>(Decl)) {
      Constant = TPOD->getValue();
      QT = TPOD->getType();
    } else {
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_cannot_query_property)
             << 2 << DescriptionOf(RV) << EvalCtx.Range;
    }

    QualType ConstantTy = ComputeResultType(QT, Constant);
    if (ConstantTy->isRecordType()) {
      auto *TPO = EvalCtx.C->getTemplateParamObjectDecl(ConstantTy, Constant);
      Constant = APValue(APValue::LValueBase{TPO}, CharUnits::Zero(), {}, false,
                         false);
      ConstantTy = QualType{};
    }

    return SetAndSucceed(*EvalCtx.Result, Constant.Lift(ConstantTy));
  }
  case ReflectionKind::Annotation: {
    CXX26AnnotationAttr *A = RV.getReflectedAnnotation();
    APValue Constant = RV.getReflectedAnnotation()->getValue();

    QualType ConstantTy = desugarType(A->getArg()->getType(),
                                      /*UnwrapAliases=*/true, /*DropCV=*/true,
                                      /*DropRefs=*/false);
    if (ConstantTy->isRecordType()) {
      auto *TPO = EvalCtx.C->getTemplateParamObjectDecl(ConstantTy, Constant);
      Constant = APValue(APValue::LValueBase{TPO}, CharUnits::Zero(), {}, false,
                         false);
      ConstantTy = QualType{};
    }
    return SetAndSucceed(*EvalCtx.Result, Constant.Lift(ConstantTy));
  }
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::Parameter:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::DataMemberSpec:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_query_property)
           << 2 << DescriptionOf(RV) << EvalCtx.Range;
  }
  llvm_unreachable("unknown reflection kind");
}

bool template_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsMetaInfo(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    TemplateName TName = findTemplateOfType(RV.getReflectedType());
    if (TName.isNull())
      return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                    "a template specialization");

    return SetReflectionAndSucceed(EvalCtx, TName);
  }
  case ReflectionKind::Declaration: {
    TemplateName TName = findTemplateOfDecl(RV.getReflectedDecl());
    if (TName.isNull())
      return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                    "a template specialization");

    return SetReflectionAndSucceed(EvalCtx, TName);
  }
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::Parameter:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "a template specialization",
                                  DescriptionOf(RV));
    return true;
  }
  llvm_unreachable("unknown reflection kind");
}

bool CanActAsTemplateArg(const APValue &RV) {
  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
    return true;
  case ReflectionKind::Declaration:
    return (!isa<FieldDecl>(RV.getReflectedDecl()));
  case ReflectionKind::Template: {
    TemplateDecl *TDecl = RV.getReflectedTemplate().getAsTemplateDecl();
    return isa<ClassTemplateDecl, TypeAliasTemplateDecl>(TDecl);
  }
  case ReflectionKind::Namespace:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
  case ReflectionKind::Null:
    return false;
  case ReflectionKind::EntityProxy:
    llvm_unreachable("expected proxies to have been unwrapped before calling");
  }
  llvm_unreachable("unknown reflection kind");
}

TemplateArgument TArgFromReflection(const MetaFunctionEvalContext &EvalCtx,
                                    const APValue &RV, SourceLocation Loc) {
  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type:
    return RV.getReflectedType().getCanonicalType();
  case ReflectionKind::Object: {
    QualType RefTy = EvalCtx.C->getLValueReferenceType(
        RV.getTypeOfReflectedResult(*EvalCtx.C));
    return TemplateArgument(*EvalCtx.C, RefTy, RV.getReflectedObject(), false);
  }
  case ReflectionKind::Value: {
    APValue Lowered = RV.getReflectedValue();
    QualType ActualResultTy = RV.getTypeOfReflectedResult(*EvalCtx.C);
    if (Lowered.isInt()) {
      return TemplateArgument(*EvalCtx.C, Lowered.getInt(),
                              ActualResultTy.getCanonicalType());
    }
    TemplateArgument TArg(*EvalCtx.C, ActualResultTy, Lowered, false);
    return TArg;
  }
  case ReflectionKind::Declaration: {
    ValueDecl *Decl = RV.getReflectedDecl();
    if (Decl->isInvalidDecl())
      break;

    if (!EvalCtx.Meta->EnsureInstantiated(Decl, SourceRange(Loc, Loc)))
      return {};

    QualType QT = desugarType(Decl->getType(), /*UnwrapAliases=*/false,
                              /*DropCV=*/false, /*DropRefs=*/true);

    // Don't worry about the cost of creating an expression here: The template
    // substitution machinery will otherwise create one from the argument
    // anyway, so we aren't really losing any efficiency here.
    Expr *Synthesized = DeclRefExpr::Create(
        *EvalCtx.C, NestedNameSpecifierLoc(), SourceLocation(), Decl, false,
        Loc, QT, VK_LValue, Decl, nullptr);

    return {Synthesized, true};
  }
  case ReflectionKind::Template:
    return {RV.getReflectedTemplate()};
    break;
  case ReflectionKind::EntityProxy:
    llvm_unreachable("expected proxies to have been unwrapped before calling");
  default:
    llvm_unreachable("unimplemented for template argument kind");
  }
  return {};
}

bool substitute(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckReflectionArrayArg(EvalCtx, 1);
  CheckIntegralArg(EvalCtx, 2);

  APValue Template;
  if (!EvalCtx.Evaluator(Template, EvalCtx.Args[0], true))
    return true;

  if (!Template.isReflectedTemplate())
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "a template", DescriptionOf(Template));

  TemplateDecl *TDecl = Template.getReflectedTemplate().getAsTemplateDecl();
  if (TDecl->isInvalidDecl())
    return true;

  APValue DiagnoseAPV;
  if (!EvalCtx.Evaluator(DiagnoseAPV, EvalCtx.Args[3], true))
    return true;
  bool NoDiagnose = !DiagnoseAPV.getInt().getBoolValue();
  auto ElideDiagnosis = [&] {
    return SetReflectionAndSucceed(EvalCtx, nullptr);
  };

  SmallVector<TemplateArgument, 4> TArgs;
  {
    // Evaluate how many template arguments were provided.
    APValue NumArgs;
    if (!EvalCtx.Evaluator(NumArgs, EvalCtx.Args[2], true))
      return true;
    size_t nArgs = NumArgs.getInt().getExtValue();
    TArgs.reserve(nArgs);

    for (uint64_t k = 0; k < nArgs; ++k) {
      llvm::APInt Idx(EvalCtx.C->getTypeSize(EvalCtx.C->getSizeType()), k,
                      false);
      Expr *Synthesized =
          IntegerLiteral::Create(*EvalCtx.C, Idx, EvalCtx.C->getSizeType(),
                                 EvalCtx.Args[1]->getExprLoc());

      Synthesized = new (*EvalCtx.C) ArraySubscriptExpr(
          EvalCtx.Args[1], Synthesized, EvalCtx.C->MetaInfoTy, VK_LValue,
          OK_Ordinary, EvalCtx.Range.getBegin());
      if (Synthesized->isValueDependent() || Synthesized->isTypeDependent())
        return true;

      APValue Unwrapped;
      if (!EvalCtx.Evaluator(Unwrapped, Synthesized, true) ||
          !Unwrapped.isReflection())
        return true;
      Unwrapped = MaybeUnproxy(*EvalCtx.C, Unwrapped);
      if (!CanActAsTemplateArg(Unwrapped))
        return NoDiagnose ? ElideDiagnosis()
                          : EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                              diag::metafn_cannot_be_arg)
                                << DescriptionOf(Unwrapped) << 1
                                << EvalCtx.Range;

      TemplateArgument TArg =
          TArgFromReflection(EvalCtx, Unwrapped, EvalCtx.Range.getBegin());
      if (TArg.isNull())
        return true;
      TArgs.push_back(TArg);
    }
  }

  SmallVector<TemplateArgument, 4> ExpandedTArgs;
  expandTemplateArgPacks(TArgs, ExpandedTArgs);

  // Lookup cached specialization; if found, return it.
  llvm::FoldingSetNodeID ID;
  {
    ID.AddPointer(TDecl);
    for (const TemplateArgument &TArg : ExpandedTArgs)
      TArg.Profile(ID, *EvalCtx.C);
  }
  unsigned SubstitutionHash = ID.ComputeHash();
  if (EvalCtx.C->checkCachedSubstitution(SubstitutionHash, EvalCtx.Result))
    return false;

  if (!EvalCtx.Meta->CheckTemplateArgumentList(TDecl, ExpandedTArgs, NoDiagnose,
                                               EvalCtx.Args[0]->getExprLoc()))
    return NoDiagnose ? ElideDiagnosis() : true;
  for (const auto &TArg : ExpandedTArgs)
    if (TArg.getKind() == TemplateArgument::Expression &&
        TArg.getAsExpr()->containsErrors())
      return true;

  if (auto *CTD = dyn_cast<ClassTemplateDecl>(TDecl)) {
    void *InsertPos;
    ClassTemplateSpecializationDecl *TSpecDecl =
        CTD->findSpecialization(ExpandedTArgs, InsertPos);

    if (!TSpecDecl) {
      TSpecDecl = ClassTemplateSpecializationDecl::Create(
          *EvalCtx.C, CTD->getTemplatedDecl()->getTagKind(),
          CTD->getDeclContext(), EvalCtx.Range.getBegin(),
          EvalCtx.Range.getBegin(), CTD, ExpandedTArgs, false, nullptr);
      CTD->AddSpecialization(TSpecDecl, InsertPos);
    }
    assert(TSpecDecl);

    // todo [merge:yukino:maybe-revert]
    APValue RV(ReflectionKind::Type,
               const_cast<Type *>(getTypeForDecl(TSpecDecl)));
    // EvalCtx.C->recordCachedSubstitution(SubstitutionHash, RV);
    return SetAndSucceed(*EvalCtx.Result, RV);
  }
  if (auto *TATD = dyn_cast<TypeAliasTemplateDecl>(TDecl)) {
    TArgs.clear();
    expandTemplateArgPacks(ExpandedTArgs, TArgs);

    QualType QT =
        EvalCtx.Meta->Substitute(TATD, TArgs, EvalCtx.Range.getBegin());
    if (QT.isNull()) {
      // substitution failed after validating arguments
      return true;
    }
    APValue RV = makeReflection(QT);
    // EvalCtx.C->recordCachedSubstitution(SubstitutionHash, RV);
    return SetReflectionAndSucceed(EvalCtx, QT);
  }
  if (auto *FTD = dyn_cast<FunctionTemplateDecl>(TDecl)) {
    FunctionDecl *Spec =
        EvalCtx.Meta->Substitute(FTD, ExpandedTArgs, EvalCtx.Range.getBegin());
    assert(Spec && "substitution failed after validating arguments?");

    if (Spec->getReturnType()->isUndeducedType())
      return NoDiagnose ? ElideDiagnosis()
                        : EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                            diag::metafn_undeduced_placeholder)
                              << Spec << Spec->getType() << EvalCtx.Range;

    APValue RV = makeReflection(Spec);
    // EvalCtx.C->recordCachedSubstitution(SubstitutionHash, RV);
    return SetAndSucceed(*EvalCtx.Result, RV);
  }
  if (auto *VTD = dyn_cast<VarTemplateDecl>(TDecl)) {
    TArgs.clear();
    expandTemplateArgPacks(ExpandedTArgs, TArgs);

    VarDecl *Spec =
        EvalCtx.Meta->Substitute(VTD, TArgs, EvalCtx.Range.getBegin());
    assert(Spec && "substitution failed after validating arguments?");

    APValue RV = makeReflection(Spec);
    // EvalCtx.C->recordCachedSubstitution(SubstitutionHash, RV);
    return SetReflectionAndSucceed(EvalCtx, Spec);
  }
  if (auto *CD = dyn_cast<ConceptDecl>(TDecl)) {
    TArgs.clear();
    expandTemplateArgPacks(ExpandedTArgs, TArgs);

    Expr *Spec = EvalCtx.Meta->Substitute(CD, TArgs, EvalCtx.Range.getBegin());
    assert(Spec && "substitution failed after validating arguments?");

    APValue SatisfiesConcept;
    if (!EvalCtx.Evaluator(SatisfiesConcept, Spec, true))
      llvm_unreachable("failed to evaluate substituted concept");

    APValue RV = SatisfiesConcept.Lift(EvalCtx.C->BoolTy);
    // EvalCtx.C->recordCachedSubstitution(SubstitutionHash, RV);
    return SetAndSucceed(*EvalCtx.Result,
                         SatisfiesConcept.Lift(EvalCtx.C->BoolTy));
  }
  llvm_unreachable("unimplemented for template kind");
}

bool extract(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckReflectionArg(EvalCtx, 1);

  bool ReturnsLValue = false;
  QualType RawResultTy = EvalCtx.ResultTy;
  if (auto *LVRT = dyn_cast<LValueReferenceType>(RawResultTy)) {
    ReturnsLValue = true;
    RawResultTy = LVRT->getPointeeType();
  }

  auto extractLambda = [&](APValue &Out, CXXRecordDecl *RD) -> bool {
    if (!RD->isCapturelessLambda())
      return true;

    CXXMethodDecl *CallOp = RD->getLambdaStaticInvoker();
    QualType LambdaPtrTy = EvalCtx.C->getPointerType(CallOp->getType());

    if (LambdaPtrTy.getCanonicalType().getTypePtr() !=
        RawResultTy.getCanonicalType().getTypePtr())
      // todo [merge:yukino:maybe-revert]
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_extract_type_mismatch)
             << 0 << QualType(getTypeForDecl(RD), 0) << 0 << RawResultTy
             << EvalCtx.Range;

    // If not already done, generate a fake body for the call-operator.
    // The real body is generated during CodeGen.
    if (!CallOp->hasBody()) {
      CallOp->markUsed(*EvalCtx.C);
      CallOp->setReferenced();
      CallOp->setBody(new (*EvalCtx.C) CompoundStmt(EvalCtx.Range.getBegin()));
    }

    APValue CallOpLV(CallOp, CharUnits::Zero(), {}, false, false);
    return SetAndSucceed(Out, CallOpLV);
  };

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[1], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Object: {
    QualType ObjectTy = RV.getTypeOfReflectedResult(*EvalCtx.C);

    if (auto *RD = ObjectTy->getAsCXXRecordDecl();
        RD && RD->isLambda() && RawResultTy->isPointerType())
      return extractLambda(*EvalCtx.Result, RD);

    if (ObjectTy.getCanonicalType().getTypePtr() !=
        RawResultTy.getCanonicalType().getTypePtr())
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_extract_type_mismatch)
             << 1 << ObjectTy << ReturnsLValue << RawResultTy << EvalCtx.Range;

    Expr *OVE = new (*EvalCtx.C)
        OpaqueValueExpr(EvalCtx.Range.getBegin(), ObjectTy, VK_LValue);
    Expr *CE = ConstantExpr::Create(*EvalCtx.C, OVE, RV.getReflectedObject());

    if (!EvalCtx.Evaluator(RV, CE, !ReturnsLValue))
      return true;

    return SetAndSucceed(*EvalCtx.Result, RV);
  }
  case ReflectionKind::Value: {
    QualType ValueTy = RV.getTypeOfReflectedResult(*EvalCtx.C);
    if (ReturnsLValue)
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_cannot_extract)
             << 1 << DescriptionOf(RV) << EvalCtx.Range;

    if (ValueTy.getCanonicalType().getTypePtr() !=
        RawResultTy.getCanonicalType().getTypePtr())
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_extract_type_mismatch)
             << 0 << ValueTy << ReturnsLValue << RawResultTy << EvalCtx.Range;

    return SetAndSucceed(*EvalCtx.Result, RV.getReflectedValue());
  }
  case ReflectionKind::Annotation: {
    if (ReturnsLValue)
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_cannot_extract)
             << 1 << DescriptionOf(RV) << EvalCtx.Range;

    CXX26AnnotationAttr *A = RV.getReflectedAnnotation();
    if (auto *RD = A->getArg()->getType()->getAsCXXRecordDecl();
        RD && RD->isLambda() && RawResultTy->isPointerType())
      return extractLambda(*EvalCtx.Result, RD);

    if (A->getArg()->getType().getCanonicalType().getTypePtr() !=
        RawResultTy.getCanonicalType().getTypePtr())
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_extract_type_mismatch)
             << 3 << A->getArg()->getType() << ReturnsLValue << RawResultTy
             << EvalCtx.Range;

    return SetAndSucceed(*EvalCtx.Result, A->getValue());
  }
  case ReflectionKind::Declaration: {
    ValueDecl *Decl = RV.getReflectedDecl();
    EvalCtx.Meta->EnsureInstantiated(Decl, EvalCtx.Args[1]->getSourceRange());

    if (auto *RD = Decl->getType()->getAsCXXRecordDecl();
        RD && RD->isLambda() && RawResultTy->isPointerType())
      return extractLambda(*EvalCtx.Result, RD);

    if (isa<VarDecl, TemplateParamObjectDecl>(Decl)) {
      Expr *Synthesized;
      if (isa<LValueReferenceType>(Decl->getType().getCanonicalType())) {
        // We have a reflection of an object with reference type.
        // Synthesize a 'DeclRefExpr' designating the object, such that constant
        // evaluation resolves the underlying referenced entity.
        ReturnsLValue = true;
        if (RawResultTy.getCanonicalType().getTypePtr() !=
            Decl->getType().getCanonicalType().getTypePtr())
          return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                   diag::metafn_extract_type_mismatch)
                 << 1 << Decl->getType() << 1 << RawResultTy << EvalCtx.Range;

        NestedNameSpecifierLocBuilder NNSLocBuilder;
        if (auto *ParentClsDecl =
                dyn_cast_or_null<CXXRecordDecl>(Decl->getDeclContext())) {
          // todo [merge:yukino:maybe-revert]
          TypeSourceInfo *TSI = EvalCtx.C->CreateTypeSourceInfo(
              QualType(getTypeForDecl(ParentClsDecl), 0), 0);
          NNSLocBuilder.Make(*EvalCtx.C, TSI->getTypeLoc(),
                             EvalCtx.Range.getBegin());
        }
        Synthesized = DeclRefExpr::Create(
            *EvalCtx.C, NNSLocBuilder.getTemporary(), SourceLocation(), Decl,
            false, EvalCtx.Range.getBegin(), RawResultTy, VK_LValue, Decl,
            nullptr);
      } else if (auto *ArrTy = dyn_cast<ArrayType>(Decl->getType())) {
        QualType Elt = ArrTy->getElementType();
        if (auto *VD = dyn_cast<VarDecl>(Decl)) {
          if (VD->isConstexpr()) {
            Elt.addConst();
          }
        }

        ReturnsLValue = true;
        if (!RawResultTy->isPointerType() ||
            !RawResultTy->getPointeeType().isAtLeastAsQualifiedAs(Elt,
                                                                  *EvalCtx.C))
          return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                   diag::metafn_extract_type_mismatch)
                 << 1 << EvalCtx.C->getPointerType(Elt) << 1 << RawResultTy
                 << EvalCtx.Range;

        NestedNameSpecifierLocBuilder NNSLocBuilder;
        if (auto *ParentClsDecl =
                dyn_cast_or_null<CXXRecordDecl>(Decl->getDeclContext())) {
          // todo [merge:yukino:maybe-revert]
          TypeSourceInfo *TSI = EvalCtx.C->CreateTypeSourceInfo(
              QualType(getTypeForDecl(ParentClsDecl), 0), 0);
          NNSLocBuilder.Make(*EvalCtx.C, TSI->getTypeLoc(),
                             EvalCtx.Range.getBegin());
        }

        APValue::LValuePathEntry Path[1] = {
            APValue::LValuePathEntry::ArrayIndex(0)};
        return SetAndSucceed(*EvalCtx.Result,
                             APValue(Decl, CharUnits::Zero(), Path, false));
      } else {
        // We have a reflection of a (possibly local) non-reference variable.
        // Synthesize an lvalue by reaching up the call stack.
        if (RawResultTy.getCanonicalType().getTypePtr() !=
            Decl->getType().getCanonicalType().getTypePtr())
          return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                   diag::metafn_extract_type_mismatch)
                 << 0 << Decl->getType() << ReturnsLValue << RawResultTy
                 << EvalCtx.Range;

        Synthesized = ExtractLValueExpr::Create(*EvalCtx.C, EvalCtx.Range,
                                                RawResultTy, Decl);
      }

      if (Synthesized->getType().getCanonicalType().getTypePtr() !=
          RawResultTy.getCanonicalType().getTypePtr())
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_extract_type_mismatch)
               << 0 << Decl->getType() << ReturnsLValue << RawResultTy
               << EvalCtx.Range;
      return !EvalCtx.Evaluator(*EvalCtx.Result, Synthesized, !ReturnsLValue);
    } else if (isa<BindingDecl>(Decl)) {
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_extract_structured_binding)
             << EvalCtx.Range;

    } else if (ReturnsLValue) {
      // Only variables may be returned as LValues.
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_cannot_extract)
             << 1 << DescriptionOf(RV);
    } else if (isa<FieldDecl, CXXMethodDecl>(Decl)) {
      // Extracting a non-static member as a pointer.
      if (auto *FD = dyn_cast<FieldDecl>(Decl); FD && FD->isBitField())
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_cannot_extract)
               << 2 << DescriptionOf(RV) << EvalCtx.Range;

      DeclContext *ObjDC = Decl->getDeclContext();
      while (ObjDC && [](DeclContext *DC) {
        if (auto *RD = dyn_cast<CXXRecordDecl>(DC))
          return RD->isAnonymousStructOrUnion();
        else
          return DC->isTransparentContext();
      }(ObjDC))
        if (isa<TranslationUnitDecl>(ObjDC))
          // Can happen if Target was a member of a static anonymous union at
          // namespace scope.
          return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                   diag::metafn_cannot_extract)
                 << 2 << "a field that is not a member of a class";
        else
          ObjDC = ObjDC->getParent();

      // todo [merge:yukino:maybe-revert]
      QualType MemPtrTy = EvalCtx.C->getMemberPointerType(
          Decl->getType(), NestedNameSpecifier::getInvalid(),
          cast<CXXRecordDecl>(ObjDC));
      if (MemPtrTy.getCanonicalType().getTypePtr() !=
          RawResultTy.getCanonicalType().getTypePtr())
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_extract_entity_type_mismatch)
               << RawResultTy << DescriptionOf(RV) << MemPtrTy << EvalCtx.Range;

      APValue MemPtrLV(Decl, false, ArrayRef<const CXXRecordDecl *>{});
      return SetAndSucceed(*EvalCtx.Result, MemPtrLV);
    } else if (auto *ECD = dyn_cast<EnumConstantDecl>(Decl)) {
      if (ECD->getType().getCanonicalType().getTypePtr() !=
          RawResultTy.getCanonicalType().getTypePtr())
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_extract_type_mismatch)
               << 2 << Decl->getType() << 0 << RawResultTy << EvalCtx.Range;

      return SetAndSucceed(*EvalCtx.Result, APValue(ECD->getInitVal()));
    } else {
      QualType FnPtrTy = EvalCtx.C->getPointerType(Decl->getType());
      if (FnPtrTy.getCanonicalType().getTypePtr() !=
          RawResultTy.getCanonicalType().getTypePtr())
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_extract_type_mismatch)
               << 0 << Decl->getType() << ReturnsLValue << RawResultTy
               << EvalCtx.Range;

      return SetAndSucceed(*EvalCtx.Result,
                           APValue(Decl, CharUnits::Zero(), {}, false, false));
    }
  }
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::Parameter:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::DataMemberSpec:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_extract)
           << (ReturnsLValue ? 1 : 0) << DescriptionOf(RV) << EvalCtx.Range;
  }
  llvm_unreachable("invalid reflection type");
}

template <AccessSpecifier Specifier>
bool is_ACCESS(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    bool HasTargetAccess = false;
    if (const Decl *D = findTypeDecl(RV.getReflectedType()))
      HasTargetAccess = (D->getAccess() == Specifier);

    return SetBoolAndSucceed(EvalCtx, HasTargetAccess);
  }
  case ReflectionKind::Declaration: {
    bool HasTargetAccess = (RV.getReflectedDecl()->getAccess() == Specifier);
    return SetBoolAndSucceed(EvalCtx, HasTargetAccess);
  }
  case ReflectionKind::EntityProxy: {
    bool HasTargetAccess =
        (RV.getReflectedEntityProxy()->getAccess() == Specifier);
    return SetBoolAndSucceed(EvalCtx, HasTargetAccess);
  }
  case ReflectionKind::Template: {
    const Decl *D = RV.getReflectedTemplate().getAsTemplateDecl();

    bool HasTargetAccess = (D->getAccess() == Specifier);
    return SetBoolAndSucceed(EvalCtx, HasTargetAccess);
  }
  case ReflectionKind::BaseSpecifier: {
    CXXBaseSpecifier *Base = RV.getReflectedBaseSpecifier();
    bool HasTargetAccess = (Base->getAccessSpecifier() == Specifier);
    return SetBoolAndSucceed(EvalCtx, HasTargetAccess);
  }
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Parameter:
  case ReflectionKind::Annotation:
  case ReflectionKind::Namespace:
    return SetBoolAndSucceed(EvalCtx, false);
  }
  llvm_unreachable("invalid reflection type");
}

template <AccessSpecifier AS>
inline bool is_ClassMember_ACCESS(const MetaFunctionEvalContext &EvalCtx) {
  [[maybe_unused]] bool scratch = is_class_member(EvalCtx);

  if (const bool isClassMember = EvalCtx.Result->getInt().getBoolValue();
      isClassMember) {
    return is_ACCESS<AS>(EvalCtx);
  }
  // fallthrough: base-class relationship
  scratch = is_base(EvalCtx);
  if (const bool isBaseClass = EvalCtx.Result->getInt().getBoolValue();
      isBaseClass) {
    return is_ACCESS<AS>(EvalCtx);
  }
  return false;
}

bool is_public(const MetaFunctionEvalContext &EvalCtx) {
  return is_ClassMember_ACCESS<AS_public>(EvalCtx);
}

bool is_protected(const MetaFunctionEvalContext &EvalCtx) {
  return is_ClassMember_ACCESS<AS_protected>(EvalCtx);
}

bool is_private(const MetaFunctionEvalContext &EvalCtx) {
  return is_ClassMember_ACCESS<AS_private>(EvalCtx);
}

bool is_virtual(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsVirtual = false;
  switch (RV.getReflectionKind()) {
  case ReflectionKind::Declaration: {
    if (auto *MD = dyn_cast<CXXMethodDecl>(RV.getReflectedDecl()))
      IsVirtual = MD->isVirtual();
    return SetBoolAndSucceed(EvalCtx, IsVirtual);
  }
  case ReflectionKind::BaseSpecifier: {
    IsVirtual = RV.getReflectedBaseSpecifier()->isVirtual();
    return SetBoolAndSucceed(EvalCtx, IsVirtual);
  }
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return SetBoolAndSucceed(EvalCtx, IsVirtual);
  }
  llvm_unreachable("invalid reflection type");
}

bool is_pure_virtual(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsPureVirtual = false;
  if (RV.isReflectedDecl())
    if (const auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl()))
      IsPureVirtual = FD->isPureVirtual();

  return SetBoolAndSucceed(EvalCtx, IsPureVirtual);
}

bool is_override(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsOverride = false;
  if (RV.isReflectedDecl())
    if (auto *MD = dyn_cast<CXXMethodDecl>(RV.getReflectedDecl()))
      IsOverride = MD->size_overridden_methods() > 0;

  return SetBoolAndSucceed(EvalCtx, IsOverride);
}

bool is_deleted(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsDeleted = false;
  if (RV.isReflectedDecl())
    if (auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl()))
      IsDeleted = FD->isDeleted();

  return SetBoolAndSucceed(EvalCtx, IsDeleted);
}

bool is_defaulted(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsDefaulted = false;
  if (RV.isReflectedDecl())
    if (auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl()))
      IsDefaulted = FD->getMostRecentDecl()->isDefaulted();

  return SetBoolAndSucceed(EvalCtx, IsDefaulted);
}

bool is_explicit(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsExplicit = false;
  if (RV.isReflectedDecl()) {
    if (auto *CtorD = dyn_cast<CXXConstructorDecl>(RV.getReflectedDecl()))
      IsExplicit = CtorD->getExplicitSpecifier().isExplicit();
    else if (auto *ConvD = dyn_cast<CXXConversionDecl>(RV.getReflectedDecl()))
      IsExplicit = ConvD->getExplicitSpecifier().isExplicit();
  }

  return SetBoolAndSucceed(EvalCtx, IsExplicit);
}

bool is_noexcept(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsNoexcept = false;
  if (RV.isReflectedType())
    IsNoexcept = isFunctionOrMethodNoexcept(RV.getReflectedType());
  else if (RV.isReflectedDecl()) {
    if (auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl()))
      EvalCtx.Meta->EnsureInstantiationOfExceptionSpec(EvalCtx.Range.getBegin(),
                                                       FD);

    IsNoexcept = isFunctionOrMethodNoexcept(RV.getReflectedDecl()->getType());
  }

  return SetBoolAndSucceed(EvalCtx, IsNoexcept);
}

bool is_bit_field(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl()) {
    if (const auto *FD = dyn_cast<FieldDecl>(RV.getReflectedDecl()))
      result = FD->isBitField();
    else if (const auto *BD = dyn_cast<BindingDecl>(RV.getReflectedDecl()))
      result = BD->getBinding()->refersToBitField();
  } else if (RV.isReflectedDataMemberSpec()) {
    result = RV.getReflectedDataMemberSpec()->BitWidth.has_value();
  }
  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_enumerator(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl())
    result = isa<EnumConstantDecl>(RV.getReflectedDecl());

  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_final(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedType()) {
    if (auto *recordDecl = dyn_cast<CXXRecordDecl>(
            RV.getReflectedType()->getAsCXXRecordDecl())) {
      result = recordDecl->hasAttr<FinalAttr>();
    }
  } else if (RV.isReflectedDecl()) {
    if (auto *funcDecl = dyn_cast<CXXMethodDecl>(RV.getReflectedDecl())) {
      result = funcDecl->hasAttr<FinalAttr>();
    } else if (auto *recordDecl =
                   dyn_cast<CXXRecordDecl>(RV.getReflectedDecl())) {
      result = recordDecl->hasAttr<FinalAttr>();
    }
  }
  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_const(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Null:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::Parameter:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return SetBoolAndSucceed(EvalCtx, false);
  case ReflectionKind::Type: {
    bool result = isConstQualifiedType(RV.getReflectedType());
    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::Declaration: {
    bool result = isConstQualifiedType(RV.getReflectedDecl()->getType());
    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::Object:
  case ReflectionKind::Value: {
    bool result = isConstQualifiedType(RV.getTypeOfReflectedResult(*EvalCtx.C));
    return SetBoolAndSucceed(EvalCtx, result);
  }
  }
  llvm_unreachable("invalid reflection type");
}

bool is_volatile(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Null:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::Parameter:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return SetBoolAndSucceed(EvalCtx, false);
  case ReflectionKind::Type: {
    bool result = isVolatileQualifiedType(RV.getReflectedType());

    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::Declaration: {
    bool result = isVolatileQualifiedType(RV.getReflectedDecl()->getType());
    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::Object:
  case ReflectionKind::Value: {
    bool result =
        isVolatileQualifiedType(RV.getTypeOfReflectedResult(*EvalCtx.C));

    return SetBoolAndSucceed(EvalCtx, result);
  }
  }
  llvm_unreachable("invalid reflection type");
}

bool is_mutable_member(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsMutableMember = false;
  if (RV.isReflectedDecl())
    if (auto *FD = dyn_cast<FieldDecl>(RV.getReflectedDecl()))
      IsMutableMember = FD->isMutable();

  return SetBoolAndSucceed(EvalCtx, IsMutableMember);
}

bool is_lvalue_reference_qualified(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedType()) {
    if (auto FT = dyn_cast<FunctionProtoType>(RV.getReflectedType()))
      result = (FT->getRefQualifier() == RQ_LValue);
  } else if (RV.isReflectedDecl()) {
    if (const auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl()))
      if (auto FT = dyn_cast<FunctionProtoType>(FD->getType()))
        result = (FT->getRefQualifier() == RQ_LValue);
  }
  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_rvalue_reference_qualified(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedType()) {
    if (auto FT = dyn_cast<FunctionProtoType>(RV.getReflectedType()))
      result = (FT->getRefQualifier() == RQ_RValue);
  } else if (RV.isReflectedDecl()) {
    if (const auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl()))
      if (auto FT = dyn_cast<FunctionProtoType>(FD->getType()))
        result = (FT->getRefQualifier() == RQ_RValue);
  }
  return SetBoolAndSucceed(EvalCtx, result);
}

bool has_static_storage_duration(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl()) {
    if (const auto *VD = dyn_cast<VarDecl>(RV.getReflectedDecl()))
      result = VD->getStorageDuration() == SD_Static;
    else if (isa<TemplateParamObjectDecl>(RV.getReflectedDecl()))
      result = true;
  } else if (RV.isReflectedObject()) {
    result = true;
  }
  return SetBoolAndSucceed(EvalCtx, result);
}

bool has_thread_storage_duration(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl())
    if (const auto *VD = dyn_cast<VarDecl>(RV.getReflectedDecl()))
      result = VD->getStorageDuration() == SD_Thread;

  return SetBoolAndSucceed(EvalCtx, result);
}

bool has_automatic_storage_duration(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl())
    if (const auto *VD = dyn_cast<VarDecl>(RV.getReflectedDecl()))
      result = VD->getStorageDuration() == SD_Automatic;

  return SetBoolAndSucceed(EvalCtx, result);
}

bool has_internal_linkage(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedType()) {
    if (NamedDecl *typeDecl =
            dyn_cast_or_null<NamedDecl>(findTypeDecl(RV.getReflectedType())))
      result = (typeDecl->getFormalLinkage() == Linkage::Internal);
  } else if (RV.isReflectedDecl()) {
    if (const auto *ND = dyn_cast<NamedDecl>(RV.getReflectedDecl()))
      result = (ND->getFormalLinkage() == Linkage::Internal);
  } else if (RV.isReflectedObject()) {
    if (APValue::LValueBase LVBase = RV.getReflectedObject().getLValueBase();
        LVBase.is<const ValueDecl *>()) {
      const ValueDecl *VD = LVBase.get<const ValueDecl *>();
      result = (VD->getFormalLinkage() == Linkage::Internal);
    }
  }
  return SetBoolAndSucceed(EvalCtx, result);
}

bool has_module_linkage(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedType()) {
    if (NamedDecl *typeDecl =
            dyn_cast_or_null<NamedDecl>(findTypeDecl(RV.getReflectedType())))
      result = (typeDecl->getFormalLinkage() == Linkage::Module);
  } else if (RV.isReflectedDecl()) {
    if (const auto *ND = dyn_cast<NamedDecl>(RV.getReflectedDecl()))
      result = (ND->getFormalLinkage() == Linkage::Module);
  } else if (RV.isReflectedObject()) {
    if (APValue::LValueBase LVBase = RV.getReflectedObject().getLValueBase();
        LVBase.is<const ValueDecl *>()) {
      const ValueDecl *VD = LVBase.get<const ValueDecl *>();
      result = (VD->getFormalLinkage() == Linkage::Module);
    }
  }
  return SetBoolAndSucceed(EvalCtx, result);
}

bool has_external_linkage(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedType()) {
    if (NamedDecl *typeDecl =
            dyn_cast_or_null<NamedDecl>(findTypeDecl(RV.getReflectedType())))
      result = (typeDecl->getFormalLinkage() == Linkage::External ||
                typeDecl->getFormalLinkage() == Linkage::UniqueExternal);
  } else if (RV.isReflectedDecl()) {
    if (const auto *ND = dyn_cast<NamedDecl>(RV.getReflectedDecl()))
      result = (ND->getFormalLinkage() == Linkage::External ||
                ND->getFormalLinkage() == Linkage::UniqueExternal);
  } else if (RV.isReflectedObject()) {
    if (APValue::LValueBase LVBase = RV.getReflectedObject().getLValueBase();
        LVBase.is<const ValueDecl *>()) {
      const ValueDecl *VD = LVBase.get<const ValueDecl *>();
      result = (VD->getFormalLinkage() == Linkage::External ||
                VD->getFormalLinkage() == Linkage::UniqueExternal);
    }
  }
  return SetBoolAndSucceed(EvalCtx, result);
}

bool has_linkage(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedType()) {
    if (NamedDecl *typeDecl =
            dyn_cast_or_null<NamedDecl>(findTypeDecl(RV.getReflectedType())))
      result = typeDecl->hasLinkage();
  } else if (RV.isReflectedDecl()) {
    if (const auto *ND = dyn_cast<NamedDecl>(RV.getReflectedDecl()))
      result = ND->hasLinkage();
  } else if (RV.isReflectedObject()) {
    if (APValue::LValueBase LVBase = RV.getReflectedObject().getLValueBase();
        LVBase.is<const ValueDecl *>()) {
      const ValueDecl *VD = LVBase.get<const ValueDecl *>();
      result = (VD->hasLinkage());
    }
  }
  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_class_member(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);
  // APValue Scratch;
  bool result = false;

  auto SCC = EvalCtx.getScratchContextCreator();
  auto ScratchCtx = SCC();
  // decltype(EvalCtx.Diagnoser) SwallowDiags{};
  if (!parent_of(ScratchCtx)) {
    assert(ScratchCtx.Result->isReflection());
    // For unscoped enumerators, parent_of will return its enumeration type
    // We need now to lookup context on that type
    if (ScratchCtx.Result->isReflectedType() &&
        ScratchCtx.Result->getReflectedType()->isUnscopedEnumerationType()) {
      Decl *D = findTypeDecl(ScratchCtx.Result->getReflectedType());
      result = D && D->getDeclContext() && D->getDeclContext()->isRecord();
    } else {
      result = ScratchCtx.Result->isReflectedType() &&
               ScratchCtx.Result->getReflectedType()->isRecordType();
    }
  }
  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_namespace_member(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  // APValue Scratch;
  bool result = false;

  auto SCC = EvalCtx.getScratchContextCreator();
  auto ScratchCtx = SCC();
  // decltype(EvalCtx.Diagnoser) SwallowDiags{};
  if (!parent_of(ScratchCtx)) {
    assert(ScratchCtx.Result->isReflection());
    result = ScratchCtx.Result->isReflectedNamespace();
  }
  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_nonstatic_data_member(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl()) {
    if (auto *FD = dyn_cast<FieldDecl>(RV.getReflectedDecl())) {
      // Unnamed bit-fields are not members, but just about every other field
      // should be a nonstatic data member.
      result = (!FD->isBitField() || FD->getIdentifier());
    }
  }
  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_static_member(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  switch (RV.getReflectionKind()) {
  case ReflectionKind::Declaration: {
    const ValueDecl *D = cast<ValueDecl>(RV.getReflectedDecl());
    if (const auto *MD = dyn_cast<CXXMethodDecl>(D))
      result = MD->isStatic();
    else if (const auto *VD = dyn_cast<VarDecl>(D))
      result = VD->isStaticDataMember();
    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::Template: {
    const Decl *D = RV.getReflectedTemplate().getAsTemplateDecl();
    if (const auto *FTD = dyn_cast<FunctionTemplateDecl>(D)) {
      if (const auto *MD = dyn_cast<CXXMethodDecl>(FTD->getTemplatedDecl()))
        result = MD->isStatic();
    } else if (const auto *VTD = dyn_cast<VarTemplateDecl>(D)) {
      if (const auto *VD = dyn_cast<VarDecl>(VTD->getTemplatedDecl()))
        result = VD->isStaticDataMember();
    }
    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Namespace:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return SetBoolAndSucceed(EvalCtx, result);
  case ReflectionKind::EntityProxy:
    llvm_unreachable("proxies should already have been unwrapped");
  }
  llvm_unreachable("unknown reflection kind");
}

bool is_base(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  return SetBoolAndSucceed(EvalCtx, RV.isReflectedBaseSpecifier());
}

bool is_data_member_spec(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  return SetBoolAndSucceed(EvalCtx, RV.isReflectedDataMemberSpec());
}

bool is_namespace(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  return SetBoolAndSucceed(EvalCtx, RV.isReflectedNamespace());
}

bool is_function(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl())
    result = isa<const FunctionDecl>(RV.getReflectedDecl());
  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_variable(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl())
    result = isa<const VarDecl>(RV.getReflectedDecl());
  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_type(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  return SetBoolAndSucceed(EvalCtx, RV.isReflectedType());
}

bool is_alias(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    bool result = isTypeAlias(RV.getReflectedType());
    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::Namespace: {
    bool result = isa<NamespaceAliasDecl>(RV.getReflectedNamespace());
    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::Template: {
    TemplateDecl *TDecl = RV.getReflectedTemplate().getAsTemplateDecl();
    bool result = isa<TypeAliasTemplateDecl>(TDecl);
    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Declaration:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Parameter:
  case ReflectionKind::Annotation:
  case ReflectionKind::EntityProxy:
    return SetBoolAndSucceed(EvalCtx, false);
  }
  llvm_unreachable("unknown reflection kind");
}

bool is_entity_proxy(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  return SetBoolAndSucceed(EvalCtx, RV.isReflectedEntityProxy());
}

bool is_complete_type(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedType()) {
    // If this is a declared type with a reachable definition, ensure that the
    // type is instantiated.
    if (Decl *typeDecl = findTypeDecl(RV.getReflectedType()))
      (void)EvalCtx.Meta->EnsureInstantiated(typeDecl, EvalCtx.Range);

    result = !RV.getReflectedType()->isIncompleteType();
  }
  return SetBoolAndSucceed(EvalCtx, result);
}

bool has_complete_definition(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type:
    if (Decl *typeDecl = findTypeDecl(RV.getReflectedType())) {
      if (auto *TD = dyn_cast<TagDecl>(typeDecl))
        result = (TD->getDefinition() != nullptr &&
                  !TD->getDefinition()->isBeingDefined());
    }
    break;
  case ReflectionKind::Declaration: {
    if (auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl()))
      result =
          (FD->getDefinition() != nullptr && FD->getDefinition()->hasBody());
    break;
  }
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    break;
  case ReflectionKind::EntityProxy:
    llvm_unreachable("proxies should already have been unwrapped");
  }

  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_enumerable_type(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type:
    if (Decl *typeDecl = findTypeDecl(RV.getReflectedType())) {
      if (auto *TD = dyn_cast<TagDecl>(typeDecl)) {
        (void)EvalCtx.Meta->EnsureInstantiated(TD, EvalCtx.Range);
        result = (TD->getDefinition() != nullptr &&
                  !TD->getDefinition()->isBeingDefined());
      }
    }
    break;
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Declaration:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    break;
  case ReflectionKind::EntityProxy:
    llvm_unreachable("proxies should already have been unwrapped");
  }

  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_template(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  return SetBoolAndSucceed(EvalCtx, RV.isReflectedTemplate());
}

bool is_function_template(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsFnTemplate = false;
  if (RV.isReflectedTemplate()) {
    const TemplateDecl *TD = RV.getReflectedTemplate().getAsTemplateDecl();
    IsFnTemplate = isa<FunctionTemplateDecl>(TD);
  }
  return SetBoolAndSucceed(EvalCtx, IsFnTemplate);
}

bool is_variable_template(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsVarTemplate = false;
  if (RV.isReflectedTemplate()) {
    const TemplateDecl *TD = RV.getReflectedTemplate().getAsTemplateDecl();
    IsVarTemplate = isa<VarTemplateDecl>(TD);
  }
  return SetBoolAndSucceed(EvalCtx, IsVarTemplate);
}

bool is_class_template(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsClsTemplate = false;
  if (RV.isReflectedTemplate()) {
    const TemplateDecl *TD = RV.getReflectedTemplate().getAsTemplateDecl();
    IsClsTemplate = isa<ClassTemplateDecl>(TD);
  }
  return SetBoolAndSucceed(EvalCtx, IsClsTemplate);
}

bool is_alias_template(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsAliasTemplate = false;
  if (RV.isReflectedTemplate()) {
    const TemplateDecl *TD = RV.getReflectedTemplate().getAsTemplateDecl();
    IsAliasTemplate = TD->isTypeAlias();
  }
  return SetBoolAndSucceed(EvalCtx, IsAliasTemplate);
}

bool is_conversion_function_template(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsConversionTemplate = false;
  if (RV.isReflectedTemplate()) {
    const TemplateDecl *TD = RV.getReflectedTemplate().getAsTemplateDecl();
    if (auto *FTD = dyn_cast<FunctionTemplateDecl>(TD))
      IsConversionTemplate = isa<CXXConversionDecl>(FTD->getTemplatedDecl());
  }
  return SetBoolAndSucceed(EvalCtx, IsConversionTemplate);
}

bool is_operator_function_template(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsOperatorTemplate = false;
  if (RV.isReflectedTemplate()) {
    const TemplateDecl *TD = RV.getReflectedTemplate().getAsTemplateDecl();
    if (auto *FTD = dyn_cast<FunctionTemplateDecl>(TD))
      IsOperatorTemplate =
          (FTD->getTemplatedDecl()->getOverloadedOperator() != OO_None);
  }
  return SetBoolAndSucceed(EvalCtx, IsOperatorTemplate);
}

bool is_literal_operator_template(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsLiteralOperator = false;
  if (RV.isReflectedTemplate()) {
    const TemplateDecl *TD = RV.getReflectedTemplate().getAsTemplateDecl();
    if (auto *FTD = dyn_cast<FunctionTemplateDecl>(TD))
      IsLiteralOperator = FTD->getDeclName().getNameKind() ==
                          DeclarationName::CXXLiteralOperatorName;
  }
  return SetBoolAndSucceed(EvalCtx, IsLiteralOperator);
}

bool is_constructor_template(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsCtorTemplate = false;
  if (RV.isReflectedTemplate()) {
    const TemplateDecl *TD = RV.getReflectedTemplate().getAsTemplateDecl();
    if (auto *FTD = dyn_cast<FunctionTemplateDecl>(TD))
      IsCtorTemplate = isa<CXXConstructorDecl>(FTD->getTemplatedDecl());
  }
  return SetBoolAndSucceed(EvalCtx, IsCtorTemplate);
}

bool is_concept(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsConcept = false;
  if (RV.isReflectedTemplate())
    IsConcept = isa<ConceptDecl>(RV.getReflectedTemplate().getAsTemplateDecl());

  return SetBoolAndSucceed(EvalCtx, IsConcept);
}

bool is_structured_binding(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl())
    result = isa<BindingDecl>(RV.getReflectedDecl());

  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_value(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  return SetBoolAndSucceed(EvalCtx, RV.isReflectedValue());
}

bool is_object(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsObject = RV.isReflectedObject();
  if (RV.isReflectedDecl())
    IsObject = isa<TemplateParamObjectDecl>(RV.getReflectedDecl());

  return SetBoolAndSucceed(EvalCtx, IsObject);
}

bool has_template_arguments(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    QualType QT = RV.getReflectedType();
    bool result = isTemplateSpecialization(QT);
    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::Declaration: {
    bool result = false;

    Decl *D = RV.getReflectedDecl();
    if (auto *FD = dyn_cast<FunctionDecl>(D))
      result = (FD->getTemplateSpecializationArgs() != nullptr);
    else if (auto *VTSD = dyn_cast<VarTemplateSpecializationDecl>(D))
      result = VTSD->getTemplateArgs().size() > 0;

    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return SetBoolAndSucceed(EvalCtx, false);
  }
  llvm_unreachable("unknown reflection kind");
}

bool has_default_member_initializer(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool HasInitializer = false;
  if (RV.isReflectedDecl())
    if (auto *FD = dyn_cast<FieldDecl>(RV.getReflectedDecl()))
      HasInitializer = FD->hasInClassInitializer();

  return SetBoolAndSucceed(EvalCtx, HasInitializer);
}

bool is_conversion_function(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsConversion = false;
  if (RV.isReflectedDecl())
    IsConversion = isa<CXXConversionDecl>(RV.getReflectedDecl());

  return SetBoolAndSucceed(EvalCtx, IsConversion);
}

bool is_operator_function(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsOperator = false;
  if (RV.isReflectedDecl())
    if (auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl()))
      IsOperator = (FD->getOverloadedOperator() != OO_None);

  return SetBoolAndSucceed(EvalCtx, IsOperator);
}

bool is_literal_operator(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsLiteralOperator = false;
  if (RV.isReflectedDecl())
    if (auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl()))
      IsLiteralOperator = FD->getDeclName().getNameKind() ==
                          DeclarationName::CXXLiteralOperatorName;

  return SetBoolAndSucceed(EvalCtx, IsLiteralOperator);
}

bool is_constructor(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Namespace:
  case ReflectionKind::Template:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return SetBoolAndSucceed(EvalCtx, false);
  case ReflectionKind::Declaration: {
    bool result = isa<CXXConstructorDecl>(RV.getReflectedDecl());
    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::EntityProxy:
    llvm_unreachable("proxies should already have been unwrapped");
  }
  llvm_unreachable("invalid reflection type");
}

bool is_default_constructor(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl())
    if (auto *CtorD = dyn_cast<CXXConstructorDecl>(RV.getReflectedDecl()))
      result = CtorD->isDefaultConstructor();

  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_copy_constructor(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl())
    if (auto *CtorD = dyn_cast<CXXConstructorDecl>(RV.getReflectedDecl()))
      result = CtorD->isCopyConstructor();

  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_move_constructor(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl())
    if (auto *CtorD = dyn_cast<CXXConstructorDecl>(RV.getReflectedDecl()))
      result = CtorD->isMoveConstructor();

  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_assignment(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl())
    if (auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl()))
      result = (FD->getOverloadedOperator() == OO_Equal);

  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_copy_assignment(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl())
    if (auto *MD = dyn_cast<CXXMethodDecl>(RV.getReflectedDecl()))
      result = MD->isCopyAssignmentOperator();

  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_move_assignment(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedDecl())
    if (auto *MD = dyn_cast<CXXMethodDecl>(RV.getReflectedDecl()))
      result = MD->isMoveAssignmentOperator();

  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_destructor(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return SetBoolAndSucceed(EvalCtx, false);
  case ReflectionKind::Declaration: {
    bool result = isa<CXXDestructorDecl>(RV.getReflectedDecl());
    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::EntityProxy:
    llvm_unreachable("proxies should already have been unwrapped");
  }
  llvm_unreachable("invalid reflection type");
}

bool is_special_member_function(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Namespace:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return SetBoolAndSucceed(EvalCtx, false);
  case ReflectionKind::Declaration: {
    bool IsSpecial = false;
    if (auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl()))
      IsSpecial = isSpecialMember(FD);

    return SetBoolAndSucceed(EvalCtx, IsSpecial);
  }
  case ReflectionKind::Template: {
    bool result = false;
    TemplateDecl *TDecl = RV.getReflectedTemplate().getAsTemplateDecl();
    if (auto *FTD = dyn_cast<FunctionTemplateDecl>(TDecl))
      result = isSpecialMember(FTD->getTemplatedDecl());
    return SetBoolAndSucceed(EvalCtx, result);
  }
  case ReflectionKind::EntityProxy:
    llvm_unreachable("proxies should already have been unwrapped");
  }
  llvm_unreachable("invalid reflection type");
}

bool is_user_provided(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsUserProvided = false;
  if (RV.isReflectedDecl())
    if (auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl())) {
      FD = cast<FunctionDecl>(FD->getFirstDecl());
      IsUserProvided =
          !(FD->isImplicit() || FD->isDeleted() || FD->isDefaulted());
    }

  return SetBoolAndSucceed(EvalCtx, IsUserProvided);
}

bool is_user_declared(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool IsUserDeclared = false;
  if (RV.isReflectedDecl())
    if (auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl())) {
      FD = cast<FunctionDecl>(FD->getFirstDecl());
      IsUserDeclared = !(FD->isImplicit());
    }

  return SetBoolAndSucceed(EvalCtx, IsUserDeclared);
}

bool reflect_result(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);

  APValue ArgTy;
  if (!EvalCtx.Evaluator(ArgTy, EvalCtx.Args[0], true))
    return true;
  assert(ArgTy.isReflectedType());
  bool IsLValue = isa<ReferenceType>(ArgTy.getReflectedType());

  if (!IsLValue && !ArgTy.getReflectedType()->isStructuralType())
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_value_not_structural_type)
           << ArgTy.getReflectedType() << EvalCtx.Range;

  APValue Arg;
  if (!EvalCtx.Evaluator(Arg, EvalCtx.Args[1], !IsLValue))
    return true;

  // Construct an expression whose result is 'Arg', and evaluate it to check if
  // it's an allowed result of a constant template argument.
  //
  // This is just a hack to get 'CheckConstantExpression' in ExprConstant.cpp
  // called on 'Arg', to diagnose cases like string literals and temporaries
  // that aren't allowed in template arguments.
  //
  // The expression is constructed in three layers:
  // - A ConstantExpr to hold 'Arg'
  // - An OpaqueValueExpr to act as the ConstantExpr's subexpression (we can
  //   otherwise ICE when e.g., checking source location of the ConstantExpr)
  // - An OpaqueValueExpr wrapper around the ConstantExpr to prevent
  //   EvaluateAsConstantExpr from grabbing 'Arg' and short-circuiting the
  //   evaluation (and, more imporantly, the result validation).
  Expr *OVE = new (*EvalCtx.C)
      OpaqueValueExpr(EvalCtx.Range.getBegin(), EvalCtx.Args[1]->getType(),
                      IsLValue ? VK_LValue : VK_PRValue);
  {
    Expr *CE = ConstantExpr::Create(*EvalCtx.C, OVE, Arg);
    OVE = new (*EvalCtx.C)
        OpaqueValueExpr(EvalCtx.Range.getBegin(), EvalCtx.Args[1]->getType(),
                        CE->getValueKind(), OK_Ordinary, CE);
  }
  {
    Expr::EvalResult Discarded;

    ConstantExprKind CEKind = (OVE->getType()->isRecordType() && !IsLValue)
                                  ? ConstantExprKind::ClassTemplateArgument
                                  : ConstantExprKind::NonClassTemplateArgument;
    if (!OVE->EvaluateAsConstantExpr(Discarded, *EvalCtx.C, CEKind))
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_result_not_representable)
             << (IsLValue ? 1 : 0) << EvalCtx.Range;
  }

  // If this is an lvalue to a function, promote the result to reflect
  // the declaration.
  if (OVE->getType()->isFunctionType() && Arg.isLValue() &&
      Arg.getLValueOffset().isZero())
    if (!Arg.hasLValuePath() || Arg.getLValuePath().size() == 0)
      if (APValue::LValueBase LVBase = Arg.getLValueBase();
          LVBase.is<const ValueDecl *>())
        return SetAndSucceed(*EvalCtx.Result,
                             makeReflection(const_cast<ValueDecl *>(
                                 LVBase.get<const ValueDecl *>())));

  QualType ReflTy = ArgTy.getReflectedType();
  if (!IsLValue && ReflTy->isRecordType()) {
    auto *TPO = EvalCtx.C->getTemplateParamObjectDecl(ReflTy, Arg);
    Arg =
        APValue(APValue::LValueBase{TPO}, CharUnits::Zero(), {}, false, false);
    ReflTy = QualType{};
  }

  return SetAndSucceed(*EvalCtx.Result, Arg.Lift(ReflTy));
}

bool data_member_spec(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);

  APValue Scratch;
  size_t ArgIdx = 0;

  // Extract the data member type.
  if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[ArgIdx++], true) ||
      !Scratch.isReflectedType())
    return true;
  QualType MemberTy = Scratch.getReflectedType();

  // Evaluate whether a member name was provided.
  std::optional<std::string> Name;
  if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[ArgIdx++], true))
    return true;

  // Evaluate the given name. Miserably inefficient, but gets the job done.
  if (static_cast<bool>(Scratch.getInt().getExtValue())) {
    // Evaluate 'name' length.
    if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[ArgIdx++], true))
      return true;
    size_t nameLen = Scratch.getInt().getExtValue();
    Name.emplace(nameLen, '\0');

    // Evaluate the character type.
    if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[ArgIdx++], true))
      return true;
    QualType CharTy = Scratch.getReflectedType();

    // Evaluate the data contents.
    for (uint64_t k = 0; k < nameLen; ++k) {
      llvm::APInt Idx(EvalCtx.C->getTypeSize(EvalCtx.C->getSizeType()), k,
                      false);
      Expr *Synthesized =
          IntegerLiteral::Create(*EvalCtx.C, Idx, EvalCtx.C->getSizeType(),
                                 EvalCtx.Args[ArgIdx]->getExprLoc());

      Synthesized = new (*EvalCtx.C)
          ArraySubscriptExpr(EvalCtx.Args[ArgIdx], Synthesized, CharTy,
                             VK_LValue, OK_Ordinary, EvalCtx.Range.getBegin());
      if (Synthesized->isValueDependent() || Synthesized->isTypeDependent())
        return true;

      if (!EvalCtx.Evaluator(Scratch, Synthesized, true))
        return true;

      (*Name)[k] = static_cast<char>(Scratch.getInt().getExtValue());
    }
    ArgIdx++;
  } else {
    ArgIdx += 3;
  }

  // Validate the name as an identifier.
  if (Name) {
    Lexer Lex(EvalCtx.Range.getBegin(), EvalCtx.C->getLangOpts(), Name->data(),
              Name->data(), Name->data() + Name->size(), false);
    if (!Lex.validateIdentifier(*Name))
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_name_invalid_identifier)
             << *Name << EvalCtx.Range;
  }

  // Evaluate whether an alignment was provided.
  std::optional<size_t> Alignment;
  if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[ArgIdx++], true))
    return true;

  if (static_cast<bool>(Scratch.getInt().getExtValue())) {
    // Evaluate 'alignment' value.
    if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[ArgIdx], true))
      return true;
    int alignment = Scratch.getInt().getExtValue();

    if (alignment < 0)
      return true;
    Alignment = static_cast<size_t>(alignment);
  }
  ArgIdx++;

  // Evaluate whether a bit width was provided.
  std::optional<size_t> BitWidth;
  if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[ArgIdx++], true))
    return true;

  if (static_cast<bool>(Scratch.getInt().getExtValue())) {
    // Evaluate 'width' value.
    if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[ArgIdx], true))
      return true;
    int width = Scratch.getInt().getExtValue();

    if (width < 0)
      return true;
    BitWidth = static_cast<size_t>(width);
  }
  ArgIdx++;

  // Evaluate whether the "no_unique_address" attribute should apply.
  if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[ArgIdx++], true))
    return true;
  bool NoUniqueAddress = Scratch.getInt().getBoolValue();
  ArgIdx++;

  TagDataMemberSpec *TDMS = new (*EvalCtx.C)
      TagDataMemberSpec{MemberTy, Name, Alignment, BitWidth, NoUniqueAddress};
  return SetReflectionAndSucceed(EvalCtx, TDMS);
}

bool define_aggregate(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);

  APValue Scratch;
  if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[0], true))
    return true;
  if (!Scratch.isReflectedType())
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "a class type", DescriptionOf(Scratch));

  QualType ToComplete = Scratch.getReflectedType();
  if (!ToComplete->isRecordType())
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "a class type", DescriptionOf(Scratch));

  // Evaluate the number of members provided.
  if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[1], true))
    return true;
  size_t NumMembers = static_cast<size_t>(Scratch.getInt().getExtValue());

  SmallVector<TagDataMemberSpec *, 4> MemberSpecs;
  llvm::FoldingSetNodeID ID;
  llvm::StringSet<> MemberNames;
  for (size_t k = 0; k < NumMembers; ++k) {
    // Extract the reflection from the list of member specs.
    llvm::APInt Idx(EvalCtx.C->getTypeSize(EvalCtx.C->getSizeType()), k, false);
    Expr *Synthesized =
        IntegerLiteral::Create(*EvalCtx.C, Idx, EvalCtx.C->getSizeType(),
                               EvalCtx.Args[2]->getExprLoc());

    Synthesized = new (*EvalCtx.C)
        ArraySubscriptExpr(EvalCtx.Args[2], Synthesized, EvalCtx.C->MetaInfoTy,
                           VK_LValue, OK_Ordinary, EvalCtx.Range.getBegin());
    if (Synthesized->isValueDependent() || Synthesized->isTypeDependent())
      return true;

    if (!EvalCtx.Evaluator(Scratch, Synthesized, true))
      return true;
    if (!Scratch.isReflectedDataMemberSpec())
      return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                    "a description of a data member",
                                    DescriptionOf(Scratch));
    MemberSpecs.push_back(Scratch.getReflectedDataMemberSpec());
    Scratch.Profile(ID);

    if (MemberSpecs.back()->Name &&
        !MemberNames.insert(*MemberSpecs.back()->Name).second)
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_duplicate_member_names)
             << *MemberSpecs.back()->Name << EvalCtx.Range;
  }
  unsigned MemberSpecHash = ID.ComputeHash();

  CXXRecordDecl *IncompleteDecl;
  {
    NamedDecl *ND;
    if (!ToComplete->isIncompleteType(&ND)) {
      // NOTE: Uncomment following lines for 'define_aggregate' idempotency.
      /*unsigned PriorHash;
      if (EvalCtx.C->checkClassMemberSpecHash(ToComplete, PriorHash) &&
          MemberSpecHash == PriorHash)
        return SetReflectionAndSucceed(EvalCtx, ToComplete);
      else*/
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_already_complete_type)
             << ToComplete << EvalCtx.Range;
    }
    IncompleteDecl = cast<CXXRecordDecl>(ND);
  }

  if (!EvalCtx.AllowInjection)
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_injected_decl_non_plainly_consteval);

  CXXRecordDecl *Definition = EvalCtx.Meta->DefineAggregate(
      IncompleteDecl, MemberSpecs, EvalCtx.ContainingDecl,
      EvalCtx.Range.getBegin());
  if (!Definition)
    return true;

  EvalCtx.C->recordClassMemberSpecHash(ToComplete, MemberSpecHash);
  return SetReflectionAndSucceed(EvalCtx, ToComplete);
}

bool offset_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[1], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "a non-static data member",
                                  DescriptionOf(RV));
  case ReflectionKind::Declaration: {
    if (const FieldDecl *FD = dyn_cast<FieldDecl>(RV.getReflectedDecl())) {
      size_t Offset = getBitOffsetOfField(*EvalCtx.C, FD) /
                      EvalCtx.C->getTypeSize(EvalCtx.C->CharTy);
      return SetAndSucceed(*EvalCtx.Result, APValue(EvalCtx.C->MakeIntValue(
                                                Offset, EvalCtx.ResultTy)));
    }
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "a non-static data member",
                                  DescriptionOf(RV));
  }
  case ReflectionKind::BaseSpecifier: {
    CXXBaseSpecifier *Base = RV.getReflectedBaseSpecifier();
    if (Base->isVirtual() && Base->getDerived()->isAbstract())
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_offset_virtual_base_of_abstract)
             << EvalCtx.Range;

    size_t Offset = getOffsetOfBase(*EvalCtx.C, Base);
    return SetAndSucceed(*EvalCtx.Result, APValue(EvalCtx.C->MakeIntValue(
                                              Offset, EvalCtx.ResultTy)));
  }
  }
  llvm_unreachable("unknown reflection kind");
}

bool size_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsSizeT(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  auto SetSizeAndSucceed = [&](auto Entity) {
    size_t Sz = EvalCtx.C->getTypeSizeInChars(Entity).getQuantity();
    return SetAndSucceed(*EvalCtx.Result, APValue(EvalCtx.C->MakeIntValue(
                                              Sz, EvalCtx.C->getSizeType())));
  };

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    QualType QT = RV.getReflectedType();

    NamedDecl *typeDecl = findTypeDecl(RV.getReflectedType());
    if (typeDecl)
      EvalCtx.Meta->EnsureInstantiated(typeDecl, EvalCtx.Range);

    if (QT->isIncompleteType())
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_cannot_introspect_type)
             << 4 << 0 << EvalCtx.Range;

    return SetSizeAndSucceed(QT);
  }
  case ReflectionKind::Object:
  case ReflectionKind::Value: {
    QualType QT = RV.getTypeOfReflectedResult(*EvalCtx.C);
    return SetSizeAndSucceed(QT);
  }
  case ReflectionKind::Declaration: {
    ValueDecl *VD = RV.getReflectedDecl();
    return SetSizeAndSucceed(VD->getType());
  }
  case ReflectionKind::DataMemberSpec: {
    TagDataMemberSpec *TDMS = RV.getReflectedDataMemberSpec();
    return SetSizeAndSucceed(TDMS->Ty);
  }
  case ReflectionKind::Null:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::Annotation:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_query_property)
           << 3 << DescriptionOf(RV);
  }
  llvm_unreachable("unknown reflection kind");
}

bool bit_offset_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[1], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "a non-static data member",
                                  DescriptionOf(RV));
  case ReflectionKind::Declaration: {
    if (FieldDecl *FD = dyn_cast<FieldDecl>(RV.getReflectedDecl())) {
      size_t Offset = getBitOffsetOfField(*EvalCtx.C, FD) %
                      EvalCtx.C->getTypeSize(EvalCtx.C->CharTy);
      return SetAndSucceed(*EvalCtx.Result, APValue(EvalCtx.C->MakeIntValue(
                                                Offset, EvalCtx.ResultTy)));
    }
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "a non-static data member",
                                  DescriptionOf(RV));
  }
  case ReflectionKind::BaseSpecifier:
    return SetAndSucceed(*EvalCtx.Result,
                         APValue(EvalCtx.C->MakeIntValue(0, EvalCtx.ResultTy)));
  }
  llvm_unreachable("unknown reflection kind");
}

bool bit_size_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsSizeT(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  auto SetSizeAndSucceed = [&](size_t Sz) {
    return SetAndSucceed(*EvalCtx.Result, APValue(EvalCtx.C->MakeIntValue(
                                              Sz, EvalCtx.C->getSizeType())));
  };

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    QualType QT = RV.getReflectedType();

    NamedDecl *typeDecl = findTypeDecl(RV.getReflectedType());
    if (typeDecl)
      EvalCtx.Meta->EnsureInstantiated(typeDecl, EvalCtx.Range);

    if (QT->isIncompleteType())
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_cannot_introspect_type)
             << 4 << 0 << EvalCtx.Range;

    return SetSizeAndSucceed(EvalCtx.C->getTypeSize(QT));
  }
  case ReflectionKind::Object:
  case ReflectionKind::Value: {
    size_t Sz = EvalCtx.C->getTypeSize(RV.getTypeOfReflectedResult(*EvalCtx.C));
    return SetSizeAndSucceed(Sz);
  }
  case ReflectionKind::Declaration: {
    const ValueDecl *VD = cast<ValueDecl>(RV.getReflectedDecl());
    size_t Sz = EvalCtx.C->getTypeSize(VD->getType());

    if (const FieldDecl *FD = dyn_cast<const FieldDecl>(VD))
      if (FD->isBitField())
        Sz = FD->getBitWidthValue();

    return SetSizeAndSucceed(Sz);
  }
  case ReflectionKind::DataMemberSpec: {
    TagDataMemberSpec *TDMS = RV.getReflectedDataMemberSpec();

    size_t Sz = TDMS->BitWidth.value_or(EvalCtx.C->getTypeSize(TDMS->Ty));
    return SetSizeAndSucceed(Sz);
  }

  case ReflectionKind::Null:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::Annotation:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_query_property)
           << 3 << DescriptionOf(RV);
  }
  llvm_unreachable("unknown reflection kind");
}

bool alignment_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsSizeT(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    QualType QT = RV.getReflectedType();
    if (QT->isIncompleteType())
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_cannot_introspect_type)
             << 3 << 0 << EvalCtx.Range;

    size_t Align = EvalCtx.C->getTypeAlignInChars(QT).getQuantity();
    return SetAndSucceed(
        *EvalCtx.Result,
        APValue(EvalCtx.C->MakeIntValue(Align, EvalCtx.C->getSizeType())));
  }
  case ReflectionKind::Object:
  case ReflectionKind::Value: {
    QualType QT = RV.getTypeOfReflectedResult(*EvalCtx.C);
    size_t Align = EvalCtx.C->getTypeAlignInChars(QT).getQuantity();
    return SetAndSucceed(
        *EvalCtx.Result,
        APValue(EvalCtx.C->MakeIntValue(Align, EvalCtx.C->getSizeType())));
  }
  case ReflectionKind::Declaration: {
    const ValueDecl *VD = cast<ValueDecl>(RV.getReflectedDecl());

    if (const FieldDecl *FD = dyn_cast<const FieldDecl>(VD)) {
      if (FD->isBitField())
        return true;
    }
    size_t Align = EvalCtx.C->getDeclAlign(VD, false).getQuantity();

    return SetAndSucceed(
        *EvalCtx.Result,
        APValue(EvalCtx.C->MakeIntValue(Align, EvalCtx.C->getSizeType())));
  }
  case ReflectionKind::DataMemberSpec: {
    TagDataMemberSpec *TDMS = RV.getReflectedDataMemberSpec();
    if (TDMS->BitWidth)
      return true;

    size_t Align = TDMS->Alignment.value_or(
        EvalCtx.C->getTypeAlignInChars(TDMS->Ty).getQuantity());

    return SetAndSucceed(
        *EvalCtx.Result,
        APValue(EvalCtx.C->MakeIntValue(Align, EvalCtx.C->getSizeType())));
  }
  case ReflectionKind::Null:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::Annotation:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_query_property)
           << 4 << DescriptionOf(RV) << EvalCtx.Range;
  }
  llvm_unreachable("unknown reflection kind");
}

bool get_ith_parameter_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsMetaInfo(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  APValue Sentinel;
  if (!EvalCtx.Evaluator(Sentinel, EvalCtx.Args[1], true))
    return true;
  assert(Sentinel.isReflectedType());

  APValue Idx;
  if (!EvalCtx.Evaluator(Idx, EvalCtx.Args[2], true))
    return true;
  size_t idx = Idx.getInt().getExtValue();

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    if (auto FT = dyn_cast<FunctionProtoType>(RV.getReflectedType())) {
      unsigned numParams = FT->getNumParams();
      if (idx >= numParams)
        return SetAndSucceed(*EvalCtx.Result, Sentinel);

      return SetReflectionAndSucceed(EvalCtx, FT->getParamType(idx));
    }
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_introspect_type)
           << 2 << 2 << EvalCtx.Range;
  }
  case ReflectionKind::Declaration: {
    if (auto FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl())) {
      unsigned numParams = FD->getNumParams();
      if (idx >= numParams)
        return SetAndSucceed(*EvalCtx.Result, Sentinel);

      return SetReflectionAndSucceed(EvalCtx, FD->getParamDecl(idx));
    }
    // Template Parameters (Class/Var/Alias Templates reflected as Decls)
    if (TemplateDecl *TD = dyn_cast<TemplateDecl>(RV.getReflectedDecl())) {
      // if (!CheckYukinoExtension(
      //         EvalCtx, EvalCtx.C->getLangOpts().UsagiTemplateReflection,
      //         ReflExtName_UsagiTemplate))
      //   return true;

      TemplateParameterList *TPL = TD->getTemplateParameters();
      if (idx >= TPL->size())
        return SetAndSucceed(*EvalCtx.Result, Sentinel);

      return SetReflectionAndSucceed(EvalCtx, TPL->getParam(idx));
    }
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_query_property)
           << 5 << DescriptionOf(RV) << EvalCtx.Range;
  }
  case ReflectionKind::Template: {
    // if (!CheckYukinoExtension(EvalCtx,
    //                           EvalCtx.C->getLangOpts().UsagiTemplateReflection,
    //                           ReflExtName_UsagiTemplate))
    //   return true;

    TemplateName TN = RV.getReflectedTemplate();
    // Convert the TemplateName to the underlying declaration
    TemplateDecl *TD = TN.getAsTemplateDecl();

    // If it's an OverloadedTemplate (which shouldn't happen given our previous
    // fixes) or a DependentTemplateName that can't be resolved, we can't
    // inspect parameters.
    if (!TD) {
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_cannot_query_property)
             << 5 << DescriptionOf(RV) << EvalCtx.Range;
    }

    // Access the list of <typename T, int N, template<...> class C>
    TemplateParameterList *TPL = TD->getTemplateParameters();

    if (idx >= TPL->size())
      return SetAndSucceed(*EvalCtx.Result, Sentinel);

    // NamedDecl* here is the TemplateTypeParmDecl, NonTypeTemplateParmDecl,
    // etc.
    return SetReflectionAndSucceed(EvalCtx, TPL->getParam(idx));
  }
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return true;
  }
  return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                           diag::metafn_cannot_query_property)
         << 5 << DescriptionOf(RV) << EvalCtx.Range;
}

bool has_ellipsis_parameter(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_query_property)
           << 5 << DescriptionOf(RV) << EvalCtx.Range;
  case ReflectionKind::Type:
    if (auto *FPT = dyn_cast<FunctionProtoType>(RV.getReflectedType())) {
      bool HasEllipsis = FPT->isVariadic();
      return SetBoolAndSucceed(EvalCtx, HasEllipsis);
    }
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_introspect_type)
           << 2 << 2;
  case ReflectionKind::Declaration: {
    if (auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl())) {
      bool HasEllipsis = FD->getEllipsisLoc().isValid();
      return SetBoolAndSucceed(EvalCtx, HasEllipsis);
    }
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_query_property)
           << 5 << DescriptionOf(RV) << EvalCtx.Range;
  }
  }
  llvm_unreachable("unknown reflection kind");
}

bool has_default_argument(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Parameter: {
    ParmVarDecl *PVD = getMostRecentParmVarDecl(RV.getReflectedParameter());
    return SetBoolAndSucceed(EvalCtx, PVD->hasDefaultArg());
  }
  case ReflectionKind::Declaration:
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "a function parameter", DescriptionOf(RV));
  }
  llvm_unreachable("unknown reflection kind");
}

bool is_explicit_object_parameter(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  bool result = false;
  if (RV.isReflectedParameter())
    result = RV.getReflectedParameter()->isExplicitObjectParameter();
  return SetBoolAndSucceed(EvalCtx, result);
}

bool is_function_parameter(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  return SetBoolAndSucceed(EvalCtx, RV.isReflectedParameter());
}

bool return_type_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsMetaInfo(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    if (auto *FPT = dyn_cast<FunctionProtoType>(RV.getReflectedType())) {
      QualType QT = desugarType(FPT->getReturnType(), /*UnwrapAliases=*/true,
                                /*DropCV=*/false, /*DropRefs=*/false);
      return SetReflectionAndSucceed(EvalCtx, QT);
    }

    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_introspect_type)
           << 3 << 2 << EvalCtx.Range;
  }
  case ReflectionKind::Declaration:
    if (auto *FD = dyn_cast<FunctionDecl>(RV.getReflectedDecl());
        FD && !isa<CXXConstructorDecl>(FD) && !isa<CXXDestructorDecl>(FD)) {
      QualType QT = desugarType(FD->getReturnType(), /*UnwrapAliases=*/true,
                                /*DropCV=*/false, /*DropRefs=*/false);
      return SetReflectionAndSucceed(EvalCtx, QT);
    }
    [[fallthrough]];
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Template:
  case ReflectionKind::Namespace:
  case ReflectionKind::EntityProxy:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_query_property)
           << 6 << DescriptionOf(RV) << EvalCtx.Range;
  }
  llvm_unreachable("unknown reflection kind");
}

bool variable_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsMetaInfo(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  if (!RV.isReflectedParameter())
    return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                  "a function parameter", DescriptionOf(RV));

  ParmVarDecl *PVD = RV.getReflectedParameter();
  FunctionDecl *FD = cast<FunctionDecl>(PVD->getDeclContext());

  if (EvalCtx.Meta->CurrentCtx()->getCanonicalDecl() != FD->getCanonicalDecl())
    return true;
  assert(FD->getDefinition());
  PVD = FD->getDefinition()->getParamDecl(PVD->getFunctionScopeIndex());

  APValue Var(ReflectionKind::Declaration, PVD);
  return SetAndSucceed(*EvalCtx.Result, Var);
}

bool get_ith_annotation_of(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsMetaInfo(EvalCtx);

  auto findAnnotation = [&](Decl *D, size_t idx, APValue Sentinel) {
    D = D ? D->getMostRecentDecl() : D;

    while (D) {
      auto Annots = D->attrs();
      for (auto It = Annots.begin(); It != Annots.end(); ++It)
        if (isa<CXX26AnnotationAttr>(*It))
          if (idx-- == 0)
            return makeReflection(dyn_cast<CXX26AnnotationAttr>(*It));
      D = D->getPreviousDecl();
    }
    return Sentinel;
  };

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  APValue Sentinel;
  if (!EvalCtx.Evaluator(Sentinel, EvalCtx.Args[1], true))
    return true;
  assert(Sentinel.isReflectedType());

  APValue Idx;
  if (!EvalCtx.Evaluator(Idx, EvalCtx.Args[2], true))
    return true;
  size_t idx = Idx.getInt().getExtValue();

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    NamedDecl *typeDecl = findTypeDecl(RV.getReflectedType());
    if (typeDecl)
      EvalCtx.Meta->EnsureInstantiated(typeDecl, EvalCtx.Range);

    return SetAndSucceed(*EvalCtx.Result,
                         findAnnotation(typeDecl, idx, Sentinel));
  }
  case ReflectionKind::Declaration: {
    ValueDecl *VD = RV.getReflectedDecl();

    return SetAndSucceed(*EvalCtx.Result, findAnnotation(VD, idx, Sentinel));
  }
  case ReflectionKind::Namespace: {
    Decl *D = RV.getReflectedNamespace();

    return SetAndSucceed(*EvalCtx.Result, findAnnotation(D, idx, Sentinel));
  }
  case ReflectionKind::EntityProxy: {
    Decl *D = RV.getReflectedEntityProxy()->getIntroducer();

    return SetAndSucceed(*EvalCtx.Result, findAnnotation(D, idx, Sentinel));
  }
  // Disallow reflecting annotations of unspecialized templates, as they might
  // contain a dependent name.
  case ReflectionKind::Template: /*{
    Decl *D = RV.getReflectedTemplate().getAsTemplateDecl()->getTemplatedDecl();

    return SetAndSucceed(*EvalCtx.Result, findAnnotation(D, idx, Sentinel));
  }*/
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_query_property)
           << 7 << DescriptionOf(RV) << EvalCtx.Range;
  }
  llvm_unreachable("unknown reflection kind");
}

bool is_annotation(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  return SetBoolAndSucceed(EvalCtx, RV.isReflectedAnnotation());
}

bool annotate(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckReflectionArg(EvalCtx, 1);
  CheckResultTyIsMetaInfo(EvalCtx);

  APValue Appertainee;
  if (!EvalCtx.Evaluator(Appertainee, EvalCtx.Args[0], true))
    return true;

  APValue Value;
  if (!EvalCtx.Evaluator(Value, EvalCtx.Args[1], true) ||
      !Value.isReflectedValue())
    return true;

  if (!EvalCtx.AllowInjection)
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_injected_decl_non_plainly_consteval);

  switch (Appertainee.getReflectionKind()) {
  case ReflectionKind::Type: {
    Decl *D = findTypeDecl(Appertainee.getReflectedType());
    if (auto *Annot = EvalCtx.Meta->Annotate(D->getMostRecentDecl(), Value,
                                             EvalCtx.ContainingDecl,
                                             EvalCtx.Range.getBegin()))
      return SetReflectionAndSucceed(EvalCtx, Annot);
    return true;
  }
  case ReflectionKind::Declaration: {
    Decl *D = Appertainee.getReflectedDecl();
    if (!isa<VarDecl, FunctionDecl>(D))
      return true;

    if (auto *Annot = EvalCtx.Meta->Annotate(D->getMostRecentDecl(), Value,
                                             EvalCtx.ContainingDecl,
                                             EvalCtx.Range.getBegin()))
      return SetReflectionAndSucceed(EvalCtx, Annot);
    return true;
  }
  case ReflectionKind::Namespace: {
    Decl *D = Appertainee.getReflectedNamespace();
    if (auto *Annot = EvalCtx.Meta->Annotate(D->getMostRecentDecl(), Value,
                                             EvalCtx.ContainingDecl,
                                             EvalCtx.Range.getBegin()))
      return SetReflectionAndSucceed(EvalCtx, Annot);
    return true;
  }
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Template:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
  case ReflectionKind::EntityProxy:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_annotate)
           << DescriptionOf(Appertainee) << EvalCtx.Range;
  }
  llvm_unreachable("unknown reflection kind");
}

bool current_access_context(const MetaFunctionEvalContext &EvalCtx) {
  CheckResultTyIsMetaInfo(EvalCtx);
  Decl *Ctx = nullptr;

  StackLocationExpr *SLE =
      StackLocationExpr::Create(*EvalCtx.C, SourceRange(), 1);
  if (!EvalCtx.Evaluator(*EvalCtx.Result, SLE, true) ||
      !EvalCtx.Result->isReflectedDecl())
    return true;
  else if (Ctx = EvalCtx.Result->getReflectedDecl(); !Ctx)
    Ctx = EvalCtx.Meta->CurrentCtx();

  if (auto *Ctor = dyn_cast<CXXConstructorDecl>(Ctx);
      Ctor && Ctor->isInheritingConstructor())
    Ctx = cast<Decl>(Ctor->getDeclContext());

  // todo [merge:yukino:maybe-revert]
  if (auto *RD = dyn_cast<CXXRecordDecl>(Ctx))
    return SetReflectionAndSucceed(EvalCtx, QualType(getTypeForDecl(RD), 0));
  return SetReflectionAndSucceed(EvalCtx, Ctx);
}

bool is_accessible(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckReflectionArg(EvalCtx, 1);
  CheckReflectionArg(EvalCtx, 2);
  CheckResultTyIsBool(EvalCtx);

  APValue Scratch;
  if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[1], true) ||
      !Scratch.isReflection())
    return true;

  bool UnconditionalAccess = false;

  DeclContext *AccessDC = nullptr;
  switch (Scratch.getReflectionKind()) {
  case ReflectionKind::Null:
    UnconditionalAccess = true;
    break;
  case ReflectionKind::Type:
    AccessDC =
        dyn_cast_or_null<DeclContext>(findTypeDecl(Scratch.getReflectedType()));
    if (!AccessDC)
      return true;
    break;
  case ReflectionKind::Namespace:
    AccessDC = dyn_cast<DeclContext>(Scratch.getReflectedNamespace());
    break;
  case ReflectionKind::Declaration:
    AccessDC = dyn_cast<DeclContext>(Scratch.getReflectedDecl());
    break;
  default:
    llvm_unreachable("invalid access context");
  }

  CXXRecordDecl *NamingCls = nullptr;
  if (!EvalCtx.Evaluator(Scratch, EvalCtx.Args[2], true) ||
      !Scratch.isReflection())
    return true;
  Scratch = MaybeUnproxy(*EvalCtx.C, Scratch);
  assert(Scratch.isNullReflection() || Scratch.isReflectedType());
  if (Scratch.isReflectedType()) {
    NamingCls = cast<CXXRecordDecl>(findTypeDecl(Scratch.getReflectedType()));

    EvalCtx.Meta->EnsureInstantiated(NamingCls, EvalCtx.Range);
    NamingCls = NamingCls->getDefinition();

    if (!NamingCls)
      return true; // TODO(P2996): Diagnostic for naming class.
  }

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  auto validate = [&](Decl *D, CXXRecordDecl *&NamingCls) -> bool {
    auto *DC = dyn_cast<CXXRecordDecl>(D->getNonTransparentDeclContext());
    if (!NamingCls)
      NamingCls = DC;

    if (DC && DC->isBeingDefined())
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_access_query_class_being_defined)
             << DC << EvalCtx.Range;
    return false;
  };

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    NamedDecl *D = findTypeDecl(RV.getReflectedType());
    if (validate(D, NamingCls))
      return true;
    else if (!NamingCls)
      return SetBoolAndSucceed(EvalCtx, true);

    bool Accessible = UnconditionalAccess ||
                      EvalCtx.Meta->IsAccessible(D, AccessDC, NamingCls);
    return SetBoolAndSucceed(EvalCtx, Accessible);
  }
  case ReflectionKind::Declaration: {
    ValueDecl *D = RV.getReflectedDecl();
    if (validate(D, NamingCls))
      return true;
    else if (!NamingCls)
      return SetBoolAndSucceed(EvalCtx, true);

    bool Accessible =
        UnconditionalAccess ||
        EvalCtx.Meta->IsAccessible(RV.getReflectedDecl(), AccessDC, NamingCls);
    return SetBoolAndSucceed(EvalCtx, Accessible);
  }
  case ReflectionKind::Template: {
    TemplateDecl *D = RV.getReflectedTemplate().getAsTemplateDecl();
    if (validate(D, NamingCls))
      return true;
    else if (!NamingCls)
      return SetBoolAndSucceed(EvalCtx, true);

    bool Accessible = UnconditionalAccess ||
                      EvalCtx.Meta->IsAccessible(D, AccessDC, NamingCls);
    return SetBoolAndSucceed(EvalCtx, Accessible);
  }
  case ReflectionKind::EntityProxy: {
    UsingShadowDecl *USD = RV.getReflectedEntityProxy();
    if (validate(USD, NamingCls))
      return true;
    else if (!NamingCls)
      return SetBoolAndSucceed(EvalCtx, true);

    bool Accessible = UnconditionalAccess ||
                      EvalCtx.Meta->IsAccessible(USD, AccessDC, NamingCls);
    return SetBoolAndSucceed(EvalCtx, Accessible);
  }
  case ReflectionKind::BaseSpecifier: {
    CXXBaseSpecifier *BaseSpec = RV.getReflectedBaseSpecifier();

    auto *Base = findTypeDecl(BaseSpec->getType());
    assert(Base && "base class has no type declaration?");

    QualType BaseTy = BaseSpec->getType();

    CXXRecordDecl *DerivedDecl = BaseSpec->getDerived();
    if (DerivedDecl->isBeingDefined())
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_access_query_class_being_defined)
             << DerivedDecl << EvalCtx.Range;
    // todo [merge:yukino:maybe-revert]
    QualType DerivedTy(getTypeForDecl(BaseSpec->getDerived()), 0);

    CXXBasePathElement bpe = {BaseSpec, BaseSpec->getDerived(), 0};
    CXXBasePath path;
    path.push_back(bpe);
    path.Access = BaseSpec->getAccessSpecifier();

    bool Accessible =
        UnconditionalAccess ||
        EvalCtx.Meta->IsAccessibleBase(BaseTy, DerivedTy, path, AccessDC,
                                       EvalCtx.Range.getBegin());
    return SetBoolAndSucceed(EvalCtx, Accessible);
  }
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Namespace:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return SetBoolAndSucceed(EvalCtx, true);
  }
  llvm_unreachable("invalid reflection type");
}

bool is_access_specified(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckResultTyIsBool(EvalCtx);

  auto findAccessSpec = [](Decl *D) -> AccessSpecifier {
    DeclContext *DC = D->getDeclContext();
    for (auto I = DC->decls_begin(); *I != D; ++I) {
      assert(I != DC->decls_end());
      if (auto *ASD = dyn_cast<AccessSpecDecl>(*I))
        return ASD->getAccess();
    }
    return AS_none;
  };

  APValue RV;
  if (!EvalCtx.Evaluator(RV, EvalCtx.Args[0], true))
    return true;

  switch (RV.getReflectionKind()) {
  case ReflectionKind::Type: {
    bool IsSpecified = false;
    if (Decl *D = findTypeDecl(RV.getReflectedType()))
      IsSpecified = findAccessSpec(D) != AS_none;

    return SetBoolAndSucceed(EvalCtx, IsSpecified);
  }
  case ReflectionKind::Declaration: {
    bool IsSpecified = findAccessSpec(RV.getReflectedDecl()) != AS_none;
    return SetBoolAndSucceed(EvalCtx, IsSpecified);
  }
  case ReflectionKind::Template: {
    Decl *D = RV.getReflectedTemplate().getAsTemplateDecl();

    bool IsSpecified = findAccessSpec(D) != AS_none;
    return SetBoolAndSucceed(EvalCtx, IsSpecified);
  }
  case ReflectionKind::EntityProxy: {
    Decl *D = RV.getReflectedEntityProxy()->getIntroducer();

    bool IsSpecified = findAccessSpec(D) != AS_none;
    return SetBoolAndSucceed(EvalCtx, IsSpecified);
  }
  case ReflectionKind::BaseSpecifier: {
    CXXBaseSpecifier *Base = RV.getReflectedBaseSpecifier();
    bool IsSpecified = (Base->getAccessSpecifierAsWritten() != AS_none);
    return SetBoolAndSucceed(EvalCtx, IsSpecified);
  }
  case ReflectionKind::Null:
  case ReflectionKind::Object:
  case ReflectionKind::Value:
  case ReflectionKind::Namespace:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return SetBoolAndSucceed(EvalCtx, false);
  }
  llvm_unreachable("invalid reflection type");
}

bool is_nonstatic_member_function(ValueDecl *FD) {
  if (!FD) {
    return false;
  }

  if (dyn_cast<CXXConstructorDecl>(FD)) {
    return false;
  }

  auto *MD = dyn_cast<CXXMethodDecl>(FD);
  if (!MD) {
    // might be a pointer to member function
    QualType QT = FD->getType();
    // check if the type is a pointer to a member
    if (const MemberPointerType *MPT = QT->getAs<MemberPointerType>()) {
      QualType PT = MPT->getPointeeType();
      // check if the pointee type is a function type
      if (PT->getAs<FunctionProtoType>()) {
        return true;
      }
    }
  } else {
    return !MD->isStatic();
  }

  return false;
}

CXXMethodDecl *getCXXMethodDeclFromDeclRefExpr(DeclRefExpr *DRE,
                                               ASTContext &C) {
  ValueDecl *VD = DRE->getDecl();

  if (auto *MD = dyn_cast<CXXMethodDecl>(VD)) {
    // method declaration
    return MD;
  } else {
    // pointer to non-static method
    // validation was done in is_nonstatic_member_function
    Expr::EvalResult ER;
    if (!DRE->EvaluateAsRValue(ER, C)) {
      return nullptr;
    }

    APValue Result = ER.Val;
    if (!Result.isMemberPointer()) {
      return nullptr;
    }

    const ValueDecl *MemberDecl = Result.getMemberPointerDecl();
    if (const CXXMethodDecl *MethodDecl = dyn_cast<CXXMethodDecl>(MemberDecl)) {
      // get non-const version
      return const_cast<CXXMethodDecl *>(MethodDecl);
    }
  }

  return nullptr;
}

bool reflect_invoke(const MetaFunctionEvalContext &EvalCtx) {
  CheckReflectionArg(EvalCtx, 0);
  CheckReflectionArrayArg(EvalCtx, 1);
  CheckIntegralArg(EvalCtx, 2);
  CheckReflectionArrayArg(EvalCtx, 3);
  CheckIntegralArg(EvalCtx, 4);

  using ReflectionVector = SmallVector<APValue, 4>;
  auto UnpackReflectionsIntoVector = [&](ReflectionVector &Out, Expr *DataExpr,
                                         Expr *SzExpr) -> bool {
    APValue Scratch;
    if (!EvalCtx.Evaluator(Scratch, SzExpr, true))
      return false;

    size_t nArgs = Scratch.getInt().getExtValue();
    Out.reserve(nArgs);
    for (uint64_t k = 0; k < nArgs; ++k) {
      llvm::APInt Idx(EvalCtx.C->getTypeSize(EvalCtx.C->getSizeType()), k,
                      false);
      Expr *Synthesized = IntegerLiteral::Create(
          *EvalCtx.C, Idx, EvalCtx.C->getSizeType(), DataExpr->getExprLoc());

      Synthesized = new (*EvalCtx.C)
          ArraySubscriptExpr(DataExpr, Synthesized, EvalCtx.C->MetaInfoTy,
                             VK_LValue, OK_Ordinary, EvalCtx.Range.getBegin());

      if (Synthesized->isValueDependent() || Synthesized->isTypeDependent())
        return false;

      if (!EvalCtx.Evaluator(Scratch, Synthesized, true) ||
          !Scratch.isReflection())
        return false;
      Scratch = MaybeUnproxy(*EvalCtx.C, Scratch);
      Out.push_back(Scratch);
    }

    return true;
  };

  APValue FnRefl;
  if (!EvalCtx.Evaluator(FnRefl, EvalCtx.Args[0], true))
    return true;
  FnRefl = MaybeUnproxy(*EvalCtx.C, FnRefl);

  SmallVector<TemplateArgument, 4> ExplicitTArgs;
  {
    SmallVector<APValue, 4> Reflections;
    if (!UnpackReflectionsIntoVector(Reflections, EvalCtx.Args[1],
                                     EvalCtx.Args[2]))
      llvm_unreachable("failed to unpack template arguments from vector?");

    if (Reflections.size() > 0 && !FnRefl.isReflectedTemplate())
      return DiagnoseReflectionKind(EvalCtx.Diagnoser, EvalCtx.Range,
                                    "a template", DescriptionOf(FnRefl));

    SmallVector<TemplateArgument, 4> TArgs;
    for (APValue RV : Reflections) {
      if (!CanActAsTemplateArg(RV))
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_cannot_be_arg)
               << DescriptionOf(RV) << 1 << EvalCtx.Range;

      TemplateArgument TArg =
          TArgFromReflection(EvalCtx, RV, EvalCtx.Range.getBegin());
      if (TArg.isNull())
        return true;
      TArgs.push_back(TArg);
    }

    expandTemplateArgPacks(TArgs, ExplicitTArgs);
  }

  SmallVector<Expr *, 4> ArgExprs;
  {
    SmallVector<APValue, 4> Reflections;
    if (!UnpackReflectionsIntoVector(Reflections, EvalCtx.Args[3],
                                     EvalCtx.Args[4]))
      llvm_unreachable("failed to unpack function arguments from vector?");

    for (APValue RV : Reflections) {
      if (RV.isReflectedObject()) {
        Expr *OVE = new (*EvalCtx.C)
            OpaqueValueExpr(EvalCtx.Range.getBegin(),
                            RV.getTypeOfReflectedResult(*EvalCtx.C), VK_LValue);
        Expr *CE =
            ConstantExpr::Create(*EvalCtx.C, OVE, RV.getReflectedObject());
        ArgExprs.push_back(CE);
      } else if (RV.isReflectedValue()) {
        Expr *OVE = new (*EvalCtx.C) OpaqueValueExpr(
            EvalCtx.Range.getBegin(), RV.getTypeOfReflectedResult(*EvalCtx.C),
            VK_PRValue);
        Expr *CE =
            ConstantExpr::Create(*EvalCtx.C, OVE, RV.getReflectedValue());
        ArgExprs.push_back(CE);
      } else if (RV.isReflectedDecl()) {
        ValueDecl *D = RV.getReflectedDecl();
        ArgExprs.push_back(DeclRefExpr::Create(
            *EvalCtx.C, NestedNameSpecifierLoc(), SourceLocation(), D, false,
            EvalCtx.Range.getBegin(), D->getType(), VK_LValue, D, nullptr));
      } else {
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_cannot_be_arg)
               << DescriptionOf(RV) << 0 << EvalCtx.Range;
      }
    }
  }

  Expr *FnRefExpr = nullptr;
  switch (FnRefl.getReflectionKind()) {
  case ReflectionKind::Null:
  case ReflectionKind::Type:
  case ReflectionKind::Namespace:
  case ReflectionKind::BaseSpecifier:
  case ReflectionKind::Parameter:
  case ReflectionKind::DataMemberSpec:
  case ReflectionKind::Annotation:
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_cannot_invoke)
           << DescriptionOf(FnRefl) << EvalCtx.Range;
  case ReflectionKind::Object: {
    Expr *OVE = new (*EvalCtx.C)
        OpaqueValueExpr(EvalCtx.Range.getBegin(),
                        FnRefl.getTypeOfReflectedResult(*EvalCtx.C), VK_LValue);
    FnRefExpr =
        ConstantExpr::Create(*EvalCtx.C, OVE, FnRefl.getReflectedObject());
    break;
  }
  case ReflectionKind::Value: {
    Expr *OVE = new (*EvalCtx.C) OpaqueValueExpr(
        EvalCtx.Range.getBegin(), FnRefl.getTypeOfReflectedResult(*EvalCtx.C),
        VK_PRValue);
    FnRefExpr =
        ConstantExpr::Create(*EvalCtx.C, OVE, FnRefl.getReflectedValue());
    break;
  }
  case ReflectionKind::Declaration: {
    ValueDecl *D = FnRefl.getReflectedDecl();
    EvalCtx.Meta->EnsureInstantiated(D, EvalCtx.Range);

    FnRefExpr = DeclRefExpr::Create(
        *EvalCtx.C, NestedNameSpecifierLoc(), SourceLocation(), D, false,
        EvalCtx.Range.getBegin(), D->getType(), VK_LValue, D, nullptr);
    break;
  }
  case ReflectionKind::Template: {
    TemplateDecl *TDecl = FnRefl.getReflectedTemplate().getAsTemplateDecl();
    auto *FTD = dyn_cast<FunctionTemplateDecl>(TDecl);
    if (!FTD) {
      return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                               diag::metafn_cannot_invoke)
             << DescriptionOf(FnRefl) << EvalCtx.Range;
    }

    FunctionDecl *Spec;
    {
      bool exclude_first_arg =
          is_nonstatic_member_function(FTD->getTemplatedDecl()) &&
          ArgExprs.size() > 0;

      SmallVector<TemplateArgument, 4> ExpandedTArgs;
      expandTemplateArgPacks(ExplicitTArgs, ExpandedTArgs);

      ArrayRef ArgView(ArgExprs.begin() + (exclude_first_arg ? 1 : 0),
                       ArgExprs.end());

      Spec = EvalCtx.Meta->DeduceSpecialization(FTD, ExpandedTArgs, ArgView,
                                                EvalCtx.Range.getBegin());
      if (!Spec)
        return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                 diag::metafn_no_specialization_found)
               << FTD << EvalCtx.Range;

      EvalCtx.Meta->EnsureInstantiated(Spec, EvalCtx.Range);
    }

    FnRefExpr = DeclRefExpr::Create(
        *EvalCtx.C, NestedNameSpecifierLoc(), SourceLocation(), Spec, false,
        EvalCtx.Range.getBegin(), Spec->getType(), VK_LValue, Spec, nullptr);
    break;
  }
  case ReflectionKind::EntityProxy:
    llvm_unreachable("proxies should already have been unwrapped");
  }

  Expr *CallExpr;
  {
    auto *DRE = dyn_cast<DeclRefExpr>(FnRefExpr);
    if (DRE && dyn_cast<CXXConstructorDecl>(DRE->getDecl())) {
      CallExpr = EvalCtx.Meta->SynthesizeCallExpr(DRE, ArgExprs);
    } else {
      Expr *FnExpr = FnRefExpr;
      bool handle_member_func =
          DRE && is_nonstatic_member_function(DRE->getDecl());

      if (handle_member_func) {
        if (ArgExprs.size() < 1)
          // need to have object as a first argument
          return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                   diag::metafn_first_argument_is_not_object)
                 << EvalCtx.Range;

        Expr *ObjExpr = ArgExprs[0];
        QualType ObjType = ObjExpr->getType();

        if (ObjType->isPointerType()) {
          ObjType = ObjType->getPointeeType();
          // Convert pointer to rvalue (if needed).
          APValue Val;
          if (!EvalCtx.Evaluator(Val, ObjExpr, true))
            return true;

          ObjExpr = new (*EvalCtx.C) OpaqueValueExpr(
              EvalCtx.Range.getBegin(), ObjExpr->getType(), VK_PRValue);
          ObjExpr = ConstantExpr::Create(*EvalCtx.C, ObjExpr, Val);
        }

        if (!ObjType->getAsCXXRecordDecl()) {
          // first argument is not an object
          return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                   diag::metafn_first_argument_is_not_object)
                 << EvalCtx.Range;
        }

        CXXMethodDecl *MD = getCXXMethodDeclFromDeclRefExpr(DRE, *EvalCtx.C);
        if (!MD) {
          // most likely, non-constexpr pointer to method was passed
          return true;
        }

        APValue ReflMD = makeReflection(MD);
        CXXReflectExpr *ReflMDExpr = CXXReflectExpr::Create(
            *EvalCtx.C, EvalCtx.Range.getBegin(), EvalCtx.Range, ReflMD);

        auto ObjClass = ObjType->getAsCXXRecordDecl();
        // check that method belongs to class
        bool IsMethodFromClassOrParent =
            (MD->getParent() == ObjClass) ||
            ObjClass->isDerivedFrom(MD->getParent());
        if (!IsMethodFromClassOrParent) {
          return EvalCtx.Diagnoser(
                     EvalCtx.Range.getBegin(),
                     diag::metafn_function_is_not_member_of_object)
                 << EvalCtx.Range;
        }

        if (MD->getReturnType()->isVoidType()) {
          // void return type is not supported
          return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                                   diag::metafn_function_returns_void)
                 << EvalCtx.Range;
        }

        FnExpr = EvalCtx.Meta->SynthesizeDirectMemberAccess(
            ObjExpr, ReflMDExpr, EvalCtx.Range.getBegin());
        if (!FnExpr)
          return true;
      }

      MutableArrayRef<Expr *> ArgView(
          ArgExprs.begin() + (handle_member_func ? 1 : 0), ArgExprs.end());
      CallExpr = EvalCtx.Meta->SynthesizeCallExpr(FnExpr, ArgView);
    }
  }

  if (!CallExpr)
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_invalid_call_expr)
           << EvalCtx.Range;

  if (CallExpr->isTypeDependent() || CallExpr->isValueDependent())
    return true;

  if (!CallExpr->getType()->isStructuralType() && !CallExpr->isLValue())
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_returns_non_structural_type)
           << CallExpr->getType() << EvalCtx.Range;

  Expr::EvalResult EvalResult;
  if (!CallExpr->EvaluateAsConstantExpr(EvalResult, *EvalCtx.C))
    return EvalCtx.Diagnoser(EvalCtx.Range.getBegin(),
                             diag::metafn_invocation_not_constant_expr)
           << EvalCtx.Range;

  // If this is an lvalue to a function, promote the result to reflect
  // the declaration.
  if (CallExpr->getType()->isFunctionType() &&
      EvalResult.Val.getKind() == APValue::LValue &&
      EvalResult.Val.getLValueOffset().isZero())
    if (!EvalResult.Val.hasLValuePath() ||
        EvalResult.Val.getLValuePath().size() == 0)
      if (APValue::LValueBase LVBase = EvalResult.Val.getLValueBase();
          LVBase.is<const ValueDecl *>())
        return SetAndSucceed(*EvalCtx.Result,
                             makeReflection(const_cast<ValueDecl *>(
                                 LVBase.get<const ValueDecl *>())));

  return SetAndSucceed(*EvalCtx.Result,
                       EvalResult.Val.Lift(CallExpr->getType()));
}
} // namespace

// -----------------------------------------------------------------------------
// class Metafunction implementation
// -----------------------------------------------------------------------------

#pragma region MetaFunction Call Relay Helpers
bool MetaFunction::evaluate(const MetaFunctionEvalContext &EvalCtx) const {
  return ImplFn(EvalCtx);
}

bool MetaFunction::Lookup(MetaFunctionID ID, const MetaFunction *&result) {
  if (llvm::to_underlying(ID) >= NumMetafunctions)
    return true;

  result = &Metafunctions[llvm::to_underlying(ID)];
  return result->ImplFn == nullptr;
}
#pragma endregion

} // end namespace clang
