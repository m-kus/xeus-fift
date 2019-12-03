define([
    'codemirror/lib/codemirror', 
    'codemirror/addon/mode/simple'
], function(CodeMirror) {
    var builtin_words = /(?<=\s|^)(?:1<<|x\._|sbitrefs|ref@\?\+|bl|ufits|ref@\?|B>Li@\+|B>base64|srefs|anon|while|sr,|\$cmp|<<\/c|boc>B|count|B@\+|\*\/c|pfxdict!\+|>>|nip|%1<<|2drop|\/rmod|Lu>B|words|b\._|eq\?|remaining|<<\/r|runvmdict|<s|2\-|B>x|allot|dbrunvm\-parallel|\*mod|atom\?|hole|dbrunvm|i,|\$@\+|rmod|boc\+>B|\(x\.\)|hash|\$\||\$>smca|base64url>B|idict!\+|udict!\+|\.tc|B>Li@|dict@\+|untuple|sgn|idict!|\(number\)|\._|1<<1\-|base64>B|explode|udict!|u@\?\+|b>idict!|<xchg>|ref@|\*\/rmod|B>Lu@|xor|>>c|B>base64url|X\.|\[\]=|file>B|negate|b>idict!\+|x>B\?|b\+|<b|word|1\-|hashB|nop|0>=|dictmapext|\-trailing|eqv\?|brembitrefs|\*>>c|filepart>B|fits|>>r|b>spec|bbitrefs|\*>>r|until|\(forget\)|\$=|\$>s|2\*|\(dump\)|if|exch|\|\+|B>i@|<<\/|box|i@\?\+|abort|cr|gasrunvm|dictmap|forget|'|sbits|dictdiff|B>boc|execute|2swap|find|\(hex\-number\)|emit|\$>B|\*\/cmod|B\+|\.s|times|'nop|tuple\?|include|rot|@|string\?|\(execute\)|\/r|s>|B>\$|\$\+|bye|=|cmp|\.sl|u,|dictforeach|\*\/mod|i@\+|runvmcode|\.dump|runvm|B>u@\+|B>i@\+|b\{|cmod|B@\?|not|crc16|B>X|BhashB|tuple|reverse|newkeypair|:|<=|drop|i@|u@\+|\(create\)|\(\-trailing\)|\[\]|ed25519_sign|gasrunvmdict|\-|\||quit|\"|0=|Li>B|i>B|0>|\-rot|tpop|atom>\$|\/cmod|space|true|\|_|pfxdict!|\$Len|vmlibs|char|runvmctx|!|b\.|Bhash|\(word\)|\(char\)|\(b\.\)|1\+|:_|bbits|B@|dict>s|::|ifnot|x>B|u@|priv>pub|\-1<<|B,|Bhashu|\$@\?|getenv|ref@\+|file\-exists\?|\(\}\)|\/|b>udict!|\.|pick|and|x\{|\(\{\)|<|csr\.|x\.|Blen|skip\-to\-eof|create|\(\.\)|b>udict!\+|\+|null|0<|depth|>=|brembits|idict@|B>file|0<=|dup|2\+|\/mod|b>|\{|\$@\?\+|roll|dictmerge|\*\/r|<<|<push>|\}|type|\]|2\/|B>u@|,|ed25519_chksign|swap|crc32c|\$len|i@\?|\*>>|getenv\?|s,|u@\?|\[|smca>\$|2over|brefs|\(atom\)|pfxdict@|gasrunvmcode|dictnew|box\?|\$Split|::_|or|<pop>|X\._|\/c|now|0|>|Bx\.|<>|2dup|0<>|empty\?|ed25519_sign_uint|1|\-roll|mod|hold|integer\?|\.l|gasrunvmctx|skipspc|B\||over|u>B|\$,|halt|\-trailing0|B@\?\+|Bcmp|false|2|bremrefs|\(compile\)|\$@|\*|tuck|B>Lu@\+|hashu|\-1|chr|crc32|null\?|udict@|B=|ref,|cond|dict,|\$reverse|exch2|\(X\.\)|\?dup|dict@|\*\/)(?=\s|$)/;
    var fift_words = /(?<=\s|^)(?:\[compile\]|\#|sign|@'|X\#|\/\*|undef\?|binary|abs|tuple\-len\?|unpair|0x\.|recursive|constant|\/\/|0!|nil\?|first|nil|\.\"|library|Digit|abort\"|second|s\-fits\?|cddr|\$\(|null!|base\#s|X\#s|decimal|does|0x\._|1\+!|list|digit|octal|b\#s|atom|1\-!|\+!|cons|untriple|library\-version|\#>|b\#|base\#|x\#s|unsingle|\('\)|\[forget\]|\(0X\.\)|car|uncons|skip\-ifdef|2=:|caddr|shash|def\?|0X\._|`|min|ten|Base\#s|B\{|pair\?|B\#|2constant|single\?|base|cadr|minmax|triple|\(0x\.\)|max|<\#|pair|single|\(def\?\)|0X\.|s>c|cdr|\-!|B\#s|third|=:|\+\"|hex|Base\#|variable|\#s|triple\?|x\#)(?=\s|$)/;
    var asm_words = /(?<=\s|^)(?:DICTMINREF|MIN|POPSAVE|DUP2|DICTIADDGET|ONLYX|QMOD|PLDILE4Q|PARSEMSGADDR|LDDICTS|CALLXARGS|DICTISET|PLDREF|STBQ|MODPOW2\#|@Defop\(8u\+1\)|@addopb|IFNBITJMPREF|MINMAX|DICTADDGET|PUSHCONT|SETFIRSTQ|ISNNEG|XCHG0|QDIVMODC|s3|DICTISETGET|@ensurebits|LDILE4Q|DICTISETGETOPTREF|DICTIDELGETREF|STI|BCHKBITS\#|LDONES|UNPACKFIRSTVAR|PFXDICTGETQ|@bigsridx|2DROP|DICTIMINREF|LSHIFT\#DIVR|TRY|NIL|JMPREFDATA|STOPTREF|DUP|@newproc|DROP|@procdict|@Defop\(8i,alt\)|ADDINT|CHKDEPTH|PLDREFVAR|CDRQ|BLESSARGS|STREFCONST|AGAIN:|@Sreg|SBITREFS|XCHG3_l|ISPOS|DICTUADDB|LDULE4|XCPU2|MULDIVR|CADR|STIRQ|SLICE|AGAINEND|LDIXQ|LDSLICEXQ|PFXDICTSWITCH|XCPUXC|LDSLICEQ|COMMA|SSKIPFIRST|SUBDICTGET|UNTRIPLE|MOD|PUSHNULL|ISNAN|2DUP|IFNOT:<\{|STSLICER|LEQ|DICTUGETNEXT|@endblk|SETNUMVARARGS|DEPTH|ATEXIT:<\{|STBREFR_l|QDIVC|@sridx\+|@proclistadd|SUBDICTURPGET|PLDULE8Q|HASHSU|LSHIFT\#|\}>c|SDCNTTRAIL1|SETSECONDQ|STSLICEQ|BINPRINT|ATEXITALT|@adj\-long\-proc|XCHGX|s\(\)|PFXDICTGET|PRINTSTR|INLINECALLDICT|\}>SETEXITALT|SDCUTLAST|ENDS|DICTUGETPREVEQ|LDOPTREF|STSAME|CALLVAR|INLINECALL|STULE4|SDPPFX|@declproc|MULDIVMOD|CHKTUPLE|IFNOTRETALT|LDDICT|split_prepare|PUSHINT|IFNOT:|DEBUGSTR|@def\-proc|DICTUSETREF|\-ROLL|PUSHPOW2DEC|REWRITESTDADDRQ|LDUQ|CDDDR|RETVARARGS|DICTUSETGET|CONT|s4|STSLICE_l|PUSHREFCONT|NIP|SCUTFIRST|PLDULE8|CARQ|DICTUREMMIN|QRSHIFT|PFXDICTGETEXEC|PLDIX|DICTIMAXREF|\}END>|\}>IFNOTJMP|SBITS|SUBR|SCHKBITREFS|JMPDICT|LDSLICEX|WHILE:<\{|PROCREF:<\{|QUFITSX|s10|RANDSEED|STUQ|PLDOPTREF|STREF_l|DICTUGETEXEC|@Defop\(8u\)|SDSKIPLAST|STONE|QXOR|PLDSLICEX|SETCP0|WHILE|SDSKIPFIRST|@cridx|REPEAT:|LDMSGADDRQ|THROWANY|THROWIF|DICTIREPLACEB|LDI_l|MULRSHIFT|SCHKREFS|SAVECTR|DICTADDGETREF|CONT:<\{|SDLEXCMP|UNSINGLE|PLDILE8|DICTIADDB|DICTUMIN|SDPPFXREV|BOOLAND|BLKPUSH|@procdictkeylen|XC2PU|RAWRESERVE|SEMPTY|SDCNTLEAD1|SKIPOPTREF|BCHKREFS|DICTGETPREV|LESS|QUIET|DICTUGETREF|DICTISETGETB|SREFS|IF|DICTUGET|NULL|SECONDQ|TLEN|COMPOSALT|SAVEALT|PUSHX|DICTREMMAXREF|MYADDR|QMULDIVMOD|@atend|POW2|@havebits|IFNOTREF|PUSHCTRX|NOT|SAVE|QFITS|STULE8|INDEX|DICTREPLACEREF|DICTIGETJMP|REWRITEVARADDRQ|DICTEMPTY|TWO|POP|STZERO|UFITS|LDU|RETURNVARARGS|DICTIREMMIN|DROP2|PLDUZ|PLDULE4|DICTSETGETOPTREF|FITS|STVARUINT16|@havebitrefs|REWRITESTDADDR|BCHKBITS|LDIQ|DICTUDELGET|REWRITEVARADDR|STB|UNTUPLE|BINDUMP|LDMSGADDR|DICTIDEL|@chkmaindef|DEBUGOFF|DICTREPLACEGET|ATEXIT|DICTISETREF|STBREFQ|JMPXARGS|DUMPTOSFMT|HASHCU|BCHKBITREFSQ|PU2XC|XOR|<\{|ATEXITALT:<\{|ROLLREV|SETINDEXVARQ|HEXDUMP|BLESSNUMARGS|STSLICERQ|DIVC|DICTIADDGETB|ABS|SPLIT|NEGATE|DICTUSETGETOPTREF|NULLROTRIF|split_install|IFNOTJMPREF|SUBDICTRPGET|SETGLOB|@pushatend|MULRSHIFT\#|QDIV|QINC|@range|STUX|PLDDICTS|LDUX|\}>UNTIL|THROWARGIFNOT|SUBDICTUGET|UNCONS|STIXR|QDIVR|RSHIFT|LDSLICE_l|DICTIGET|JMPVAR|SETINDEX|DICTIREPLACE|LSHIFTDIVR|@sridxrange|CDDR|LDILE8|STIXQ|SECOND|SETCONTVARARGS|IFJMP:<\{|CALLCC|DICTDELGETREF|IF:<\{|DICTADD|>libref|@addop|RETURNARGS|BCHKBITSQ\#|LDZEROES|NOW|IFNOTJMP|MUL|LDSAME|BOOLOR|REPEAT:<\{|SDEQ|\}>DO:|SETTHIRD|STDICTS|QDEC|DICTIREPLACEREF|RETFALSE|c2|LDILE4|PLDU|STU|TRUE|ISNZERO|STUR|RETTRUE|SDBEGINSQ|DICTIREMMAXREF|STREF|@Defop\(s,s\)|@doifnotelse|BLOCKLT|INDEX2|DEBUGSTRI|CALLXVARARGS|GETPARAM|GETGLOB|DICTREPLACEB|OVER2|PLDSLICE|LDVARUINT16|COMPOS|IFBITJMP|s\(\-2\)|SETINDEXQ|POPROOT|DICTDEL|COMPOSBOTH|XCHG2|SDSFX|\}>ATEXIT|DICTREMMINREF|QPOW2|SETCP|SETCONTCTR|EXPLODE|TEN|ONE|BREMBITREFS|DUMPSTKTOP|LDULE8Q|@Defop\(4u,4u\)|JMP|BOOLEVAL|\}>REPEAT|SWAP|DICTIREPLACEGET|TPOP|REVERSE|c7|STDICT|PLDILE8Q|UNTIL:<\{|THROW|THIRDQ|PLDI|LSHIFT\#DIV|STBRQ|INDEXQ|COMMIT|LDULE8|CHKSIGNS|STREFQ|STSLICE|IFJMP|CONS|SUB|ROTREV|NULLSWAPIF|DICTUREMMINREF|DICTGETNEXT|PFXDICTGETJMP|\}>AGAIN|LDU_l|@Defop|UNTIL:|DICTISETB|PLDDICT|PUSHCTR|DICTADDGETB|SDPSFX|INTSORT2|SSKIPLAST|DICTSETGET|QNOT|ROLLX|WHILEEND|LDDICTQ|CALLCCARGS|RETALT|TRY:<\{|@doifelse|s\(\-1\)|DICTUDELGETREF|2OVER|THENRET|@rangechk|CALLCCVARARGS|THROWARGANYIFNOT|QNEGATE|DICTIMIN|CALL|LDILE8Q|STUXR|GTINT|MULCONST|EXECUTE|DICTUADDGET|DICTUGETPREV|DICTIREMMINREF|PUSHSLICE|\}>s|ADDCONST|ACCEPT|s12|CDR|LSHIFT|RETARGS|SETCONTARGS|DICTREMMAX|DICTGETNEXTEQ|add\-lib|LOGFLUSH|STVARINT16|QSUBR|STBREF|QUFITS|POPCTR|\}>ATEXITALT|@fail\-ifdef|@PROC:<\{|CTOS|@\-range|STILE8|SWAP2|@ensurebitrefs|s5|SDCNTTRAIL0|TUPLE|\}>CONT|SCHKBITSQ|SDEMPTY|recv_external|DICTUREPLACEREF|QMUL|MULRSHIFTR|THIRD|UNTIL|DUMP|@trycatch|SDSFXREV|BLKDROP|EQINT|DICTUREPLACE|DIV|SUBINT|SDBEGINSXQ|QOR|@sridx|@chkdict|DICTDELGET|@Defop\(8i\)|RETDATA|GETGLOBVAR|2SWAP|PROGRAM\{|QDIVMODR|2ROT|ROT2|CALLDICT|LDVARINT16|SDPFXREV|@scomplete|IFNOT|SDCNTLEAD0|SETGLOBVAR|DEC|\}>CATCH<\{|BALANCE|s11|s2|UNPACKFIRST|DICTIREPLACEGETREF|SDBEGINSX|DICTREPLACEGETREF|c1|AND|SETCODE|GEQINT|@dowhile|RSHIFTR|\}>ELSE:|STBR|s0|TRIPLE|DICTIGETOPTREF|SETEXITALT:<\{|LDREF|@Defop\(4u\)|DICTREMMIN|INDEXVAR|\}>IFNOT|DICTGETPREVEQ|QLSHIFT|\}>|IFNOTRET|IFELSE|s8|JMPXVARARGS|RET|DICTREPLACEGETB|LTIME|BITSIZE|DUMPSTK|STRPRINT|DICTIADDGETREF|DICTADDREF|DICTUSETB|PLDUXQ|SDBEGINS:imm|BCHKBITSQ|\}END|PUSH|REPEATEND|s13|SPLITQ|IFRET|STI_l|UNPAIR|DICTIDELGET|CAR|SETEXITALT|\}>IFJMP|DEBUGON|BCONCATQ|SETRETCTR|PFXDICTREPLACE|\}>DO<\{|OVER|DICTIGETEXEC|AGAIN:<\{|PLDIXQ|si\(\)|PUXCPU|SETNUMARGS|DICTMIN|DICTUGETOPTREF|NEQ|CONFIGPARAM|DICTISETGETREF|c3|SDPFX|PARSEMSGADDRQ|DICTSET|@chkdicts|DICTUREPLACEB|SAVEBOTH|TPUSH|DICTREPLACE|NEQINT|SUBCONST|PROC|THROWARGIF|DICTIREPLACEGETB|THENRETALT|DICTIADD|BLESS|ENDCST|IF:|SETCONT|DICTUSET|STU_l|STONES|STRDUMP|DICTGETREF|PAIR|@\||c\(\)|CALLREF|AGAIN|PREPARE|DICTSETREF|SCUTLAST|FITSX|PLDILE4|DICTUREMMAX|STIR|LDSLICE|BCHKREFSQ|hash>libref|DICTUADD|ADD|QTLEN|ISNPOS|MULDIV|SETFIRST|PUXC|JMPXDATA|INLINE|EQUAL|STBREFRQ|PLDUQ|CHKBOOL|PFXDICTADD|LDGRAMS|CHKNAN|s14|NEWDICT|SUBDICTIGET|MULINT|DICTUREMMAXREF|STIXRQ|DICTIGETPREVEQ|PUSHNAN|PLDULE4Q|UNTILEND|CONFIGROOT|LDREFRTOS|SETCPX|DICTSETGETB|THROWANYIF|DICTUMAXREF|FIRST|TUPLEVAR|CONDSEL|s9|DICTIGETPREV|DICTMAXREF|CONDSELCHK|PROC:<\{|STZEROES|REVX|@Defop\(ref\)|SETINDEXVAR|UFITSX|PUSH2|SETTHIRDQ|IFRETALT|DICTGETOPTREF|DIVMODC|PFXDICTCONSTGETJMP|PUSHPOW2|SDPSFXREV|SGN|DICTUADDGETREF|SDFIRST|CONFIGOPTPARAM|DICTUMAX|DICTIREMMAX|DICTUSETGETREF|\}>IF|PUSHNEGPOW2|SAVEBOTHCTR|TRYARGS|STIX|DICTMAX|c0|\}END>s|STBREFR|THROWARGANY|CMP|THROWARG|STUXQ|JMPREF|INDEXVARQ|PLDIQ|c4|SDSUBSTR|PRINT|BCONCAT|XCHG3|IFJMPREF|DICTIMAX|DICTUREPLACEGETREF|STILE4|PFXDICTDEL|THROWIFNOT|DICTGET|PUSHREF|DICTUREPLACEGET|UNTUPLEVAR|PLDUX|STURQ|@simpleuop|ENDC|IFNBITJMP|@Defop\(c\)|SCHKREFSQ|DIVMOD|@iscr\?|DICTSETB|PLDDICTQ|PICK|PUSHROOT|NEWC|@Creg|LDULE4Q|THROWANYIFNOT|s7|@doafter<\{|TUCK|SDBEGINSQ:imm|IFBITJMPREF|LESSINT|PLDSLICEQ|LAST|ROT|DICTUSETGETB|PUSH3|SUBSLICE|BBITREFS|SETGASLIMIT|ZERO|LDIX|\-ROT|PREPAREVAR|STREF2CONST|FALSE|2x<=|MULRSHIFTR\#|PREPAREDICT|CHKBIT|SHA256U|SETALTCTR|QSUB|PUXC2|CONFIGDICT|POPCTRSAVE|@normal\?|RSHIFT\#|FIRSTQ|STREFR|JMPX|s6|NULLSWAPIFNOT|SENDRAWMSG|SKIPDICT|RETBOOL|OR|EXPLODEVAR|PUSHREFSLICE|STSLICECONST|BLESSVARARGS|MAX|QADD|recv_internal|DICTIGETNEXTEQ|s15|\}>ELSE<\{|@Defop\(s\)|DICTIGETNEXT|QDIVMOD|DROPX|LEQINT|BLKSWAP|INVERT|UBITSIZE|PLDREFIDX|BCHKBITREFS|INDEX3|STIQ|CALLX|ISTUPLE|INC|SUBDICTIRPGET|QFITSX|c5|SINGLE|NULLROTRIFNOT|DICTUADDREF|ROLLREVX|SCHKBITS|STREFRQ|SREMPTY|LOGSTR|PLDSLICEXQ|ISNULL|DICTADDB|DIVMODR|ISZERO|STGRAMS|XCHG|ROLL|DICTUGETNEXTEQ|SAVEALTCTR|@inline|DICTIADDREF|INT|SDCUTFIRST|DICTUGETJMP|ISNEG|LSHIFTDIV|BREFS|DICTUMINREF|SCHKBITREFSQ|POPCTRX|BBITS|XCPU|DICTUADDGETB|LDI|DIVR|PFXDICTSET|DICTUDEL|DICTUREPLACEGETB|\}END>c|CADDR|run_ticktock|DEBUG|SETCONTCTRX|ONLYTOPX|PROCREF|GREATER|RSHIFTR\#|SETSECOND|QMULDIVR|RAWRESERVEX|STUXRQ|REPEAT|s1|BREMBITS|DICTIGETREF|BREMREFS|IFREF|\-ROLLX|NOP|TVM_Asm|DICTPUSHCONST|HEXPRINT|LDUXQ|BRANCH|GEQ|BLKSWX|CHKSIGNU|IFNOTJMP:<\{|THROWARGANYIF|SDBEGINS|DICTSETGETREF|QAND)(?=\s|$)/;
    var onload = function() {
        CodeMirror.defineSimpleMode("fift", {
            start: [
                // strings
                { regex: /"(?:[^\\]|\\.)*?(?:"|$)/, token: "string" },
                // bytes
                { regex: /(?<=\s|^)B\{[A-z0-9_]+\}(?=\s|$)/, token: "string" },
                // slices
                { regex: /(?<=\s|^)x\{[A-z0-9_]+\}(?=\s|$)/, token: "string-2" },
                { regex: /(?<=\s|^)b\{[01]+\}(?=\s|$)/, token: "string-2" },
                // comments
                { regex: /\/\/.*/, token: "comment" },
                { regex: /\/\*/, token: "comment", next: "comment" },
                // registers c0-7, s0-15
                { regex: /(?<=\s|^)(?:c[0-7])|(?:s[0-9]{1,3})(?=\s|$)/, token: "builtin" },
                // asm constants
                { regex: /(?<=\s|^)(?:main|recv_internal|recv_external)(?=\s|$)/, token: "builtin" },
                // vm specific words
                { regex: /(?<=\s|^)(?:runvmcode|runvmdict|runvm|runvmctx|gasrunvmcode|gasrunvmdict|gasrunvm|gasrunvmctx)(?=\s|$)/, token: "operator" },
                // some control words
                { regex: /(?<=\s|^)(?:true|false|if|ifnot|cond|until|while|abort|words|does|[\[\(]?compile[\]\)]?|\.s?|csr\.|include)(?=\s|$)/, token: "keyword" },
                // numbers
                { regex: /(?<=\s|^)(?:(?:[+-]?[0-9]+\.?[0-9]*)|(?:0x[0-9a-f]+)|(?:0b[01]+))(?=\s|$)/i, token: "number" },
                // builder <b ... b> and cell <s ... s> (multiline)
                { regex: /(?<=\s|^)\<[bs](?=\s|$)/, token: "atom", indent: true },
                { regex: /(?<=\s|^)[bs]\>(?=\s|$)/, token: "atom", dedent: true },
                // asm code <{ ... }>s/c and control instructions PROC, CONT, REPEAT, IF, ELSE, etc (multiline)
                { regex: /(?<=\s|^)[A-Z:]*<\{(?=\s|$)/, token: "atom", indent: true },
                { regex: /(?<=\s|^)(?:\}(?:END)?>[A-Zsc:]*)(?=\s|$)/, token: "atom", dedent: true },
                { regex: /(?<=\s|^)(?:\}>[A-Z]+<\{)(?=\s|$)/, token: "atom" },
                // word lists { ... } (multiline)
                { regex: /(?<=\s|^)\{(?=\s|$)/, token: "atom", indent: true },
                { regex: /(?<=\s|^)\}(?=\s|$)/, token: "atom", dedent: true },
                // definitions
                { regex: /(?<=\s|^)(?:2?constant|variable|2?=:|:_?|::_?|create)(?=\s|$)/, token: "builtin", next: "def" },
                // words
                { regex: /(?<=\s|^)(?:abort)(?=\")/, token: "keyword" },
                { regex: builtin_words, token: "variable-2"},
                { regex: fift_words, token: "variable-3"},
                { regex: asm_words, token: "meta"},
                // fallback
                { regex: /[^\s]+/, token: "variable"},
            ],
            comment: [
                { regex: /.*?\*\//, token: "comment", next: "start" },
                { regex: /.*/, token: "comment" }
            ],
            def: [
                { regex: /.*$/, token: "def", next: "start" },
            ],
            // The meta property contains global information about the mode. It
            // can contain properties like lineComment, which are supported by
            // all modes, and also directives like dontIndentStates, which are
            // specific to simple modes.
            meta: {
                dontIndentStates: ["comment"],
                lineComment: "//",
                blockCommentStart: "/*",
                blockCommentEnd: "*/"
            }
        });
        CodeMirror.defineMIME("text/x-fift", "fift");
        CodeMirror.modeInfo.push({
            ext: ["fif"],
            mime: "text/x-fift",
            mode: "fift",
            name: "Fift"
        });

        // Force mode on refresh
        // Big thanks to https://github.com/kelvich for this solution
        IPython.CodeCell.options_default["cm_config"]["mode"] = "fift";
        [...document.querySelectorAll('.code_cell .CodeMirror')].forEach(c => {
            c.CodeMirror.setOption('mode', 'fift');
        });
    }
    return { onload: onload }
});