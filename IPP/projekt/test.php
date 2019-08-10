<?php

// Vypis napovedy k tomuto skriptu
function print_help() {
    echo "Tento skript slouzi pro automaticke testovani postupne aplikace parse.php a interpret.py.\nSkript projde zadany adresar s testy a vyuzije je pro automaticke otestovani spravne funkcnosti obou predchozich programu vcetne vygenerovani prehledneho souhrnu v HTML 5 do standardniho vystupu.\n";
    echo "Skript pracuje s parametry:\n --help - tisk napovedy\n --directory=path - testy bude hledat v zadanem adresari (chybi-li tento parametr, skript prochazi aktualni adresar)\n --recursive - testy bude hledat nejen v zadanem adresari, ale i rekurzivne ve vsech jeho podadresarich\n --parse-script=file - soubor se skriptem v PHP 7.3 pro analyzu zdrojoveho kodu v IPPcode19 (chybi-li, tak implicitni hodnotou je parse.php ulozeny v aktualnim adresari)\n --int-script=file - soubor se skriptem v Python 3.6 pro interpret XML reprezentace kodu v IPPcode19 (chybi-li tneto parametr, tak implicitni hodnotou je interpret .py ulozeny v aktualnim adresari)\n --parse-only - bud testovan pouze skript pro analyzu zdrojoveho kodu v IPPcode19\n --int-only - bude testovat pouze skript pro interpret XML reprezentace kodu v IPPcode19.\n";
}

// Generuje "zeleny" radek v HTML vystupu - uspesny test
function generate_html_ok_record() {
    global $result_array_ok;
    $html = "";
    foreach ($result_array_ok as $value) {
        $html .= '<tr><td bgcolor="#9bea99">' . $value . '</td></tr>';
    }

    return $html;

}

// Generuje "cerveny" radek v HTML vystupu - neuspesny test
function generate_html_nok_record() {
    global $result_array_nok;
    $html = '';
    foreach ($result_array_nok as $value) {
        $html .= '<tr><td bgcolor="#ff9898">' . $value . '</td></tr>';
    }

    return $html;
}

// Generuje HTML vystupni stranku na standardni vystup
function create_html_report() {
    // Globalni promenne s poctem uspesnych a neuspesnych testu
    global $result_test_ok;
    global $result_test_nok;

    return '

        <!DOCTYPE HTML>
        <html>
        <head>
        <style>
           table, th, td {
                border: 1px solid black;
                border-collapse: collapse;
            }
            th, td {
            padding: 15px;
            text-align: left;
            }
        </style>
        <style type="text/css">
            #wrap {
                width:1600px;
                margin:0 auto;
            }
            #left_col {
               float:left;
               width:800px;
            }
            #right_col {
               float:right;
               width:800px;
            }
            </style>
        </head>
        <body>

        <div id="wrap">       
            <div id="left_col">
            
                <h2>Uspesne testy (' . $result_test_ok . ')</h2>
                <table style="">
                    <tr>
                        <th>Testovany soubor</th>
                    </tr>
                ' . generate_html_ok_record() . '
                </table>
                
            </div>    
        
            <div id="right_col">
                <h2>Neuspesne testy (' . $result_test_nok . ')</h2>      
                <table style="">
                    <tr>
                        <th>Testovany soubor</th>
                    </tr>
                ' . generate_html_nok_record() . '
                </table>
            </div>
        </div>
        
        </body>
        </html>';

}

// Porovnava obsah promenne s obsahem souboru
function compare_var_file($output, $ref_output_file) {
    if(endsWith($ref_output_file, ".rc")) {
        if(trim($output) == trim(file_get_contents($ref_output_file))) {
            // Shoduje se
            result_inc_test_ok();
            result_append_test_ok($ref_output_file);
        }
        else {
            // Neshoduje se
            result_inc_test_nok();
            result_append_test_nok($ref_output_file);
        }
    } else {
        if ($output == file_get_contents($ref_output_file)) {
            // Shoduje se
            result_inc_test_ok();
            result_append_test_ok($ref_output_file);
        } else {
            // Neshoduje
            result_inc_test_nok();
            result_append_test_nok($ref_output_file);
        }
    }
}

// Provadi pouze kontrolu parsovani
// Spusti parser a vlozi do nej obsah *.src souboru a porovna s ocekavanymi vysledky
function parse_only($parse_script, $ref_src_file, $ref_out_file, $ref_rc_file) {
    $output = shell_exec("php7.3 $parse_script < \"$ref_src_file\" > output_temp");

    // Ziskani navratove hodnoty parseru
    $output_rc = shell_exec("php7.3 $parse_script < \"$ref_src_file\" >/dev/null 2>&1; echo $?");

    // Porovname navratovou hodnotu programu s referencni v souboru *.rc
    compare_var_file($output_rc, $ref_rc_file);

    // Porovname vystup skriptu s referencnim vystupem v *.out
    $output_xmldiff = shell_exec("java -jar /pub/courses/ipp/jexamxml/jexamxml.jar $ref_out_file output_temp diffs.xml /D /pub/courses/ipp/jexamxml/options; NAVRATOVA_HODNOTA=\"$?\"");
    $return_val = shell_exec("echo \$NAVRATOVA_HODNOTA");

    if($return_val == 0) {
        result_inc_test_ok();
        result_append_test_ok($ref_out_file);
    } else {
        result_inc_test_nok();
        result_append_test_nok($ref_out_file);
    }

    // Smazani pomocneho souboru
    $output_rm = shell_exec("rm output_temp");
}

// Provadi pouze kontrolu interpretu, spusti interpret a vlozi do nej zdrojova data a soubor, ktery simuluje standardni vstup
function int_only($int_script, $ref_src_file, $ref_out_file,$ref_in_file, $ref_rc_file) {
    $output = shell_exec("python3.6 $int_script --source=$ref_src_file --input=$ref_in_file");
    // Porovnani vysledku s ocekavanym vysledkem
    compare_var_file($output, $ref_out_file);
    // Porovnani hodnoty navratoveho kodu s ocekavanym navratovym kodem
    $output_rc = shell_exec("python3.6 $int_script --source=$ref_src_file --input=$ref_in_file >/dev/null 2>&1; echo $?");
    compare_var_file($output_rc, $ref_rc_file);

}

// Komplexni kontrola funkcnosti, zapojeni a spoluprace obou skriptu parse.php i interpret.py
function both($parse_script, $int_script, $ref_src_file, $ref_out_file, $ref_rc_file, $ref_in_file) {
    $output_parse = shell_exec("php7.3 $parse_script < \"$ref_src_file\" > output_temp");
    $output_int = shell_exec("python3.6 $int_script --source=output_temp --input=$ref_in_file");
    compare_var_file($output_int, $ref_out_file);
    $output_rc = shell_exec("python3.6 $int_script --source=output_temp --input=$ref_in_file >/dev/null 2>&1; echo $?");
    compare_var_file($output_rc, $ref_rc_file);
    // Odstraneni docasneho souboru
    $output_rm = shell_exec("rm output_temp");
}

// Kontroluje zda konec souboru obsahuje zadany substring
function endsWith($string, $ending) {
    $length = strlen($ending);
    if ($length == 0) {
        return true;
    }

    return (substr($string, -$length) === $ending);
}

// Inkrementuje pocitadlo uspesnych testu
function result_inc_test_ok() {
    global $result_test_ok;
    $result_test_ok += 1;
}

// Inkrementuje pocitadlo neuspesnych testu
function result_inc_test_nok() {
    global $result_test_nok;
    $result_test_nok += 1;
}

// Prida soubor do pole uspesnych testu
function result_append_test_ok($file) {
    global $result_array_ok;
    array_push($result_array_ok, $file);
}

// Prida soubor do pole neuspesnych testu
function result_append_test_nok($file) {
    global $result_array_nok;
    array_push($result_array_nok, $file);
}

# Zpracovani parametru
$longopts = array("help", "directory:", "recursive", "parse-script:", "int-script:", "parse-only", "int-only");
$options = getopt("", $longopts);

if ((sizeof($options) == 1) and array_key_exists("help", $options)) {
    print_help();
    exit(0);
} else if ((sizeof($options) > 1) and array_key_exists("help", $options)) {
    fwrite(STDERR, "Combination of the --help param with other params is not allowed!\n");
    exit(10);
} else if (array_key_exists("parse-only", $options) and array_key_exists("int-only", $options)) {
    fwrite(STDERR, "Combination of the --parse-only and --int-only params is not allowed!\n");
    exit(10);
}

// Adresar, ve kterem se hledaji testy
if (array_key_exists("directory", $options)) {
    $directory = $options["directory"];
} else {
    $directory = ".";
}
// Bool, zda se prochazi adresar rekurzivne
$recursive = array_key_exists("recursive", $options);

// Urceni cesty k souboru parse.php
if (array_key_exists("parse-script", $options)) {
    $parse_script = $options["parse-script"];
} else {
    $parse_script = "parse.php";
}

// Urceni cesty souboru k interpret.py
if (array_key_exists("int-script", $options)) {
    $int_script = $options["int-script"];
} else {
    $int_script = "interpret.py";
}

// Prepinace pro pouze parsovani nebo pouze interpretaci
$parse_only = array_key_exists("parse-only", $options);
$int_only = array_key_exists("int-only", $options);

// Globalni promenne pro uchovani statistik o vysledcich testu
global $result_test_ok;
$result_test_ok = 0;
global $result_array_ok;
$result_array_ok = array();
global $result_test_nok;
$result_test_nok = 0;
global $result_array_nok;
$result_array_nok = array();

// Kontrola, zda zadany adresar je validni
if(!is_dir($directory)) {
    fwrite(STDERR, "The directory is not valid!\n");
    exit(11);
}

function test($directory, $parse_script, $parse_only, $int_script, $int_only, $recursive) {

    // Prochazi se soubory v adresari
    foreach (scandir($directory) as $file) {
        // Rekurzivita
        if (is_dir($directory . "/" . $file . "/") and ($file != "." and $file != "..") and $recursive) {
            // Rekurzivni pruchod adresarem
            test($directory . "/" . $file . "/", $parse_script, $parse_only, $int_script, $int_only, $recursive);
        }

        $ref_src_file = "";
        $ref_in_file = "";
        $ref_out_file = "";
        $ref_rc_file = "";

        // Vypsani pouze .src souboru (ten tam musi byt vzdy)
        if(preg_match('/.*\.src$/', $file)) {
            $name = explode(".src", $file)[0];

            $ref_src_file = $directory . "/" . $name . ".src";

            // Dale se divame, jestli existuji ostatni soubory, pripadne je vytvorim podle zadani

            $ref_in_file = $directory . "/" . $name . ".in";
            // Vytvoreni prazdneho *.in souboru, pokud neexistuje
            if(!file_exists($ref_in_file)) {
                touch($ref_in_file);
            }

            $ref_out_file = $directory . "/" . $name . ".out";
            // Vytvoreni prazdneho *.out souboru, pokud neexistuje
            if(!file_exists($ref_out_file)) {
                touch($directory . "/" . $name . ".out");
            }

            $ref_rc_file = $directory . "/" . $name . ".rc";
            // Vytvoreni *.rc souboru s defaultni hodnotou, pokud neexistuje
            if(!file_exists($ref_rc_file)) {
                touch($ref_rc_file);
                file_put_contents($ref_rc_file, "0");
            }
        }

        if(!endsWith($file, ".src")) {
            continue;
        }

        if ($parse_only) {
            parse_only($parse_script, $ref_src_file, $ref_out_file, $ref_rc_file);
        } else if ($int_only) {
            int_only($int_script, $ref_src_file, $ref_out_file, $ref_in_file, $ref_rc_file);
        } else {
            // Cely proces
            both($parse_script, $int_script, $ref_src_file, $ref_out_file, $ref_rc_file, $ref_in_file);
        }
    }
}

test($directory, $parse_script, $parse_only, $int_script, $int_only, $recursive);

echo create_html_report();
