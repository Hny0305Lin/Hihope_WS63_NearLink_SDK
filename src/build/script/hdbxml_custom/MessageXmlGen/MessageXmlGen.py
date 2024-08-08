#!/usr/bin/env python3
# coding=utf-8

#===============================================================================
# @brief    XML tools
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2022. All rights reserved.
#===============================================================================
import ctypes
import sys
import inspect
import os
import re
from  xml.etree import ElementTree
from xml.dom import minidom

sys.path.append(os.path.join("build", "script", "hdbxml_custom", "parse"))
import parse_msgdefs


def isStackmessage(id, core):
    if (core in ['protocol_core']) and ((id >> 26) != 0x10):
        return True
    return False

def swap16(x):
    return (((x <<  8) & 0xFF00) |
            ((x >>  8) & 0x00FF))

def swap32(x):
    return (((x << 24) & 0xFF000000) |
            ((x <<  8) & 0x00FF0000) |
            ((x >>  8) & 0x0000FF00) |
            ((x >> 24) & 0x000000FF))

def getIdFromParts(core, msg_class, msg_module, msg_id):
    return (((core << 30) & 0xC0000000) | ((msg_class << 29) & 0x20000000) | ((msg_module << 16) & 0x00FF0000) | (swap16(msg_id) & 0x0000FFFF))

def getIdForMessage(id, core):
    if isStackmessage(id, core):
        # A stack message needs to be dissasembled and put back together
        msg_module =    (((id) >> 24) & 0xFF)
        msg_log =       (((id) >> 21) & 0x1)
        msg_verbosity = (((id) >> 22) & 0x3)
        msg_id =        (((id) & 0x3ff) |  (msg_log << 13) | (msg_verbosity << 14))
        messageIDOut = getIdFromParts(1, 1, msg_module, msg_id)
    else:
        # All other messages need their low two bytes swapped
        messageIDOut = ((id) & 0xFFFF0000) | (((id) & 0x0000FF00) >> 8) | (((id) & 0x000000FF) << 8)

    messageIDOut = swap32(messageIDOut)

    return messageIDOut

def getFirmwareVersion():
    versionfile = os.path.join("build_scons", "VERSION_STRING")
    if os.path.exists(versionfile):
        with open(versionfile) as f:
            firmware_version = f.read().strip()
    else:
        # This can happen when the messagexml generation is used outside the build.
        # The DSP build uses this via tools/scripts/messagesxml.bat
        firmware_version = "UNKNOWN"
    return firmware_version

# just used to include comments from rules files.
class CommentedTreeBuilder(ElementTree.TreeBuilder):
    def __init__(self, *args, **kwargs):
        super(CommentedTreeBuilder, self).__init__(*args, **kwargs)

    def comment(self, data):
        self.start(ElementTree.Comment, {})
        self.data(data)
        self.end(ElementTree.Comment)

class messageTree(object):
    def __init__(self, distribution, firmware_version, coreName, imageName):
        self.parser = ElementTree.XMLParser(target=CommentedTreeBuilder())
        ElementTree.register_namespace("","http://tempuri.org/xmlDefinition.xsd" )
        debugInformationAttributes = { \
                                          'Distribution': distribution, \
                                          'FirmwareVersion':firmware_version, \
                                          'MulticoreLoggingSupported':"true", \
                                          'SocXMLVersion':"2", \
                                          'SequenceNumbersSupported':"true", \
                                          'xmlns':"http://tempuri.org/xmlDefinition.xsd", \
                                          }
        if imageName in ['SEBoot', 'SERecovery', 'updater']:
            isBootImage = 'true'
        else:
            isBootImage = 'false'
        coreAttributes = { \
                         'CoreName': str(coreName), \
                         'ImageName': str(imageName), \
                         'BootImage': isBootImage, \
                         }

        self.top = ElementTree.Element('DebugInformation', debugInformationAttributes)
        self.tree = ElementTree.ElementTree(element=self.top, file=None)
        loghdr = ElementTree.SubElement(self.top, 'CommonLogHeader', {'LogIndexMask': '0x0000001F'})
        coreEntry = ElementTree.SubElement(self.top, 'Core', coreAttributes)

        self.messages = ElementTree.SubElement(coreEntry, 'Messages')
        self.structs_dict = ElementTree.SubElement(coreEntry, 'StructureDictionary')
        self.enums_dict = ElementTree.SubElement(coreEntry, 'EnumsDictionary')

    def addMessageHeader(self, messageEnumName, messageId, core, elemName):
        messageAttributes = { \
                                      'Name': messageEnumName, \
                                      'MessageID': "0x"+str(hex(messageId))[2:].zfill(8), \
                                      'Type': 'structure', \
                                      }
        if isStackmessage(messageId, core):
             messageAttributes['MessageType'] = 'StackMessage'
        message = ElementTree.SubElement(self.messages, "Message",messageAttributes)
        return message

    def addStructure(self, nodeTypeName, fieldElementType, elementSize):
        structureAttributes = { \
                                  'Type': nodeTypeName, \
                                  'FieldType': fieldElementType, \
                                  }
        fieldEntry = ElementTree.SubElement(self.structs_dict, 'Structure', structureAttributes)
        return fieldEntry

    def structureInDictionary(self, structName):
        found = False
        for i in self.structs_dict:
            nameEntry = i.attrib["Type"]
            if structName == nameEntry:
                found = True
        return found

    def addField(self, parent, fieldName, nodeTypeName, fieldElementType, elementSize, length, enumName):
        fieldAttributes = { \
                                          'FieldName': fieldName, \
                                          'Type': nodeTypeName, \
                                          'FieldType': fieldElementType, \
                                          'Size': str(elementSize), \
                                          'Length': str(length), \
                                          }
        if enumName is not None:
            fieldAttributes['Enum'] = enumName
        fieldSubEntry = ElementTree.SubElement(parent, 'Field', fieldAttributes)
        return fieldSubEntry

    def addEnum(self, enum_type_name, enumList):
        found = False
        for i in self.enums_dict:
            nameEntry = i.find("Name").text
            if enum_type_name == nameEntry:
                found = True

        if not found:
            enumsEntry = ElementTree.SubElement(self.enums_dict, 'EnumsEntry')
            enumName = ElementTree.SubElement(enumsEntry, 'Name')
            enumName.text = enum_type_name
            #enumsArray = ElementTree.SubElement(enumsEntry, 'Enums')
            for name, val in enumList:
                self.addEnumEntry(enumsEntry, name, val)

    def addEnumEntry(self, parent, name, val):
        enumAttributes = { \
                                      'Name': name, \
                                      'Value': str(val), \
                                      }
        enumValueEntry = ElementTree.SubElement(parent, 'Enum', enumAttributes)

    def dump(self):
        return ElementTree.dump(self.top)

    def prettify(self, elem, encoding = 'utf-8'):
        """Return a pretty-printed XML string for the Element.
        """
        initial_string = ElementTree.tostring(elem, encoding=encoding, method='xml')
        reparsed_string = minidom.parseString(initial_string)
        return reparsed_string.toprettyxml(indent="  ")

    def write_xml_file (self, file, xml_root_element, xml_declaration=False, pretty_print=False, encoding='unicode', indent='\t'):
        if xml_declaration:
            file.write('<?xml version="1.0" encoding="utf-8" ?>')
        pretty_printed_xml = self.prettify(xml_root_element, encoding = 'utf-8')
        file.write(pretty_printed_xml)

    def outputMessageTree(self, regexp, rulesfilename, outfilename):
        if regexp:
            matcher = re.compile(regexp)

        # Filter out any messages we don't want to publish, then add the rules and write it out
        internal_message_list = []
        for message in self.messages:
            name = message.attrib['Name']
            internal = regexp and matcher.search(name) is not None
            if internal:
                internal_message_list.append(message)

        for message in internal_message_list:
            name = message.attrib['Name']
            self.messages.remove(message)

        if rulesfilename != None:
            rules_data = ElementTree.parse(rulesfilename, self.parser)
            if rules_data is None:
                raise SystemExit('Error: failed to parse %s' % rulesfilename)
            rules_root = rules_data.getroot()
            if rules_root is None:
                raise SystemExit('Error: failed to find root section in %s' % rulesfilename)
            rulesEntry = self.top.find('Core')
            if rulesEntry is None:
                raise SystemExit('Error: failed to find "Core" section')
            rulesEntry.extend(rules_root)

        with open(outfilename, 'w') as outfile:
            self.write_xml_file(outfile, self.top, xml_declaration=False, pretty_print=True, encoding='unicode', indent='\t')


# Taking some ctypes element, recursively analyse it, and build a tree of nodes representing it
def buildTree(xmlTree, message_fields, elem):
    if hasattr(elem, "_fields_"):
        for field in elem._fields_:
            structAlreadyPopulated = False
            current_message_fields = message_fields
            fieldType = field[1]
            arrayItemNamePostfix=''

            # This is potentiallay a multi-dimensional array that must be descended into so do so until an actual type is encountered
            while True:
                # If this is an array there will be some kind of length
                length = fieldType._length_ if hasattr(fieldType, "_length_") else 1
                # This size includes all the different elements (if this is an array) or just the size of the single field if not
                size = ctypes.sizeof(fieldType)
                # Deduce the size of a single element
                elementSize = size // length
                # Pointers have a "contents" attribute
                isPointer = hasattr(fieldType, "contents")

                # Simple single dimension arrays can be handled using the length
                isSomeKindOfArray = issubclass(fieldType, ctypes.Array)
                if isSomeKindOfArray:
                    # This is some kind of array so get the element within the array - the fieldType from now onwards is within the array
                    fieldType = fieldType._type_

                # Base types have no fields in their "type" (note - arrays of base types are considered to be base types)
                isBaseType = not hasattr(fieldType, "_fields_")
                isUnion = isinstance(fieldType, ctypes.Union)
                isEnum = hasattr(fieldType, "members")

                if isEnum:
                    for class_ in inspect.getmro(fieldType):
                        if issubclass(class_, ctypes._SimpleCData) and class_ is not fieldType:
                            fullTypeName = class_
                            break
                    else:
                        raise TypeError("Can't find a ctypes class to use for enum %s" % fieldType)
                else:
                    fullTypeName = fieldType

                # Extract a useful type name from the verbose name in CTypes
                (junk, separator, fullTypeName) = str(fullTypeName).partition(".")
                (fullTypeName, separator, junk) = fullTypeName.partition("'")

                # Check to see if there are further array dimensions that need to be unwrapped and if not exit to actually
                if not issubclass(fieldType, ctypes.Array):
                    break

                # This is a multi-dimensional array so use a fake struct here to hold the next dimensionality
                arrayItemNamePostfix='_item'
                nodeTypeName = ''.join(["struct_", fullTypeName])
                fieldEntry = xmlTree.addField(current_message_fields, field[0], nodeTypeName, 'struct', elementSize, length, enum_type_name)

                # If the fake structure has already been populated then stop here
                if xmlTree.structureInDictionary(nodeTypeName):
                    structAlreadyPopulated = True
                    break

                # Add the fake struct definition and start populating it during the next iteration
                current_message_fields = xmlTree.addStructure(nodeTypeName, 'struct', elementSize)

            # Only need to do anything if a struct has not already been populated
            if not structAlreadyPopulated:
                # Now dealing with something that is not an array
                (friendlyTypeName, separator, junk) = fullTypeName.partition("_Array_")
                nodeTypeName = friendlyTypeName
                if 'struct' in nodeTypeName and not isPointer:
                    fieldElementType = 'struct'
                elif 'union' in nodeTypeName:
                    fieldElementType = 'union'
                else:
                    fieldElementType = 'base'

                if isEnum:
                    enum_type_name = inspect.getmro(fieldType)[0].__name__
                    xmlTree.addEnum(enum_type_name, sorted(fieldType.members.items()))
                else:
                    enum_type_name = None

                # write the message entry in all cases.
                fieldEntry = xmlTree.addField(current_message_fields, field[0] + arrayItemNamePostfix, nodeTypeName, fieldElementType, elementSize, length, enum_type_name)

                # if it's a structure add to structure dict etc, if not it's a base type and go to the next one
                if fieldElementType == 'struct':
                    if not xmlTree.structureInDictionary(nodeTypeName):
                        fields_SubEntry = xmlTree.addStructure(nodeTypeName, fieldElementType, elementSize)
                        buildTree(xmlTree, fields_SubEntry, fieldType)
                else:
                    if not isBaseType and not isPointer:
                        buildTree(xmlTree, fieldEntry, fieldType)


def xml_gen_main(outputBasepath, source, cfgfilename, core, image, rulesfilename):
    messages = parse_msgdefs.parse_preprocessed_headers(source, core)
    # Load the configuration file
    with open(cfgfilename) as cfgfile:
        if cfgfile == None:
            raise SystemExit("Error: Could not find configuration file %s." % cfgfilename)

        # Load the core
        if core not in ['acore', 'protocol_core', 'security_core']:
            raise SystemExit("Error: Invalid core %s." % core)

        if rulesfilename is not None:
            with open(rulesfilename) as rulesfile:
                if rulesfile == None:
                    raise SystemExit("Error: Could not find rules file %s." %rulesfilename)

        # List of filenames and regular expressions
        if not os.path.isdir(outputBasepath):
            # If given an output filename rather than base directory,
            # strip off the filename and keep the path
            outputBasepath = os.path.dirname(outputBasepath)

        outputs = []

        # Parse the pairs of filenames and regular expressions
        for line in cfgfile.readlines():
            if not line.startswith("#"):   # Exclude comments
                if line.strip() != "": # Exclude empty lines
                    # Format is: filename , regexp
                    distribution, filename, regexp = line.split(",", 2)
                    # Remove regexp whitespace
                    regexp = regexp.strip()
                    # Remove whitespace and add base path
                    filename = filename.strip()
                    filename = os.path.join(outputBasepath, filename)
                    outputs.append((distribution, filename, regexp))

    firmware_version = getFirmwareVersion()

    for distribution, outfilename, regexp in outputs:
        #with messageTree(distribution, firmware_version, core) as xmlTree:
        xmlTree = None
        xmlTree = messageTree(distribution, firmware_version, core, image)

        # And a list of message IDs already used
        # (These should ideally be stored in a tree for scalable search speed, but the number of messages is small so the wasted time isn't a problem)
        messageIdList = []

        for messageEnumName, structname, messageId, struct in messages:
            # If not a stack xml then convert the IDs to the log IDs
            #
            # if distribution != "stack":
            #     messageId = getIdForMessage(messageId, core)
            fieldsEntry = xmlTree.addMessageHeader(messageEnumName, messageId, core, structname)
            buildTree(xmlTree, fieldsEntry, struct)

        xmlTree.outputMessageTree(regexp, rulesfilename, outfilename)
        del xmlTree


if len(sys.argv) < 6:
    raise SystemExit("Usage: python MessageXmlGen.py <ctypes_library> <output_dir> <configuration_file> <corename> <imagename> [<rules_file>]")

source = sys.argv[1]
outputBasepath = sys.argv[2]
cfgfilename = sys.argv[3]
corename = sys.argv[4]
imagename = sys.argv[5]

# Optional Rules File
if len(sys.argv) == 7:
    rulesfilename = sys.argv[6]
else:
    rulesfilename = None

# 删除.i文件中的#pragma预编译指令
with open(source, "rb+") as f:
    write_lines = []
    for line in f.readlines():
        if not re.match("#pragma", line.decode("utf-8", errors="replace").strip()):
            write_lines.append(line)
    f.seek(0, 0)
    f.truncate()

    for line in write_lines:
        f.write(line)

xml_gen_main(outputBasepath, source, cfgfilename, corename, imagename, rulesfilename)
