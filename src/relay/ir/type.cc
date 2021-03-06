/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*!
 * \file src/tvm/ir/type.cc
 * \brief The type system AST nodes of Relay.
 */
#include <tvm/relay/type.h>

namespace tvm {
namespace relay {

using tvm::IRPrinter;
using namespace tvm::runtime;

TensorType TensorTypeNode::make(Array<IndexExpr> shape, DataType dtype) {
  ObjectPtr<TensorTypeNode> n = make_object<TensorTypeNode>();
  n->shape = std::move(shape);
  n->dtype = std::move(dtype);
  return TensorType(n);
}

TensorType TensorTypeNode::Scalar(DataType dtype) {
  return TensorTypeNode::make({}, dtype);
}

IndexExpr TensorTypeNode::Size() const {
  if (shape.size() == 0) {
    return make_const(DataType::Int(64), 1);
  }

  IndexExpr size = shape[0];
  for (size_t i = 1; i < shape.size(); ++i) {
    size *= shape[i];
  }
  return size;
}

TVM_REGISTER_NODE_TYPE(TensorTypeNode);

TVM_REGISTER_API("relay._make.TensorType")
.set_body_typed(TensorTypeNode::make);

TVM_STATIC_IR_FUNCTOR(IRPrinter, vtable)
.set_dispatch<TensorTypeNode>([](const ObjectRef& ref, IRPrinter* p) {
  auto* node = static_cast<const TensorTypeNode*>(ref.get());
  p->stream << "TensorType(" << node->shape << ", " << node->dtype << ")";
});

TypeVar TypeVarNode::make(std::string name, Kind kind) {
  ObjectPtr<TypeVarNode> n = make_object<TypeVarNode>();
  n->var = tvm::Var(name);
  n->kind = std::move(kind);
  return TypeVar(n);
}

TVM_REGISTER_NODE_TYPE(TypeVarNode);

TVM_REGISTER_API("relay._make.TypeVar")
.set_body_typed<TypeVar(std::string, int)>([](std::string name, int kind) {
    return TypeVarNode::make(name, static_cast<Kind>(kind));
    });

TVM_STATIC_IR_FUNCTOR(IRPrinter, vtable)
.set_dispatch<TypeVarNode>([](const ObjectRef& ref, IRPrinter* p) {
    auto* node = static_cast<const TypeVarNode*>(ref.get());
  p->stream << "TypeVarNode(" << node->var->name_hint << ", "
    << node->kind << ")";
});

GlobalTypeVar GlobalTypeVarNode::make(std::string name, Kind kind) {
  ObjectPtr<GlobalTypeVarNode> n = make_object<GlobalTypeVarNode>();
  n->var = tvm::Var(name);
  n->kind = std::move(kind);
  return GlobalTypeVar(n);
}

TVM_REGISTER_NODE_TYPE(GlobalTypeVarNode);

TVM_REGISTER_API("relay._make.GlobalTypeVar")
.set_body_typed<GlobalTypeVar(std::string, int)>([](std::string name, int kind) {
    return GlobalTypeVarNode::make(name, static_cast<Kind>(kind));
    });

TVM_STATIC_IR_FUNCTOR(IRPrinter, vtable)
.set_dispatch<GlobalTypeVarNode>([](const ObjectRef& ref, IRPrinter* p) {
    auto* node = static_cast<const GlobalTypeVarNode*>(ref.get());
  p->stream << "GlobalTypeVarNode(" << node->var->name_hint << ", "
            << node->kind << ")";
});

TypeCall TypeCallNode::make(Type func, tvm::Array<Type> args) {
  ObjectPtr<TypeCallNode> n = make_object<TypeCallNode>();
  n->func = std::move(func);
  n->args = std::move(args);
  return TypeCall(n);
}

TVM_REGISTER_NODE_TYPE(TypeCallNode);

TVM_REGISTER_API("relay._make.TypeCall")
.set_body_typed(TypeCallNode::make);

TVM_STATIC_IR_FUNCTOR(IRPrinter, vtable)
.set_dispatch<TypeCallNode>([](const ObjectRef& ref, IRPrinter* p) {
    auto* node = static_cast<const TypeCallNode*>(ref.get());
  p->stream << "TypeCallNode(" << node->func << ", "
            << node->args << ")";
});

IncompleteType IncompleteTypeNode::make(Kind kind) {
  auto n = make_object<IncompleteTypeNode>();
  n->kind = std::move(kind);
  return IncompleteType(n);
}

TVM_REGISTER_NODE_TYPE(IncompleteTypeNode);

TVM_REGISTER_API("relay._make.IncompleteType")
.set_body_typed<IncompleteType(int)>([](int kind) {
    return IncompleteTypeNode::make(static_cast<Kind>(kind));
  });

TVM_STATIC_IR_FUNCTOR(IRPrinter, vtable)
.set_dispatch<IncompleteTypeNode>([](const ObjectRef& ref, IRPrinter* p) {
    auto* node = static_cast<const IncompleteTypeNode*>(ref.get());
    p->stream << "IncompleteTypeNode(" << node->kind << ", " << node << ")";
  });

FuncType FuncTypeNode::make(tvm::Array<Type> arg_types,
                            Type ret_type,
                            tvm::Array<TypeVar> type_params,
                            tvm::Array<TypeConstraint> type_constraints) {
  ObjectPtr<FuncTypeNode> n = make_object<FuncTypeNode>();
  n->arg_types = std::move(arg_types);
  n->ret_type = std::move(ret_type);
  n->type_params = std::move(type_params);
  n->type_constraints = std::move(type_constraints);
  return FuncType(n);
}

TVM_REGISTER_NODE_TYPE(FuncTypeNode);

TVM_REGISTER_API("relay._make.FuncType")
.set_body_typed(FuncTypeNode::make);

TVM_STATIC_IR_FUNCTOR(IRPrinter, vtable)
.set_dispatch<FuncTypeNode>([](const ObjectRef& ref, IRPrinter* p) {
  auto* node = static_cast<const FuncTypeNode*>(ref.get());
  p->stream << "FuncTypeNode(" << node->type_params << ", "
            << node->arg_types << ", " << node->ret_type << ", "
            << node->type_constraints << ")";
});

TypeRelation TypeRelationNode::make(TypeRelationFn func,
                                    Array<Type> args,
                                    int num_inputs,
                                    Attrs attrs) {
  ObjectPtr<TypeRelationNode> n = make_object<TypeRelationNode>();
  n->func = std::move(func);
  n->args = std::move(args);
  n->num_inputs = num_inputs;
  n->attrs = std::move(attrs);
  return TypeRelation(n);
}

TVM_REGISTER_NODE_TYPE(TypeRelationNode);

TVM_REGISTER_API("relay._make.TypeRelation")
.set_body_typed(TypeRelationNode::make);

TVM_STATIC_IR_FUNCTOR(IRPrinter, vtable)
.set_dispatch<TypeRelationNode>([](const ObjectRef& ref, IRPrinter* p) {
    auto* node = static_cast<const TypeRelationNode*>(ref.get());
    p->stream << "TypeRelationNode("
              << node->func->name
              << ", " << node->args << ")";
});

TupleType TupleTypeNode::make(Array<Type> fields) {
  ObjectPtr<TupleTypeNode> n = make_object<TupleTypeNode>();
  n->fields = std::move(fields);
  return TupleType(n);
}

TVM_REGISTER_NODE_TYPE(TupleTypeNode);

TVM_REGISTER_API("relay._make.TupleType")
.set_body_typed(TupleTypeNode::make);

TVM_STATIC_IR_FUNCTOR(IRPrinter, vtable)
.set_dispatch<TupleTypeNode>([](const ObjectRef& ref, IRPrinter* p) {
  auto* node = static_cast<const TupleTypeNode*>(ref.get());
  p->stream << "TupleTypeNode(" << node->fields << ")";
});

RefType RefTypeNode::make(Type value) {
  ObjectPtr<RefTypeNode> n = make_object<RefTypeNode>();
  n->value = std::move(value);
  return RefType(n);
}

TVM_REGISTER_API("relay._make.RefType")
.set_body_typed(RefTypeNode::make);

TVM_REGISTER_NODE_TYPE(RefTypeNode);

TVM_STATIC_IR_FUNCTOR(IRPrinter, vtable)
.set_dispatch<RefTypeNode>([](const ObjectRef& ref, IRPrinter* p) {
  auto* node = static_cast<const RefTypeNode*>(ref.get());
  p->stream << "RefTypeNode(" << node->value << ")";
});

TVM_REGISTER_API("relay._make.Any")
.set_body_typed<IndexExpr()>([]() { return Any::make(); });


}  // namespace relay
}  // namespace tvm
