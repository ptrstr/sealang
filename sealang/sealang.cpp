#include "sealang.h"

#include "clang/AST/Stmt.h"
#include "clang/AST/Expr.h"
#include "clang/AST/ExprCXX.h"
#include "clang/AST/ExprObjC.h"
#include "llvm/ADT/SmallString.h"

/************************************************************************
 * Duplicated libclang functionality
 *
 * The following methods are duplicates of methods that are implemented
 * in libclang, but aren't exposed as symbols that can be used by third-
 * party libraries.
 ************************************************************************/

namespace clang {
    enum CXStringFlag {
      /// CXString contains a 'const char *' that it doesn't own.
      CXS_Unmanaged,

      /// CXString contains a 'const char *' that it allocated with malloc().
      CXS_Malloc,

      /// CXString contains a CXStringBuf that needs to be returned to the
      /// CXStringPool.
      CXS_StringBuf
    };

    const clang::Stmt *getCursorStmt(CXCursor cursor) {
        if (cursor.kind == CXCursor_ObjCSuperClassRef ||
            cursor.kind == CXCursor_ObjCProtocolRef ||
            cursor.kind == CXCursor_ObjCClassRef) {

            return nullptr;
        }
        return static_cast<const clang::Stmt *>(cursor.data[1]);
    }

    const clang::Expr *getCursorExpr(CXCursor cursor) {
      return clang::dyn_cast_or_null<clang::Expr>(getCursorStmt(cursor));
    }

    namespace cxstring {
        CXString createEmpty() {
            CXString str;
            str.data = "";
            str.private_flags = CXS_Unmanaged;
            return str;
        }

        CXString createDup(StringRef string) {
            CXString result;
            char *spelling = static_cast<char *>(malloc(string.size() + 1));
            memmove(spelling, string.data(), string.size());
            spelling[string.size()] = 0;
            result.data = spelling;
            result.private_flags = (unsigned) CXS_Malloc;
            return result;
        }

    }

    namespace cxcursor {
        CXCursor MakeCXCursorInvalid(CXCursorKind K, CXTranslationUnit TU = nullptr) {
          assert(K >= CXCursor_FirstInvalid && K <= CXCursor_LastInvalid);
          CXCursor C = {K, 0, {nullptr, nullptr, TU}};
          return C;
        }

        const Decl *getCursorDecl(CXCursor Cursor) {
            return static_cast<const Decl *>(Cursor.data[0]);
        }

        CXCursor getSelectorIdentifierCursor(int SelIdx, CXCursor cursor) {
          CXCursor newCursor = cursor;

          if (cursor.kind == CXCursor_ObjCMessageExpr) {
            if (SelIdx == -1 ||
                unsigned(SelIdx) >= cast<ObjCMessageExpr>(getCursorExpr(cursor))
                                                                 ->getNumSelectorLocs())
              newCursor.xdata = -1;
            else
              newCursor.xdata = SelIdx;
          } else if (cursor.kind == CXCursor_ObjCClassMethodDecl ||
                     cursor.kind == CXCursor_ObjCInstanceMethodDecl) {
            if (SelIdx == -1 ||
                unsigned(SelIdx) >= cast<ObjCMethodDecl>(getCursorDecl(cursor))
                                                                 ->getNumSelectorLocs())
              newCursor.xdata = -1;
            else
              newCursor.xdata = SelIdx;
          }

          return newCursor;
        }

        CXCursor MakeCXCursor(const Stmt *S, const Decl *Parent,
                                        CXTranslationUnit TU,
                                        SourceRange RegionOfInterest = SourceRange()) {
          assert(S && TU && "Invalid arguments!");
          CXCursorKind K = CXCursor_NotImplemented;

          switch (S->getStmtClass()) {
          case Stmt::NoStmtClass:
            break;

          case Stmt::CaseStmtClass:
            K = CXCursor_CaseStmt;
            break;

          case Stmt::DefaultStmtClass:
            K = CXCursor_DefaultStmt;
            break;

          case Stmt::IfStmtClass:
            K = CXCursor_IfStmt;
            break;

          case Stmt::SwitchStmtClass:
            K = CXCursor_SwitchStmt;
            break;

          case Stmt::WhileStmtClass:
            K = CXCursor_WhileStmt;
            break;

          case Stmt::DoStmtClass:
            K = CXCursor_DoStmt;
            break;

          case Stmt::ForStmtClass:
            K = CXCursor_ForStmt;
            break;

          case Stmt::GotoStmtClass:
            K = CXCursor_GotoStmt;
            break;

          case Stmt::IndirectGotoStmtClass:
            K = CXCursor_IndirectGotoStmt;
            break;

          case Stmt::ContinueStmtClass:
            K = CXCursor_ContinueStmt;
            break;

          case Stmt::BreakStmtClass:
            K = CXCursor_BreakStmt;
            break;

          case Stmt::ReturnStmtClass:
            K = CXCursor_ReturnStmt;
            break;

          case Stmt::GCCAsmStmtClass:
            K = CXCursor_GCCAsmStmt;
            break;

          case Stmt::MSAsmStmtClass:
            K = CXCursor_MSAsmStmt;
            break;

          case Stmt::ObjCAtTryStmtClass:
            K = CXCursor_ObjCAtTryStmt;
            break;

          case Stmt::ObjCAtCatchStmtClass:
            K = CXCursor_ObjCAtCatchStmt;
            break;

          case Stmt::ObjCAtFinallyStmtClass:
            K = CXCursor_ObjCAtFinallyStmt;
            break;

          case Stmt::ObjCAtThrowStmtClass:
            K = CXCursor_ObjCAtThrowStmt;
            break;

          case Stmt::ObjCAtSynchronizedStmtClass:
            K = CXCursor_ObjCAtSynchronizedStmt;
            break;

          case Stmt::ObjCAutoreleasePoolStmtClass:
            K = CXCursor_ObjCAutoreleasePoolStmt;
            break;

          case Stmt::ObjCForCollectionStmtClass:
            K = CXCursor_ObjCForCollectionStmt;
            break;

          case Stmt::CXXCatchStmtClass:
            K = CXCursor_CXXCatchStmt;
            break;

          case Stmt::CXXTryStmtClass:
            K = CXCursor_CXXTryStmt;
            break;

          case Stmt::CXXForRangeStmtClass:
            K = CXCursor_CXXForRangeStmt;
            break;

          case Stmt::SEHTryStmtClass:
            K = CXCursor_SEHTryStmt;
            break;

          case Stmt::SEHExceptStmtClass:
            K = CXCursor_SEHExceptStmt;
            break;

          case Stmt::SEHFinallyStmtClass:
            K = CXCursor_SEHFinallyStmt;
            break;

          case Stmt::SEHLeaveStmtClass:
            K = CXCursor_SEHLeaveStmt;
            break;

          case Stmt::CoroutineBodyStmtClass:
          case Stmt::CoreturnStmtClass:
            K = CXCursor_UnexposedStmt;
            break;

          case Stmt::ArrayTypeTraitExprClass:
          case Stmt::AsTypeExprClass:
          case Stmt::AtomicExprClass:
          case Stmt::BinaryConditionalOperatorClass:
          case Stmt::TypeTraitExprClass:
          case Stmt::CoawaitExprClass:
          case Stmt::ConceptSpecializationExprClass:
          case Stmt::RequiresExprClass:
          case Stmt::DependentCoawaitExprClass:
          case Stmt::CoyieldExprClass:
          case Stmt::CXXBindTemporaryExprClass:
          case Stmt::CXXDefaultArgExprClass:
          case Stmt::CXXDefaultInitExprClass:
          case Stmt::CXXFoldExprClass:
          case Stmt::CXXRewrittenBinaryOperatorClass:
          case Stmt::CXXStdInitializerListExprClass:
          case Stmt::CXXScalarValueInitExprClass:
          case Stmt::CXXUuidofExprClass:
          case Stmt::ChooseExprClass:
          case Stmt::DesignatedInitExprClass:
          case Stmt::DesignatedInitUpdateExprClass:
          case Stmt::ArrayInitLoopExprClass:
          case Stmt::ArrayInitIndexExprClass:
          case Stmt::ExprWithCleanupsClass:
          case Stmt::ExpressionTraitExprClass:
          case Stmt::ExtVectorElementExprClass:
          case Stmt::ImplicitCastExprClass:
          case Stmt::ImplicitValueInitExprClass:
          case Stmt::NoInitExprClass:
          case Stmt::MaterializeTemporaryExprClass:
          case Stmt::ObjCIndirectCopyRestoreExprClass:
          case Stmt::OffsetOfExprClass:
          case Stmt::ParenListExprClass:
          case Stmt::PredefinedExprClass:
          case Stmt::ShuffleVectorExprClass:
          case Stmt::SourceLocExprClass:
          case Stmt::ConvertVectorExprClass:
          case Stmt::VAArgExprClass:
          case Stmt::ObjCArrayLiteralClass:
          case Stmt::ObjCDictionaryLiteralClass:
          case Stmt::ObjCBoxedExprClass:
          case Stmt::ObjCSubscriptRefExprClass:
            K = CXCursor_UnexposedExpr;
            break;

          case Stmt::OpaqueValueExprClass:
            if (Expr *Src = cast<OpaqueValueExpr>(S)->getSourceExpr())
              return MakeCXCursor(Src, Parent, TU, RegionOfInterest);
            K = CXCursor_UnexposedExpr;
            break;

          case Stmt::PseudoObjectExprClass:
            return MakeCXCursor(cast<PseudoObjectExpr>(S)->getSyntacticForm(),
                                Parent, TU, RegionOfInterest);

          case Stmt::CompoundStmtClass:
            K = CXCursor_CompoundStmt;
            break;

          case Stmt::NullStmtClass:
            K = CXCursor_NullStmt;
            break;

          case Stmt::LabelStmtClass:
            K = CXCursor_LabelStmt;
            break;

          case Stmt::AttributedStmtClass:
            K = CXCursor_UnexposedStmt;
            break;

          case Stmt::DeclStmtClass:
            K = CXCursor_DeclStmt;
            break;

          case Stmt::CapturedStmtClass:
            K = CXCursor_UnexposedStmt;
            break;

          case Stmt::IntegerLiteralClass:
            K = CXCursor_IntegerLiteral;
            break;

          case Stmt::FixedPointLiteralClass:
            K = CXCursor_FixedPointLiteral;
            break;

          case Stmt::FloatingLiteralClass:
            K = CXCursor_FloatingLiteral;
            break;

          case Stmt::ImaginaryLiteralClass:
            K = CXCursor_ImaginaryLiteral;
            break;

          case Stmt::StringLiteralClass:
            K = CXCursor_StringLiteral;
            break;

          case Stmt::CharacterLiteralClass:
            K = CXCursor_CharacterLiteral;
            break;

          case Stmt::ConstantExprClass:
            return MakeCXCursor(cast<ConstantExpr>(S)->getSubExpr(),
                                Parent, TU, RegionOfInterest);

          case Stmt::ParenExprClass:
            K = CXCursor_ParenExpr;
            break;

          case Stmt::UnaryOperatorClass:
            K = CXCursor_UnaryOperator;
            break;

          case Stmt::UnaryExprOrTypeTraitExprClass:
          case Stmt::CXXNoexceptExprClass:
            K = CXCursor_UnaryExpr;
            break;

          case Stmt::MSPropertySubscriptExprClass:
          case Stmt::ArraySubscriptExprClass:
            K = CXCursor_ArraySubscriptExpr;
            break;

          case Stmt::OMPArraySectionExprClass:
            K = CXCursor_OMPArraySectionExpr;
            break;

          case Stmt::BinaryOperatorClass:
            K = CXCursor_BinaryOperator;
            break;

          case Stmt::CompoundAssignOperatorClass:
            K = CXCursor_CompoundAssignOperator;
            break;

          case Stmt::ConditionalOperatorClass:
            K = CXCursor_ConditionalOperator;
            break;

          case Stmt::CStyleCastExprClass:
            K = CXCursor_CStyleCastExpr;
            break;

          case Stmt::CompoundLiteralExprClass:
            K = CXCursor_CompoundLiteralExpr;
            break;

          case Stmt::InitListExprClass:
            K = CXCursor_InitListExpr;
            break;

          case Stmt::AddrLabelExprClass:
            K = CXCursor_AddrLabelExpr;
            break;

          case Stmt::StmtExprClass:
            K = CXCursor_StmtExpr;
            break;

          case Stmt::GenericSelectionExprClass:
            K = CXCursor_GenericSelectionExpr;
            break;

          case Stmt::GNUNullExprClass:
            K = CXCursor_GNUNullExpr;
            break;

          case Stmt::CXXStaticCastExprClass:
            K = CXCursor_CXXStaticCastExpr;
            break;

          case Stmt::CXXDynamicCastExprClass:
            K = CXCursor_CXXDynamicCastExpr;
            break;

          case Stmt::CXXReinterpretCastExprClass:
            K = CXCursor_CXXReinterpretCastExpr;
            break;

          case Stmt::CXXConstCastExprClass:
            K = CXCursor_CXXConstCastExpr;
            break;

          case Stmt::CXXFunctionalCastExprClass:
            K = CXCursor_CXXFunctionalCastExpr;
            break;

          case Stmt::CXXTypeidExprClass:
            K = CXCursor_CXXTypeidExpr;
            break;

          case Stmt::CXXBoolLiteralExprClass:
            K = CXCursor_CXXBoolLiteralExpr;
            break;

          case Stmt::CXXNullPtrLiteralExprClass:
            K = CXCursor_CXXNullPtrLiteralExpr;
            break;

          case Stmt::CXXThisExprClass:
            K = CXCursor_CXXThisExpr;
            break;

          case Stmt::CXXThrowExprClass:
            K = CXCursor_CXXThrowExpr;
            break;

          case Stmt::CXXNewExprClass:
            K = CXCursor_CXXNewExpr;
            break;

          case Stmt::CXXDeleteExprClass:
            K = CXCursor_CXXDeleteExpr;
            break;

          case Stmt::ObjCStringLiteralClass:
            K = CXCursor_ObjCStringLiteral;
            break;

          case Stmt::ObjCEncodeExprClass:
            K = CXCursor_ObjCEncodeExpr;
            break;

          case Stmt::ObjCSelectorExprClass:
            K = CXCursor_ObjCSelectorExpr;
            break;

          case Stmt::ObjCProtocolExprClass:
            K = CXCursor_ObjCProtocolExpr;
            break;

          case Stmt::ObjCBoolLiteralExprClass:
            K = CXCursor_ObjCBoolLiteralExpr;
            break;

          case Stmt::ObjCAvailabilityCheckExprClass:
            K = CXCursor_ObjCAvailabilityCheckExpr;
            break;

          case Stmt::ObjCBridgedCastExprClass:
            K = CXCursor_ObjCBridgedCastExpr;
            break;

          case Stmt::BlockExprClass:
            K = CXCursor_BlockExpr;
            break;

          case Stmt::PackExpansionExprClass:
            K = CXCursor_PackExpansionExpr;
            break;

          case Stmt::SizeOfPackExprClass:
            K = CXCursor_SizeOfPackExpr;
            break;

          case Stmt::DeclRefExprClass:
            if (const ImplicitParamDecl *IPD =
                 dyn_cast_or_null<ImplicitParamDecl>(cast<DeclRefExpr>(S)->getDecl())) {
              if (const ObjCMethodDecl *MD =
                    dyn_cast<ObjCMethodDecl>(IPD->getDeclContext())) {
                if (MD->getSelfDecl() == IPD) {
                  K = CXCursor_ObjCSelfExpr;
                  break;
                }
              }
            }

            K = CXCursor_DeclRefExpr;
            break;

          case Stmt::DependentScopeDeclRefExprClass:
          case Stmt::SubstNonTypeTemplateParmExprClass:
          case Stmt::SubstNonTypeTemplateParmPackExprClass:
          case Stmt::FunctionParmPackExprClass:
          case Stmt::UnresolvedLookupExprClass:
          case Stmt::TypoExprClass: // A typo could actually be a DeclRef or a MemberRef
            K = CXCursor_DeclRefExpr;
            break;

          case Stmt::CXXDependentScopeMemberExprClass:
          case Stmt::CXXPseudoDestructorExprClass:
          case Stmt::MemberExprClass:
          case Stmt::MSPropertyRefExprClass:
          case Stmt::ObjCIsaExprClass:
          case Stmt::ObjCIvarRefExprClass:
          case Stmt::ObjCPropertyRefExprClass:
          case Stmt::UnresolvedMemberExprClass:
            K = CXCursor_MemberRefExpr;
            break;

          case Stmt::CallExprClass:
          case Stmt::CXXOperatorCallExprClass:
          case Stmt::CXXMemberCallExprClass:
          case Stmt::CUDAKernelCallExprClass:
          case Stmt::CXXConstructExprClass:
          case Stmt::CXXInheritedCtorInitExprClass:
          case Stmt::CXXTemporaryObjectExprClass:
          case Stmt::CXXUnresolvedConstructExprClass:
          case Stmt::UserDefinedLiteralClass:
            K = CXCursor_CallExpr;
            break;

          case Stmt::LambdaExprClass:
            K = CXCursor_LambdaExpr;
            break;

          case Stmt::ObjCMessageExprClass: {
            K = CXCursor_ObjCMessageExpr;
            int SelectorIdIndex = -1;
            // Check if cursor points to a selector id.
            if (RegionOfInterest.isValid() &&
                RegionOfInterest.getBegin() == RegionOfInterest.getEnd()) {
              SmallVector<SourceLocation, 16> SelLocs;
              cast<ObjCMessageExpr>(S)->getSelectorLocs(SelLocs);
              SmallVectorImpl<SourceLocation>::iterator I =
                  llvm::find(SelLocs, RegionOfInterest.getBegin());
              if (I != SelLocs.end())
                SelectorIdIndex = I - SelLocs.begin();
            }
            CXCursor C = { K, 0, { Parent, S, TU } };
            return getSelectorIdentifierCursor(SelectorIdIndex, C);
          }

          case Stmt::MSDependentExistsStmtClass:
            K = CXCursor_UnexposedStmt;
            break;
          case Stmt::OMPParallelDirectiveClass:
            K = CXCursor_OMPParallelDirective;
            break;
          case Stmt::OMPSimdDirectiveClass:
            K = CXCursor_OMPSimdDirective;
            break;
          case Stmt::OMPForDirectiveClass:
            K = CXCursor_OMPForDirective;
            break;
          case Stmt::OMPForSimdDirectiveClass:
            K = CXCursor_OMPForSimdDirective;
            break;
          case Stmt::OMPSectionsDirectiveClass:
            K = CXCursor_OMPSectionsDirective;
            break;
          case Stmt::OMPSectionDirectiveClass:
            K = CXCursor_OMPSectionDirective;
            break;
          case Stmt::OMPSingleDirectiveClass:
            K = CXCursor_OMPSingleDirective;
            break;
          case Stmt::OMPMasterDirectiveClass:
            K = CXCursor_OMPMasterDirective;
            break;
          case Stmt::OMPCriticalDirectiveClass:
            K = CXCursor_OMPCriticalDirective;
            break;
          case Stmt::OMPParallelForDirectiveClass:
            K = CXCursor_OMPParallelForDirective;
            break;
          case Stmt::OMPParallelForSimdDirectiveClass:
            K = CXCursor_OMPParallelForSimdDirective;
            break;
          case Stmt::OMPParallelMasterDirectiveClass:
            K = CXCursor_OMPParallelMasterDirective;
            break;
          case Stmt::OMPParallelSectionsDirectiveClass:
            K = CXCursor_OMPParallelSectionsDirective;
            break;
          case Stmt::OMPTaskDirectiveClass:
            K = CXCursor_OMPTaskDirective;
            break;
          case Stmt::OMPTaskyieldDirectiveClass:
            K = CXCursor_OMPTaskyieldDirective;
            break;
          case Stmt::OMPBarrierDirectiveClass:
            K = CXCursor_OMPBarrierDirective;
            break;
          case Stmt::OMPTaskwaitDirectiveClass:
            K = CXCursor_OMPTaskwaitDirective;
            break;
          case Stmt::OMPTaskgroupDirectiveClass:
            K = CXCursor_OMPTaskgroupDirective;
            break;
          case Stmt::OMPFlushDirectiveClass:
            K = CXCursor_OMPFlushDirective;
            break;
          case Stmt::OMPOrderedDirectiveClass:
            K = CXCursor_OMPOrderedDirective;
            break;
          case Stmt::OMPAtomicDirectiveClass:
            K = CXCursor_OMPAtomicDirective;
            break;
          case Stmt::OMPTargetDirectiveClass:
            K = CXCursor_OMPTargetDirective;
            break;
          case Stmt::OMPTargetDataDirectiveClass:
            K = CXCursor_OMPTargetDataDirective;
            break;
          case Stmt::OMPTargetEnterDataDirectiveClass:
            K = CXCursor_OMPTargetEnterDataDirective;
            break;
          case Stmt::OMPTargetExitDataDirectiveClass:
            K = CXCursor_OMPTargetExitDataDirective;
            break;
          case Stmt::OMPTargetParallelDirectiveClass:
            K = CXCursor_OMPTargetParallelDirective;
            break;
          case Stmt::OMPTargetParallelForDirectiveClass:
            K = CXCursor_OMPTargetParallelForDirective;
            break;
          case Stmt::OMPTargetUpdateDirectiveClass:
            K = CXCursor_OMPTargetUpdateDirective;
            break;
          case Stmt::OMPTeamsDirectiveClass:
            K = CXCursor_OMPTeamsDirective;
            break;
          case Stmt::OMPCancellationPointDirectiveClass:
            K = CXCursor_OMPCancellationPointDirective;
            break;
          case Stmt::OMPCancelDirectiveClass:
            K = CXCursor_OMPCancelDirective;
            break;
          case Stmt::OMPTaskLoopDirectiveClass:
            K = CXCursor_OMPTaskLoopDirective;
            break;
          case Stmt::OMPTaskLoopSimdDirectiveClass:
            K = CXCursor_OMPTaskLoopSimdDirective;
            break;
          case Stmt::OMPMasterTaskLoopDirectiveClass:
            K = CXCursor_OMPMasterTaskLoopDirective;
            break;
          case Stmt::OMPMasterTaskLoopSimdDirectiveClass:
            K = CXCursor_OMPMasterTaskLoopSimdDirective;
            break;
          case Stmt::OMPParallelMasterTaskLoopDirectiveClass:
            K = CXCursor_OMPParallelMasterTaskLoopDirective;
            break;
          case Stmt::OMPParallelMasterTaskLoopSimdDirectiveClass:
            K = CXCursor_OMPParallelMasterTaskLoopSimdDirective;
            break;
          case Stmt::OMPDistributeDirectiveClass:
            K = CXCursor_OMPDistributeDirective;
            break;
          case Stmt::OMPDistributeParallelForDirectiveClass:
            K = CXCursor_OMPDistributeParallelForDirective;
            break;
          case Stmt::OMPDistributeParallelForSimdDirectiveClass:
            K = CXCursor_OMPDistributeParallelForSimdDirective;
            break;
          case Stmt::OMPDistributeSimdDirectiveClass:
            K = CXCursor_OMPDistributeSimdDirective;
            break;
          case Stmt::OMPTargetParallelForSimdDirectiveClass:
            K = CXCursor_OMPTargetParallelForSimdDirective;
            break;
          case Stmt::OMPTargetSimdDirectiveClass:
            K = CXCursor_OMPTargetSimdDirective;
            break;
          case Stmt::OMPTeamsDistributeDirectiveClass:
            K = CXCursor_OMPTeamsDistributeDirective;
            break;
          case Stmt::OMPTeamsDistributeSimdDirectiveClass:
            K = CXCursor_OMPTeamsDistributeSimdDirective;
            break;
          case Stmt::OMPTeamsDistributeParallelForSimdDirectiveClass:
            K = CXCursor_OMPTeamsDistributeParallelForSimdDirective;
            break;
          case Stmt::OMPTeamsDistributeParallelForDirectiveClass:
            K = CXCursor_OMPTeamsDistributeParallelForDirective;
            break;
          case Stmt::OMPTargetTeamsDirectiveClass:
            K = CXCursor_OMPTargetTeamsDirective;
            break;
          case Stmt::OMPTargetTeamsDistributeDirectiveClass:
            K = CXCursor_OMPTargetTeamsDistributeDirective;
            break;
          case Stmt::OMPTargetTeamsDistributeParallelForDirectiveClass:
            K = CXCursor_OMPTargetTeamsDistributeParallelForDirective;
            break;
          case Stmt::OMPTargetTeamsDistributeParallelForSimdDirectiveClass:
            K = CXCursor_OMPTargetTeamsDistributeParallelForSimdDirective;
            break;
          case Stmt::OMPTargetTeamsDistributeSimdDirectiveClass:
            K = CXCursor_OMPTargetTeamsDistributeSimdDirective;
            break;
          case Stmt::BuiltinBitCastExprClass:
            K = CXCursor_BuiltinBitCastExpr;
          }

          CXCursor C = { K, 0, { Parent, S, TU } };
          return C;
        }
    }
}

/************************************************************************
 * New Sealang functionality
 *
 * The following methods expose useful features of the LLVM AST. They are
 * all potentially candidates for inclusion upstream in libclang.
 ************************************************************************/

CXString clang_Cursor_getOperatorString(CXCursor cursor)
{
    if (cursor.kind == CXCursor_BinaryOperator) {
        const clang::BinaryOperator *op = (const clang::BinaryOperator *) clang::getCursorExpr(cursor);
        return clang::cxstring::createDup(clang::BinaryOperator::getOpcodeStr(op->getOpcode()));
    }

    if (cursor.kind == CXCursor_CompoundAssignOperator) {
        const clang::CompoundAssignOperator *op = (const clang::CompoundAssignOperator*) clang::getCursorExpr(cursor);
        return clang::cxstring::createDup(clang::BinaryOperator::getOpcodeStr(op->getOpcode()));
    }

    if (cursor.kind == CXCursor_UnaryOperator) {
        const clang::UnaryOperator *op = (const clang::UnaryOperator*) clang::getCursorExpr(cursor);
        return clang::cxstring::createDup(clang::UnaryOperator::getOpcodeStr(op->getOpcode()));
    }

    return clang::cxstring::createEmpty();
}

clang::BinaryOperatorKind clang_Cursor_getBinaryOpcode(CXCursor cursor)
{
    if (cursor.kind == CXCursor_BinaryOperator) {
        const clang::BinaryOperator *op = (const clang::BinaryOperator *) clang::getCursorExpr(cursor);
        return static_cast<clang::BinaryOperatorKind>(op->getOpcode());
    }

    if (cursor.kind == CXCursor_CompoundAssignOperator) {
        const clang::CompoundAssignOperator *op = (const clang::CompoundAssignOperator *) clang::getCursorExpr(cursor);
        return static_cast<clang::BinaryOperatorKind>(op->getOpcode());
    }

    return (clang::BinaryOperatorKind) 99999;
}

clang::UnaryOperatorKind clang_Cursor_getUnaryOpcode(CXCursor cursor)
{
    if (cursor.kind == CXCursor_UnaryOperator) {
        const clang::UnaryOperator *op = (const clang::UnaryOperator*) clang::getCursorExpr(cursor);
        return static_cast<clang::UnaryOperatorKind>(op->getOpcode());
    }

    return (clang::UnaryOperatorKind) 99999;
}

CXString clang_Cursor_getLiteralString(CXCursor cursor)
{
    if (cursor.kind == CXCursor_IntegerLiteral) {
        const clang::IntegerLiteral *intLiteral = (const clang::IntegerLiteral *) clang::getCursorExpr(cursor);
        return clang::cxstring::createDup(intLiteral->getValue().toString(10, true));
    }

    if (cursor.kind == CXCursor_FloatingLiteral) {
        const clang::FloatingLiteral *floatLiteral = (const clang::FloatingLiteral *) clang::getCursorExpr(cursor);
        llvm::SmallString<1024> str;
        floatLiteral->getValue().toString(str);
        return clang::cxstring::createDup(str.c_str());
    }

    if (cursor.kind == CXCursor_CharacterLiteral) {
        const clang::CharacterLiteral *charLiteral = (const clang::CharacterLiteral *) clang::getCursorExpr(cursor);
        char c[2];
        c[0] = (char) charLiteral->getValue();
        c[1] = '\0';
        return clang::cxstring::createDup(c);
    }

    if (cursor.kind == CXCursor_StringLiteral) {
        const clang::StringLiteral *stringLiteral = (const clang::StringLiteral *) clang::getCursorExpr(cursor);
        return clang::cxstring::createDup(stringLiteral->getBytes());
    }

    if (cursor.kind == CXCursor_CXXBoolLiteralExpr) {
        const clang::CXXBoolLiteralExpr *boolLiteral = (const clang::CXXBoolLiteralExpr *) clang::getCursorExpr(cursor);
        return clang::cxstring::createDup(boolLiteral->getValue() ? "true" : "false");
    }

    return clang::cxstring::createEmpty();
}

CXCursor clang_getForStmtInit(CXCursor cursor)
{
	if(cursor.kind!=CXCursor_ForStmt) return clang::cxcursor::MakeCXCursorInvalid(CXCursor_InvalidCode);

	clang::ForStmt* Node=(clang::ForStmt*)(cursor.data[1]);
	const CXTranslationUnit tu=(const CXTranslationUnit)(cursor.data[2]);

	clang::Stmt* init=Node->getInit();
	if (init) return clang::cxcursor::MakeCXCursor(init,0,tu);
	else return clang::cxcursor::MakeCXCursorInvalid(CXCursor_NoDeclFound);
}

CXCursor clang_getForStmtCond(CXCursor cursor)
{
	if(cursor.kind!=CXCursor_ForStmt) return clang::cxcursor::MakeCXCursorInvalid(CXCursor_InvalidCode);

	clang::ForStmt* Node=(clang::ForStmt*)(cursor.data[1]);
	const CXTranslationUnit tu=(const CXTranslationUnit)(cursor.data[2]);

	clang::Stmt* cond=Node->getCond();
	if (cond) return clang::cxcursor::MakeCXCursor(cond,0,tu);
	else return clang::cxcursor::MakeCXCursorInvalid(CXCursor_NoDeclFound);
}

CXCursor clang_getForStmtInc(CXCursor cursor)
{
	if(cursor.kind!=CXCursor_ForStmt) return clang::cxcursor::MakeCXCursorInvalid(CXCursor_InvalidCode);

	clang::ForStmt* Node=(clang::ForStmt*)(cursor.data[1]);
	const CXTranslationUnit tu=(const CXTranslationUnit)(cursor.data[2]);

	clang::Stmt* inc=Node->getInc();
	if (inc) return clang::cxcursor::MakeCXCursor(inc,0,tu);
	else return clang::cxcursor::MakeCXCursorInvalid(CXCursor_NoDeclFound);
}

CXCursor clang_getForStmtBody(CXCursor cursor)
{
	if(cursor.kind!=CXCursor_ForStmt) return clang::cxcursor::MakeCXCursorInvalid(CXCursor_InvalidCode);

	clang::ForStmt* Node=(clang::ForStmt*)(cursor.data[1]);
	const CXTranslationUnit tu=(const CXTranslationUnit)(cursor.data[2]);

	clang::Stmt* body=Node->getBody();
	if (body) return clang::cxcursor::MakeCXCursor(body,0,tu);
	else return clang::cxcursor::MakeCXCursorInvalid(CXCursor_NoDeclFound);
}

/************************************************************************
 * Python module definition
 *
 * This is a stub module definition; we aren't exposing any Python
 * methods - we're just making the module .so easy to find.
 ************************************************************************/

static PyMethodDef methods[] = {
    {NULL, NULL, 0, NULL}
};

#if PY_MAJOR_VERSION <= 2

PyMODINIT_FUNC initsealang()
{
    (void) Py_InitModule("sealang", methods);
}

#else

static struct PyModuleDef sealangmodule = {
    PyModuleDef_HEAD_INIT,
    "sealang",
    NULL,
    -1,
    methods,
    NULL,
    NULL,
    NULL,
    NULL
};

PyMODINIT_FUNC PyInit_sealang()
{
    return PyModule_Create(&sealangmodule);
}

#endif
