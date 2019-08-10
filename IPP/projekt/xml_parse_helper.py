
import xml.etree.ElementTree as ET
import sys
import os
from instruction import *


def is_all_uniq(lst):
    """
    Funkce zjistil zda jsou vsechny prvky v seznamu unikatni
    :param lst: List
    :return: True / False
    """
    return len(set(lst)) == len(lst)


def file_exists(path):
    """
    Zjisti existenci souboru
    :param path: Cesta k souboru
    :return: True / False
    """
    exists = os.path.isfile(path)
    if exists:
        return True
    else:
        return False

class XMLParseHelper:
    """
    Parsuje XML a prevadi instrukce ze XML na vnitrni reprezentaci
    :param arg_helper: Objekt s ulozenymi vstupnimi parametry
    """

    def __init__(self, arg_helper):
        self.arg_helper = arg_helper

    def load_xml(self):
        """
        Funkce zparsuje XML a prevede na vnitrni reprezentaci
        :return: The list of Instructions
        """
        try:
            if file_exists(self.arg_helper.source):
                tree = ET.parse(self.arg_helper.source)
            else:
                sys.stderr.write("The source file does not exist!\n")
                exit(11)
        except Exception:
            sys.stderr.write("The XML code is not well-formed (invalid token)!\n")
            exit(31)

        root = tree.getroot()

        instructions = []
        order_list = []

        for child in root:
            try:
                instruction = Instruction(child.attrib['order'], child.attrib['opcode'])
                order_list.append(child.attrib['order'])
                for baby_child in child:
                    instruction.add_arg(Argument(baby_child.tag, baby_child.attrib['type'], baby_child.text))
            except KeyError:
                sys.stderr.write("Error XML attrib!\n")
                exit(32)

            instructions.append(instruction)

        if not is_all_uniq(order_list):
            sys.stderr.write("Repeated order attrib!\n")
            exit(32)

        return sorted(instructions, key=lambda x: x.instruction_order, reverse=False)

