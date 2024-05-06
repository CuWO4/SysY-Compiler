#include "parser.hpp"

namespace parser {

    Array::Array(ast::Expr* length, TypeManger* element_type)
        : length(length), element_type(element_type) {}

    ast::Type* Array::to_ast_type(ast::Type* primitive_type) {
        return new ast::Array(
            element_type->to_ast_type(primitive_type),
            length
        );
    }

    Pointer::Pointer(TypeManger* pointed_type): pointed_type(pointed_type) {}

    ast::Type* Pointer::to_ast_type(ast::Type* primitive_type) {
        return new ast::Pointer(pointed_type->to_ast_type(primitive_type));
    }

    Primitive::Primitive() {}

    ast::Type* Primitive::to_ast_type(ast::Type* primitive_type) {
        return primitive_type;
    }

    VarDefManager::VarDefManager(TypeManger* type_manager, ast::Id* id)
        : type_manager(type_manager), id(id), has_init_bool(false), init(nullptr) {}

    VarDefManager::VarDefManager(
        TypeManger* type_manager, ast::Id* id, ast::Initializer* init
    ) : type_manager(type_manager), id(id), has_init_bool(true), init(init) {}

    ast::Type* VarDefManager::get_ast_type(ast::Type* primitive_type) {
        return type_manager->to_ast_type(primitive_type);
    }

    ast::Id* VarDefManager::get_id() { return id; }

    bool VarDefManager::has_init() { return has_init_bool; }

    void VarDefManager::wrap_pointer() {
        type_manager = new Pointer(type_manager);
    }

    void VarDefManager::wrap_array(ast::Expr* length) {
        type_manager = new Array(length, type_manager);
    }

}