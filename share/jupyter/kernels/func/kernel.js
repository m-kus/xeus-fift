// Based on https://github.com/codemirror/CodeMirror/blob/master/mode/clike/clike.js
define([
    'codemirror/lib/codemirror', 
    'codemirror/addon/mode/simple'
], function(CodeMirror) {
    
    function Context(indented, column, type, info, align, prev) {
        this.indented = indented;
        this.column = column;
        this.type = type;
        this.info = info;
        this.align = align;
        this.prev = prev;
    }
    function pushContext(state, col, type, info) {
        var indent = state.indented;
        if (state.context 
            && state.context.type == "statement" 
            && type != "statement") 
        {
            indent = state.context.indented;
        }            
        return state.context = new Context(indent, col, type, info, null, state.context);
    }

    function popContext(state) {
        var t = state.context.type;
        if (t == ")" || t == "}") {
            state.indented = state.context.indented;
        }
        return state.context = state.context.prev;
    }
      
    function typeBefore(stream, state, pos) {
        if (state.prevToken == "variable" || state.prevToken == "type") {
            return true;
        }
        if (/\S(?:[^- ]>|[*\]])\s*$|\*$/.test(stream.string.slice(0, pos))) { 
            return true;
        } 
        if (state.typeAtEndOfLine && stream.column() == stream.indentation()) {
            return true;
        }
    }
      
    function isTopScope(context) {
        for (;;) {
            if (!context || context.type == "top") {
                return true;
            }
            if (context.type == "}") {
                return false;
            }
            context = context.prev;
        }
    }

    function words(str) {
        var obj = {}, words = str.split(" ");
        for (var i = 0; i < words.length; ++i) {
            obj[words[i]] = true;
        }
        return obj;
    };

    function contains(words, word) {
        if (typeof words === "function") {
          return words(word);
        } else {
          return words.propertyIsEnumerable(word);
        }
    };
      
    CodeMirror.defineMode("func", function(config, parserConfig) {
        var indentUnit = config.indentUnit,
            statementIndentUnit = parserConfig.statementIndentUnit || indentUnit,
            dontAlignCalls = parserConfig.dontAlignCalls,
            keywords = parserConfig.keywords || {},
            types = parserConfig.types || {},
            builtin = parserConfig.builtin || {},
            blockKeywords = parserConfig.blockKeywords || {},
            defKeywords = parserConfig.defKeywords || {},
            atoms = parserConfig.atoms || {},
            hooks = parserConfig.hooks || {},
            multiLineStrings = parserConfig.multiLineStrings,
            indentStatements = parserConfig.indentStatements !== false,
            indentSwitch = parserConfig.indentSwitch !== false,
            namespaceSeparator = parserConfig.namespaceSeparator,
            isPunctuationChar = parserConfig.isPunctuationChar || /[\[\]{}\(\),;\:\.]/,
            numberStart = parserConfig.numberStart || /[\d\.]/,
            number = parserConfig.number || /^(?:0x[a-f\d]+|0b[01]+|(?:\d+\.?\d*|\.\d+)(?:e[-+]?\d+)?)(u|ll?|l|f)?/i,
            isOperatorChar = parserConfig.isOperatorChar || /[+\-*&%=<>!?|\/]/,
            isIdentifierChar = parserConfig.isIdentifierChar || /[\w\$_\xa1-\uffff]/,
            // An optional function that takes a {string} token and returns true if it
            // should be treated as a builtin.
            isReservedIdentifier = parserConfig.isReservedIdentifier || false;
      
        var curPunc, isDefKeyword;
      
        function tokenBase(stream, state) {
            var ch = stream.next();
            if (hooks[ch]) {
                var result = hooks[ch](stream, state);
                if (result !== false) {
                    return result;
                }
            }
            if (ch == '"') {
                state.tokenize = tokenString(ch);
                return state.tokenize(stream, state);
            }
            if (isPunctuationChar.test(ch)) {
                curPunc = ch;
                return null;
            }
            if (stream.match(number)) {
                return "number"
            }
            if (ch == ";" && stream.eat(";")) {
                stream.skipToEnd();
                return "comment";
            }
            if (ch == ":" && stream.eat(":")) {
                state.tokenize = tokenMethod(stream.current());
                return "variable";
            }
            if (isOperatorChar.test(ch)) {
                while (!stream.match(/^\/[\/*]/, false) 
                       && stream.eat(isOperatorChar)) {}
                return "operator";
            }

            stream.eatWhile(isIdentifierChar);
            var cur = stream.current();

            if (contains(keywords, cur)) {
                if (contains(blockKeywords, cur)) {
                    curPunc = "newstatement";
                }
                if (contains(defKeywords, cur)) { 
                    isDefKeyword = true;
                }
                return "keyword";
            }
            if (contains(types, cur)) {
                return "type";
            }
            if (contains(builtin, cur)) {
                return "builtin";
            }
            if (contains(atoms, cur)) {
                return "atom";
            }
            return "variable";
        }
      
        function tokenString(quote) {
            return function(stream, state) {
                var escaped = false, next, end = false;
                while ((next = stream.next()) != null) {
                    if (next == quote && !escaped) {
                        end = true; break;
                    }
                    escaped = !escaped && next == "\\";
                }
                if (end || !(escaped || multiLineStrings)) {
                    state.tokenize = null;
                }
                return "string";
            };
        }
           
        function maybeEOL(stream, state) {
            if (parserConfig.typeFirstDefinitions 
                && stream.eol() 
                && isTopScope(state.context))
            {
                state.typeAtEndOfLine = typeBefore(stream, state, stream.pos)
            }
        }
      
        return {
          startState: function(basecolumn) {
            return {
              tokenize: null,
              context: new Context((basecolumn || 0) - indentUnit, 0, "top", null, false),
              indented: 0,
              startOfLine: true,
              prevToken: null
            };
          },
          token: function(stream, state) {
                var ctx = state.context;
                if (stream.sol()) {
                    if (ctx.align == null) {
                        ctx.align = false;
                    }
                    state.indented = stream.indentation();
                    state.startOfLine = true;
                }
                if (stream.eatSpace()) { 
                    maybeEOL(stream, state); 
                    return null; 
                }
                curPunc = null;
                var style = (state.tokenize || tokenBase)(stream, state);
                if (style == "comment" || style == "meta") {
                    return style;
                }
                if (ctx.align == null) {
                    ctx.align = true;
                }
        
                if (curPunc == ";"
                    || (curPunc == "," && stream.match(/^\s*(?:;;.*)?$/, false)))
                {
                    while (state.context.type == "statement") {
                        popContext(state);
                    }
                } else if (curPunc == "{") {
                    pushContext(state, stream.column(), "}");
                } else if (curPunc == "(") {
                    pushContext(state, stream.column(), ")");
                } else if (curPunc == "}") {
                    while (ctx.type == "statement") {
                        ctx = popContext(state);
                    }
                    if (ctx.type == "}") {
                        ctx = popContext(state);
                    }
                    while (ctx.type == "statement") {
                        ctx = popContext(state);
                    }
                } else if (curPunc == ctx.type) {
                    popContext(state);
                } else if (((ctx.type == "}" || ctx.type == "top") && curPunc != ";") 
                         || (ctx.type == "statement" && curPunc == "newstatement")) 
                {
                    pushContext(state, stream.column(), "statement", stream.current());
                }
             
                state.startOfLine = false;
                state.prevToken = style || curPunc;
                maybeEOL(stream, state);
                return style;
          },
          indent: function(state, textAfter) {
            if (state.tokenize != tokenBase 
                && state.tokenize != null 
                || state.typeAtEndOfLine) 
            {
                return CodeMirror.Pass;
            }
            var ctx = state.context;
            var firstChar = textAfter && textAfter.charAt(0);
            var closing = firstChar == ctx.type;

            if (ctx.type == "statement" && firstChar == "}") {
                ctx = ctx.prev;
            }
            if (parserConfig.dontIndentStatements) {
                while (ctx.type == "statement" 
                       && parserConfig.dontIndentStatements.test(ctx.info)) 
                {
                    ctx = ctx.prev;
                }
            }
            if (parserConfig.allmanIndentation && /[{(]/.test(firstChar)) {
                while (ctx.type != "top" && ctx.type != "}") {
                    ctx = ctx.prev;
                }
                return ctx.indented;
            }
            if (ctx.type == "statement") {
                return ctx.indented + (firstChar == "{" ? 0 : statementIndentUnit);
            }
            if (ctx.align && (!dontAlignCalls || ctx.type != ")")) {
                return ctx.column + (closing ? 0 : 1);
            }
            if (ctx.type == ")" && !closing) {
                return ctx.indented + statementIndentUnit;
            }
            return ctx.indented + (closing ? 0 : indentUnit);
          },
          electricInput: /^\s*[{}]$/,
          lineComment: ";;",
          fold: "brace"
        };
      });

    
    let modifiers_str = "extern asm impure inline inline_ref method_id operator infix infixl infixr";
    let block_keywords_str = "repeat do if ifnot then else elseif elseifnot";
    let modifiers = words(modifiers_str);
    let types = words("int cell slice builder cont tuple _");
    let builtin = words("divmod moddiv muldivr muldiv muldivmod null? throw throw_if throw_unless load_int " + 
                        "load_uint preload_int preload_uint store_int store_uint load_bits preload_bits int_at " + 
                        "cell_at slice_at tuple_at at touch touch2 dump run_method0 run_method1 run_method2 run_method3 " + 
                        "~divmod ~moddiv ~store_int ~store_uint ~touch ~touch2");

    let tokenMethod = function(delim) {
        return function(stream, state) {
            stream.eatWhile(/[^\s\(]/);
            state.tokenize = null;
            var cur = stream.current();
            if (delim == "~") {
                cur = delim + cur;
            }
            if (contains(builtin, cur)) {
                return "builtin";
            } else {
                return "string-2";
            }
        }
    }

    let method_hook = function(stream, state) {
        if (state.prevToken == "variable" 
            && stream.match(/^[^\s\(]+/, false)) 
        {
            stream.eat(".");
            state.tokenize = tokenMethod(stream.current());
            return "operator";
        }
        return false;
    }

    let ns_hook = function(stream, state) {
        if (state.prevToken == ":" 
            && stream.match(/^[^\s\(]+/, false)) 
        {
            stream.eat(":");
            state.tokenize = tokenMethod(stream.current());
            return "variable";
        }
        return false;
    }

    var onload = function() {
        CodeMirror.defineMIME("text/x-func", {
            name: "clike",
            keywords: words("#include var return while until forall type " + block_keywords_str + " " + modifiers_str),
            types: types,
            builtin: builtin,
            blockKeywords: words(block_keywords_str),
            defKeywords: words("forall type"),
            dontIndentStatements: /^;;.*$/,
            typeFirstDefinitions: true,
            atoms: words("true false nil Nil main"),
            number: /^(?:-?0x[a-f\d]+)|(?:-?\d+)/i,
            isOperatorChar: /[+\-*&%=<>!?|\/~\^]/,
            isIdentifierChar: /[_\w?]/,
            hooks: {
                token: function(stream, _state, style) {
                    if (style == "keyword" && contains(modifiers, stream.current())) {
                        return "meta";
                    }
                    if (style == "type" && contains(types, stream.current())) {
                        return "builtin";
                    }
                },
                ".": method_hook,
                "~": method_hook,
                ":": ns_hook,
                ";": function(stream, _state) {
                    if (!stream.eat(";")) {
                        return false;
                    }
                    stream.skipToEnd();
                    return "comment";
                },
            },
            modeProps: {
                lineComment: ";;",
            }
        });
        CodeMirror.modeInfo.push({
            ext: ["fc"],
            mime: "text/x-func",
            mode: "clike",
            name: "FunC"
        });
    }
    return { onload: onload }
});