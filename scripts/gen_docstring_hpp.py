import requests
import re
from collections import defaultdict
from os.path import dirname, join
from TexSoup import TexSoup, TexCmd, TexEnv, TexNode, RArg

project_dir = dirname(dirname(__file__))


def read_file(rel_path):
    with open(join(project_dir, rel_path)) as f:
        return f.read()


def write_file(rel_path, data):
    with open(join(project_dir, rel_path), 'w+') as f:
        return f.write(data)


def escape(s):
    return s.replace('\\', '\\\\') \
            .replace('"', '\\"') \
            .replace('\n', '\\n')


def expand_symbols(s):
    symbols = {
        # symbols
        r'\\lfloor\s?': ' ⌊',
        r'\s?\\rfloor': '⌋ ',
        r'\\lceil\s?': ' ⌈',
        r'\s?\\rceil': '⌉ ',
        r'\\langle\s?': '❬',
        r'\s?\\rangle': '❭',
        r'\\lsqbr': '[',
        r'\\rsqbr': ']',
        r'\\leq': '≤',
        r'\\geq': '≥',
        r'\\neq': '≠',
        r'\\approx': '≈',
        r'\\oplus': '⊕',
        r'\\max': 'max',
        r'\\min': 'min',
        r'\\dots': '…',
        r'\\ldots': '…',
        r'\\cdot': '⋅',
        r'\\Delta': 'Δ',
        r'\\vee': '∨',
        r'\\sgn': 'sgn',
        r'\\emptyset': '∅',
        r'\\leftarrow': '🡐',
        r'\\rightarrow': '🡒',
        r'\\circ': '◦',
        r'\\caret': '^',
        r'\\tilde c': 'ć',
        r'``': '“',
        r'\'\'': '”',
        r'\\b': ' ',
        r'\\,': '',
        r'igl\(': '(',
        r'igr\)': ')',
        r'\s\\Sha\s': ' SHA256 ',
        r'\\HashmapE': 'HashmapE',
        r'---': '—',
        r'--': '—',
        r'_1': '₁',
        r'_n': 'ₙ',
        r'_m': 'ₘ',
        r'_0': '₀',
        r'\^x': 'ˣ ',
        r'\^y': 'ʸ ',
        r'\^\{-y\}': '⁻ʸ ',
        r'\^\{y-1\}': 'ʸ⁻¹ ',
        r'\^z': 'ᶻ ',
        r'1/2': '½',
        # syntax
        r'\s([\.,;:])': '\\1',
        r'~': ' ',
        r'\\/': '',
        r'\\([\$\{\}%&])': '\\1',
        r'\s+': ' ',
        # commands
        r'\\textit\{([^\}]+)\}': '\\1',
        r'\\texttt\{([^\}]+)\}': '\\1',
        r'\{\\tt\s([^\}]+)\}': '\\1',
        # adapt for jupyter notebook
        r'(^[^\(]+\([^\)]+\)),\s([A-z])': lambda m: m.group(1) + '\n' + m.group(2).upper(),
    }
    for pattern, symbol in symbols.items():
        s = re.sub(pattern, symbol, s)
    assert '\\' not in s, s
    return s.strip()


def non_space(x):
    return x != ' '


def parse_token(token):
    if isinstance(token, str):
        return token
    elif isinstance(token, TexCmd):
        if token.name in {'tt', 'em', '/', 'ptref', 'item', 'cite', '/s', 'underline', 'texttt', 'footnote'}:
            return ''.join(map(parse_token, token.contents))
        elif token.name in {'lsqbr', 'rsqbr', 'dots', 'ldots'}:
            return f'\\{token.name}'
        else:
            assert len(list(token.contents)) == 0, token
            return token.name
    elif isinstance(token, RArg):
        return ''.join(map(parse_token, token.contents))
    elif isinstance(token, TexEnv):
        return ''.join(map(parse_token, token.contents))
    elif isinstance(token, TexNode):
        return ' '.join(filter(non_space, map(parse_token, token.tokens)))
    else:
        assert False, token


def parse_word(rarg):
    if not isinstance(rarg, RArg):
        return None
    
    contents = list(rarg.contents)
    assert isinstance(contents[0], TexCmd) and contents[0].name == 'tt'

    i = next(i + 1 for i, c in enumerate(contents[1:]) if c != ' ')
    
    if isinstance(contents[i], TexCmd) and contents[i].name == 'underline':
        res = contents[i].tokens
    else:
        assert 'underline' not in str(rarg), rarg
        res = contents[i:]
    
    return ''.join(map(parse_token, res))


def get_word_definitions() -> list:
    fiftbase_tex = read_file('third-party/ton/doc/fiftbase.tex') 
    sections = re.split(r'\\clearpage', fiftbase_tex)
    assert sections[-1].startswith('\n\\appendix\\myappendix'), "Cannot locate Appendix"
    
    appendix = '\\begin{document}' + sections[-1].replace('[', '\\lsqbr ').replace(']', '\\rsqbr ')
    soup = TexSoup(appendix)
    items = soup.find_all('item')
    
    definitions = []
    for item in items:
        word = parse_word(next(iter(item.tokens)))
        if not word:
            continue
        
        definitions.append(dict(
            word=escape(expand_symbols(word)), 
            definition=escape(expand_symbols(parse_token(item)))
        ))

    return definitions


def parse_instr(line):
    main = re.findall(r'--- \{\\tt ([A-Z0-9-]+)', line)
    alt = re.findall(r'\} or \{\\tt ([A-Z0-9-]{2,})', line)
    aka = re.findall(r'also known as \{\\tt ([A-Z0-9-]+)', line)
    return main + alt + aka


def get_asm_definitions() -> list:
    tvm_tex = read_file('third-party/ton/doc/tvm.tex')
    lines = tvm_tex.split('\n')
    lines = filter(lambda x: re.match(r'\\item \{\\tt [A-Z0-9_a-z\\\$]+\} --- \{\\tt', x), lines)
    
    definitions = defaultdict(list)
    for line in lines:
        item = next(iter(TexSoup(line)))
        definition = escape(expand_symbols(parse_token(item)))
        for instr in parse_instr(line):
            definitions[instr].append(definition)

    def make_res(x):
        return dict(
            word=x[0], 
            definition=escape('\n'.join(x[1]))
        )

    return list(map(make_res, definitions.items()))


def get_register_definitions() -> list:
    tvm_tex = read_file('third-party/ton/doc/tvm.tex')
    lines = tvm_tex.split('\n')
    lines = filter(lambda x: re.match(r'\\item \\texttt\{c[0-9]\}', x), lines)
    items = map(lambda x: next(iter(TexSoup(x))), lines)
    definitions = map(lambda x: escape(expand_symbols(parse_token(x))), items)
    return list(map(lambda x: dict(word=x.split()[0], definition=x), definitions))


def generate_docs():
    word_definitions = get_word_definitions()
    asm_definitions = get_asm_definitions()
    register_definitions = get_register_definitions()
    template = read_file('src/docstring.hpp.in')
    placeholder = next(line for line in template.split('\n') if line.endswith('item_template'))
    item_template = placeholder.rstrip('// item_template')
    docs = '\n'.join(map(lambda x: item_template.format(**x), word_definitions + asm_definitions + register_definitions))
    data = template.replace(placeholder, docs)
    write_file('src/docstring.hpp', data)


if __name__ == '__main__':
    generate_docs()
