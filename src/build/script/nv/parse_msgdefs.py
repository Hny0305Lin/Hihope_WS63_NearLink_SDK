#!/usr/bin/env python3
# coding=utf-8
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.

"""
* Description: NV binary create.
* Create: 2020-3-10
"""
import pycparser
import ctypes

binary_operators = {
    "&": lambda x, y: x & y,
    "|": lambda x, y: x | y,
    "*": lambda x, y: x * y,
    "+": lambda x, y: x + y,
    "-": lambda x, y: x - y,
    "/": lambda x, y: x // y,
    "<<": lambda x, y: x << y,
    ">>": lambda x, y: x >> y,
    "<=": lambda x, y: int(x <= y),
    ">=": lambda x, y: int(x >= y),
    "==": lambda x, y: int(x == y),
    "&&": lambda x, y: int(bool(x and y)),
    "||": lambda x, y: int(bool(x or y)),
}

unary_operators = {
    "-": lambda x: -x,
    "!": lambda x: 1 if x else 0,
    "sizeof": lambda x: ctypes.sizeof(x),
}

fundamental_types = {
    "": ctypes.c_int,
    "_Bool": ctypes.c_bool,
    "char unsigned": ctypes.c_ubyte,
    "char": ctypes.c_byte,
    "double": ctypes.c_double,
    "double long": ctypes.c_longdouble,
    "float": ctypes.c_float,
    "long long unsigned": ctypes.c_ulonglong,
    "long long": ctypes.c_longlong,
    "long unsigned": ctypes.c_ulong,
    "long": ctypes.c_long,
    "short unsigned": ctypes.c_ushort,
    "short": ctypes.c_short,
    "unsigned": ctypes.c_uint,
    "void": None,
}

def get_typename(names):
    specifiers = []
    for name in sorted(names):
        if name in ["signed", "int"]:
            continue
        specifiers.append(name)
    return " ".join(specifiers)

def create_enum_type(enumname, enums):
    # enum size fixed to 4Bytes
    return type(enumname, (ctypes.c_uint,), {"members": enums})

    # enum size depends on max enum value
    smallest = min(enums.values())
    largest = max(enums.values())
    if smallest < 0:
        if -128 <= smallest and largest <= 127:
            base = ctypes.c_byte
        elif -32768 <= smallest and largest <= 32767:
            base = ctypes.c_short
        else:
            base = ctypes.c_int
    else:
        if largest < 256:
            base = ctypes.c_ubyte
        elif largest < 65536:
            base = ctypes.c_ushort
        else:
            base = ctypes.c_uint
    return type(enumname, (base,), {"members": enums})


class Visitor(pycparser.c_ast.NodeVisitor):
    # Parse all typedefs and expand identifiers.
    def __init__(self):
        self.enums = {}
        self.typedefs = dict(fundamental_types)

        # The context in which the ID names exist.
        self.id_context = self.enums
        self.anon_struct_count = 1
        self.anon_enum_count = 1
        self.anon_union_count = 1

    def generic_visit(self, node):
        # Dump ast to see what is not implemented.
        raise SystemExit("Unhandled ast element at %s: %s" % (node.coord, node))

    def visit_Decl(self, node):
        return

    def visit_FuncDef(self, node):
        return

    def visit_FuncDecl(self, node):
        return

    def visit_FileAST(self, node):
        for c in node:
            self.visit(c)

    def visit_ID(self, node):
        try:
            return self.id_context[node.name]
        except KeyError:
            raise SystemExit("Failed to resolve identifier '%s' at %s" % (node.name, node.coord))

    def visit_Typename(self, node):
        return self.visit(node.type)

    def visit_TypeDecl(self, node):
        return self.visit(node.type)

    def visit_CompoundLiteral(self, node):
        return self.visit(node.type)

    def visit_PtrDecl(self, node):
        return ctypes.POINTER(self.visit(node.type))

    def visit_Typedef(self, node):
        if node.name in self.typedefs:
            raise SystemExit("Duplicate typedef '%s' at %s" % (node.name, node.coord))
        value = self.visit(node.type)
        self.typedefs[node.name] = value

    def visit_ArrayRef(self, node):
        # For accessing type of an array element
        array = self.visit(node.name)
        #index = self.visit(node.subscript)
        return array._type_

    def visit_StructRef(self, node):
        # This is needed to get access to types inside a struct.
        struct = self.visit(node.name)
        self.id_context = dict(struct._fields_)
        try:
            return self.visit(node.field)
        finally:
            self.id_context = self.enums

    def visit_BinaryOp(self, node):
        try:
            op = binary_operators[node.op]
        except KeyError:
            raise SystemExit("Unhandled binary operator '%s' at %s" % (node.op, node.coord))
        leftval = self.visit(node.left)
        rightval = self.visit(node.right)
        return op(leftval, rightval)

    def visit_UnaryOp(self, node):
        value = self.visit(node.expr)
        try:
            op = unary_operators[node.op]
        except KeyError:
            raise SystemExit("Unhandled unary operator '%s' at %s" % (node.op, node.coord))
        return op(value)

    def visit_Enum(self, node):
        # Mapping of enum names to enum values from all parsed enums.
        value = -1
        enums = {}
        for enum in node.values:
            if enum.value is None:
                value += 1
            else:
                value = self.visit(enum.value)
            self.enums[enum.name] = enums[enum.name] = value

        if node.name is None:
            enumname = "enum_anon_%d" % self.anon_enum_count
            self.anon_enum_count += 1
        else:
            enumname = "enum_%s" % str(node.name)
        return create_enum_type(enumname, enums)

    def visit_Constant(self, node):
        if node.type not in ["int", "unsigned int"]:
            raise SystemExit("Unhandled Constant node type '%s' at %s" % (node.type, node.coord))
        value = node.value.rstrip("LlUu")
        if value.startswith(("0x", "0X")):
            return int(value, 16)
        return int(value)

    def visit_IdentifierType(self, node):
        name = get_typename(node.names)
        try:
            return self.typedefs[name]
        except KeyError:
            raise SystemExit("Invalid type specifier '%s' at %s" % (name, node.coord))

    def visit_Struct(self, node):
        fields = []
        # node.decls can be None when the struct declaration is outside the typedef.
        if node.decls is not None:
            for decl in node.decls:
                    if decl.bitsize is not None:
                        temp = self.visit(decl.type)
                        value = type(decl.name, (temp,),{"bitsize": decl.bitsize.value})
                    else:
                        value = self.visit(decl.type)
                    if value is None:
                        # This is the void type - indicates an invalid message definition type.
                        return None
                    fields.append((decl.name, value))

        if node.name is None:
            structname = "struct_anon_%d" % self.anon_struct_count
            self.anon_struct_count += 1
        else:
            structname = "struct_%s" % str(node.name)
        return type(structname, (ctypes.Structure,), {"_fields_": fields})

    def visit_Union(self, node):
        fields = []
        for decl in node.decls:
                value = self.visit(decl.type)
                if value is None:
                    return None
                fields.append((decl.name, value))

        if node.name is None:
            unionname = "union_anon_%d" % self.anon_union_count
            self.anon_struct_count += 1
        else:
            unionname = "union_%s" % str(node.name)
        return type(unionname, (ctypes.Union,), {"_fields_": fields})

    def visit_ArrayDecl(self, node):
        length = self.visit(node.dim)
        type = self.visit(node.type)
        if length is None or length < 0:
            raise SystemExit("Invalid array len %s at %s" % (length, node.dim.coord))
        if type is None:
            raise SystemExit("Invalid array type %s at %s" % (type, node.type.coord))
        return type * length

def message_enum_name(elemName):
    if elemName.endswith("_t"):
        return elemName[:-2].upper()
    if elemName.endswith("_s"):
        # NAS messages, already  uppercase
        return elemName[:-2]
    if elemName.endswith("_STRUCT"):
        return elemName[:-7]
    if elemName.find("LOG_MESSAGE") != -1:
        return elemName +'_ID'

def parse_preprocessed_headers(source):
    # Public function that returns the required data for producing messagexml
    try:
        node = pycparser.parse_file(source)
    except pycparser.plyparser.ParseError as e:
        raise SystemExit("ERROR parsing msgdefs %s: %s" % (source, e))
    v = Visitor()
    v.visit(node)

    messages = []
    for structname, fields in sorted(v.typedefs.items()):
        messageEnumName = message_enum_name(structname)
        if messageEnumName is None:
            # Not a valid message definition name.
            continue
        try:
            messageId = v.enums[messageEnumName]
        except KeyError:
            # No associated message id, so not a message definition.
            continue
        if fields is None:
            raise SystemExit("Message definition contains undefined type: %s" % structname)
        messages.append((messageEnumName, structname, messageId, fields))
    return messages
