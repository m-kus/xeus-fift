define([
    'codemirror/lib/codemirror', 
    'codemirror/addon/mode/simple'
], function(CodeMirror) {
    var builtin_words = /(?<=\s|^)(?:ref@\+|\(hex\-number\)|gasrunvmdict|bbitrefs|drop|\(\{\)|ref@\?\+|remaining|BhashB|filepart>B|B>base64url|%1<<|@|\]|\(x\.\)|mod|0|dbrunvm|>=|B=|\*\/rmod|dict@\+|\*|emit|\-1|bl|<|vmlibs|brembits|\$Split|sgn|\(char\)|b\.|::_|tuck|udict!|\/cmod|>>|B@\?\+|B,|2\/|B>boc|bremrefs|B@\+|B>Li@|\(dump\)|\"|if|0<|Blen|\*\/|<s|\(b\.\)|chr|dictmerge|bye|cmp|count|dbrunvm\-parallel|eqv\?|b>udict!\+|\*mod|udict!\+|<push>|boc>B|\[|smca>\$|empty\?|'|false|>>r|i>B|B>u@|atom\?|\||crc32c|'nop|not|\.sl|u@\+|eq\?|box|<b|\.dump|ref@|B\+|b>idict!|B>u@\+|<<\/|idict!|Bcmp|cond|2dup|dictforeach|ed25519_chksign|quit|srefs|0<=|\.tc|\(\.\)|1<<1\-|B>\$|crc32|\-roll|\*>>r|<<\/r|udict@|bbits|Bhash|2|x>B\?|hashB|u@\?|reverse|string\?|depth|execute|brefs|sbits|1<<|0>=|!|\.|B@|\*\/cmod|hash|idict@|nip|\$@\+|B>i@|\-trailing0|b\._|true|hashu|crc16|allot|\(forget\)|2swap|b>udict!|swap|1\-|words|2over|sbitrefs|<>|u@\?\+|priv>pub|space|hole|>>c|\+|halt|\$@\?|\/r|brembitrefs|pfxdict!|i@\?\+|word|base64>B|null|u@|\/rmod|type|rmod|\(create\)|forget|times|2drop|dictdiff|ifnot|\/|nop|until|\.s|\(number\)|>|i@\+|\*\/r|<pop>|\(atom\)|skip\-to\-eof|sr,|tuple|1\+|s,|or|\$@\?\+|i@|\(word\)|x>B|2\-|Li>B|\$>s|cr|B@\?|and|B>base64|dictmapext|dict@|exch2|0>|roll|anon|B>i@\+|\*>>c|\$,|i,|\$Len|dictnew|boc\+>B|0=|gasrunvmctx|u,|ed25519_sign|exch|<=|include|b\+|:|\$cmp|\.l|Bhashu|xor|runvmctx|s>|b\{|find|dup|ref,|u>B|\(\}\)|x\._|\|\+|gasrunvmcode|\-1<<|file>B|,|Lu>B|\/mod|explode|\(X\.\)|rot|B>x|abort|\(compile\)|1|=|negate|\-trailing|\._|\$\+|cmod|\{|\}|\$\||\-rot|\[\]=|getenv\?|ed25519_sign_uint|pfxdict@|base64url>B|\*\/mod|\$=|box\?|<xchg>|char|\$@|fits|\$>smca|hold|dict,|\|_|:_|b>spec|\$reverse|\$len|tpop|ref@\?|B>Lu@|2\*|now|B>Lu@\+|B>file|dictmap|skipspc|while|untuple|\*\/c|csr\.|idict!\+|X\._|::|\-|gasrunvm|\/c|<<\/c|\?dup|file\-exists\?|b>idict!\+|i@\?|\(\-trailing\)|0<>|integer\?|B>X|\[\]|over|null\?|runvm|ufits|pick|\$>B|tuple\?|getenv|atom>\$|x\.|<<|runvmdict|x\{|X\.|B>Li@\+|b>|Bx\.|dict>s|\*>>|newkeypair|2\+|B\||\(execute\)|pfxdict!\+|create|runvmcode)(?=\s|$)/;
    var fift_words = /(?<=\s|^)(?:\[forget\]|min|abs|\+!|\-!|single|octal|digit|cdr|triple|\(0x\.\)|b\#s|Base\#|sign|first|\#>|minmax|x\#|cddr|variable|s>c|0!|1\-!|third|cadr|untriple|single\?|base\#s|shash|tuple\-len\?|binary|hex|B\#s|uncons|def\?|2constant|car|\.\"|null!|\/\*|unpair|cons|0x\.|x\#s|undef\?|s\-fits\?|atom|b\#|abort\"|\/\/|\+\"|caddr|constant|B\{|Base\#s|list|\$\(|2=:|\(def\?\)|triple\?|pair|nil\?|nil|0X\.|@'|second|library|unsingle|library\-version|base|=:|pair\?|\#s|max|0X\._|B\#|X\#s|recursive|\('\)|X\#|decimal|Digit|skip\-ifdef|0x\._|`|<\#|\[compile\]|\(0X\.\)|ten|\#|1\+!|base\#|does)(?=\s|$)/;
    var asm_words = /(?<=\s|^)(?:REWRITEVARADDRQ|@dowhile|BREMREFS|CDRQ|NULLROTRIFNOT|CALLCC|STREFR|STULE8|\}>ATEXIT|\}>|PROCREF:<\{|UNSINGLE|DICTIREPLACEGETREF|QSUB|QUIET|XC2PU|LDDICTS|@ensurebits|BLKDROP|DEBUG|STIX|DICTSETB|INLINECALL|PLDUQ|DICTUADDREF|DICTUSETREF|SETCONTARGS|DICTIADDB|STILE8|DICTREMMAX|BLKSWX|DICTIADDGET|STB|DICTDEL|DICTUSETGETOPTREF|LDZEROES|GETPARAM|CONFIGROOT|split_install|REPEATEND|FALSE|STRPRINT|ROLL|ATEXIT|DIVC|DICTISETB|DICTUGETPREVEQ|@adj\-long\-proc|DUMPTOSFMT|CALL|PLDUX|STURQ|CDR|DICTISETREF|DICTSETREF|DICTREPLACEGET|CDDR|THROW|POPCTR|RETURNARGS|PLDSLICEX|ISNAN|PLDSLICEQ|IFNOTRET|BOOLOR|NULLSWAPIFNOT|QPOW2|STU|UNTRIPLE|PLDILE8|SDBEGINS|PLDDICT|STUQ|STOPTREF|DICTGETREF|@range|DICTUSETB|INDEX3|TUPLE|CHKSIGNU|DICTUGETNEXTEQ|ONE|PLDIX|SPLITQ|SAVE|UNTIL:<\{|@inline|DEPTH|STSAME|DICTGET|WHILE:<\{|\}END>s|NEQ|@chkdicts|\}>DO<\{|@Defop\(8i,alt\)|XCPU|DICTUGETNEXT|SETTHIRD|IFBITJMP|DICTIGETEXEC|DICTSETGET|QDIVMOD|PREPARE|si\(\)|PLDIXQ|MODPOW2\#|CHKTUPLE|JMP|QDIVC|\}>CATCH<\{|LDOPTREF|DICTISETGETB|@doifelse|INDEXQ|RETFALSE|PLDUZ|SHA256U|DICTREPLACEREF|IF|CDDDR|SETCPX|DICTISET|CTOS|TUPLEVAR|QNOT|SBITS|ROLLX|DICTUREPLACEGET|\}>AGAIN|DICTUGETPREV|DICTUREMMIN|DUMPSTK|MINMAX|SETINDEXVAR|STREFRQ|DICTIREMMINREF|SETCP|IF:|NOP|@Defop|PLDREFIDX|SDPFXREV|DICTUADD|PUSHCONT|PLDIQ|JMPREFDATA|@Defop\(c\)|LDU_l|MULRSHIFTR\#|BLOCKLT|IFNBITJMP|JMPREF|c4|DICTIGETNEXTEQ|LDULE4Q|SDSFX|CARQ|QDIV|UNPACKFIRSTVAR|CONDSEL|run_ticktock|@sridx\+|DICTIGETNEXT|ISNPOS|QINC|LDULE4|PARSEMSGADDR|INDEXVARQ|STSLICEQ|COMPOS|PUSHROOT|STDICTS|QNEGATE|BOOLAND|SETTHIRDQ|@Defop\(8u\)|FIRSTQ|s1|split_prepare|STVARINT16|c3|PFXDICTADD|AGAIN:|SETGLOB|@Defop\(s\)|QUFITSX|\}>SETEXITALT|DICTUSET|RSHIFTR\#|AGAIN|SETEXITALT:<\{|QDEC|STBREF|DICTUADDGETB|DICTGETPREV|SDCUTFIRST|PREPAREDICT|QADD|GREATER|SKIPOPTREF|RET|DICTADDREF|QMULDIVR|DICTUSETGETB|SUBCONST|IFNOT|SUB|TVM_Asm|CONT|SDPPFX|SDBEGINSX|ONLYTOPX|BREMBITS|@simpleuop|\}>REPEAT|CHKNAN|SCHKBITS|FIRST|POP|\}>IF|IFNOTJMP:<\{|STUR|STIQ|BLKPUSH|INVERT|ENDC|\}END>c|DICTUREPLACEGETB|LOGSTR|IFNOTJMP|STREF|LDSLICE_l|BBITREFS|MULRSHIFTR|DICTADDGET|ATEXITALT|STSLICE|\}>CONT|ATEXITALT:<\{|PFXDICTREPLACE|UFITSX|hash>libref|PRINT|s7|@procdictkeylen|PFXDICTGET|SCUTFIRST|DICTUGETOPTREF|SETCONTCTRX|IFRETALT|MYADDR|@iscr\?|XCHG|LSHIFT\#DIV|s4|DICTADD|SDSFXREV|@doifnotelse|XCHG2|SETCONTVARARGS|STIRQ|RETURNVARARGS|2SWAP|AGAIN:<\{|RSHIFT|TRYARGS|SDFIRST|SKIPDICT|LOGFLUSH|DICTSETGETB|ABS|LDUQ|s0|PICK|THROWARGANYIF|BCHKBITREFSQ|DICTIREPLACEGET|DICTIDELGET|SDPSFXREV|SCHKREFSQ|STRDUMP|ROTREV|LDSAME|DICTUREPLACEREF|STU_l|DICTUGETJMP|2DROP|UNPAIR|CALLCCARGS|XCHG3_l|RETARGS|SDCUTLAST|DICTIREPLACE|DICTIREMMIN|SUBDICTIRPGET|PLDSLICE|SETFIRST|RAWRESERVEX|add\-lib|STBREFQ|STUX|JMPDICT|LSHIFT\#DIVR|ISTUPLE|@Defop\(8i\)|DICTUMAX|LDILE8|DICTGETNEXTEQ|IFJMP:<\{|@atend|SAVECTR|DICTIADDREF|SETGASLIMIT|@PROC:<\{|PUSHX|ROLLREVX|TRY:<\{|DICTDELGET|THENRETALT|ROT|LDUXQ|SUBDICTURPGET|RSHIFT\#|PUXC2|SETFIRSTQ|DEBUGOFF|PLDILE8Q|QMULDIVMOD|DICTADDGETB|STIXR|THROWARGIFNOT|SETSECOND|LDUX|SCUTLAST|s\(\)|IF:<\{|PUXCPU|SDSKIPFIRST|CADR|DICTGETNEXT|PLDSLICEXQ|ROLLREV|DICTIADD|RAWRESERVE|@Creg|@trycatch|PARSEMSGADDRQ|MULDIV|SETINDEXQ|QAND|DICTIGETPREVEQ|CMP|s11|s6|STULE4|MULDIVR|s\(\-2\)|SDPFX|DICTUSETGETREF|GEQINT|@Defop\(4u,4u\)|s15|EQUAL|PLDILE4|UNTILEND|PUSHCTRX|STREF2CONST|LDMSGADDR|WHILE|WHILEEND|BLESSARGS|QSUBR|DEBUGSTRI|INLINECALLDICT|BITSIZE|IFREF|PLDDICTQ|SEMPTY|QUFITS|UNTIL|DICTIGETJMP|LDMSGADDRQ|BLESSNUMARGS|PAIR|STSLICE_l|QDIVMODC|NEGATE|LDULE8|DICTSET|DICTIGETREF|CALLXARGS|SDCNTLEAD0|@scomplete|POPSAVE|DROP2|SBITREFS|SLICE|SAVEALT|LDIQ|GETGLOBVAR|DICTUMINREF|BCONCATQ|SAVEALTCTR|SETCP0|MULRSHIFT|PFXDICTGETEXEC|@rangechk|\}>ELSE:|BBITS|BCHKBITSQ\#|PUSHNULL|PFXDICTSET|SDBEGINS:imm|QLSHIFT|BCHKREFS|BCHKREFSQ|@newproc|s13|DICTISETGETOPTREF|PLDREFVAR|THROWANYIF|LDU|ISNNEG|PFXDICTGETJMP|NULLSWAPIF|PLDILE4Q|c0|ZERO|DICTIREPLACEB|IFJMP|PUSHREF|STUXRQ|s14|SETCONTCTR|ADDINT|PU2XC|@Sreg|SPLIT|XCHG0|LDONES|@proclistadd|STREFCONST|DICTISETGETREF|XCPUXC|SETINDEX|CALLREF|PFXDICTCONSTGETJMP|DICTUREPLACEB|THIRDQ|SSKIPLAST|THROWIFNOT|STIR|DICTREPLACEGETREF|MULINT|DICTSETGETREF|<\{|LEQINT|TRIPLE|IFJMPREF|SETEXITALT|UBITSIZE|EXPLODEVAR|ROT2|DICTIDELGETREF|HASHCU|\-ROT|DEC|ONLYX|STSLICER|XCHGX|STGRAMS|PUSHCTR|DEBUGON|SREFS|IFBITJMPREF|PUSHREFCONT|IFNOTREF|MIN|DICTSETGETOPTREF|XOR|GEQ|HASHSU|\}>DO:|SUBINT|PLDOPTREF|THROWARGANYIFNOT|POPROOT|THIRD|UNPACKFIRST|STBREFR_l|STBREFR|@sridxrange|BLKSWAP|STZEROES|REPEAT:<\{|PLDULE8|SETNUMARGS|s12|DICTUMAXREF|STI_l|c1|INDEXVAR|SUBDICTUGET|SCHKBITREFSQ|DICTUDELGETREF|SETCODE|SDEQ|@ensurebitrefs|POPCTRSAVE|DROP|DICTUGET|CONS|SDCNTLEAD1|\}END>|STZERO|TPUSH|LSHIFT|PREPAREVAR|MOD|DICTGETOPTREF|@sridx|IFNOTRETALT|DICTUREPLACE|CONFIGOPTPARAM|TEN|PUSHNEGPOW2|STONES|DICTIDEL|STREFQ|@cridx|DICTUDEL|XCHG3|FITSX|SINGLE|DICTREPLACEB|LDIX|DICTADDB|DICTMAX|DUP2|DICTIMIN|DIVMODC|LTIME|LDREF|UNTUPLE|CALLX|QRSHIFT|LSHIFTDIVR|LDILE4|PLDUXQ|UFITS|BLESSVARARGS|TPOP|DICTIMINREF|DICTREPLACE|QDIVMODR|BRANCH|PROCREF|QMOD|ENDCST|DICTIREPLACEGETB|INDEX|TUCK|\}>IFNOTJMP|JMPVAR|SCHKBITSQ|QFITSX|OR|SUBDICTGET|DICTIREMMAXREF|NOW|REWRITESTDADDR|PFXDICTDEL|CONDSELCHK|DICTUREPLACEGETREF|DUMP|JMPX|DICTUREMMAX|s3|SUBSLICE|DICTMINREF|ATEXIT:<\{|REWRITEVARADDR|SDPPFXREV|c7|STUXQ|DICTPUSHCONST|DUMPSTKTOP|DICTGETPREVEQ|\}>s|NIP|LDVARINT16|BOOLEVAL|PUSHINT|TWO|s5|CONFIGPARAM|PUXC|TRY|SDBEGINSQ:imm|AGAINEND|DICTIREPLACEREF|THROWIF|PUSH3|INC|PROGRAM\{|LDSLICEX|CALLXVARARGS|@\||QDIVR|DICTUMIN|HEXPRINT|DICTIADDGETREF|RETTRUE|JMPXARGS|QTLEN|SETNUMVARARGS|@normal\?|COMMIT|SDBEGINSQ|INTSORT2|IFNOT:<\{|AND|SUBR|STSLICECONST|PLDULE4|SCHKREFS|SGN|@pushatend|BCHKBITSQ|@fail\-ifdef|THROWARGIF|STREF_l|REPEAT:|SDSKIPLAST|DICTUADDGETREF|DIVR|SETGLOBVAR|CALLVAR|INLINE|TLEN|SDPSFX|STVARUINT16|ISNEG|SSKIPFIRST|STBR|MUL|CHKBIT|PUSHREFSLICE|PFXDICTSWITCH|\}>c|NEWC|@declproc|IFNBITJMPREF|MAX|2ROT|LDULE8Q|NEWDICT|STBREFRQ|PUSH|LESS|DICTUGETEXEC|POW2|UNCONS|CALLDICT|LDSLICEQ|LDGRAMS|s\(\-1\)|DICTIMAXREF|DICTIGET|@Defop\(8u\+1\)|s10|STILE4|DICTIGETPREV|CALLCCVARARGS|RSHIFTR|PRINTSTR|DICTUDELGET|s2|ISNULL|BINDUMP|POPCTRX|IFNOTJMPREF|DICTUADDGET|THROWARGANY|DICTDELGETREF|RETBOOL|INDEX2|@havebits|LSHIFTDIV|QOR|NULL|PUSH2|SREMPTY|@chkdict|LDVARUINT16|RANDSEED|c2|PLDULE8Q|LDILE4Q|LDSLICE|SWAP2|GTINT|QFITS|REVERSE|DICTIADDGETB|THROWANY|RETVARARGS|s9|DICTREPLACEGETB|NIL|ISPOS|DIVMODR|PFXDICTGETQ|DICTEMPTY|PLDULE4Q|@havebitrefs|EXPLODE|LAST|REWRITESTDADDRQ|SETINDEXVARQ|ISZERO|@endblk|DICTREMMINREF|recv_internal|STBQ|@Defop\(ref\)|BCHKBITREFS|SETALTCTR|CHKSIGNS|@addop|\-ROLLX|SDSUBSTR|LDDICTQ|@Defop\(4u\)|DICTUREMMINREF|\}>ELSE<\{|COMMA|recv_external|MULCONST|\-ROLL|CHKDEPTH|THROWANYIFNOT|COMPOSALT|STDICT|>libref|ADDCONST|NOT|OVER|REPEAT|UNTUPLEVAR|ACCEPT|BINPRINT|SDEMPTY|NEQINT|LDREFRTOS|CONFIGDICT|SECONDQ|XCPU2|STIXQ|c5|STONE|JMPXDATA|MULRSHIFT\#|PUSHPOW2DEC|INT|DICTUGETREF|DROPX|HEXDUMP|@\-range|RETDATA|\}>IFNOT|CADDR|COMPOSBOTH|SDCNTTRAIL1|ISNZERO|SUBDICTIGET|@chkmaindef|PROC:<\{|MULDIVMOD|REVX|@doafter<\{|BCHKBITS\#|\}>ATEXITALT|CONT:<\{|CHKBOOL|DICTREMMIN|FITS|\}>IFJMP|SECOND|GETGLOB|STUXR|LDILE8Q|DICTREMMAXREF|ADD|DIVMOD|DICTMIN|BLESS|\}END|BALANCE|@addopb|LEQ|DIV|DICTUREMMAXREF|STSLICERQ|JMPXVARARGS|@bigsridx|DICTIGETOPTREF|IFRET|OVER2|SETRETCTR|2x<=|LDSLICEXQ|PROC|EQINT|NULLROTRIF|DICTUSETGET|BCONCAT|SETSECONDQ|@def\-proc|STI|LSHIFT\#|PUSHPOW2|SWAP|SCHKBITREFS|LESSINT|SAVEBOTHCTR|DICTIMAX|THENRET|SETCONT|DUP|DICTIREMMAX|BREFS|SDBEGINSXQ|UNTIL:|LDDICT|DEBUGSTR|LDI|BCHKBITS|\}>UNTIL|2OVER|PLDI|SDLEXCMP|2DUP|CAR|SUBDICTRPGET|PUSHSLICE|STBRQ|SENDRAWMSG|@procdict|STIXRQ|PLDREF|ENDS|s8|THROWARG|@Defop\(s,s\)|RETALT|LDIXQ|DICTISETGET|SAVEBOTH|SDCNTTRAIL0|PUSHNAN|IFNOT:|QXOR|PLDU|QMUL|LDI_l|DICTADDGETREF|DICTUADDB|DICTMAXREF|PLDDICTS|EXECUTE|IFELSE|BREMBITREFS|TRUE|c\(\))(?=\s|$)/;
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
                { regex: /(?<=\s|^)(?:c[0-7])|(?:s[0-9]{1,3})(?=\s|$)/, token: "meta" },
                // asm constants: main, recv_internal, recv_external and booleans
                { regex: /(?<=\s|^)(?:main|recv_internal|recv_external|true|false)(?=\s|$)/, token: "meta" },
                // numbers
                { regex: /(?<=\s|^)(?:(?:[+-]?[0-9]+\.?[0-9]*)|(?:0x[0-9a-f]+)|(?:0b[01]+))(?=\s|$)/i, token: "number" },
                // builder <b ... b> and cell <s ... s> (multiline)
                { regex: /(?<=\s|^)\<[bs](?=\s|$)/, token: "atom", indent: true },
                { regex: /(?<=\s|^)[bs]\>(?=\s|$)/, token: "atom", dedent: true },
                // asm code <{ ... }>s/c and control instructions PROC, CONT, REPEAT, IF, ELSE, etc (multiline)
                { regex: /(?<=\s|^)[A-Z:]*<\{(?=\s|$)/, token: "builtin", indent: true },
                { regex: /(?<=\s|^)(?:\}(?:END)?>[A-Zsc:]?)(?=\s|$)/, token: "bultin", dedent: true },
                { regex: /(?<=\s|^)(?:\}>[A-Z]+<\{)(?=\s|$)/, token: "builtin" },
                // word lists { ... } (multiline)
                { regex: /(?<=\s|^)\{(?=\s|$)/, token: "variable-2", indent: true },
                { regex: /(?<=\s|^)\}(?=\s|$)/, token: "variable-2", dedent: true },
                // definitions
                { regex: /(?<=\s|^)(?:2?constant|variable|2?=:|:_?|::_?)(?=\s|$)/, token: "keyword", next: "def" },
                // words
                { regex: /(?<=\s|^)(?:abort)(?=\")/, token: "keyword" },
                { regex: builtin_words, token: "keyword"},
                { regex: fift_words, token: "variable"},
                { regex: asm_words, token: "operator"},
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
    }
    return { onload: onload }
});