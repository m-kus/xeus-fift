define([
    'codemirror/lib/codemirror', 
    'codemirror/addon/mode/simple'
], function(CodeMirror) {
    var builtin_words = /(?<=\s|^)(?:x>B\?|not|u@\?\+|<<|\-roll|2\*|rmod|0|or|X\.|<s|1\+|'|Lu>B|emit|B>Li@\+|if|dictmap|hashu|\[\]|\-|1<<|depth|\*\/rmod|\(forget\)|\.dump|crc16|>>c|while|runvmctx|runvmcode|remaining|2dup|dictnew|\-rot|>|\/rmod|2swap|find|cr|Blen|ed25519_sign_uint|\$cmp|halt|B@\?|forget|2\-|:_|\(\{\)|B>X|empty\?|hole|<<\/c|b>idict!\+|b\+|bbitrefs|B@\+|tuck|=|i@\?\+|smca>\$|hashB|\$\||b>idict!|Bx\.|\(create\)|b\.|\.sl|B@\?\+|srefs|execute|\(number\)|0>=|include|B>file|\$len|b>udict!|B>\$|idict!|pfxdict!\+|ref@\?\+|reverse|\?dup|ed25519_chksign|dbrunvm|i@|rot|\*\/|space|<b|\(dump\)|cmod|ref@\+|\*\/cmod|type|\-1|dup|u@\?|nop|\-trailing|skip\-to\-eof|nip|2\+|x>B|B>u@|%1<<|\$=|\*>>c|,|u@\+|box\?|cmp|idict!\+|\*mod|2|exch2|s>|>>r|Li>B|negate|skipspc|filepart>B|runvm|\-trailing0|:|dict@|<xchg>|sbits|x\._|B\+|chr|dictforeach|b\._|\$,|i,|\[\]=|tpop|udict!\+|B>u@\+|\|_|Bhash|!|exch|\*\/c|1\-|create|B=|allot|X\._|b>|2\/|s,|tuple\?|file>B|dictmapext|\$reverse|x\.|\(b\.\)|file\-exists\?|crc32|gasrunvmcode|eq\?|'nop|atom>\$|atom\?|\*\/r|newkeypair|true|>>|\{|boc>B|sr,|ref@\?|pick|\]|<<\/|word|1|B>Li@|abort|B,|\$\+|crc32c|dictdiff|sgn|0=|\[|\.l|B>x|dictmerge|B>i@|getenv|\(compile\)|\$>smca|::_|\-1<<|explode|over|base64url>B|::|i>B|brembits|false|i@\?|B>i@\+|B>base64|\/c|dict>s|until|<|\(\}\)|\.|b>udict!\+|\(X\.\)|gasrunvmctx|\(atom\)|<push>|swap|\+|words|dict,|2drop|\.tc|\$Len|vmlibs|<=|<>|xor|\$>s|ref,|b>spec|runvmdict|gasrunvmdict|0<|\.s|@|pfxdict@|eqv\?|\(x\.\)|B>boc|udict!|hold|\$@\?|<pop>|B>Lu@|tuple|\*\/mod|\(word\)|and|null\?|u@|string\?|udict@|gasrunvm|mod|base64>B|\/mod|i@\+|untuple|bremrefs|\$@\+|BhashB|0<=|boc\+>B|\$@|B@|idict@|\}|B>base64url|\$Split|bbits|sbitrefs|\*|csr\.|\"|B\||ed25519_sign|\(hex\-number\)|hash|roll|priv>pub|now|\/r|quit|count|pfxdict!|ifnot|\(execute\)|B>Lu@\+|x\{|bl|\|\+|<<\/r|u,|times|dbrunvm\-parallel|null|\||\/|brefs|bye|\(char\)|\$>B|anon|cond|char|brembitrefs|u>B|getenv\?|\*>>|fits|box|>=|\/cmod|0<>|\$@\?\+|\._|0>|integer\?|\*>>r|drop|2over|b\{|Bhashu|ufits|\(\-trailing\)|\(\.\)|1<<1\-|ref@|Bcmp|dict@\+)(?=\s|$)/;
    var fift_words = /(?<=\s|^)(?:skip\-ifdef|cddr|0x\.|B\{|atom|\+\"|s>c|untriple|sign|third|0X\.|\('\)|undef\?|max|s\-fits\?|cadr|X\#s|second|base\#|digit|\#s|x\#s|B\#|Base\#s|B\#s|\.\"|0X\._|cdr|x\#|single|hex|X\#|recursive|cons|triple\?|2constant|\(0X\.\)|Base\#|\(def\?\)|\#>|min|\$\(|\[compile\]|abort\"|1\-!|list|tuple\-len\?|octal|ten|shash|\(0x\.\)|pair\?|triple|abs|unpair|uncons|caddr|single\?|null!|\-!|<\#|nil|base|constant|car|`|=:|b\#s|variable|does|\#|nil\?|binary|0x\._|\/\*|library|def\?|first|@'|\[forget\]|0!|unsingle|b\#|base\#s|decimal|\/\/|minmax|library\-version|1\+!|pair|2=:|Digit|\+!)(?=\s|$)/;
    var asm_words = /(?<=\s|^)(?:@Defop\(ref\)|RETVARARGS|DICTIGETOPTREF|DICTSETGETREF|TRIPLE|QDIVR|UNCONS|IF:<\{|SETCPX|SDBEGINSXQ|DEBUGSTR|JMPXARGS|PLDSLICE|COMMIT|BALANCE|REWRITEVARADDR|STREF2CONST|DICTIADDGETREF|SAVEALTCTR|DICTUADDGETREF|CMP|DICTDELGET|THROWARGIFNOT|ISNZERO|@\-range|DUMP|ROT2|LDGRAMS|PUSHREFCONT|c\(\)|2SWAP|DIVMODR|XCHG0|CALL|IFNBITJMPREF|STIXQ|@procdictkeylen|ROLLX|DICTUREPLACEB|@def\-proc|MULINT|LESSINT|SDPFX|SDPFXREV|SDCNTTRAIL1|PREPAREDICT|PUSHINT|SDBEGINSQ|DICTUSETB|DICTUSET|s15|PUSHX|@Defop\(4u,4u\)|REPEATEND|2ROT|QMULDIVMOD|MIN|ROLLREVX|DICTUDELGET|s2|DICTREMMIN|LDI|SETCONTARGS|XCHG|@newproc|BLKSWAP|DICTREPLACEB|BCHKREFSQ|REVERSE|LDMSGADDRQ|IFREF|STBQ|SETINDEX|POPSAVE|ADD|LDU|HEXDUMP|SWAP|LEQINT|POPCTRX|DICTUGETPREV|LDMSGADDR|JMP|REWRITEVARADDRQ|PRINT|PLDU|s\(\-2\)|STUR|NEQ|BLESSVARARGS|DICTISETGETREF|PLDI|BLKSWX|SBITREFS|THROWIF|SECOND|@Defop\(c\)|DICTMAX|c3|THROWARGANY|COMPOS|QOR|s11|DICTIDELGET|REPEAT:|RSHIFTR|DICTEMPTY|SDBEGINSX|STUXR|GETGLOB|MOD|QDIVMODC|SETINDEXVARQ|STIRQ|DICTUGETNEXTEQ|DICTDELGETREF|CADDR|SETINDEXVAR|LDSLICEX|OVER|PLDSLICEQ|INC|\}>IFJMP|IFNOT:<\{|DICTIREMMIN|SETNUMVARARGS|UNPAIR|DICTIREPLACEGETB|2OVER|FITSX|SETALTCTR|XC2PU|NEWDICT|\}>ATEXIT|CHKBOOL|DICTIDELGETREF|MULDIV|DICTREMMAXREF|INLINE|THENRETALT|AGAIN|STREFRQ|DICTIADDB|EXPLODEVAR|PLDILE4Q|CONT:<\{|LDSLICE_l|DICTUDEL|PLDREFIDX|\}>ATEXITALT|INDEXVARQ|DICTSETB|POW2|NOW|@Creg|STONES|INDEX|JMPREF|ROLLREV|PUSHCTR|JMPVAR|SETCONTCTR|DICTGETOPTREF|STVARUINT16|CHKDEPTH|@proclistadd|@normal\?|SDPPFXREV|NIP|@havebits|STIXRQ|\}>CONT|RETBOOL|LDIQ|QFITSX|PROC|LSHIFT|CHKBIT|TEN|JMPDICT|\}>ELSE<\{|DROP|ISNPOS|PUXCPU|DICTIGETNEXTEQ|BINPRINT|s13|STSLICE_l|SPLIT|2DROP|CDRQ|CALLX|PLDIQ|@addop|WHILEEND|IFNOTJMP:<\{|NULLSWAPIF|ISZERO|IFJMP|BREMBITREFS|RSHIFT\#|DICTSETREF|PUSHNULL|BCHKREFS|@chkdict|LSHIFTDIV|PFXDICTGET|CDR|DICTISETGETOPTREF|c0|DICTADDREF|SETCP|DICTUREPLACE|SUBDICTIGET|PFXDICTSET|LDILE4Q|SCHKREFSQ|ROT|SAVEALT|BLOCKLT|PUSHROOT|CALLCCARGS|recv_external|@sridx|MULRSHIFT\#|IFNOT|ADDCONST|DICTADDGETREF|IFBITJMP|DUP|STREFCONST|QFITS|ROTREV|@Defop\(8u\+1\)|DICTADDB|BCHKBITREFS|COMPOSALT|INT|\}>REPEAT|SDSUBSTR|CALLXARGS|DICTGETREF|DICTIREPLACEB|DICTUDELGETREF|CTOS|DROP2|NULLROTRIF|PUXC2|IFNOTJMPREF|@Defop|PUSH|BINDUMP|@ensurebits|UNTIL:|SETSECOND|SETGLOB|SUB|STGRAMS|STSLICERQ|STREFQ|SETSECONDQ|MULDIVMOD|STURQ|LEQ|DICTGETNEXT|@Defop\(s,s\)|SEMPTY|RANDSEED|QDEC|SETRETCTR|LSHIFT\#DIVR|IFNOT:|SCUTFIRST|THIRDQ|LDZEROES|MYADDR|TRY|DICTIGETPREV|hash>libref|STI|RETTRUE|PUSHSLICE|FALSE|\}>DO<\{|MULCONST|PUXC|DICTUADDGET|DEPTH|INDEXQ|RAWRESERVEX|DICTIREPLACEGET|DIVMOD|POPCTRSAVE|s\(\-1\)|UNTIL|STDICTS|REVX|IFRETALT|NEWC|DICTUGETNEXT|IFELSE|PFXDICTADD|TVM_Asm|@ensurebitrefs|@sridxrange|DICTUGET|SAVE|STULE4|XCPU|SKIPOPTREF|PLDSLICEXQ|DICTUMAXREF|s10|PLDREFVAR|QDIV|GETGLOBVAR|LDUQ|LDULE8Q|STI_l|PARSEMSGADDR|ATEXIT:<\{|LDIX|IFJMPREF|SDSKIPFIRST|RAWRESERVE|SUBDICTURPGET|COMMA|RET|REPEAT:<\{|NULLROTRIFNOT|DICTIADD|@rangechk|BCHKBITREFSQ|RETALT|s0|THROWARGIF|UNTRIPLE|DIVR|DICTIADDREF|QUFITS|QMULDIVR|BBITS|PLDSLICEX|TRUE|@range|XOR|AGAIN:|SDCUTLAST|LDULE4Q|@addopb|STBREFR_l|STULE8|DICTGETPREVEQ|UNTUPLE|PRINTSTR|DICTUREPLACEGET|DICTREPLACE|c5|SDSFX|PLDUX|DICTISETREF|@Defop\(8u\)|\-ROLL|POPROOT|DICTSET|CALLCC|LDSLICEQ|SCUTLAST|SUBR|UNSINGLE|BCHKBITSQ|BCHKBITSQ\#|\}>DO:|FIRSTQ|UNTUPLEVAR|FIRST|AGAIN:<\{|QUFITSX|QDIVC|IFJMP:<\{|PUSH2|TUPLE|DICTSETGET|STOPTREF|@pushatend|DICTISET|DICTUREMMAX|PLDILE8|POPCTR|SENDRAWMSG|STREF|STSLICER|SETCONTCTRX|SDSKIPLAST|DICTGETNEXTEQ|SBITS|BREMREFS|CONS|TPUSH|DICTREPLACEREF|DICTIREMMAX|STU_l|UNPACKFIRST|ATEXITALT:<\{|SUBDICTUGET|@doafter<\{|PUSHCONT|SETFIRSTQ|DEBUGON|GREATER|GETPARAM|INLINECALL|DICTUREMMAXREF|SETCODE|STSLICE|XCHGX|QDIVMOD|SETGASLIMIT|DICTUSETGET|SREMPTY|LDSLICEXQ|PROCREF|AGAINEND|s\(\)|STB|ZERO|RETURNVARARGS|SGN|SINGLE|QTLEN|LDILE8|SDCNTLEAD0|PLDUXQ|@iscr\?|STZEROES|CAR|CALLREF|BLESSNUMARGS|@Defop\(8i\)|BLKDROP|GTINT|NIL|BCHKBITS\#|MULRSHIFTR\#|TRYARGS|STIXR|DICTUMINREF|ISNULL|PLDULE4Q|QUIET|PLDIX|EXECUTE|DICTREPLACEGETB|2DUP|STBREFR|SLICE|LOGSTR|STIX|DICTUADD|INDEX2|THROWANY|PLDULE8Q|s6|INLINECALLDICT|RSHIFT|LDILE8Q|THROW|SCHKBITREFSQ|@declproc|SHA256U|STUQ|DICTIMIN|LDVARINT16|\}>SETEXITALT|TUCK|XCPUXC|DICTMAXREF|STUXRQ|DICTIGETNEXT|PROC:<\{|DICTREPLACEGETREF|@endblk|SETCP0|SREFS|BOOLAND|STREF_l|LDDICT|CHKSIGNU|LDOPTREF|@Defop\(4u\)|IF:|IFNBITJMP|IFNOTRETALT|MULRSHIFTR|ONLYTOPX|DICTUREMMINREF|LDI_l|@dowhile|SSKIPFIRST|\-ROT|LSHIFTDIVR|SCHKREFS|DICTUSETGETOPTREF|STBREFQ|SUBDICTGET|STRPRINT|CARQ|STUX|IFNOTJMP|RETFALSE|QDIVMODR|LDDICTQ|LDREFRTOS|INDEX3|STIR|XCHG3|DICTPUSHCONST|\}END|HASHSU|LDVARUINT16|ATEXIT|THROWANYIFNOT|IFNOTRET|DICTUGETPREVEQ|GEQINT|CALLXVARARGS|RSHIFTR\#|BOOLOR|BCONCAT|@fail\-ifdef|QSUBR|LDUX|PUSHREFSLICE|IF|SAVEBOTH|DICTISETGET|JMPX|<\{|BCHKBITS|PLDIXQ|SKIPDICT|@doifnotelse|DICTSETGETOPTREF|UNPACKFIRSTVAR|LDULE8|DICTIREPLACE|PFXDICTCONSTGETJMP|\}END>s|MULRSHIFT|STBRQ|STUXQ|DICTIREMMINREF|DICTADDGETB|s14|DICTDEL|DICTUADDREF|STSLICEQ|DICTUGETREF|@trycatch|ENDC|PFXDICTGETQ|DICTIMINREF|DICTISETGETB|PFXDICTGETEXEC|PLDDICTS|QNOT|\}>|BLESS|s1|\}>CATCH<\{|ENDCST|LDSAME|STBR|DICTGETPREV|PLDUQ|UNTILEND|MAX|PUSHPOW2|UNTIL:<\{|ISNEG|SETCONTVARARGS|\}>IFNOTJMP|SUBDICTRPGET|DICTREMMAX|RETARGS|CALLCCVARARGS|SDBEGINSQ:imm|s4|DICTUMAX|LDIXQ|DICTUSETGETREF|BREMBITS|DICTIADDGET|DICTADDGET|DEBUGOFF|ROLL|add\-lib|TLEN|PUSHCTRX|XCPU2|>libref|SUBDICTIRPGET|THROWIFNOT|SETTHIRDQ|SDEMPTY|DICTIMAX|ENDS|SETGLOBVAR|\}>ELSE:|QADD|CONFIGDICT|DUMPTOSFMT|NULLSWAPIFNOT|DIVMODC|THENRET|LDUXQ|DICTUSETGETB|DEBUG|BRANCH|WHILE|QLSHIFT|DICTUGETEXEC|HASHCU|CALLVAR|WHILE:<\{|@Sreg|PFXDICTDEL|SDSFXREV|UFITS|QAND|PARSEMSGADDRQ|DICTIMAXREF|CONDSELCHK|\}END>c|THROWARGANYIFNOT|SDCUTFIRST|PROGRAM\{|ONE|CADR|QINC|CONT|NOP|SAVECTR|PICK|CDDR|DICTADD|PFXDICTSWITCH|ISNNEG|\}>s|\}END>|THROWARGANYIF|PFXDICTREPLACE|SETEXITALT|@Defop\(s\)|STRDUMP|BREFS|INVERT|LTIME|@sridx\+|PLDDICT|@PROC:<\{|s3|\}>IFNOT|ACCEPT|STREFR|COMPOSBOTH|DICTIGET|PFXDICTGETJMP|SETTHIRD|NOT|DICTIGETPREVEQ|PUSHREF|SCHKBITREFS|SDBEGINS|@\||DICTIGETEXEC|JMPXVARARGS|PLDOPTREF|DICTIGETREF|THROWANYIF|SETFIRST|MODPOW2\#|SDBEGINS:imm|DIVC|\}>AGAIN|BLESSARGS|QRSHIFT|STBREFRQ|SUBINT|INDEXVAR|CONFIGROOT|s7|@adj\-long\-proc|QSUB|PU2XC|TUPLEVAR|DICTIDEL|c2|OR|DEC|HEXPRINT|ABS|ATEXITALT|LOGFLUSH|RETDATA|REWRITESTDADDR|@havebitrefs|QNEGATE|DUP2|STU|CDDDR|2x<=|ISTUPLE|QMUL|STILE8|SUBCONST|DICTUSETREF|RETURNARGS|c1|LDREF|DICTGET|MUL|DICTIREPLACEREF|TRY:<\{|LAST|ADDINT|DEBUGSTRI|NEGATE|MULDIVR|STVARINT16|@scomplete|CONFIGPARAM|DICTUREPLACEREF|PUSHPOW2DEC|\-ROLLX|DICTREPLACEGET|\}>IF|IFBITJMPREF|DICTUADDB|SETCONT|CHKSIGNS|PROCREF:<\{|STZERO|PLDDICTQ|QMOD|EQINT|DICTUREPLACEGETREF|DICTMIN|BITSIZE|PLDULE8|DICTIREPLACEGETREF|LDULE4|REPEAT|REWRITESTDADDRQ|BOOLEVAL|DIV|NEQINT|LDONES|s12|PUSH3|IFRET|CALLDICT|POP|CHKNAN|SDPPFX|recv_internal|s8|DICTUMIN|DICTIGETJMP|SDEQ|STILE4|XCHG2|LESS|DUMPSTKTOP|MINMAX|BCONCATQ|XCHG3_l|EQUAL|DICTISETB|SUBSLICE|SETEXITALT:<\{|IFNOTREF|ISPOS|c7|PLDUZ|SDLEXCMP|INTSORT2|@chkmaindef|GEQ|CONDSEL|SDCNTTRAIL0|DICTREMMINREF|ISNAN|split_prepare|DICTUADDGETB|DICTUREPLACEGETB|SDFIRST|OVER2|LDU_l|THROWARG|UFITSX|PUSHNAN|DICTUGETOPTREF|PLDILE4|LSHIFT\#|DICTIADDGETB|PLDREF|THIRD|@bigsridx|LDDICTS|JMPXDATA|PUSHNEGPOW2|SCHKBITS|@chkdicts|DICTMINREF|STONE|\}>c|SECONDQ|CHKTUPLE|DICTUGETJMP|DICTUREMMIN|PREPARE|TWO|SAVEBOTHCTR|run_ticktock|STSLICECONST|LDILE4|SPLITQ|@Defop\(8i,alt\)|STIQ|@doifelse|EXPLODE|BLKPUSH|STSAME|s9|QXOR|STBREF|s5|TPOP|UBITSIZE|PLDULE4|PLDILE8Q|SDCNTLEAD1|ONLYX|\}>UNTIL|QPOW2|SSKIPLAST|LDSLICE|@atend|SDPSFX|DUMPSTK|DICTIREMMAXREF|LSHIFT\#DIV|PREPAREVAR|SETINDEXQ|SWAP2|JMPREFDATA|FITS|DICTSETGETB|NULL|@inline|@simpleuop|split_install|BBITREFS|SDPSFXREV|AND|CONFIGOPTPARAM|si\(\)|STDICT|@cridx|PAIR|SCHKBITSQ|c4|SETNUMARGS|DROPX|@procdict)(?=\s|$)/;
    var onload = function() {
        CodeMirror.defineSimpleMode("fift", {
            start: [
                // strings
                { regex: /"(?:[^\\]|\\.)*?(?:"|$)/, token: "string" },
                // slices
                { regex: /(?<=\s|^)x\{[A-z0-9_]+\}(?=\s|$)/, token: "string-2" },
                { regex: /(?<=\s|^)b\{[01]+\}(?=\s|$)/, token: "string-2" },
                // bytes
                { regex: /(?<=\s|^)B\{[A-z0-9_]+\}(?=\s|$)/, token: "string-3" },
                // comments
                { regex: /\/\/.*/, token: "comment" },
                { regex: /\/\*/, token: "comment", next: "comment" },
                // registers c0-7, s0-15
                { regex: /(?<=\s|^)(?:c[0-7])|(?:s[0-9]{1,3})(?=\s|$)/, token: "atom" },
                // asm constants: main, recv_internal, recv_external and booleans
                { regex: /(?<=\s|^)(?:main|recv_internal|recv_external|true|false)(?=\s|$)/, token: "atom" },
                // numbers
                { regex: /(?<=\s|^)(?:(?:[+-]?[0-9]+\.?[0-9]*)|(?:0x[0-9a-f]+)|(?:0b[01]+))(?=\s|$)/i, token: "number" },
                // builder <b ... b> and cell <s ... s> (multiline)
                { regex: /(?<=\s|^)\<[bs](?=\s|$)/, token: "def", indent: true },
                { regex: /(?<=\s|^)[bs]\>(?=\s|$)/, token: "def", dedent: true },
                // asm code <{ ... }>s/c and control instructions PROC, CONT, REPEAT, IF, ELSE, etc (multiline)
                { regex: /(?<=\s|^)[A-Z:]*<\{(?=\s|$)/, token: "def", indent: true },
                { regex: /(?<=\s|^)(?:\}(?:END)?>[A-Zsc:]?)(?=\s|$)/, token: "def", dedent: true },
                { regex: /(?<=\s|^)(?:\}>[A-Z]+<\{)(?=\s|$)/, token: "def" },
                // word lists { ... } (multiline)
                { regex: /(?<=\s|^)\{(?=\s|$)/, token: "def", indent: true },
                { regex: /(?<=\s|^)\}(?=\s|$)/, token: "def", dedent: true },
                // other definitions
                { regex: /(?<=\s|^)(?:2?constant|variable|create|\(create\)|2?=:|:_?|::_?)(?=\s|$)/, token: "def" },
                // words
                { regex: /(?<=\s|^)(?:abort)(?=\")/, token: "keyword" },
                { regex: builtin_words, token: "keyword"},
                { regex: fift_words, token: "keyword"},
                { regex: asm_words, token: "operator"},
                // fallback
                { regex: /[^\s]+/, token: "variable"},
            ],
            comment: [
                { regex: /.*?\*\//, token: "comment", next: "start" },
                { regex: /.*/, token: "comment" }
            ],
            // The meta property contains global information about the mode. It
            // can contain properties like lineComment, which are supported by
            // all modes, and also directives like dontIndentStates, which are
            // specific to simple modes.
            meta: {
                dontIndentStates: ["comment"],
                lineComment: "//",
                blockCommentStart: "/*",
                blockCommentEnd: "*/",
                closeBrackets: { pairs: "{}\"\"<{}><bb><ss>" },
            }
        });
        CodeMirror.defineMIME("text/x-fift", "fift");
        CodeMirror.modeInfo.push({
            ext: ["fif"],
            mime: "text/x-fift",
            mode: "fift",
            name: "Fift"
        });
    }
    return { onload: onload }
});