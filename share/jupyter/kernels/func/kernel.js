define([
    'codemirror/lib/codemirror', 
    'codemirror/addon/mode/simple'
], function(CodeMirror) {
    var onload = function() {
        CodeMirror.defineSimpleMode("func", {
            start: [
                // strings
                { regex: /"(?:[^\\]|\\.)*?(?:"|$)/, token: "string" },
                // comments
                { regex: /;;.*/, token: "comment" },
                // keywords
                { regex: /\b(?:int|cell|slice|builder|cont|tuple)\b/, token: "builtin"},
                { regex: /\b(?:return|var|repeat|do|while|until|if|ifnot|then|else|elseif|elseifnot)\b/, token: "keyword"},
                { regex: /\b(?:extern|asm|impure|inline|inline_ref|method_id|operator|infix|infixl|infixr)\b/, token: "meta"},
                // fallback
                { regex: /[^\s]+/, token: "variable"},
            ],
            meta: {
                dontIndentStates: ["comment"],
                lineComment: ";;",
            }
        });
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