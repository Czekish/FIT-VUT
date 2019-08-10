import argparse
import sys


class UserFile:
    """
    Singleton pattern
    Funkce vytvori jedinou instanci oteveneho souboru
    :param file: Cesta k souboru
    """
    __instance = None
    user_file = None

    @staticmethod
    def getInstance(file):
        if UserFile.__instance is None:
            UserFile(open(file, "r"))
        return UserFile.__instance

    def __init__(self, file):
        if UserFile.__instance is not None:
            sys.stderr.write("Singleton class!!")
            exit(99)
        else:
            UserFile.__instance = self
            UserFile.__instance.user_file = file

    def read_line(self):
        return self.user_file.readline()


class ArgParseHelper:
    """
    Funkce pro parsovani vstupnich parametru
    """

    def __init__(self):
        self.parser = argparse.ArgumentParser(add_help=False)
        self.parser.add_argument("--source",
                                 help="input file containing the XML representation of the IPPcode19 source code")
        self.parser.add_argument("--input",
                                 help="file containing inputs for interpretation of the source code")
        self.parser.add_argument("--help",
                                 help="show this help message and exit",
                                 action="store_true")

        self.args = self.parser.parse_args()

    def check_params_and_print_help(self):
        """
        Osetreni neplatnych kombinaci parametru a tisk napovedy
        :return True or False
        """
        if self.args.help and (self.args.source or self.args.input):
            sys.stderr.write("The invalid combination of the --help parameter with other parameters!\n")
            exit(10)

        elif self.args.help:
            ArgParseHelper.print_help()
            exit(0)

        elif not (self.args.source or self.args.input):
            sys.stderr.write("At least one of the parameters --source=file or --input=file has to be set!\n")
            exit(10)

    @property
    def source(self):
        """
        V zavislosti na nastavenem parametru vrati ukazatel na standardni vstup a nebo soubor
        :return: Ukazatel na soubor / stdin
        """
        if self.args.source:
            return self.args.source
        else:
            return sys.stdin

    @property
    def input(self):
        return self.args.input

    @staticmethod
    def print_help():
        """
        :return: Napoveda k programu
        """
        print("Program nacita XML reprezentaci programu ze zadaneho souboru a tento program s vyuzitim standardniho vstupu a vystupu interpretuje.\n" +
              "Skript pracuje s nasledujicimi parametry:\n" +
              "--help - vypis napovedy\n" +
              "--source=file - vstupni soubor s XML reprezentaci zdrojoveho kodu\n" +
              "--input=file - soubor se vstupy pro samotnou interpretaci zadaneho zdrojoveho kodu\n" +
              "Alespon jeden z parametru (--source nebo --input) musi byt zadan. Pokud jeden z nich chyby, tak jsou data nacitana ze standardniho vstupu")
