#!/usr/bin/env python3
from collections import Counter

class Polynomial(object):
    """ Trida reprezentujici polynom """
    
    def __init__(self, *args, **kwargs):
        """ Konstruktor """
        self.dict_poly = {}
        self.hodnota = []
            
        if len(args) != 0:
            try:
                enumerate(*args)
                for arg in args:    
                    argumenty = arg
            except:
                argumenty = args
            
            for idx, arg in enumerate(argumenty):
        #        print(arg)
                self.dict_poly['x'+str(idx)] = arg
        #    print(self.dict_poly)
            
        elif len(kwargs) != 0:    
            for arg in kwargs:
                self.dict_poly[arg] = kwargs[arg]
       #     print(self.dict_poly)

    def __repr__(self):
        """Reprezentace tridy"""
        ret_str = self.create_str_poly()
        if ret_str == '':
            return '0'
        else:
            return ret_str
        
    def __eq__(self, new):
        """Porovnavani objektu"""
        ret_str = self.create_str_poly()
        if ret_str == '':
            return '0'
        else:
            return ret_str

    def __add__(self, new):
        """Scitani objektu"""
        pol = Polynomial()
        pol.dict_poly = Counter(self.dict_poly) + Counter(new.dict_poly)
        return pol

    def __multiply(self, a, b):
        """
            Pomocna funkce pro nasobeni
            [a1, a2, a3] -> a1 + a2*x + a3*x^2
            [b1, b2, b3] -> b1 + b2*x + b3*x^2
        """
        c = [0.0]*(len(a) + len(b)-1)

        for i in range(len(a)):
            ai = a[i]
            for j in range(len(b)):
                c[i + j] += ai * b[j]

        return c

    def __pow(self, a, n):
        """
         Pomocna funkce pro pow
         a^n
        """
        c = [1]
        for i in range(n):
            c = self.__multiply(c, a)
        return c

    def __pow__(self, rhs):
        """Umocnovani objektu"""
        self.hodnota = [0 for x in range(len(self.dict_poly))]
        for idx, key in enumerate(sorted(self.dict_poly)):
            val = int(self.dict_poly[key])
            idx = int(key[1:])
            self.hodnota[idx] = val
        self.hodnota = self.__pow(self.hodnota, rhs)

        val_dict = {}
        for idx, arg in enumerate(self.hodnota):
            val_dict['x'+str(idx)] = arg

        self.dict_poly, val_dict = val_dict, self.dict_poly

        return_str = self.create_str_poly()

        self.dict_poly, val_dict = val_dict, self.dict_poly

        return str(return_str)

    
    def derivative(self):
        """Derivace"""
        
        pole = [0 for x in range(len(self.dict_poly))]
        
        for idx, key in enumerate(sorted(self.dict_poly)):
            val = int(self.dict_poly[key])
            idx = int(key[1:])
            pole[idx-1] = val*idx
        return Polynomial(pole)
            

    def create_str_poly(self):
        """Vytvoreni stringove reprezentace polynomu"""
        poly_str_list = []
        for idx, key in enumerate(sorted(self.dict_poly)):
            val = int(self.dict_poly[key])
            idx = int(key[1:])
            if val > 0:
                if val == 1:
                    if idx == 1:
                        poly_str_list.append(str(' + ' + 'x'))
                    elif idx == 0:
                        poly_str_list.append(str(' + ' + str(val)))
                    else:
                        poly_str_list.append(str(' + ' + 'x^' + str(idx)))
                else:
                    if idx == 1:
                        poly_str_list.append(str(' + ' + str(val) + 'x'))
                    elif idx == 0:
                        poly_str_list.append(str(' + ' + str(val)))
                    else:
                        poly_str_list.append(str(' + ' + str(val) + 'x^' + str(idx)))                    
            elif val < 0:
                if val == -1:
                    if idx == 1:
                        poly_str_list.append(str(' - ' + 'x'))
                    elif idx == 0:
                        poly_str_list.append(str(' - ' + str(abs(val))))
                    else:
                        poly_str_list.append(str(' - ' + 'x^' + str(idx)))
                else:
                    if idx == 1:
                        poly_str_list.append(str(' - ' + str(abs(val)) + 'x'))
                    elif idx == 0:
                        poly_str_list.append(str(' - ' + str(abs(val))))
                    else:
                        poly_str_list.append(str(' - ' + str(abs(val)) + 'x^' + str(idx)))                    
        return ''.join(poly_str_list[::-1])[3:]
    
    def count_val(self, num, val, idx):
        """Pomocna funkce"""
        sumval = 0
        if val > 0:
            if val == 1:
                if idx == 1:
                    sumval += num
                elif idx == 0:
                    sumval += val
                else:
                    sumval += num**idx
            else:
                if idx == 1:
                    sumval += val*num
                elif idx == 0:
                    sumval += val
                else:
                    sumval += val*(num**idx)
        elif val < 0:
            if val == -1:
                if idx == 1:
                    sumval -= num
                elif idx == 0:
                    sumval -= abs(val)
                else:
                    sumval -= num**idx
            else:
                if idx == 1:
                    sumval -= abs(val)*num
                elif idx == 0:
                    sumval -= abs(val)
                else:
                    sumval -= abs(val)*(num**idx)
        return sumval

    def at_value(self, *numx):
        """Hodnota polynomu pro urcitou hodnotu"""
        poly_str_list = []
        sum_vals = []

        for num in numx:
            sum_foo = []
            for idx, key in enumerate(sorted(self.dict_poly)):
                val = int(self.dict_poly[key])
                idx = int(key[1:])
                sum_foo.append(self.count_val(num, val, idx))
            sum_vals.append(sum(sum_foo))
            
        if len(sum_vals) == 1:
            return sum_vals[0]
        else:
            return sum_vals[1]-sum_vals[0]
      
def test():
    """Jednotkove testy"""
    assert str(Polynomial(0,1,0,-1,4,-2,0,1,3,0)) == "3x^8 + x^7 - 2x^5 + 4x^4 - x^3 + x"
    assert str(Polynomial([-5,1,0,-1,4,-2,0,1,3,0])) == "3x^8 + x^7 - 2x^5 + 4x^4 - x^3 + x - 5"
    assert str(Polynomial(x7=1, x4=4, x8=3, x9=0, x0=0, x5=-2, x3= -1, x1=1)) == "3x^8 + x^7 - 2x^5 + 4x^4 - x^3 + x"
    assert str(Polynomial(x2=0)) == "0"
    assert str(Polynomial(x0=0)) == "0"
    assert Polynomial(x0=2, x1=0, x3=0, x2=3) == Polynomial(2,0,3)
    assert Polynomial(x2=0) == Polynomial(x0=0)
    assert str(Polynomial(x0=1)+Polynomial(x1=1)) == "x + 1"
    assert str(Polynomial([-1,1,1,0])+Polynomial(1,-1,1)) == "2x^2"
    pol1 = Polynomial(x2=3, x0=1)
    pol2 = Polynomial(x1=1, x3=0)
    assert str(pol1+pol2) == "3x^2 + x + 1"
    assert str(pol1+pol2) == "3x^2 + x + 1"   
    assert str(Polynomial(x0=-1,x1=1)**1) == "x - 1"
    assert str(Polynomial(x0=-1,x1=1)**2) == "x^2 - 2x + 1"
    pol3 = Polynomial(x0=-1,x1=1)
    assert str(pol3**4) == "x^4 - 4x^3 + 6x^2 - 4x + 1"
    assert str(pol3**4) == "x^4 - 4x^3 + 6x^2 - 4x + 1"
    assert str(Polynomial(x0=2).derivative()) == "0"
    assert str(Polynomial(x3=2,x1=3,x0=2).derivative()) == "6x^2 + 3"
    assert str(Polynomial(x3=2,x1=3,x0=2).derivative().derivative()) == "12x"
    pol4 = Polynomial(x3=2,x1=3,x0=2)
    assert str(pol4.derivative()) == "6x^2 + 3"
    assert str(pol4.derivative()) == "6x^2 + 3"
    assert Polynomial(-2,3,4,-5).at_value(0) == -2
    assert Polynomial(x2=3, x0=-1, x1=-2).at_value(3) == 20
    assert Polynomial(x2=3, x0=-1, x1=-2).at_value(3,5) == 44
    pol5 = Polynomial([1,0,-2])
    assert pol5.at_value(-2.4) == -10.52
    assert pol5.at_value(-2.4) == -10.52
    assert pol5.at_value(-1,3.6) == -23.92
    assert pol5.at_value(-1,3.6) == -23.92
    

if __name__ == '__main__':
    test()
