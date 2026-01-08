//===-- Metafunction.h - Classes for representing metafunctions--*- C++ -*-===//
//
// Copyright 2024 Bloomberg Finance L.P.
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
#include "clang/AST/MetaFunctionBase.h"
#include "clang/AST/TypeBase.h"

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

private:
  using impl_fn_t = MetaFunctionImplSignature;

  // 8 bytes
  impl_fn_t *ImplFn = nullptr;
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

  bool evaluate(const MetaFunctionEvalContext &EvalCtx) const;

  // Get a pointer to the metafunction with the given ID.
  // Returns true in the case of error (i.e., no such metafunction exists).
  static bool Lookup(MetaFunctionID ID, const Metafunction *&result);
};

// Make it compact.
static_assert(sizeof(Metafunction) == 2 * sizeof(void *));

} // namespace clang

#endif
