#include "../include/parser.h"

namespace parser {

    // TODO  implement parser::Array & parser::Pointer

    Primitive::Primitive() {}

    ast::Type *Primitive::to_ast_type(ast::Type *primitive_type) {
        return primitive_type;
    }

    VarDefManager::VarDefManager(TypeManger *type_manager, ast::Id *id)
        : type_manager(type_manager), id(id), has_init(false), init(nullptr) {}

    VarDefManager::VarDefManager(TypeManger *type_manager, ast::Id *id, ast::Expr *init)
        : type_manager(type_manager), id(id), has_init(true), init(init) {}

    ast::VarDef *VarDefManager::to_ast_var_def(
        ast::Type *primitive_type, 
        ast::DeclType decl_type
    ) {
        if (has_init) {
            return new ast::VarDef(
                type_manager->to_ast_type(primitive_type),
                id,
                init,
                decl_type
            );
        }
        else {
            return new ast::VarDef(
                type_manager->to_ast_type(primitive_type),
                id,
                decl_type
            );
        }
    }

    ast::GlobalVarDef *VarDefManager::to_ast_global_var_def(
        ast::Type *primitive_type, 
        ast::DeclType decl_type
    ) {
        if (has_init) {
            return new ast::GlobalVarDef(
                type_manager->to_ast_type(primitive_type),
                id,
                init,
                decl_type
            );
        }
        else {
            return new ast::GlobalVarDef(
                type_manager->to_ast_type(primitive_type),
                id,
                decl_type
            );
        }
    }

}