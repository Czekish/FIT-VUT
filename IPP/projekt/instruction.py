
import sys
import contextlib
import re


def check_var(value):
    """
    Syntakticka kontrola, zda hodnota uvnitr value odpovida zadanemu formatu var
    :param value: Kontrolovana hodnota
    :return: True / False
    """
    var_regex = r"^(LF|TF|GF)@[a-zA-Z_\-$&%*!?]+[0-9a-zA-Z_\-$&%!?]*$"
    return re.match(var_regex, value)


def check_const(symb):
    """
    Syntakticka kontrola, zda hodnota uvnitr symb odpovida zadanemu formatu const
    :param symb: Kontrolovana hodnota
    :return: True / False
    """
    nil_regex = r"^nil$"
    bool_regex = r"^(false|true)"
    number_regex = r"^(\+|-)?\d+$" # propusti i cislo (v type je int) 3a2
    string_regex = r"^(?!.*((\\\d[^\d])|(#)|(\\\d\d[^\d])|(\\[^\d])).*).*$"

    return re.match(nil_regex, symb) \
           or re.match(bool_regex, symb) \
           or re.match(number_regex, symb) \
           or re.match(string_regex, symb)


def check_symb(symb):
    """
    Syntakticka kontrola, zda hodnota uvnitr symb odpovida zadanemu formatu symb
    :param symb: Kontrovana hodnota
    :return: True / False
    """
    return check_var(symb) or check_const(symb)


def check_label(label, type):
    """
    Syntakticka kontrola, zda hodnota uvnitr label odpovida zadanemu formatu label
    :param label: Kontrolovana hodnota label
    :param type: Kontrolovana hodnota type
    :return: True / False
    """
    label_regex = r"^[a-zA-Z_\-$&%*!?]+[0-9a-zA-Z_\-$&%!?]*$"
    return re.match(label_regex, label) and (type == "label")


def check_type(type):
    """
    Syntakticka kontrola, zda hodnota uvnitr type odpovida zadanemu formatu type
    :param type: Kontrolovana hodnota
    :return: True / False
    """
    type_regex = r"^(string|int|bool)$"
    return re.match(type_regex, type)


class Instruction:
    """
    Vnitrni reprezentace instrukce.
    Unitr objektu instrukce jsou ulozeny i argumenty
    """

    opcodes = {
                'MOVE': ['var', 'symb', None],
                'CREATEFRAME': [None, None, None],
                'PUSHFRAME': [None, None, None],
                'POPFRAME': [None, None, None],
                'DEFVAR': ['var', None, None],
                'CALL': ['label', None, None],
                'RETURN': [None, None, None],
                'PUSHS': ['symb', None, None],
                'POPS': ['var', None, None],
                'ADD': ['var', 'symb', 'symb'],
                'SUB': ['var', 'symb', 'symb'],
                'MUL': ['var', 'symb', 'symb'],
                'IDIV': ['var', 'symb', 'symb'],
                'LT': ['var', 'symb', 'symb'],
                'GT': ['var', 'symb', 'symb'],
                'EQ': ['var', 'symb', 'symb'],
                'AND': ['var', 'symb', 'symb'],
                'OR': ['var', 'symb', 'symb'],
                'NOT': ['var', 'symb', None],
                'INT2CHAR': ['var', 'symb', None],
                'STRI2INT': ['var', 'symb', 'symb'],
                'READ': ['var', 'type', None],
                'WRITE': ['symb', None, None],
                'CONCAT': ['var', 'symb', 'symb'],
                'STRLEN': ['var', 'symb', None],
                'GETCHAR': ['var', 'symb', 'symb'],
                'SETCHAR': ['var', 'symb', 'symb'],
                'TYPE': ['var', 'symb', None],
                'LABEL': ['label', None, None],
                'JUMP': ['label', None, None],
                'JUMPIFEQ': ['label', 'symb', 'symb'],
                'JUMPIFNEQ': ['label', 'symb', 'symb'],
                'EXIT': ['symb', None, None],
                'DPRINT': ['symb', None, None],
                'BREAK': [None, None, None]
                }

    def __init__(self, instruction_order, opcode):
        self.instruction_order = int(instruction_order)
        self.opcode = str(opcode).upper()
        arg = Argument()
        self.arg1 = arg
        self.arg2 = arg
        self.arg3 = arg

    def add_arg(self, arg):
        """
        Syntakticka kontrola spravnosti a ocislovani argumentu pri prevodu ze XML formatu na vnitrni reprezentaci
        :param arg: Kontrolovana hodnota
        """
        if arg.tag == "arg1":
            self.arg1 = arg
        elif arg.tag == "arg2":
            self.arg2 = arg
        elif arg.tag == "arg3":
            self.arg3 = arg
        else:
            sys.stderr.write("Wrong XML attribute!\n")
            exit(32)

    def is_syntax_correct(self):
        """
        Funkce kontroluje syntaktickou spravnost a pocet a poradi argumentu u instrukci
        """

        if self.opcode in Instruction.opcodes:

            for index, arg in enumerate([self.arg1, self.arg2, self.arg3]):

                if arg.type == "string" and arg.value is None:
                    arg.value = ''

                if arg.type is None and Instruction.opcodes[self.opcode][index] is None:
                    pass
                elif arg.type is not None and Instruction.opcodes[self.opcode][index] is not None:

                    if Instruction.opcodes[self.opcode][index] == 'var':
                        if not check_var(arg.value):
                            sys.stderr.write("Lexical or syntax error!\n")
                            exit(32)

                    elif Instruction.opcodes[self.opcode][index] == 'symb' and arg.type != 'label':
                        if not check_symb(arg.value):
                            sys.stderr.write("Lexical or syntax error!\n")
                            exit(32)

                    elif Instruction.opcodes[self.opcode][index] == 'label':
                        if not check_label(arg.value, arg.type):
                            sys.stderr.write("Lexical or syntax error!\n")
                            exit(32)

                    elif Instruction.opcodes[self.opcode][index] == 'type':
                        if not check_type(arg.value):
                            sys.stderr.write("Lexical or syntax error!\n")
                            exit(32)

                    else:
                        sys.stderr.write("Lexical or syntax error!\n")
                        exit(32)

                else:
                    sys.stderr.write("Lexical or syntax error!\n")
                    exit(32)

        else:
            sys.stderr.write("Unknown opcode!\n")
            exit(32)


class Argument:
    """
    Trida reprezentujici argumenty u funkci
    """
    def __init__(self, tag=None, type=None, value=None):
        self.tag = tag
        self.type = type
        self.value = value
