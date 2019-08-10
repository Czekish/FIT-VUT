import sys
from instruction import *
from arg_parse_helper import UserFile


class Stack:
    """
    Trida vytvari objekt typu zasobnik
    """
    def __init__(self):
        self.items = []

    def isEmpty(self):
        return self.items == []

    def push(self, item):
        self.items.append(item)

    def _pop(self):
        return self.items.pop()

    def size(self):
        return len(self.items)


class FrameStack(Stack):
    """
    Konkretni implementace stacku pro praci s lokalnimi ramci
    """
    def __init__(self):
        super().__init__()

    def pop_local_frame(self):
        if self.isEmpty():
            sys.stderr.write("Local frame does not exist!\n")
            exit(55)
        return self._pop()

    def read_local_frame(self):
        if self.isEmpty():
            sys.stderr.write("Local frame does not exist!\n")
            exit(55)
        return self.items[self.size()-1]


def save_to_var(global_frame, local_frame, temporary_frame, address_in_frame, arg_to_save):
    """
    Funkce ulozi argument do ramce zadaneho v promenne address_in_frame
    """
    frame, frame_address = address_in_frame.split("@")
    if frame == "GF":
        if frame_address in global_frame.dictionary:
            global_frame.dictionary[frame_address] = arg_to_save
        else:
            sys.stderr.write("The variable '%s' does not exist!\n" % frame_address)
            exit(54)

    elif frame == "TF":
        if is_temporary_frame_defined(temporary_frame):
            if frame_address in temporary_frame.dictionary:
                temporary_frame.dictionary[frame_address] = arg_to_save
            else:
                sys.stderr.write("The variable '%s' does not exist!\n" % frame_address)
                exit(54)
        else:
            sys.stderr.write("Temporary frame is not defined!\n")
            exit(55)

    elif frame == "LF":
        local_frame_to_edit = local_frame.pop_local_frame()
        if frame_address in local_frame_to_edit.dictionary:
            local_frame_to_edit.dictionary[frame_address] = arg_to_save
            local_frame.push(local_frame_to_edit)
        else:
            sys.stderr.write("The variable '%s' does not exist!\n" % frame_address)
            exit(54)


def define_var_in_frame(global_frame, local_frame, temporary_frame, address_in_frame):
    """
    Funkce definuje promennou v ramci zadanem v promenne address_in_frame
    """
    frame, frame_address = address_in_frame.split("@")
    if frame == "GF":
        global_frame.dictionary[frame_address] = None

    elif frame == "TF":
        if is_temporary_frame_defined(temporary_frame):
            temporary_frame.dictionary[frame_address] = None
        else:
            sys.stderr.write("Temporary frame is not defined!\n")
            exit(55)

    elif frame == "LF":
        local_frame_to_edit = local_frame.pop_local_frame()
        local_frame_to_edit.dictionary[frame_address] = None
        local_frame.push(local_frame_to_edit)


def read_from_global_frame(global_frame, address):
    """
    Funkce cte promennou z globalniho ramce z adresy ulozene v promenne address
    """
    if address in global_frame.dictionary:
        if global_frame.dictionary[address] is not None:
            return global_frame.dictionary[address]
        else:
            sys.stderr.write("The '%s' variable is not initialised!\n" % address)
            exit(56)
    else:
        sys.stderr.write("The '%s' variable does not exist!\n" % address)
        exit(54)


def read_from_temporary_frame(temporary_frame, address):
    """
    Funkce cte promennou z docasneho ramce z adresy ulozene v promenne address
    """
    if is_temporary_frame_defined(temporary_frame):
        if address in temporary_frame.dictionary:
            if temporary_frame.dictionary[address] is not None:
                return temporary_frame.dictionary[address]
            else:
                sys.stderr.write("The '%s' variable is not initialised!\n" % address)
                exit(56)
        else:
            sys.stderr.write("The '%s' variable does not exist!\n" % address)
            exit(54)
    else:
        sys.stderr.write("Temporary frame is not defined!\n")
        exit(55)


def read_from_local_frame(local_frame, address):
    """
    Funkce cte promennou z lokalniho ramce z adresy ulozene v promenne address
    """
    local_frame_read = local_frame.read_local_frame()

    if address in local_frame_read.dictionary:
        if local_frame_read.dictionary[address] is not None:
            return local_frame_read.dictionary[address]
        else:
            sys.stderr.write("The '%s' variable is not initialised!\n" % address)
            exit(56)
    else:
        sys.stderr.write("The '%s' variable does not exist!\n" % address)
        exit(54)


def change_escape_to_char(string):
    """
    Funkce zmeni escape sekvenci v retezci na znak.
    :param string: String, ve kterem se bude vyhledavat escape sekvence a menit na znak
    """
    to_replace = set(re.findall(r"\\\d\d\d", string))

    for item in to_replace:
        string = string.replace(item, chr(int(item[1:])))

    return string


def read_arg_from_var(global_frame, local_frame, temporary_frame, arg):
    """
    Funkce precte argument ze zadaneho ramce, kde adresa je ulozena v arg a je data prefixem jeho hodnoty
    """
    try:
        prefix, value = arg.value.split("@")
    except ValueError:
        sys.stderr.write("The wrong format of var!\n")
        exit(52)

    if prefix == "GF":
        loaded_arg = read_from_global_frame(global_frame, value)
    elif prefix == "TF":
        loaded_arg = read_from_temporary_frame(temporary_frame, value)
    elif prefix == "LF":
        loaded_arg = read_from_local_frame(local_frame, value)
    else:
        sys.stderr.write("Invalid frame!\n")
        exit(32)

    return loaded_arg


def write_value(global_frame, local_frame, temporary_frame, arg):
    """
    Funkce zapise hodnotu na standardni vystup.
    """

    if arg.type == "var":
        loaded_arg = read_arg_from_var(global_frame, local_frame, temporary_frame, arg)

        if loaded_arg.type == "string":
            print(change_escape_to_char(loaded_arg.value), end='')
        else:
            print(loaded_arg.value, end='')

    elif arg.type == "string":
        print(change_escape_to_char(arg.value), end='')

    else:
        print(arg.value, end='')


def jump_to_label(instruction_list, label):
    """
    Funkce vraci cislo indexu, na ktery se bude skakat, dle pozice labelu v instrukcni pasce
    """
    number_of_labels = 0
    label_index = None
    for idx, instruction in enumerate(instruction_list):
        if instruction.arg1.type == "label" and instruction.arg1.value == label and instruction.opcode != "JUMP" \
                and instruction.opcode != "JUMPIFEQ" and instruction.opcode != "JUMPIFNEQ" and instruction.opcode != "CALL":
            number_of_labels += 1
            label_index = idx
    # returns the index of label
    if number_of_labels > 1:
        sys.stderr.write("Label redefinition!\n")
        exit(52)
    elif number_of_labels == 0:
        sys.stderr.write("Label not found!\n")
        exit(52)
    else:
        return label_index


def concat(global_frame, local_frame, temporary_frame, symb1, symb2):
    """
    Funkce provede konkatenaci dvou retezcu
    """
    if symb1.type == "var":
        arg = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)

        if arg.type == "string":
            string_first_part = arg.value
        else:
            sys.stderr.write("The wrong operand!\n")
            exit(53)

    elif symb1.type == "string":
        string_first_part = symb1.value
    else:
        sys.stderr.write("The wrong operand!\n")
        exit(53)

    if symb2.type == "var":
        arg = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)

        if arg.type == "string":
            string_second_part = arg.value
        else:
            sys.stderr.write("The wrong operand!\n")
            exit(53)

    elif symb2.type == "string":
        string_second_part = symb2.value
    else:
        sys.stderr.write("The wrong operand!\n")
        exit(53)

    return string_first_part + string_second_part


def jump_if_eq(global_frame, local_frame, temporary_frame, label, symb1, symb2, instruction_list):
    """
    Funkce vraci hodnotu indexu, na ktery se bude skakat v pripade, ze dve hodnoty jsou si rovny
    """
    if symb1.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg1 = symb1

    if symb2.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg2 = symb2

    if arg1.type != arg2.type:
        sys.stderr.write("The operands are not of the same type!\n")
        exit(53)

    if arg1.value == arg2.value:
        return jump_to_label(instruction_list, label)

    else:
        return None


def jump_if_neq(global_frame, local_frame, temporary_frame, label, symb1, symb2, instruction_list):
    """
    Funkce vraci hodnotu indexu, na ktery se bude skakat v pripade, ze dve hodnoty jsou rozdilne
    """
    if symb1.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg1 = symb1

    if symb2.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg2 = symb2

    if arg1.type != arg2.type:
        sys.stderr.write("The operands are not of the same type!\n")
        exit(53)

    if arg1.value != arg2.value:
        return jump_to_label(instruction_list, label)

    else:
        return None


def my_exit(global_frame, local_frame, temporary_frame, symb):
    """
    Funkce ukonci program s vybranym chybovym kodem
    """
    if symb.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb)
    else:
        arg1 = symb

    if arg1.type == "int":
        try:
            val = int(arg1.value)
            if 0 <= val <= 49:
                return val
            else:
                sys.stderr.write("Number is out of the interval!\n")
                exit(57)

        except TypeError:
            sys.stderr.write("The operand is expected to be int!\n")
            exit(53)

    else:
        sys.stderr.write("The wrong operand's type!\n")
        exit(53)


def add(global_frame, local_frame, temporary_frame, symb1, symb2):
    """
    Funkce secte dve cisla
    """
    if symb1.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg1 = symb1

    if symb2.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg2 = symb2

    if arg1.type == "int" and arg2.type == "int":
        try:
            val1 = int(arg1.value)
            val2 = int(arg2.value)
            return val1+val2
        except TypeError:
            sys.stderr.write("The operand is expected to be int!\n")
            exit(53)
    else:
        sys.stderr.write("The operands are not of the same type!\n")
        exit(53)


def sub(global_frame, local_frame, temporary_frame, symb1, symb2):
    """
    Funkce odecte dve cisla
    """
    if symb1.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg1 = symb1

    if symb2.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg2 = symb2

    if arg1.type == "int" and arg2.type == "int":
        try:
            val1 = int(arg1.value)
            val2 = int(arg2.value)
            return val1-val2
        except TypeError:
            sys.stderr.write("The operand is expected to be int!\n")
            exit(53)
    else:
        sys.stderr.write("The operands are not of the same type!\n")
        exit(53)


def mul(global_frame, local_frame, temporary_frame, symb1, symb2):
    """
    Funkce vynasobi dve cisla
    """
    if symb1.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg1 = symb1

    if symb2.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg2 = symb2

    if arg1.type == "int" and arg2.type == "int":
        try:
            val1 = int(arg1.value)
            val2 = int(arg2.value)
            return val1*val2
        except TypeError:
            sys.stderr.write("The operand is expected to be int!\n")
            exit(53)
    else:
        sys.stderr.write("The operands are not of the same type!\n")
        exit(53)


def idiv(global_frame, local_frame, temporary_frame, symb1, symb2):
    """
    Funkce vydeli dve cisla
    """
    if symb1.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg1 = symb1

    if symb2.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg2 = symb2

    if arg1.type == "int" and arg2.type == "int":
        try:
            val1 = int(arg1.value)
            val2 = int(arg2.value)
            if val2 == 0:
                sys.stderr.write("Division by zero!\n")
                exit(57)
            return int(val1/val2)
        except TypeError:
            sys.stderr.write("The operand is expected to be int!\n")
            exit(53)
    else:
        sys.stderr.write("The operands are not of the same type!\n")
        exit(53)


def lt(global_frame, local_frame, temporary_frame, symb1, symb2):
    """
    Funkce porovna dve hodnoty pomoci relace <
    """
    if symb1.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg1 = symb1

    if symb2.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg2 = symb2

    if arg1.type == "int" and arg2.type == "int":
        try:
            val1 = int(arg1.value)
            val2 = int(arg2.value)
            return str(val1 < val2).lower()
        except TypeError:
            sys.stderr.write("The operand is expected to be int!\n")
            exit(53)

    elif arg1.type == "bool" and arg2.type == "bool":
        if arg1.value == "false" and arg2.value == "true":
            return "true"
        else:
            return "false"

    elif arg1.type == "string" and arg2.type == "string":
        return str(arg1.value < arg2.value).lower()

    elif arg1.type == "nil" or arg2.type == "nil":
        sys.stderr.write("Nil can be compared only by EQ instruction!\n")
        exit(53)

    else:
        sys.stderr.write("LT can compare only the same type of int, bool or string!\n")
        exit(53)


def gt(global_frame, local_frame, temporary_frame, symb1, symb2):
    """
    Funkce porovna dve hodnoty pomoci relace >
    """
    if symb1.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg1 = symb1

    if symb2.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg2 = symb2

    if arg1.type == "int" and arg2.type == "int":
        try:
            val1 = int(arg1.value)
            val2 = int(arg2.value)
            return str(val1 > val2).lower()
        except TypeError:
            sys.stderr.write("The operand is expected to be int!\n")
            exit(53)

    elif arg1.type == "bool" and arg2.type == "bool":
        if arg1.value == "true" and arg2.value == "false":
            return "true"
        else:
            return "false"
    elif arg1.type == "string" and arg2.type == "string":
        return str(arg1.value > arg2.value).lower()

    elif arg1.type == "nil" or arg2.type == "nil":
        sys.stderr.write("Nil can be compared only by EQ instruction!\n")
        exit(53)

    else:
        sys.stderr.write("GT can compare only the same type of int, bool or string!\n")
        exit(53)


def eq(global_frame, local_frame, temporary_frame, symb1, symb2):
    """
    Funkce porovna dve hodnoty pomoci relace =
    """
    if symb1.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg1 = symb1

    if symb2.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg2 = symb2

    if arg1.type == "int" and arg2.type == "int":
        try:
            val1 = int(arg1.value)
            val2 = int(arg2.value)
            return str(val1 == val2).lower()
        except TypeError:
            sys.stderr.write("The operand is expected to be int!\n")
            exit(53)

    elif arg1.type == "bool" and arg2.type == "bool":
        return str(arg1.value == arg2.value).lower()

    elif arg1.type == "string" and arg2.type == "string":
        return str(arg1.value == arg2.value).lower()

    elif arg1.type == "nil" and arg2.type == "nil":
        return "true"

    elif (arg1.type == "nil" and arg2.type != "nil") or (arg1.type != "nil" and arg2.type == "nil"):
        return "false"
    else:
        sys.stderr.write("Comparison error!\n")
        exit(53)


def and_ipp(global_frame, local_frame, temporary_frame, symb1, symb2):
    """
    Funkce vraci vysledek logickeho AND
    """
    if symb1.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg1 = symb1

    if symb2.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg2 = symb2

    if arg1.type == "bool" and arg2.type == "bool":
        if arg1.value == "true" and arg2.value == "true":
            return "true"

        else:
            return "false"

    else:
        sys.stderr.write("The wrong operands' type!\n")
        exit(53)


def or_ipp(global_frame, local_frame, temporary_frame, symb1, symb2):
    """
    Funkce vraci vysledek logickeho OR
    """
    if symb1.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg1 = symb1

    if symb2.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg2 = symb2

    if arg1.type == "bool" and arg2.type == "bool":
        if arg1.value == "true" or arg2.value == "true":
            return "true"

        else:
            return "false"

    else:
        sys.stderr.write("The wrong operands' type!\n")
        exit(53)


def not_ipp(global_frame, local_frame, temporary_frame, symb):
    """Funkce vraci vysledek vysledek logickeho NOT"""
    if symb.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb)
    else:
        arg1 = symb

    if arg1.type == "bool":
        if arg1.value == "true":
            return "false"

        else:
            return "true"

    else:
        sys.stderr.write("The wrong operands' type!\n")
        exit(53)


def int2char(global_frame, local_frame, temporary_frame, symb):
    """Funkce prevede znak z ASCII hodnoty na znak"""
    if symb.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb)
    else:
        arg1 = symb

    if arg1.type == "int":
        try:
            number_to_string = int(arg1.value)
            result_char = chr(number_to_string)
            return result_char
        except:
            sys.stderr.write("Invalid value!\n")
            exit(58)
    else:
        sys.stderr.write("The wrong operand's type!\n")
        exit(53)


def stri2int(global_frame, local_frame, temporary_frame, symb1, symb2):
    """Prevede znak na jeho ordinalni (ASCII) hodnotu"""
    if symb1.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg1 = symb1

    if symb2.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg2 = symb2

    if arg1.type == "string" and arg2.type == "int":
        try:
            index = int(arg2.value)
            return str(ord(arg1.value[index]))
        except IndexError:
            sys.stderr.write("Index out of the range!\n")
            exit(58)
        except TypeError:
            sys.stderr.write("The wrong operand's type!\n")
            exit(53)
    else:
        sys.stderr.write("The wrong operand's type!\n")
        exit(53)


def read(input_type, arg_helper):
    """Nacte retezec bud ze souboru nebo ze standardniho vstupu, v zavislosti na zadanem vstupnim parametru"""
    if arg_helper.input:
        try:
            user_file_helper = UserFile.getInstance(arg_helper.input)
        except Exception:
            sys.stderr.write("File does not exist or you are not allowed to access the file!\n")
            exit(11)
        loaded_value = user_file_helper.read_line()
    else:
        loaded_value = input()

    if input_type.value == "string":
        return str(loaded_value)

    elif input_type.value == "int":
        try:
            return str(int(loaded_value))
        except:
            return "0"

    elif input_type.value == "bool":
        if loaded_value.lower() == "true":
            return "true"
        else:
            return "false"


def strlen(global_frame, local_frame, temporary_frame, symb):
    """Vrati delku retezce"""
    if symb.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb)
    else:
        arg1 = symb

    if arg1.type != "string":
        sys.stderr.write("The wrong operand's type!\n")
        exit(53)

    return str(len(arg1.value))


def getchar(global_frame, local_frame, temporary_frame, symb1, symb2):
    """Vrati znak ve stringu na urcitem indexu"""
    if symb1.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg1 = symb1

    if symb2.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg2 = symb2

    if arg1.type != "string" or arg2.type != "int":
        sys.stderr.write("The wrong operands' type!\n")
        exit(53)

    index = int(arg2.value)
    string = arg1.value

    if index >= len(string):
        sys.stderr.write("Index is out of the string's boundary!!\n")
        exit(58)

    return string[index]


def setchar(global_frame, local_frame, temporary_frame, var, symb1, symb2):
    """Nastavi znak v retezci na urcitem indexu na zadany jiny znak"""
    if var.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, var)
    else:
        arg1 = var

    if symb1.type == "var":
        arg2 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb1)
    else:
        arg2 = symb1

    if symb2.type == "var":
        arg3 = read_arg_from_var(global_frame, local_frame, temporary_frame, symb2)
    else:
        arg3 = symb2

    if arg1.type != "string" or arg2.type != "int" or arg3.type != "string":
        sys.stderr.write("The wrong operands' type!\n")
        exit(53)

    index = int(arg2.value)
    string_to_be_changed = arg1.value
    char_to_replace = arg3.value

    if index >= len(string_to_be_changed):
        sys.stderr.write("Index is out of the string's boundary!!\n")
        exit(58)
    elif char_to_replace == "":
        sys.stderr.write("The string is empty!\n")
        exit(58)

    string_to_be_changed = list(string_to_be_changed)
    string_to_be_changed[index] = char_to_replace[0]

    return ''.join(string_to_be_changed)


def is_temporary_frame_defined(temporary_frame):
    """Funkce vrati True / False v zavislosti na tom, zde je definovan docasny ramec"""
    return temporary_frame.dictionary is not None


def push(global_frame, local_frame, temporary_frame, var):
    """Funkce precte hodnotu z ramce, nebo vrati hodnotu v pripade konstanty"""
    if var.type == "var":
        arg1 = read_arg_from_var(global_frame, local_frame, temporary_frame, var)
    else:
        arg1 = var

    return arg1


def get_type(global_frame, local_frame, temporary_frame, symb):
    """Funkce dynamicky zjisti typ promenne a tento typ vrati"""
    if symb.type == "var":
        try:
            prefix, address = symb.value.split("@")
        except ValueError:
            sys.stderr.write("The wrong format of var!\n")
            exit(52)

        if prefix == "GF":
            if address in global_frame.dictionary:
                if global_frame.dictionary[address] is not None:
                    arg1 = global_frame.dictionary[address]
                else:
                    return ''
            else:
                sys.stderr.write("The '%s' variable does not exist!\n" % address)
                exit(54)

        elif prefix == "TF":
            if is_temporary_frame_defined(temporary_frame):
                if address in temporary_frame.dictionary:
                    if temporary_frame.dictionary[address] is not None:
                        arg1 = temporary_frame.dictionary[address]
                    else:
                        return ''
                else:
                    sys.stderr.write("The '%s' variable does not exist!\n" % address)
                    exit(54)
            else:
                sys.stderr.write("Temporary frame is not defined!\n")
                exit(55)

        elif prefix == "LF":
            local_frame_read = local_frame.read_local_frame()

            if address in local_frame_read.dictionary:
                if local_frame_read.dictionary[address] is not None:
                    return local_frame_read.dictionary[address]
                else:
                    return ''
            else:
                sys.stderr.write("The '%s' variable does not exist!\n" % address)
                exit(54)

        else:
            sys.stderr.write("Invalid frame!\n")
            exit(32)

    else:
        arg1 = symb

    return arg1.type
