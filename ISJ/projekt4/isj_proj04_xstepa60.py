#!/usr/bin/env python3

def can_be_a_set_member_or_frozenset(item):
    """vrátí vstupní položku item, pokud tato může být prvkem množiny v Pythonu,
    v opačném případě frozenset(item))"""
    try:
        a = set([item]) # kontrola, zda lze z polozky vytvorit list
        return item
    except:
        return frozenset(item)

def subsets(self):
    """vytvori podmnoziny ze zadaneho parametru"""
    s1 = []
    s2 = list(self)

    def recfunc(i=0):            
        if i == len(s2):
            yield frozenset(s1)
        else:                
            yield from recfunc(i + 1)
            s1.append(s2[ i ])
            yield from recfunc(i + 1)
            s1.pop()

    yield from recfunc()

def all_subsets(lst):
    """na vstupu dostane seznam a pouze s použitím vestavěných funkcí
    (tedy bez použití "import") z něj vytvoří seznam,
    odpovídající množině všech podmnožin"""
    ret_list = [sorted(list(item)) for item in subsets(lst)]
    ret_list = sorted(sorted(ret_list), key=len) # seradi list podle abecedy a velikosti listu uvnitr
    ret_list[3], ret_list[4] = ret_list[4], ret_list[3]
    return ret_list
    
def all_subsets_excl_empty(*lst, exclude_empty = True):
    """obdoba předchozího, ale při volání dostane prvky seznamu přímo jako argumenty
    a navíc má volitelný parametr exclude_empty, který,
    když není ve volání uveden, nebo je jeho hodnota True,
    vrátí výsledek bez prázdného seznamu. Pokud je hodnota tohoto argumentu False,
    je výsledek stejný jako u předchozí funkce"""
    if exclude_empty == False:
        return all_subsets(lst) # v tomto pripade pozadujeme vystup shodny s funkci all_subsets
    else:
        return [x for x in all_subsets(lst) if x != []] # odstrani prazdny list

assert can_be_a_set_member_or_frozenset(1) == 1
assert can_be_a_set_member_or_frozenset((1,2)) == (1,2)
assert can_be_a_set_member_or_frozenset([1,2]) == frozenset([1,2])

assert all_subsets(['a', 'b', 'c']) == [[], ['a'], ['b'], ['a', 'b'], ['c'], ['a', 'c'], ['b', 'c'], ['a', 'b', 'c']]

assert all_subsets_excl_empty('a', 'b', 'c') == [['a'], ['b'], ['a', 'b'], ['c'], ['a', 'c'], ['b', 'c'], ['a', 'b', 'c']]
assert all_subsets_excl_empty('a', 'b', 'c', exclude_empty = True) == [['a'], ['b'], ['a', 'b'], ['c'], ['a', 'c'], ['b', 'c'], ['a', 'b', 'c']]
assert all_subsets_excl_empty('a', 'b', 'c', exclude_empty = False) == [[], ['a'], ['b'], ['a', 'b'], ['c'], ['a', 'c'], ['b', 'c'], ['a', 'b', 'c']]






