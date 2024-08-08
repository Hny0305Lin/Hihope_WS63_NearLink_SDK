#!/usr/bin/env python3
# coding=utf-8

#===============================================================================
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
# Description: Parse C headers and extract message definitions as ctypes objects.
#===============================================================================
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
    #
    # 32bit arm and riscv compilers define "long double" type as 8 and 16 byte long respectively.
    # ctypes.c_longdouble under 32bit windows, 32bit linux, 64bit windows, 64bit linux OS has length 8, 12, 8, 16 respectively.
    # We cannot find a ctype mapping to generalize.
    # Especially that 16 byte riscv "long double" type cannot be interpreted by any 32/64bit windows ctype.
    # Fortunately we don't have any long doubles in messages and etypes. So disable this mapping for now.
    #
    # "double long": ctypes.c_longdouble,
    "float": ctypes.c_float,
    "long long unsigned": ctypes.c_ulonglong,
    "long long": ctypes.c_longlong,
    #
    # 32bit arm and riscv compilers define "long" type as 4 byte long.
    # Messages xml generation calls this script with 32bit python while kv generation uses 64bit python.
    # We need to map "long" type to ctypes.c_int which is 4 byte across 32/64bit OSs
    # instead of ctypes.c_long which is 8 byte on 64bit linux and 4 byte otherwise.
    # This also applies to "unsigned long" type.
    #
    "long unsigned": ctypes.c_uint,
    "long": ctypes.c_int,
    "short unsigned": ctypes.c_ushort,
    "short": ctypes.c_short,
    "unsigned": ctypes.c_uint,
    "void": None,
}

# These types will have a typedef but map to alternative ctypes values.
override_typedefs = {
    "osal_char": ctypes.c_char,
}

def get_typename(names):
    specifiers = []
    for name in sorted(names):
        if name in ["signed", "int"]:
            continue
        specifiers.append(name)
    return " ".join(specifiers)

def create_enum_type(enumname, enums):
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
        if node.name in override_typedefs:
            value = override_typedefs[node.name]
        else:
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

    def _handle_fields(self, decls):
        fields = []
        # node.decls can be None when the struct declaration is outside the typedef.
        if decls is not None:
            field_count = 1
            for decl in decls:
                    value = self.visit(decl.type)
                    if value is None:
                        # This is the void type - indicates an invalid message definition type.
                        return None
                    if decl.name is None:
                        fieldname = "unnamed_%d" % field_count
                        field_count += 1
                    else:
                        fieldname = decl.name
                    fields.append((fieldname, value))
        return fields

    def visit_Struct(self, node):
        fields =  self._handle_fields(node.decls)
        if fields is None:
            return None

        if node.name is None:
            structname = "struct_anon_%d" % self.anon_struct_count
            self.anon_struct_count += 1
        else:
            structname = "struct_%s" % str(node.name)
        return type(structname, (ctypes.Structure,), {"_fields_": fields})

    def visit_Union(self, node):
        fields =  self._handle_fields(node.decls)
        if fields is None:
            return None

        if node.name is None:
            unionname = "union_anon_%d" % self.anon_union_count
            self.anon_union_count += 1
        else:
            unionname = "union_%s" % str(node.name)
        return type(unionname, (ctypes.Union,), {"_fields_": fields})

    def visit_ArrayDecl(self, node):
        type = self.visit(node.type)
        if node.dim is None:
            # Flexible array members do not have a size.
            # Return a pointer for compatibility with old ctypesgen based implementation.
            return ctypes.POINTER(type)
        length = self.visit(node.dim)
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
    if elemName.upper().find("LOG_MESSAGE") != -1:
        return elemName.upper() +'_ID'

def parse_preprocessed_headers(source, core):
    # Public function that returns the required data for producing messagexml
    try:
        node = pycparser.parse_file(source)
    except pycparser.plyparser.ParseError as e:
        raise SystemExit("ERROR parsing msgdefs %s: %s" % (source, e))
    v = Visitor()
    if core == 'acore':
        v.anon_struct_count = 0x8001
        v.anon_enum_count = 0x8001
        v.anon_union_count = 0x8001
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
