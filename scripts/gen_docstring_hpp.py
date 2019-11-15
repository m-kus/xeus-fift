import requests
import re
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
        # commands
        r'\\textit\{([^\}]+)\}': '\\1',
        r'\\texttt\{([^\}]+)\}': '\\1',
        r'\{\\tt\s([^\}]+)\}': '\\1',
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
        # adapt for jupyter notebook
        r'(^[^\(]+\([^\)]+\)),\s([A-z])': lambda m: m.group(1) + '\n' + m.group(2).upper(),
    }
    for pattern, symbol in symbols.items():
        s = re.sub(pattern, symbol, s)
    assert '\\' not in s, s
    return s.strip()


def parse_token(token):
    if isinstance(token, str):
        return token.strip()
    elif isinstance(token, TexCmd):
        if token.name in {'tt', 'em', '/', 'ptref', 'item', 'cite', '/s', 'underline'}:
            return ''.join(map(parse_token, token.contents))
        elif token.name in {'lsqbr', 'rsqbr', 'dots', 'ldots'}:
            return f'\\{token.name}'
        else:
            assert len(list(token.contents)) == 0, token
            return token.name.strip()
    elif isinstance(token, RArg):
        return ''.join(map(parse_token, token.contents))
    elif isinstance(token, TexEnv):
        return ''.join(map(parse_token, token.contents))
    elif isinstance(token, TexNode):
        return ' '.join(map(parse_token, token.tokens))
    else:
        assert False, token


def parse_word(rarg):
    if not isinstance(rarg, RArg):
        return None
    
    contents = list(rarg.contents)
    assert isinstance(contents[0], TexCmd) and contents[0].name == 'tt'
    
    if isinstance(contents[1], TexCmd) and contents[1].name == 'underline':
        res = contents[1].tokens
    else:
        res = contents[1:]
    
    return ''.join(map(parse_token, res))


def parse_item(item, word_offset=0):
    tokens = list(item.tokens)
    if len(tokens) <= word_offset:
        return None
    word = parse_word(tokens[word_offset])
    if not word:
        return None
    return dict(
        word=escape(expand_symbols(word)), 
        definition=escape(expand_symbols(parse_token(item)))
    )


def get_word_definitions() -> list:
    fiftbase_tex = read_file('third-party/ton/doc/fiftbase.tex') 
    sections = re.split(r'\\clearpage', fiftbase_tex)
    assert sections[-1].startswith('\n\\appendix\\myappendix'), "Cannot locate Appendix"
    appendix = '\\begin{document}' + sections[-1].replace('[', '\\lsqbr ').replace(']', '\\rsqbr ')
    soup = TexSoup(appendix)
    items = soup.find_all('item')
    return list(filter(lambda x: x, map(parse_item, items)))


def get_asm_definitions() -> list:
    tvm_tex = read_file('third-party/ton/doc/tvm.tex')
    lines = tvm_tex.split('\n')
    lines = filter(lambda x: re.match(r'\\item \{\\tt [A-Z0-9]+\} --- \{\\tt [A-Z]+\}', x), lines)
    items = list(map(lambda x: next(iter(TexSoup(x))), lines))
    return list(filter(lambda x: x, map(lambda i: parse_item(i, 2), items)))


def generate_docs():
    word_definitions = get_word_definitions()
    asm_definitions = get_asm_definitions()
    template = read_file('src/docstring.hpp.in')
    placeholder = next(line for line in template.split('\n') if line.endswith('item_template'))
    item_template = placeholder.rstrip('// item_template')
    docs = '\n'.join(map(lambda x: item_template.format(**x), word_definitions + asm_definitions))
    data = template.replace(placeholder, docs)
    write_file('src/docstring.hpp', data)


if __name__ == '__main__':
    generate_docs()
