#!/usr/bin/env python3

from arg_parse_helper import *
from xml_parse_helper import *
from frame import *
from stack import *
import sys

# Kontrola vstupnich parametru
arg_helper = ArgParseHelper()
arg_helper.check_params_and_print_help()


xml_helper = XMLParseHelper(arg_helper)

# Nacteni XML a prevod na objekty typu Instrukce
instruction_list = xml_helper.load_xml()

# Syntakticka kontrola
for instruction in instruction_list:
    instruction.is_syntax_correct()


data_stack = Stack()
calls_stack = Stack()

global_frame = Frame(dictionary={})
local_frame = FrameStack()
temporary_frame = Frame(dictionary=None)


# Zpracovava pasku s instrukcemi a dle operacniho kodu vykonava patricne operace
i = 0
while i < len(instruction_list):
    instruction = instruction_list[i]
    i += 1

    if "PUSHS" == instruction.opcode:  # <var>
        var = instruction.arg1
        value_to_push = push(global_frame, local_frame, temporary_frame, var)
        data_stack.push(value_to_push)

    elif "POPS" == instruction.opcode:  # <var>
        if data_stack.size() == 0:
            sys.stderr.write("The data stack is empty!\n")
            exit(56)

        arg_to_save = data_stack._pop()

        save_to_var(global_frame, local_frame, temporary_frame, instruction.arg1.value, arg_to_save)
        # print(global_frame.__dict__) # debug

    elif "DEFVAR" == instruction.opcode:  # <var>
        define_var_in_frame(global_frame, local_frame, temporary_frame, instruction.arg1.value)

    elif "WRITE" == instruction.opcode:  # <symb>
        write_value(global_frame, local_frame, temporary_frame, instruction.arg1)

    elif "MOVE" == instruction.opcode:  # <var> <symb>
        var = instruction.arg1.value
        symb = instruction.arg2
        if symb.type == "var":
            arg = read_arg_from_var(global_frame, local_frame, temporary_frame, symb)
        else:
            arg = symb
        save_to_var(global_frame, local_frame, temporary_frame, var, arg)

    elif "JUMP" == instruction.opcode:  # <label>
        label = instruction.arg1.value
        i = jump_to_label(instruction_list, label)

    elif "CONCAT" == instruction.opcode:  # <var> <symb1> <symb2>
        var = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        concatenated_string = concat(global_frame, local_frame, temporary_frame, symb1, symb2)
        save_to_var(global_frame, local_frame, temporary_frame, var,
                    Argument(type="string", value=concatenated_string))

    elif "JUMPIFEQ" == instruction.opcode:  # <label> <symb1> <symb2>
        label = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        jump_to_index = jump_if_eq(global_frame, local_frame, temporary_frame, label, symb1, symb2, instruction_list)
        if jump_to_index is not None:
            i = jump_to_index

    elif "JUMPIFNEQ" == instruction.opcode:  # <label> <symb1> <symb2>
        label = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        jump_to_index = jump_if_neq(global_frame, local_frame, temporary_frame, label, symb1, symb2, instruction_list)
        if jump_to_index is not None:
            i = jump_to_index

    elif "EXIT" == instruction.opcode:  # <symb>
        symb = instruction.arg1
        exit_value = my_exit(global_frame, local_frame, temporary_frame, symb)
        exit(exit_value)

    elif "ADD" == instruction.opcode:  # <var> <symb1> <symb2>
        var = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        value_to_save = add(global_frame, local_frame, temporary_frame, symb1, symb2)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="int", value=str(value_to_save)))

    elif "SUB" == instruction.opcode:  # <var> <symb1> <symb2>
        var = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        value_to_save = sub(global_frame, local_frame, temporary_frame, symb1, symb2)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="int", value=str(value_to_save)))

    elif "MUL" == instruction.opcode:  # <var> <symb1> <symb2>
        var = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        value_to_save = mul(global_frame, local_frame, temporary_frame, symb1, symb2)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="int", value=str(value_to_save)))

    elif "IDIV" == instruction.opcode:  # <var> <symb1> <symb2>
        var = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        value_to_save = idiv(global_frame, local_frame, temporary_frame, symb1, symb2)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="int", value=str(value_to_save)))

    elif "LT" == instruction.opcode:  # <var> <symb1> <symb2>
        var = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        result = lt(global_frame, local_frame, temporary_frame, symb1, symb2)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="bool", value=result))

    elif "GT" == instruction.opcode:  # <var> <symb1> <symb2>
        var = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        result = gt(global_frame, local_frame, temporary_frame, symb1, symb2)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="bool", value=result))

    elif "EQ" == instruction.opcode:  # <var> <symb1> <symb2>
        var = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        result = eq(global_frame, local_frame, temporary_frame, symb1, symb2)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="bool", value=result))

    elif "AND" == instruction.opcode:  # <var> <symb1> <symb2>
        var = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        result = and_ipp(global_frame, local_frame, temporary_frame, symb1, symb2)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="bool", value=result))

    elif "OR" == instruction.opcode:  # <var> <symb1> <symb2>
        var = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        result = or_ipp(global_frame, local_frame, temporary_frame, symb1, symb2)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="bool", value=result))

    elif "NOT" == instruction.opcode:  # <var> <symb>
        var = instruction.arg1.value
        symb = instruction.arg2
        result = not_ipp(global_frame, local_frame, temporary_frame, symb)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="bool", value=result))

    elif "INT2CHAR" == instruction.opcode:  # <var> <symb>
        var = instruction.arg1.value
        symb = instruction.arg2
        result = int2char(global_frame, local_frame, temporary_frame, symb)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="string", value=result))

    elif "LABEL" == instruction.opcode:
        pass

    elif "STRI2INT" == instruction.opcode:  # <var> <symb1> <symb2>
        var = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        result = stri2int(global_frame, local_frame, temporary_frame, symb1, symb2)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="int", value=result))

    elif "READ" == instruction.opcode:  # <var> <type>
        var = instruction.arg1.value
        input_type = instruction.arg2
        result = read(input_type, arg_helper)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type=input_type.value, value=result))

    elif "STRLEN" == instruction.opcode:  # <var> <symb>
        var = instruction.arg1.value
        symb = instruction.arg2
        number_of_chars = strlen(global_frame, local_frame, temporary_frame, symb)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="string", value=number_of_chars))

    elif "GETCHAR" == instruction.opcode:  # <var> <symb1> <symb2>
        var = instruction.arg1.value
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        char_from_string = getchar(global_frame, local_frame, temporary_frame, symb1, symb2)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="string", value=char_from_string))

    elif "SETCHAR" == instruction.opcode:  # <var> <symb1> <symb2>
        var = instruction.arg1
        symb1 = instruction.arg2
        symb2 = instruction.arg3
        modified_value = setchar(global_frame, local_frame, temporary_frame, var, symb1, symb2)
        save_to_var(global_frame, local_frame, temporary_frame, var.value, Argument(type="string", value=modified_value))

    elif "CREATEFRAME" == instruction.opcode:  #
        temporary_frame = Frame(dictionary={})

    elif "PUSHFRAME" == instruction.opcode:  #
        if is_temporary_frame_defined(temporary_frame):
            local_frame.push(temporary_frame)
            temporary_frame = Frame(dictionary=None)

        else:
            sys.stderr.write("Temporary frame is not defined!\n")
            exit(55)

    elif "POPFRAME" == instruction.opcode:  #
        temporary_frame = local_frame.pop_local_frame()

    elif "TYPE" == instruction.opcode:  # <var> <symb>
        var = instruction.arg1.value
        symb = instruction.arg2
        value_to_save = get_type(global_frame, local_frame, temporary_frame, symb)
        save_to_var(global_frame, local_frame, temporary_frame, var, Argument(type="string", value=value_to_save))

    elif "CALL" == instruction.opcode:  # <label>
        label = instruction.arg1.value
        calls_stack.push(i)
        i = jump_to_label(instruction_list, label)

    elif "RETURN" == instruction.opcode:  #
        if calls_stack.isEmpty():
            sys.stderr.write("No value in the call stack!\n")
            exit(56)
        else:
            i = calls_stack._pop()
