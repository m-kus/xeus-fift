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

    var funcMode = function(config) {
        var blockKeywordsStr = "repeat do if ifnot then else elseif elseifnot";
        var blockKeywords = words(blockKeywordsStr);
        var types = words("int cell slice builder cont tuple _");
        var keywords = words("#include var return while until forall type " + blockKeywordsStr);
        var modifiers = words("extern asm impure inline inline_ref method_id operator infix infixl infixr");
        var atoms = words("true false nil Nil main");
        var builtins = words("divmod moddiv muldivr muldiv muldivmod null? throw throw_if throw_unless load_int " + 
                             "load_uint preload_int preload_uint store_int store_uint load_bits preload_bits int_at " + 
                             "cell_at slice_at tuple_at at touch touch2 dump run_method0 run_method1 run_method2 run_method3 " + 
                             "~divmod ~moddiv ~store_int ~store_uint ~touch ~touch2");


        var indentUnit = config.indentUnit;
        var curPunc = null;
      
        function tokenBase(stream, state) {
            var ch = stream.next();
            if (state.prevToken == "variable" || state.prevToken == ")") {
                if (ch == ":" && stream.eat(":")) {
                    state.tokenize = tokenMethod(ch, "string-2");
                    return "variable-2";
                }
                if (ch == "~") {
                    state.tokenize = tokenMethod(ch, "def");
                    return "def";
                }
                if (ch == ".") {
                    state.tokenize = tokenMethod(ch, "def");
                    return "operator";
                }
            }
            if (ch == ";" && stream.eat(";")) { 
                stream.skipToEnd();
                return "comment";
            }
            if (ch == '"') {
                state.tokenize = tokenString(ch);
                return state.tokenize(stream, state);
            }
            if (/[{}\(\),;\.]/.test(ch)) {
                curPunc = ch;
                return null;
            }
            if (stream.match(/^-?(?:0x[a-f\d]+)|(?:\d+)/i, true)) {
                return "number"
            }
            if (/[+\-*&%=<>!?|\/~\^]/.test(ch)) {
                stream.eatWhile(/[+\-*&%=<>!?|\/~\^]/);
                return "operator";
            }

            stream.eatWhile(/[_\w?=]/);
            var cur = stream.current();

            if (contains(modifiers, cur)) {
                return "meta";
            }
            if (contains(keywords, cur)) {
                if (contains(blockKeywords, cur)) {
                    curPunc = "newstatement";
                }
                return "keyword";
            }
            if (contains(types, cur)) {
                return "builtin"; // type
            }
            if (contains(builtins, cur)) {
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

        function tokenMethod(prefix, style) {
            return function(stream, state) {
                stream.eatWhile(/[^\s\(]/);
                state.tokenize = null;
                var cur = stream.current();
                if (prefix == "~") {
                    cur = prefix + cur;
                }
                if (contains(builtins, cur)) {
                    return "builtin";
                }
                return style;
            }
        }
      
        function startState(basecolumn) {
            return {
              tokenize: null,
              context: new Context((basecolumn || 0) - indentUnit, 0, "top", null, false),
              indented: 0,
              startOfLine: true,
              prevToken: null
            };
        }

        function token(stream, state) {
            var ctx = state.context;
            if (stream.sol()) {
                if (ctx.align == null) {
                    ctx.align = false;
                }
                state.indented = stream.indentation();
                state.startOfLine = true;
            }
            
            if (stream.eatSpace()) {
                return null; 
            }
            
            curPunc = null;
            
            var style = (state.tokenize || tokenBase)(stream, state);
            if (style == "comment") {
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
            return style;
        }

        function indent(state, textAfter) {
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
        
            if (ctx.type == "statement") {
                return ctx.indented + (firstChar == "{" ? 0 : indentUnit);
            }
            if (ctx.align) {
                return ctx.column + (closing ? 0 : 1);
            }
            if (ctx.type == ")" && !closing) {
                return ctx.indented + indentUnit;
            }
            return ctx.indented + (closing ? 0 : indentUnit);
        }
      
        return {
            startState: startState,
            token: token,
            indent: indent,
            electricInput: /^\s*[{}]$/,
            lineComment: ";;",
            fold: "brace"
        };
    }
  
    var onload = function() {
        CodeMirror.defineMode("func", funcMode);
        CodeMirror.defineMIME("text/x-func", "func");
        CodeMirror.modeInfo.push({
            ext: ["fc"],
            mime: "text/x-func",
            mode: "func",
            name: "FunC"
        });      
    }
    return { onload: onload }
});