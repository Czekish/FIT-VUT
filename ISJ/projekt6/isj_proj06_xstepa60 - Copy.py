#!/usr/bin/env python3
from collections import Counter
import itertools
import re


def first_nonrepeating(string):
    """ The function returns a first non repeating string """

    if string == '' or string.isspace() or type(string) == type([]) \
        or type(string) == type({}):
        return None

    for (key, val) in Counter(str(string).strip()).items():
        if val == 1:
            return key

    return None


def combine4(lst, result):
    """ The function returns list of expressions,
    which are combinations of all operators """

    if len(lst) != 4:
        return None
    operators = ['+', '*', '-', '/']
    expressions = []
    for x in list(itertools.permutations(lst)):

        lst_numbers = [str(y) for y in x]
        for a in range(len(operators)):
            for b in range(len(operators)):
                for c in range(len(operators)):

                    expr = str(lst_numbers[0] + operators[a]
                               + lst_numbers[1] + operators[b]
                               + lst_numbers[2] + operators[c]
                               + lst_numbers[3])
                    try:
                        if eval(expr) == result:
                            expressions.append(expr)
                    except:
                        pass
                    expr = str('(' + lst_numbers[0] + operators[a]
                               + lst_numbers[1] + ')' + operators[b]
                               + lst_numbers[2] + operators[c]
                               + lst_numbers[3])
                    try:
                        if eval(expr) == result:
                            expressions.append(expr)
                    except:
                        pass
                    expr = str(lst_numbers[0] + operators[a] + '('
                               + lst_numbers[1] + operators[b]
                               + lst_numbers[2] + ')' + operators[c]
                               + lst_numbers[3])
                    try:
                        if eval(expr) == result:
                            expressions.append(expr)
                    except:
                        pass
                    expr = str(lst_numbers[0] + operators[a]
                               + lst_numbers[1] + operators[b] + '('
                               + lst_numbers[2] + operators[c]
                               + lst_numbers[3] + ')')
                    try:
                        if eval(expr) == result:
                            expressions.append(expr)
                    except:
                        pass
    if len(expressions) != 0:
        return set(expressions)
    else:
        return []

