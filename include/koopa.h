#ifndef KOOPA_H_
#define KOOPA_H_

#include "ast.h"

#include <iostream>
#include <assert.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>

namespace riscv_trans {
    class Info {
    public:
        int stack_frame_size = 0;

        std::string res_lit = "";

        bool is_reg_used[7] = { 0 };
        std::string get_unused_reg();

        void refresh_reg(std::string lit);
    };
}

namespace koopa {

class Base {
public:
    Base *pa = nullptr;

    virtual std::string to_string() const = 0;

    virtual void to_riscv(std::string &str, riscv_trans::Info &info) const = 0;

    virtual ~Base() = default;
};

class Type : public Base {
public:
    void to_riscv(std::string &str, riscv_trans::Info &info) const override {}
};

    class Int : public Type {

        std::string to_string() const override;

    };

    class Array : public Type {
    public:
        Type *elem_type = nullptr;
        int length = 0;

        std::string to_string() const override;

        Array(Type *elem_type, int length) : elem_type(elem_type), length(length) {
            elem_type->pa = this;
        }

        ~Array() override;
    };

    class Pointer : public Type {
    public:
        Type *pointed_type = nullptr;

        std::string to_string() const override;

        Pointer(Type *pointed_type) : pointed_type(pointed_type) {
            pointed_type->pa = this;
        }

        ~Pointer() override;
    };

    class FuncType : public Type {
    public:
        std::vector<Type *>     arg_types   = {};
        Type                    *ret_type   = nullptr;

        std::string to_string() const override;

        FuncType(std::vector<Type *> arg_types, Type *ret_type) :
            arg_types(arg_types), ret_type(ret_type) {

            for (auto arg_type : arg_types) arg_type->pa = this;
            ret_type->pa = this;

        }

        ~FuncType() override;
    };

    class Label : public Type {

        std::string to_string() const override;

    };

    class Void : public Type {

        std::string to_string() const override;

    };

class Value : public Base {
public:
    bool is_const = false;
    int val = 0;
};

    class Id : public Value {
    public:
        Type        *type   = nullptr;
        std::string *lit    = nullptr;

        int sf_offset = 0;

        std::string to_string() const override;

        void to_riscv(std::string &str, riscv_trans::Info &info) const override;

        Id(Type *type, std::string *lit, bool is_const = false, int val = 0) 
            : type(type), lit(lit) {
                
            this->val = val;
            this->is_const = is_const;

            type->pa = this;
        }

        ~Id() override;
    };

    class Const : public Value {
    public:
        std::string to_string() const override;

        void to_riscv(std::string &str, riscv_trans::Info &info) const override;

        Const(int val)  {
            this->val = val;
            this->is_const = true;
        }
    };

    class Undef : public Value {

        std::string to_string() const override;

    };

class Initializer : public Base {
};

    class ConstInitializer : public Initializer {
    public:
        int val = 0;

        std::string to_string() const override;

        ConstInitializer(int val) : val(val) {}
    };

    class Aggregate : public Initializer {
    public:
        std::vector<Initializer *> initializers = {};

        std::string to_string() const override;

        Aggregate(std::vector<Initializer *> initializers) : initializers(initializers) {

            for (auto initializer : initializers) initializer->pa = this;

        }

        ~Aggregate() override;
    };

    class Zeroinit : public Initializer {

        std::string to_string() const override;

    };

    class UndefInitializer : public Initializer {

        std::string to_string() const override;

    };

class Stmt : public Base {
public:
    bool is_unit = true;
};
    class NotEndStmt : public Stmt {
    };

        class Rvalue : public Base {
        };

            class MemoryDecl : public Rvalue {
            public:
                Type *type = nullptr;

                MemoryDecl(Type *type) : type(type) {

                    type->pa = this;

                }

                std::string to_string() const override;

                void to_riscv(std::string &str, riscv_trans::Info &info) const override;

                ~MemoryDecl() override;
            };

            class Load : public Rvalue {
            public:
                Id *addr = nullptr;

                Load(Id *addr) : addr(addr) {

                    addr->pa = this;

                }

                std::string to_string() const override;

                void to_riscv(std::string &str, riscv_trans::Info &info) const override;

                ~Load() override;
            };

            class GetPtr : public Rvalue {
            public:
                Id      *base   = nullptr;
                Value   *offset = nullptr;

                std::string to_string() const override;

                GetPtr(Id *base, Value *offset) : base(base), offset(offset) {

                    base->pa = this;
                    offset->pa = this;

                }

                ~GetPtr() override;
            };

            class GetElemPtr : public Rvalue {
            public:
                Id      *base   = nullptr;
                Value   *offset = nullptr;

                std::string to_string() const override;

                GetElemPtr(Id *base, Value *offset) : base(base), offset(offset) {

                    base->pa = this;
                    offset->pa = this;

                }

                ~GetElemPtr() override;
            };

            namespace op {
                enum Op {
                    NE, EQ, GT, LT, GE, LE, ADD, SUB, MUL,
                    DIV, MOD, AND, OR, XOR, SHL, SHR, SAR,
                };  

                extern std::function<int(int, int)> op_func[17];
            }

            class Expr : public Rvalue {
            public:
                op::Op op;
                Value *lv, *rv = nullptr;

                bool is_const = false;
                int val = 0;

                std::string to_string() const override;

                void to_riscv(std::string &str, riscv_trans::Info &info) const override;

                Expr(op::Op op, Value *lv, Value *rv, bool is_const = false) 
                    : op(op), lv(lv), rv(rv), is_const(is_const) {

                    lv->pa = this;
                    rv->pa = this;

                }

                ~Expr() override;
            };

                class ExprStmt : public NotEndStmt {
                public:
                    Expr *expr = nullptr;

                    ExprStmt(Expr *expr) : expr(expr) {}

                    std::string to_string() const override;

                    void to_riscv(std::string &str, riscv_trans::Info &info) const override;

                    ~ExprStmt() override;
                };

            class FuncCall : public Rvalue {
            public:
                Id                      *id     = nullptr;
                std::vector<Value *>    args    = {};

                std::string to_string() const override;

                FuncCall(Id *id, std::vector<Value *> args) : id(id), args(args) {

                    auto base_this = this;

                    id->pa = base_this;
                    for (auto arg : args) arg->pa = base_this;

                }

                ~FuncCall() override;
            };

                class FuncCallStmt : public NotEndStmt {
                public:
                    FuncCall *func_call = nullptr;

                    // TODO
                };

        class SymbolDef : public NotEndStmt {
        public:
            Id      *id     = nullptr;
            Rvalue  *val    = nullptr;

            std::string to_string() const override;

            void to_riscv(std::string &str, riscv_trans::Info &info) const override;

            SymbolDef(Id *id, Rvalue *val) : id(id), val(val) {

                id->pa = this;
                val->pa = this;

                is_unit = false;

            }

            ~SymbolDef() override;
        };

        class Store : public NotEndStmt {
        };

            class StoreValue : public Store {
            public:
                Value   *value  = nullptr;
                Id      *addr   = nullptr;

                StoreValue(Value *value, Id *addr) : value(value), addr(addr) {

                    value->pa = this;
                    addr->pa = this;

                }

                std::string to_string() const override;

                void to_riscv(std::string &str, riscv_trans::Info &info) const override;

                ~StoreValue() override;
            };

            class StoreInitializer : public Store {
            public:
                Initializer *initializer    = nullptr;
                Id          *addr           = nullptr;

                std::string to_string() const override;

                StoreInitializer(Initializer *initializer, Id *addr) :
                    initializer(initializer), addr(addr) {

                    initializer->pa = this;
                    addr->pa = this;

                }

                ~StoreInitializer() override;
            };

    class EndStmt : public Stmt {
    };

        class Branch : public EndStmt {
        public:
            Value   *cond       = nullptr;
            Id      *target1    = nullptr;
            Id      *target2    = nullptr;

            std::string to_string() const override;

            Branch(Value *cond, Id *target1, Id *target2) : 
                cond(cond), target1(target1), target2(target2) {

                cond->pa = this;
                target1->pa = this;
                target2->pa = this;

            }

            ~Branch() override;
        };

        class Jump : public EndStmt {
        public:
            Id *target = nullptr;

            std::string to_string() const override;

            Jump(Id *target) : target(target) {

                target->pa = this;

            }

            ~Jump() override;
        };

        class Return : public EndStmt {
        public:
            bool has_return_val = true;
            Value *val = nullptr;

            std::string to_string() const override;

            void to_riscv(std::string &str, riscv_trans::Info &info) const override;

            Return() : has_return_val(false) {}
            Return(Value *val) : val(val) {

                val->pa = this;

            }

            ~Return() override;
        };

    class GlobalStmt : public Stmt {
    };

        class Block : public Base {
        public:
            Id                  *id         = nullptr;
            std::vector<Stmt *> stmts       = {};

            std::vector<std::string> preds;
            std::vector<std::string> succs;

            int get_stack_frame_size();

            void set_id_offset(int &offset);

            friend void operator+=(Block &self, Stmt *stmt) {
                self.stmts.push_back(stmt);
            }

            friend void operator+=(Block &self, std::vector<Stmt *> stmts) {
                self.stmts.reserve(self.stmts.size() + stmts.size());
                self.stmts.insert(self.stmts.end(), stmts.begin(), stmts.end());
            }

            std::string to_string() const override;

            void to_riscv(std::string &str, riscv_trans::Info &info) const override;

            Block(Id *id, std::vector<Stmt *> stmts) :
                id(id), stmts(stmts) {

                id->pa = this;
                for (auto stmt : stmts) stmt->pa = this;

            }

            ~Block() override;
        };

        class FuncParamDecl : public Base {
        public:
            Id      *id     = nullptr;
            Type    *type   = nullptr;

            std::string to_string() const override;

            FuncParamDecl(Id *id, Type *type) :
                id(id), type(type) {

                id->pa = this;
                type->pa = this;

            }

            ~FuncParamDecl() override;
        };

        class FuncDef : public GlobalStmt {
        public:
            Id                              *id                 = nullptr;
            std::vector<FuncParamDecl *>    func_param_decls    = {};
            Type                            *ret_type           = nullptr;
            std::vector<Block *>            blocks              = {};

            std::string to_string() const override;

            void to_riscv(std::string &str, riscv_trans::Info &info) const override;

            FuncDef(Id *id, std::vector<FuncParamDecl *> func_param_decls,
                    Type *ret_type, std::vector<Block *> blocks) :
                id(id), func_param_decls(func_param_decls), ret_type(ret_type), blocks(blocks) {

                id->pa = this;
                for (auto func_param_decl : func_param_decls) func_param_decl->pa = this;
                ret_type->pa = this;
                for (auto block : blocks) block->pa = this;

            }

            ~FuncDef() override;
        };

        class FuncDecl : public GlobalStmt {
        public:
            Id                  *id             = nullptr;
            std::vector<Type *> param_types     = {};
            Type                *ret_type       = nullptr;

            std::string to_string() const override;

            FuncDecl(Id *id, std::vector<Type *> param_types, Type *ret_type) :
                id(id), param_types(param_types), ret_type(ret_type) {

                id->pa = this;
                for (auto param_type : param_types) param_type->pa = this;
                ret_type->pa = this;

            }

            ~FuncDecl() override;
        };

        class GlobalMemoryDecl : public Base {
        public:
            Type        *type           = nullptr;
            Initializer *initializer    = nullptr;

            std::string to_string() const override;

            GlobalMemoryDecl(Type *type, Initializer *initializer) :
                type(type), initializer(initializer) {

                type->pa = this;
                initializer->pa = this;

            }

            ~GlobalMemoryDecl() override;
        };

        class GlobalSymbolDef : public GlobalStmt {
        public:
            Id                  *id     = nullptr;
            GlobalMemoryDecl    *decl   = nullptr;

            std::string to_string() const override;

            GlobalSymbolDef(Id *id, GlobalMemoryDecl *decl) :
                id(id), decl(decl) {

                id->pa = this;
                decl->pa = this;

            }

            ~GlobalSymbolDef() override;
        };

class Program : public Base {
public:
    std::vector<GlobalStmt*> global_stmts;

    std::string to_string() const override;

    void to_riscv(std::string &str, riscv_trans::Info &info) const override;

    Program(std::vector<GlobalStmt *> global_stmts) : 
        global_stmts(global_stmts) {

        for (auto global_stmt : global_stmts) global_stmt->pa = this;

    }

    ~Program() override;
};

class ValueSaver {
public:
    std::unordered_map<std::string, Id *> ids = {};
    void insert_id(Id *new_id) { 
        ids.insert(
            std::pair<std::string, Id *>(*new_id->lit, new_id)
        ); 
    }

    static std::string *build_name(std::string *lit, ast::NestingInfo *nesting_info) {
        if (!nesting_info->need_suffix) return lit;
        
        auto res = new std::string(*lit 
            + '_' + std::to_string(nesting_info->nesting_level) 
            + '_' + std::to_string(nesting_info->nesting_count));
        return res;
    }

    Id *new_id(Type *type, std::string *lit, ast::NestingInfo *nesting_info, bool is_const = false, int val = 0) {

        auto res = new Id(type, build_name(lit, nesting_info), is_const, val);
        insert_id(res);
        return res;

    }

    bool is_id_declared(std::string lit, ast::NestingInfo *nesting_info) {
        return ids.find(*build_name(&lit, nesting_info)) != ids.end();
    }

    /* return nullptr if id is not defined */
    Id *get_id(std::string lit, ast::NestingInfo *nesting_info) {
        if (nesting_info == nullptr) return nullptr;

        auto res = ids.find(*build_name(&lit, nesting_info));

        if (res == ids.end()) {
            return get_id(lit, nesting_info->pa);
        }
        else {
            return res->second;
        }
    }

    std::unordered_set<Const *> consts = {};
    void insert_const(Const *new_const) {
        consts.insert(new_const);
    }

    Const *new_const(int val) {

        auto res = new Const(val);
        insert_const(res);
        return res;

    }

    Undef *undef = nullptr;
    ValueSaver() {
        //TODO
        // undef = new Undef;
    }

    Undef *new_undef() {
        return undef;
    }

    ~ValueSaver() { 
        // TODO in koopa::Base *ast::Id::to_koopa(), refree of pointed type
        // for (auto id_pair : ids) delete id_pair.second;
        // for (auto const_val : consts) delete const_val;
        // if (undef != nullptr) delete undef;
    }
};

}



#endif