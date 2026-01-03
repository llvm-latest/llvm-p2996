//===-- Metafunction.h - Classes for representing metafunctions--*- C++ -*-===//
//
// Copyright 2024 Bloomberg Finance L.P.
// Copyright 2026 Yukino Hayakawa
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines facilities for representing functions involving reflections.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_AST_METAFUNCTION_H
#define LLVM_CLANG_AST_METAFUNCTION_H

#include "clang/AST/ASTContext.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/MetaActions.h"
#include "clang/AST/TypeBase.h"
#include "clang/AST/MetaFunctionBase.h"

namespace clang {

class APValue;

class Metafunction {
public:
  // Enumerators identifying the return-type of a metafunction.
  enum ResultKind : std::uint8_t {
    MFRK_bool,
    MFRK_metaInfo,
    MFRK_sizeT,
    MFRK_sourceLoc,
    MFRK_spliceFromArg,
    MFRK_maxNum, // sentinel
  };

  using EvaluateFn = CXXMetafunctionExpr::EvaluateFn;
  using DiagnoseFn = CXXMetafunctionExpr::DiagnoseFn;

private:
  // todo [Yukino:This might be too verbose. Consider using a context object.]
  using impl_fn_t = bool (*)(APValue &Result, ASTContext &C, MetaActions &Meta,
                             EvaluateFn Evaluator, DiagnoseFn Diagnoser,
                             bool AllowInjection, QualType ResultType,
                             SourceRange Range, ArrayRef<Expr *> Args,
                             Decl *ContainingDecl);

  // 8 bytes
  impl_fn_t ImplFn;
  // 4 bytes
  ResultKind Kind;
  std::uint8_t MinArgs;
  std::uint8_t MaxArgs;
  MetaFunctionID ID;

public:
  constexpr Metafunction(ResultKind ResultKind, std::uint8_t MinArgs,
                         std::uint8_t MaxArgs, MetaFunctionID ID,
                         impl_fn_t ImplFn)
      : ImplFn(ImplFn), Kind(ResultKind), MinArgs(MinArgs), MaxArgs(MaxArgs),
        ID(ID) {}

  ResultKind getResultKind() const { return Kind; }

  std::uint8_t getMinArgs() const { return MinArgs; }

  std::uint8_t getMaxArgs() const { return MaxArgs; }

  bool evaluate(APValue &Result, ASTContext &C, MetaActions &Meta,
                EvaluateFn Evaluator, DiagnoseFn Diagnoser, bool AllowInjection,
                QualType ResultType, SourceRange Range, ArrayRef<Expr *> Args,
                Decl *ContainingDecl) const;

  // Get a pointer to the metafunction with the given ID.
  // Returns true in the case of error (i.e., no such metafunction exists).
  static bool Lookup(MetaFunctionID ID, const Metafunction *&result);
};

// Make it compact.
static_assert(sizeof(Metafunction) == 2 * sizeof(void *));

} // namespace clang

#endif
