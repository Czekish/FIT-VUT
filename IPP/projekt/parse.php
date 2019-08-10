<?php
include_once("parse_functions.php");

# Zpracovani parametru
$longopts = array("help", "stats:", "loc", "comments", "labels", "jumps");
$options = getopt("", $longopts);

if ((sizeof($options) == 1) and array_key_exists("help", $options)) {
    print_help();
    exit(0);
} else if ((sizeof($options) > 1) and array_key_exists("help", $options)) {
    fwrite(STDERR, "Combination of the --help param with other params is not allowed!\n");
    exit(10);
} else if (array_key_exists("loc", $options) or array_key_exists("comments", $options) or array_key_exists("labels", $options) or array_key_exists("jumps", $options)) {
            if(!array_key_exists("stats", $options)) {
                fwrite(STDERR, "Missing parameter --stats!\n");
                exit(10);
            }
}

// Promenne pro rozsireni
global $num_loc;
$num_loc = 0;
global $num_comments;
$num_comments = 0;
global $num_labels;
$num_labels = 0;
global $num_jumps;
$num_jumps = 0;

# Kontrola prvniho radku programu
$line = trim(fgets(STDIN));
check_header($line);

// Vytvoreni XML souboru a uvodniho tagu <program>
$xml = new DOMDocument('1.0');
$xml->formatOutput = true;
$xml->encoding = 'UTF-8';
$xml_program = $xml->createElement("program");
$xml->appendChild($xml_program);
$xml_program->setAttribute("language", "IPPcode19");

// Pocitadlo instrukci
$instruction_counter = 1;

// Cyklus, ve kterem se nacitaji postupne jednotlive radky ze zdrojoveho programu
while(($line = fgets(STDIN)) !== false) {
    // Vlozeni mezery pred kazdy # - osetreni komentare, ktery je "nalepen" hned na instrukci
    $line = str_replace("#", " #", $line);
    // Oriznuti bilych znaku
    $line = trim($line);
    // Rozdeleni do pole podle bilych znaku
    $arr_line = preg_split("/\s+/", $line);
    // Osetreni prazdnych radku
    if($arr_line[0] == null) continue;
    // Odstraneni komentaru
    $arr_line = remove_comments($arr_line);
    // Nasel se celoradkovy komentar, jde se na dalsi instrukci
    if(count($arr_line) == 0) continue;
    // Inkrementace cisla LOC pro rozsireni
    num_loc_inc();
    // Vytvoreni Instrukce v XML
    $xml_instruction = xml_create_instruction($xml, $xml_program, $arr_line[0], $instruction_counter);
    // Kontrola operacniho kodu
    check_opcode($arr_line[0], $instructions);
    // Syntakticka kontrola parametru za instrukci
    check_operands($arr_line, $instructions, $xml, $xml_instruction);
    // Inkrementace pocitadla
    $instruction_counter++;
}
// Ulozeni a vypis XML na stdout
echo $xml->saveXML();

// Vypsani statistik do souboru - ROZSIRENI STATP
if (array_key_exists("stats", $options)) {
    if(($file = fopen($options["stats"], "w")) === false) {
        fwrite(STDERR, "The file couldn't be opened for writing!\n");
        exit(12);
    }
    // Diva se, ktere parametry rozsireni jsou zadane
    foreach ($options as $param => $value) {
        if ($param == "stats") continue;
        else if ($param == "loc") fwrite($file, $num_loc . "\n");
        else if ($param == "comments") fwrite($file, $num_comments . "\n");
        else if ($param == "labels") fwrite($file, $num_labels . "\n");
        else if ($param == "jumps") fwrite($file, $num_jumps . "\n");
    }

    fclose($file);
}

// Vse probehlo OK
exit(0);


