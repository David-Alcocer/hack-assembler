// =============================================================
//  Ensamblador Hack - Proyecto 7 (Misa y Leo)
//
//  Compilar:  g++ -std=c++17 -o assembler assembler.cpp
//  Usar:      ./assembler Add.asm      -> genera Add.hack en la misma carpeta
//  (en Windows: assembler.exe Add.asm)
//
//  YA ESTA HECHO: tablas, limpieza de lineas, lectura/escritura de archivos.
//  FALTA (TODO):  traducirC (Misa), primeraPasada y traducirA (Leo).
// =============================================================
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <cctype>
using namespace std;

// ===================== TABLAS (ya completas) =====================

// comp: 7 bits = bit "a" + c1..c6   (a=0 usa A, a=1 usa M)
const unordered_map<string, string> COMP = {
    // a = 0
    {"0",   "0101010"}, {"1",   "0111111"}, {"-1",  "0111010"},
    {"D",   "0001100"}, {"A",   "0110000"}, {"!D",  "0001101"},
    {"!A",  "0110001"}, {"-D",  "0001111"}, {"-A",  "0110011"},
    {"D+1", "0011111"}, {"A+1", "0110111"}, {"D-1", "0001110"},
    {"A-1", "0110010"}, {"D+A", "0000010"}, {"D-A", "0010011"},
    {"A-D", "0000111"}, {"D&A", "0000000"}, {"D|A", "0010101"},
    // a = 1
    {"M",   "1110000"}, {"!M",  "1110001"}, {"-M",  "1110011"},
    {"M+1", "1110111"}, {"M-1", "1110010"}, {"D+M", "1000010"},
    {"D-M", "1010011"}, {"M-D", "1000111"}, {"D&M", "1000000"},
    {"D|M", "1010101"},
    // variantes conmutativas (por si acaso)
    {"A+D", "0000010"}, {"M+D", "1000010"}, {"A&D", "0000000"},
    {"M&D", "1000000"}, {"A|D", "0010101"}, {"M|D", "1010101"},
    {"1+D", "0011111"}, {"1+A", "0110111"}, {"1+M", "1110111"},
};

// dest: 3 bits (A D M)
const unordered_map<string, string> DEST = {
    {"",    "000"}, {"M",   "001"}, {"D",   "010"}, {"MD",  "011"},
    {"A",   "100"}, {"AM",  "101"}, {"AD",  "110"}, {"AMD", "111"},
    // otros ordenes validos
    {"DM",  "011"}, {"MA",  "101"}, {"DA",  "110"}, {"ADM", "111"},
};

// jump: 3 bits
const unordered_map<string, string> JUMP = {
    {"",    "000"}, {"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"},
    {"JLT", "100"}, {"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"},
};

// Simbolos predefinidos: R0..R15, SP, LCL, ARG, THIS, THAT, SCREEN, KBD
unordered_map<string, int> crearTablaSimbolos() {
    unordered_map<string, int> t = {
        {"SP", 0}, {"LCL", 1}, {"ARG", 2}, {"THIS", 3}, {"THAT", 4},
        {"SCREEN", 16384}, {"KBD", 24576},
    };
    for (int i = 0; i < 16; i++) t["R" + to_string(i)] = i;
    return t;
}

// ===================== LIMPIEZA (ya hecha) =====================
// Quita comentarios (//...) y TODOS los espacios, tabs y \r.
// Devuelve "" si la linea queda vacia.
string limpiar(const string& linea) {
    string s = linea;
    size_t pos = s.find("//");
    if (pos != string::npos) s = s.substr(0, pos);

    string r;
    for (char c : s)
        if (!isspace((unsigned char)c)) r += c;
    return r;
}

// ===================== LEO: PRIMERA PASADA =====================
// Recorre las lineas y guarda cada etiqueta (XXX) en la tabla
// con la direccion ROM de la instruccion que viene despues.
void primeraPasada(const vector<string>& lineas, unordered_map<string, int>& tabla) {
    int rom = 0;
    for (const string& l : lineas) {
        if (l[0] == '(') {
            string nombre = l.substr(1, l.size() - 2); 
            tabla[nombre] = rom;
        } else {
            rom++;
        }
    }
}

// ===================== LEO: INSTRUCCION A =====================
// @valor  ->  0 + 15 bits de la direccion
string traducirA(const string& linea, unordered_map<string, int>& tabla, int& siguienteVariable) {
    string valor = linea.substr(1);
    int direccion = 0;

    if (isdigit((unsigned char)valor[0])) {
        direccion = stoi(valor);
    } else if (tabla.count(valor)) {
        direccion = tabla[valor];
    } else {
        tabla[valor] = siguienteVariable;
        direccion = siguienteVariable;
        siguienteVariable++;
    }
    return bitset<16>(direccion).to_string();
}

// ===================== MISA: INSTRUCCION C =====================
// dest=comp;jump   ->   111 + comp(7) + dest(3) + jump(3)
// Ejemplos:  D=A   (sin jump)    D;JGT  (sin dest)    AM=M-1
string traducirC(const string& linea) {
    string dest = "";
    string comp = linea;
    string jump = "";

    size_t igual = comp.find('=');
    if (igual != string::npos) {
        dest = comp.substr(0, igual);
        comp = comp.substr(igual + 1);
    }

    size_t pc = comp.find(';');
    if (pc != string::npos) {
        jump = comp.substr(pc + 1);
        comp = comp.substr(0, pc);
    }

    return "111" + COMP.at(comp) + DEST.at(dest) + JUMP.at(jump);
}

// ===================== MAIN  =====================
int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Uso: ./assembler Prog.asm\n";
        return 1;
    }
    string entrada = argv[1];
    ifstream in(entrada);
    if (!in) {
        cerr << "No se pudo abrir " << entrada << "\n";
        return 1;
    }

    // Leer y limpiar todas las lineas
    vector<string> lineas;
    string linea;
    while (getline(in, linea)) {
        string l = limpiar(linea);
        if (!l.empty()) lineas.push_back(l);
    }
    in.close();

    // Pasada 1: etiquetas
    unordered_map<string, int> tabla = crearTablaSimbolos();
    primeraPasada(lineas, tabla);

    // Pasada 2: traducir y escribir Prog.hack
    string salida = entrada.substr(0, entrada.rfind('.')) + ".hack";
    ofstream out(salida);
    int siguienteVariable = 16;

    for (const string& l : lineas) {
        if (l[0] == '(') continue;                     
        if (l[0] == '@') out << traducirA(l, tabla, siguienteVariable) << "\n";
        else             out << traducirC(l) << "\n";
    }

    out.close();
    cout << "Listo: " << salida << "\n";
    return 0;
}
