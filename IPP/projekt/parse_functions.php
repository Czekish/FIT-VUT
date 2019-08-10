<?php

// Pole vsech operacnich kodu jazyka IPPcode19
$instructions = array
    (
        "MOVE" => array("var", "symb"),
        "CREATEFRAME" => array(),
        "PUSHFRAME" => array(),
        "POPFRAME" => array(),
        "DEFVAR" => array("var"),
        "CALL" => array("label"),
        "RETURN" => array(),
        "PUSHS" => array("symb"),
        "POPS" => array("var"),
        "ADD" => array("var", "symb", "symb"),
        "SUB" => array("var", "symb", "symb"),
        "MUL" => array("var", "symb", "symb"),
        "IDIV" => array("var", "symb", "symb"),
        "LT" => array("var", "symb", "symb"),
        "GT" => array("var", "symb", "symb"),
        "EQ" => array("var", "symb", "symb"),
        "AND" => array("var", "symb", "symb"),
        "OR" => array("var", "symb", "symb"),
        "NOT" => array("var", "symb"),
        "INT2CHAR" => array("var", "symb"),
        "STRI2INT" => array("var", "symb", "symb"),
        "READ" => array("var", "type"),
        "WRITE" => array("symb"),
        "CONCAT" => array("var", "symb", "symb"),
        "STRLEN" => array("var", "symb"),
        "GETCHAR" => array("var", "symb", "symb"),
        "SETCHAR" => array("var", "symb", "symb"),
        "TYPE" => array("var", "symb"),
        "LABEL" => array("label"),
        "JUMP" => array("label"),
        "JUMPIFEQ" => array("label", "symb", "symb"),
        "JUMPIFNEQ" => array("label", "symb", "symb"),
        "EXIT" => array("symb"),
        "DPRINT" => array("symb"),
        "BREAK" => array(),
    );

// Funkce prijima jako parametr prvni radek zdrojoveho kodu a diva se, zda obsahuje pozadovanou hlavicku .IPPcode19
function check_header($first_line) {
    // Oddeleni pripadneho komentare od hlavicky
    $header = explode("#", $first_line);
    // Pridani komentare do statistik
    if(count($header) > 1) num_comments_inc();
    // Oriznuti bilych znaku od hlavicky
    $header = trim($header[0]);
    // Kontrola, zda hlavicka souboru splnuje zadani
    if(strtoupper($header) != ".IPPCODE19") {
        fwrite(STDERR, "Wrong or missing header in the source code!\n");
        exit(21);
    }
}

// Odstraneni komentaru ze zdrojoveho kodu
// Param $line = radek zdrojoveho kodu
function remove_comments($line) {

    $comment_found = false;

    $line_count = count($line);

    for($index = 0; $index < $line_count; $index++) {

        if($line[$index][0] == "#") {
            $comment_found = true;
            num_comments_inc();
        }

        if($comment_found) {
            unset($line[$index]);
        }
    }
    return array_values($line);
}

// Zjisti, zda se jedna o validni operacni kod.
// Param $opcode = operacni kod ze zdrojoveho souboru
// Param $list_opcodes = list vsech validnich operacnich kodu
function check_opcode($opcode, $list_opcodes) {
    if(!array_key_exists(strtoupper($opcode), $list_opcodes)) {
        // Kontrola, zda se nejedna o komentar na novem radku
        if($opcode[0] == "#") {
            // Nasel se komentar
        }
        else {
            fwrite(STDERR, "Unknown or wrong operation code!\n");
            exit(22);
        }
    } else {
        // Pocitani skokovych instrukci a navesti - pro rozsireni
        switch (strtoupper($opcode)) {
            case "LABEL":
                num_labels_inc();
                break;
            case "JUMP":
            case "JUMPIFEQ":
            case "JUMPIFEQS":
            case "JUMPIFNEQ":
            case "JUMPIFNEQS":
                num_jumps_inc();
                break;
        }
    }
}

// Kontrola, zda promenna ve zdrojovem kodu je lexikalne-syntakticky spravna
function check_var($var) {
    $var_regex = "/^(LF|TF|GF)@[a-zA-Z_\-$&%*!?]+[0-9a-zA-Z_\-$&%!?]*$/";
    return preg_match($var_regex, $var, $match);
}

// Kontrola, zda konstanta ve zdrojovem kodu je lexikalne-syntakticky spravna
function check_const($const) {
    $const_nil = "/^nil@nil$/";
    $const_bool = "/^bool@(false|true)$/";
    $const_number = "/^int@(\+|-)?\d+$/";
    $const_string = "/^string@(?!.*((\\\\\d[^\d])|(#)|(\\\\\d\d[^\d])|(\\\\[^\d])).*).*$/";
    return  preg_match($const_nil, $const, $match) or
            preg_match($const_bool, $const, $match) or
            preg_match($const_number, $const, $match) or
            preg_match($const_string, $const, $match);
}

// Kontrola, zda symbol ve zdrojovem kodu je lexikalne-syntakticky spravny
function check_symb($symb) {
    // Muze jit o promennou nebo konstantu
    return check_var($symb) or check_const($symb);
}

// Kontrola, zda label ve zdrojovem kodu je lexikalne-syntakticky spravny
function check_label($label) {
    $label_regex = "/^[a-zA-Z_\-$&%*!?]+[0-9a-zA-Z_\-$&%!?]*$/";
    return preg_match($label_regex, $label, $match);
}

// Kontrola, zda type ve zdrojovem kodu je lexikalne-syntakticky spravny
function check_type($type) {
    $type_regex = "/^(string|int|bool)$/";
    return preg_match($type_regex, $type, $match);
}

// Funkce prebira argumenty za operacnim kodem a vytvari vystupni XML
function xml_create_arg(DOMDocument $xml, DOMElement $xml_instruction, $arg_order, $arg_value, $arg_type) {
    if($arg_type != "symb") {
        $xml_arg = $xml->createElement("arg".$arg_order, htmlspecialchars($arg_value));
        $xml_instruction->appendChild($xml_arg);
        $xml_arg->setAttribute("type", $arg_type);
    } else {
        $instruction_prefix_postfix = explode("@", $arg_value);

        if(in_array($instruction_prefix_postfix[0], array("nil","bool","int","string"))) {
            $xml_arg = $xml->createElement("arg".$arg_order, htmlspecialchars($instruction_prefix_postfix[1]));
            $xml_instruction->appendChild($xml_arg);
            $xml_arg->setAttribute("type", $instruction_prefix_postfix[0]);
        } else {
            $xml_arg = $xml->createElement("arg".$arg_order, htmlspecialchars($arg_value));
            $xml_instruction->appendChild($xml_arg);
            $xml_arg->setAttribute("type", "var");
        }
    }
}

// Funkce vytvori XML instrukci s doplnenym operacnim kodem a jejim poradim
function xml_create_instruction(DOMDocument $xml, DOMElement $xml_program, $op_code, $instruction_counter) {
    $xml_instruction = $xml->createElement("instruction");
    $xml_program->appendChild($xml_instruction);
    $xml_instruction->setAttribute("order", $instruction_counter);
    $xml_instruction->setAttribute("opcode", $op_code);
    return $xml_instruction;
}

// Funkce kontroluje lexikalni-syntaktickou spravnost operandu za operacnim kodem, spousti se pro kazdy radek
// zdrojoveho kodu
function check_operands($instruction_line, $instructions, DOMDocument $xml, DOMElement $xml_instruction) {
    // Nacteni atributu konkretni instrukce do pole
    $attribut_array = $instructions[strtoupper($instruction_line[0])];

    if (count($attribut_array) != (count($instruction_line) - 1)) {
        // chyba
        fwrite(STDERR, "Number of param err!\n");
        exit(23);
    }

    for ($counter = 0; $counter < count($attribut_array); $counter++) {
        switch($attribut_array[$counter]) {
            // Typ parametru var
            case "var":
                if(!check_var($instruction_line[$counter + 1])) {
                    fwrite(STDERR, "Not the correct variable name!\n");
                    exit(23);
                } else {
                    xml_create_arg($xml, $xml_instruction,($counter+1), $instruction_line[$counter+1], $attribut_array[$counter]);
                }
                break;
            // Typ parametru symb
            case "symb":
                if(!check_symb($instruction_line[$counter + 1])) {
                    fwrite(STDERR, "Not the correct symb name!\n");
                    exit(23);
                } else {
                    xml_create_arg($xml, $xml_instruction,($counter+1), $instruction_line[$counter+1], $attribut_array[$counter]);
                 }
                break;
            // Typ parametru label
            case "label":
                if(!check_label($instruction_line[$counter + 1])) {
                    fwrite(STDERR, "Not the correct label name!\n");
                    exit(23);
                } else {
                    xml_create_arg($xml, $xml_instruction,($counter+1), $instruction_line[$counter+1], $attribut_array[$counter]);
                }
                break;
            // Typ parametru type
            case "type":
                if(!check_type($instruction_line[$counter + 1])) {
                    fwrite(STDERR, "Not the correct type name!\n");
                    exit(23);
                } else {
                    xml_create_arg($xml, $xml_instruction,($counter+1), $instruction_line[$counter+1], $attribut_array[$counter]);
                }
                break;
        }
    }
}

// Funkce pro rozsireni - inkrementuje radky kodu
function num_loc_inc() {
    global $num_loc;
    $num_loc += 1;
}

// Funkce pro rozsireni - inkrementuje komentare
function num_comments_inc() {
    global $num_comments;
    $num_comments += 1;
}

// Funkce pro rozsireni - inkrementuje navesti
function num_labels_inc() {
    global $num_labels;
    $num_labels +=1 ;
}

// Funkce pro rozsireni - inkrementuje radky kodu
function num_jumps_inc() {
    global $num_jumps;
    $num_jumps += 1;
}

// Vypis napovedy po zadani parametru --help
function print_help() {
    echo "Tento skript typu filtr nacte ze standardniho vstupu zdrojovy kod v jazyce IPPcode19, zkontroluje lexikalni a syntaktickou spravnost kodu a vypise na standardni vystup XML reprezentaci programu.\n";
    echo "Parametry:\n --help - Skript vypise napovedu. Tento parametr se nesmi kombinovat s zadnym jinym.\n";
    echo "Navratove kody:\n 0 - Skript probehl bez problemu.\n 21 - Chybna nebo chybejici chlavicka ve zdrojovem kodu zapsanem v IPPcode19.\n 22 - Neznamy nebo chybny operacni kod ve zdrojovem kodu zapsanem v IPPcode19.\n 23 - Jina lexikalni nebo syntakticka chyba zdrojoveho kodu zapsaneho v IPPcode19.\n 10 - Kombinace parametru --help s jinym parametrem.\n";
}