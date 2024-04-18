#ifndef PARSER_H_
#define PARSER_H_

#include "ast.h"

/**
 * provide intermediate representations for parsing
 */

namespace parser {

    /**
     * hold incomplete `ast::Type`, whose primitive type has not been specified
     */
    class TypeManger {
    public:
        virtual ast::Type *to_ast_type(ast::Type *primitive_type) = 0;
    };

        class Array: public TypeManger {
        public:
            Array(int, TypeManger);
            ast::Type *to_ast_type(ast::Type *primitive_type) override;

        private:
            int size;
            TypeManger *element_type;
        };

        class Pointer: public TypeManger {
        public:
            Pointer(TypeManger);
            ast::Type *to_ast_type(ast::Type *primitive_type) override;

        private:
            TypeManger *pointed_type;
        };

        class Primitive: public TypeManger {
        public:
            Primitive();
            ast::Type *to_ast_type(ast::Type *primitive_type) override;
        };

    /**
     * hold incomplete `ast::VarDef`, whose primitive type 
     * and constancy have not been specified
     * @example  a[][3] 
     *      => VarDefManger { a, Pointer { Array[3] { Primitive } } }

     * use `to_ast_var_def( extra information )` to transform it 
     * into a complete `ast::VarDef`
     * @example const int a[3] 
     *      => VarDefManager { a, Array[3] { Primitive } }
     *          .to_ast_var_def(is_constant = true, ast::Int *)
     */
    class VarDefManager {
    public:
        VarDefManager(TypeManger *, ast::Id *);
        VarDefManager(TypeManger *, ast::Id *, ast::Expr *init);

        ast::VarDef *to_ast_var_def(
            ast::Type *primitive_type, 
            ast::DeclType
        );

        ast::GlobalVarDef *to_ast_global_var_def(
            ast::Type *primitive_type, 
            ast::DeclType
        );

    private:
        TypeManger *type_manager;
        ast::Id    *id;
        bool        has_init;
        ast::Expr  *init;
    };

}

#endif