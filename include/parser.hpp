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
        virtual ast::Type* to_ast_type(ast::Type* primitive_type) = 0;
    };

        class Array: public TypeManger {
        public:
            Array(ast::Expr* length, TypeManger* element_type);
            ast::Type* to_ast_type(ast::Type* primitive_type) override;

        private:
            ast::Expr* length;
            TypeManger* element_type;
        };

        class Pointer: public TypeManger {
        public:
            Pointer(TypeManger* pointed_type);
            ast::Type* to_ast_type(ast::Type* primitive_type) override;

        private:
            TypeManger* pointed_type;
        };

        class Primitive: public TypeManger {
        public:
            Primitive();
            ast::Type* to_ast_type(ast::Type* primitive_type) override;
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
        VarDefManager(TypeManger *, ast::Id *, ast::Initializer* init);

        template <typename AstDef>
        AstDef* to_ast_var_def(ast::Type* primitive_type );

        /**
         * wrap type with pointer/array
         * @example  { a, Primitive }.wrap_pointer()
         *      => { a, Pointer { Primitive } }
         */
        void wrap_pointer();
        void wrap_array(ast::Expr* length);

    private:
        TypeManger* type_manager;
        ast::Id* id;
        bool has_init;
        ast::Initializer* init;
    };



    template <typename AstDef>
    AstDef* VarDefManager::to_ast_var_def(ast::Type* primitive_type ) {
        if (has_init) {
            return new AstDef(
                type_manager->to_ast_type(primitive_type),
                id,
                init
            );
        }
        else {
            return new AstDef(
                type_manager->to_ast_type(primitive_type),
                id
            );
        }
    }

}

#endif