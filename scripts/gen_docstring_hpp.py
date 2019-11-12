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


def join_contents(token):
    res = ''
    for content in token.contents:
        if isinstance(content, TexCmd):
            if content.name == 'tt':
                continue
            if content.name == 'underline':
                return join_contents(content)
            res += content.name
        elif isinstance(content, str):
            res += content
        else:
            break
    return res.strip()


def escape_tex(s):
    return s.replace('\\', '\\\\') \
            .replace('"', '\\"') \
            .replace('\n', '\\\\n')


def math_replace(expr):
    symbols = {
        r'\\b': '',
        r'\\lfloor\s?': '⌊',
        r'\\rfloor\s?': '⌋',
        r'\\langle\s?': '〈',
        r'\\rangle\s?': '〉',
        r'\\textit{(.+)}': '\\1',
        r'\\leq\s?': '≤',
        r'\\geq\s?': '≥',
        r'\\dots\s?': '…',
        r'\\ldots\s?': '…',
        r'\\lsqbr\s?': '[',
        r'\\rsqbr\s?': ']',
    }
    for cmd, symbol in symbols.items():
        expr = re.sub(cmd, symbol, expr)
    return expr


def filter_output(s):
    s = re.sub(r'\s([\.,;:])', '\\1', s)
    s = re.sub(r', cf\.[^\.]*', '', s)
    s = s.replace('~', ' ')
    s = s.replace('\\$', '$')
    s = s.replace('``', '“')
    s = s.replace('\'\'', '”')
    s = s.replace('\\', '')
    s = re.sub(r'\s+', ' ', s)
    return s.strip()

def expand_cmd(cmd):
    symbols = {
        'lsqbr': '[',
        'rsqbr': ']',
        'dots': '…',
        'ldots': '…',
        'tt': ' ',
    }
    if cmd in symbols:
        return symbols[cmd]
    else:
        assert cmd in {'tt', 'em', '/', 'ptref', '$', 'item', 'cite', '/s'}, cmd
        return ''


def join_tokens(tokens: list, delim=''):
    return delim.join(filter(lambda x: x, map(parse_token, tokens)))


def parse_token(token):
    if isinstance(token, str):
        return token.strip()
    elif isinstance(token, TexCmd):
        if token.name[0] in '#_{}':
            return token.name
        elif token.name == 'underline':
            return join_tokens(token.contents)
        else:
            return expand_cmd(token.name)
    elif isinstance(token, RArg):
        return join_tokens(token.contents)
    elif isinstance(token, TexEnv):
        return math_replace(join_tokens(token.contents))
    elif isinstance(token, TexNode):
        return filter_output(join_tokens(token.tokens, delim=' '))
    else:
        assert False, token


def parse_item(item):
    return dict(
        word=escape_tex(join_contents(next(item.tokens))), 
        definition=escape_tex(parse_token(item))
    )


def get_word_definitions() -> list:
    fiftbase_tex = read_file('third-party/ton/doc/fiftbase.tex') 
    sections = re.split(r'\\clearpage', fiftbase_tex)
    assert sections[-1].startswith('\n\\appendix\\myappendix'), "Cannot locate Appendix"
    appendix = '\\begin{document}' + sections[-1].replace('[', ' ').replace(']', ' ')  # TexSoup does not understand [ ]
    soup = TexSoup(appendix)
    items = soup.find_all('item')
    return list(map(parse_item, items))


def generate_docs():
    word_definitions = get_word_definitions()
    template = read_file('src/docstring.hpp.in')
    placeholder = next(line for line in template.split('\n') if line.endswith('item_template'))
    item_template = placeholder.rstrip('// item_template')
    docs = '\n'.join(map(lambda x: item_template.format(**x), word_definitions))
    data = template.replace(placeholder, docs)
    write_file('src/docstring.hpp', data)


if __name__ == '__main__':
    generate_docs()
