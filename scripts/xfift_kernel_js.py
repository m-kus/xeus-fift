import re
from os.path import dirname, join

project_dir = dirname(dirname(__file__))

# I know, I should have parse those from the source files, but I'm too lazy :)
words = """
!  " $+  $,  $=  $>B  $>s  $>smca  $@  $@+  $@?  $@?+  $Len  $Split  $cmp  $len  $reverse  $|  %1<<  '  'nop  (-trailing)  (.)  (X.)  (atom)  (b.)  (char)  (compile)  (create)  (dump)  (execute)  (forget)  (hex-number)  (number)  (word)  (x.)  ({)  (})  *  */  */c  */cmod  */mod  */r  */rmod  *>>  *>>c  *>>r  *mod  +  ,  -  -1  -1<<  -roll  -rot  -trailing  -trailing0  .  ._  .dump  .l  .s  .sl  .tc  /  /c  /cmod  /mod  /r  /rmod  0  0<  0<=  0<>  0=  0>  0>=  1  1+  1-  1<<  1<<1-  2  2*  2+  2-  2/  2drop  2dup  2over  2swap  :  ::  ::_  :_  <  <<  <</  <</c  <</r  <=  <>  <b  <pop>  <push>  <s  <xchg>  =  >  >=  >>  >>c  >>r  ?dup  @  B+  B,  B=  B>$  B>Li@  B>Li@+  B>Lu@  B>Lu@+  B>X  B>base64  B>base64url  B>boc  B>file  B>i@  B>i@+  B>u@  B>u@+  B>x  B@  B@+  B@?  B@?+  Bcmp  Bhash  BhashB  Bhashu  Blen  Bx.  B|  Li>B  Lu>B  X.  X._  [  []  []=  ]  abort  allot  and  anon  atom>$  atom?  b+  b.  b._  b>  b>idict!  b>idict!+  b>spec  b>udict!  b>udict!+  base64>B  base64url>B  bbitrefs  bbits  bl  boc+>B  boc>B  box  box?  brefs  brembitrefs  brembits  bremrefs  bye  b{ char  chr  cmod  cmp  cond  count  cr  crc16  crc32  crc32c  create  csr.  dbrunvm  dbrunvm-parallel  depth  dict,  dict>s  dict@  dict@+  dictdiff  dictforeach  dictmap  dictmapext  dictmerge  dictnew  drop  dup  ed25519_chksign  ed25519_sign  ed25519_sign_uint  emit  empty?  eq?  eqv?  exch  exch2  execute  explode  false  file-exists?  file>B  filepart>B  find  fits  forget  gasrunvm  gasrunvmcode  gasrunvmctx  gasrunvmdict  getenv  getenv?  halt  hash  hashB  hashu  hold  hole  i,  i>B  i@  i@+  i@?  i@?+  idict!  idict!+  idict@  if  ifnot  include  integer?  mod  negate  newkeypair  nip  nop  not  now  null  null?  or  over  pfxdict!  pfxdict!+  pfxdict@  pick  priv>pub  quit  ref,  ref@  ref@+  ref@?  ref@?+  remaining  reverse  rmod  roll  rot  runvm  runvmcode  runvmctx  runvmdict  s,  s>  sbitrefs  sbits  sgn  skip-to-eof  skipspc  smca>$  space  sr,  srefs  string?  swap  times  tpop  true  tuck  tuple  tuple?  type  u,  u>B  u@  u@+  u@?  u@?+  udict!  udict!+  udict@  ufits  until  untuple  vmlibs  while  word  words  x.  x._  x>B  x>B?  xor  x{ {  |  |+  |_  }
"""
fift_words = """
!  " #  #>  #s  $( $+  $,  $=  $>B  $>s  $>smca  $@  $@+  $@?  $@?+  $Len  $Split  $cmp  $len  $reverse  $|  %1<<  '  'nop  (')  (-trailing)  (.)  (0X.)  (0x.)  (X.)  (atom)  (b.)  (char)  (compile)  (create)  (def?)  (dump)  (execute)  (forget)  (hex-number)  (number)  (word)  (x.)  ({)  (})  *  */  */c  */cmod  */mod  */r  */rmod  *>>  *>>c  *>>r  *mod  +  +!  +" ,  -  -!  -1  -1<<  -roll  -rot  -trailing  -trailing0  .  ." ._  .dump  .l  .s  .sl  .tc  /  /*  //  /c  /cmod  /mod  /r  /rmod  0  0!  0<  0<=  0<>  0=  0>  0>=  0X.  0X._  0x.  0x._  1  1+  1+!  1-  1-!  1<<  1<<1-  2  2*  2+  2-  2/  2=:  2constant  2drop  2dup  2over  2swap  :  ::  ::_  :_  <  <#  <<  <</  <</c  <</r  <=  <>  <b  <pop>  <push>  <s  <xchg>  =  =:  >  >=  >>  >>c  >>r  ?dup  @  @'  B#  B#s  B+  B,  B=  B>$  B>Li@  B>Li@+  B>Lu@  B>Lu@+  B>X  B>base64  B>base64url  B>boc  B>file  B>i@  B>i@+  B>u@  B>u@+  B>x  B@  B@+  B@?  B@?+  Base#  Base#s  Bcmp  Bhash  BhashB  Bhashu  Blen  Bx.  B{ B|  Digit  Li>B  Lu>B  X#  X#s  X.  X._  [  []  []=  [compile]  [forget]  ]  ` abort  abort" abs  allot  and  anon  atom  atom>$  atom?  b#  b#s  b+  b.  b._  b>  b>idict!  b>idict!+  b>spec  b>udict!  b>udict!+  base  base#  base#s  base64>B  base64url>B  bbitrefs  bbits  binary  bl  boc+>B  boc>B  box  box?  brefs  brembitrefs  brembits  bremrefs  bye  b{ caddr  cadr  car  cddr  cdr  char  chr  cmod  cmp  cond  cons  constant  count  cr  crc16  crc32  crc32c  create  csr.  dbrunvm  dbrunvm-parallel  decimal  def?  depth  dict,  dict>s  dict@  dict@+  dictdiff  dictforeach  dictmap  dictmapext  dictmerge  dictnew  digit  does  drop  dup  ed25519_chksign  ed25519_sign  ed25519_sign_uint  emit  empty?  eq?  eqv?  exch  exch2  execute  explode  false  file-exists?  file>B  filepart>B  find  first  fits  forget  gasrunvm  gasrunvmcode  gasrunvmctx  gasrunvmdict  getenv  getenv?  halt  hash  hashB  hashu  hex  hold  hole  i,  i>B  i@  i@+  i@?  i@?+  idict!  idict!+  idict@  if  ifnot  include  integer?  library  library-version  list  max  min  minmax  mod  negate  newkeypair  nil  nil?  nip  nop  not  now  null  null!  null?  octal  or  over  pair  pair?  pfxdict!  pfxdict!+  pfxdict@  pick  priv>pub  quit  recursive  ref,  ref@  ref@+  ref@?  ref@?+  remaining  reverse  rmod  roll  rot  runvm  runvmcode  runvmctx  runvmdict  s,  s-fits?  s>  s>c  sbitrefs  sbits  second  sgn  shash  sign  single  single?  skip-ifdef  skip-to-eof  skipspc  smca>$  space  sr,  srefs  string?  swap  ten  third  times  tpop  triple  triple?  true  tuck  tuple  tuple-len?  tuple?  type  u,  u>B  u@  u@+  u@?  u@?+  udict!  udict!+  udict@  ufits  uncons  undef?  unpair  unsingle  until  untriple  untuple  variable  vmlibs  while  word  words  x#  x#s  x.  x._  x>B  x>B?  xor  x{ {  |  |+  |_  }
"""
asm_fift_words = """
!  " #  #>  #s  $( $+  $,  $=  $>B  $>s  $>smca  $@  $@+  $@?  $@?+  $Len  $Split  $cmp  $len  $reverse  $|  %1<<  '  'nop  (')  (-trailing)  (.)  (0X.)  (0x.)  (X.)  (atom)  (b.)  (char)  (compile)  (create)  (def?)  (dump)  (execute)  (forget)  (hex-number)  (number)  (word)  (x.)  ({)  (})  *  */  */c  */cmod  */mod  */r  */rmod  *>>  *>>c  *>>r  *mod  +  +!  +" ,  -  -!  -1  -1<<  -ROLL  -ROLLX  -ROT  -roll  -rot  -trailing  -trailing0  .  ." ._  .dump  .l  .s  .sl  .tc  /  /*  //  /c  /cmod  /mod  /r  /rmod  0  0!  0<  0<=  0<>  0=  0>  0>=  0X.  0X._  0x.  0x._  1  1+  1+!  1-  1-!  1<<  1<<1-  2  2*  2+  2-  2/  2=:  2DROP  2DUP  2OVER  2ROT  2SWAP  2constant  2drop  2dup  2over  2swap  2x<=  :  ::  ::_  :_  <  <#  <<  <</  <</c  <</r  <=  <>  <b  <pop>  <push>  <s  <xchg>  <{  =  =:  >  >=  >>  >>c  >>r  >libref  ?dup  @  @'  @-range  @Creg  @Defop  @Defop(4u)  @Defop(4u,4u)  @Defop(8i)  @Defop(8i,alt)  @Defop(8u)  @Defop(8u+1)  @Defop(c)  @Defop(ref)  @Defop(s)  @Defop(s,s)  @PROC:<{  @Sreg  @addop  @addopb  @adj-long-proc  @atend  @bigsridx  @chkdict  @chkdicts  @chkmaindef  @cridx  @declproc  @def-proc  @doafter<{  @doifelse  @doifnotelse  @dowhile  @endblk  @ensurebitrefs  @ensurebits  @fail-ifdef  @havebitrefs  @havebits  @inline  @iscr?  @newproc  @normal?  @procdict  @procdictkeylen  @proclistadd  @pushatend  @range  @rangechk  @scomplete  @simpleuop  @sridx  @sridx+  @sridxrange  @trycatch  @|  ABS  ACCEPT  ADD  ADDCONST  ADDINT  AGAIN  AGAIN:  AGAIN:<{  AGAINEND  AND  ATEXIT  ATEXIT:<{  ATEXITALT  ATEXITALT:<{  B#  B#s  B+  B,  B=  B>$  B>Li@  B>Li@+  B>Lu@  B>Lu@+  B>X  B>base64  B>base64url  B>boc  B>file  B>i@  B>i@+  B>u@  B>u@+  B>x  B@  B@+  B@?  B@?+  BALANCE  BBITREFS  BBITS  BCHKBITREFS  BCHKBITREFSQ  BCHKBITS  BCHKBITS#  BCHKBITSQ  BCHKBITSQ#  BCHKREFS  BCHKREFSQ  BCONCAT  BCONCATQ  BINDUMP  BINPRINT  BITSIZE  BLESS  BLESSARGS  BLESSNUMARGS  BLESSVARARGS  BLKDROP  BLKPUSH  BLKSWAP  BLKSWX  BLOCKLT  BOOLAND  BOOLEVAL  BOOLOR  BRANCH  BREFS  BREMBITREFS  BREMBITS  BREMREFS  Base#  Base#s  Bcmp  Bhash  BhashB  Bhashu  Blen  Bx.  B{ B|  CADDR  CADR  CALL  CALLCC  CALLCCARGS  CALLCCVARARGS  CALLDICT  CALLREF  CALLVAR  CALLX  CALLXARGS  CALLXVARARGS  CAR  CARQ  CDDDR  CDDR  CDR  CDRQ  CHKBIT  CHKBOOL  CHKDEPTH  CHKNAN  CHKSIGNS  CHKSIGNU  CHKTUPLE  CMP  COMMA  COMMIT  COMPOS  COMPOSALT  COMPOSBOTH  CONDSEL  CONDSELCHK  CONFIGDICT  CONFIGOPTPARAM  CONFIGPARAM  CONFIGROOT  CONS  CONT  CONT:<{  CTOS  DEBUG  DEBUGOFF  DEBUGON  DEBUGSTR  DEBUGSTRI  DEC  DEPTH  DICTADD  DICTADDB  DICTADDGET  DICTADDGETB  DICTADDGETREF  DICTADDREF  DICTDEL  DICTDELGET  DICTDELGETREF  DICTEMPTY  DICTGET  DICTGETNEXT  DICTGETNEXTEQ  DICTGETOPTREF  DICTGETPREV  DICTGETPREVEQ  DICTGETREF  DICTIADD  DICTIADDB  DICTIADDGET  DICTIADDGETB  DICTIADDGETREF  DICTIADDREF  DICTIDEL  DICTIDELGET  DICTIDELGETREF  DICTIGET  DICTIGETEXEC  DICTIGETJMP  DICTIGETNEXT  DICTIGETNEXTEQ  DICTIGETOPTREF  DICTIGETPREV  DICTIGETPREVEQ  DICTIGETREF  DICTIMAX  DICTIMAXREF  DICTIMIN  DICTIMINREF  DICTIREMMAX  DICTIREMMAXREF  DICTIREMMIN  DICTIREMMINREF  DICTIREPLACE  DICTIREPLACEB  DICTIREPLACEGET  DICTIREPLACEGETB  DICTIREPLACEGETREF  DICTIREPLACEREF  DICTISET  DICTISETB  DICTISETGET  DICTISETGETB  DICTISETGETOPTREF  DICTISETGETREF  DICTISETREF  DICTMAX  DICTMAXREF  DICTMIN  DICTMINREF  DICTPUSHCONST  DICTREMMAX  DICTREMMAXREF  DICTREMMIN  DICTREMMINREF  DICTREPLACE  DICTREPLACEB  DICTREPLACEGET  DICTREPLACEGETB  DICTREPLACEGETREF  DICTREPLACEREF  DICTSET  DICTSETB  DICTSETGET  DICTSETGETB  DICTSETGETOPTREF  DICTSETGETREF  DICTSETREF  DICTUADD  DICTUADDB  DICTUADDGET  DICTUADDGETB  DICTUADDGETREF  DICTUADDREF  DICTUDEL  DICTUDELGET  DICTUDELGETREF  DICTUGET  DICTUGETEXEC  DICTUGETJMP  DICTUGETNEXT  DICTUGETNEXTEQ  DICTUGETOPTREF  DICTUGETPREV  DICTUGETPREVEQ  DICTUGETREF  DICTUMAX  DICTUMAXREF  DICTUMIN  DICTUMINREF  DICTUREMMAX  DICTUREMMAXREF  DICTUREMMIN  DICTUREMMINREF  DICTUREPLACE  DICTUREPLACEB  DICTUREPLACEGET  DICTUREPLACEGETB  DICTUREPLACEGETREF  DICTUREPLACEREF  DICTUSET  DICTUSETB  DICTUSETGET  DICTUSETGETB  DICTUSETGETOPTREF  DICTUSETGETREF  DICTUSETREF  DIV  DIVC  DIVMOD  DIVMODC  DIVMODR  DIVR  DROP  DROP2  DROPX  DUMP  DUMPSTK  DUMPSTKTOP  DUMPTOSFMT  DUP  DUP2  Digit  ENDC  ENDCST  ENDS  EQINT  EQUAL  EXECUTE  EXPLODE  EXPLODEVAR  FALSE  FIRST  FIRSTQ  FITS  FITSX  GEQ  GEQINT  GETGLOB  GETGLOBVAR  GETPARAM  GREATER  GTINT  HASHCU  HASHSU  HEXDUMP  HEXPRINT  IF  IF:  IF:<{  IFBITJMP  IFBITJMPREF  IFELSE  IFJMP  IFJMP:<{  IFJMPREF  IFNBITJMP  IFNBITJMPREF  IFNOT  IFNOT:  IFNOT:<{  IFNOTJMP  IFNOTJMP:<{  IFNOTJMPREF  IFNOTREF  IFNOTRET  IFNOTRETALT  IFREF  IFRET  IFRETALT  INC  INDEX  INDEX2  INDEX3  INDEXQ  INDEXVAR  INDEXVARQ  INLINE  INLINECALL  INLINECALLDICT  INT  INTSORT2  INVERT  ISNAN  ISNEG  ISNNEG  ISNPOS  ISNULL  ISNZERO  ISPOS  ISTUPLE  ISZERO  JMP  JMPDICT  JMPREF  JMPREFDATA  JMPVAR  JMPX  JMPXARGS  JMPXDATA  JMPXVARARGS  LAST  LDDICT  LDDICTQ  LDDICTS  LDGRAMS  LDI  LDILE4  LDILE4Q  LDILE8  LDILE8Q  LDIQ  LDIX  LDIXQ  LDI_l  LDMSGADDR  LDMSGADDRQ  LDONES  LDOPTREF  LDREF  LDREFRTOS  LDSAME  LDSLICE  LDSLICEQ  LDSLICEX  LDSLICEXQ  LDSLICE_l  LDU  LDULE4  LDULE4Q  LDULE8  LDULE8Q  LDUQ  LDUX  LDUXQ  LDU_l  LDVARINT16  LDVARUINT16  LDZEROES  LEQ  LEQINT  LESS  LESSINT  LOGFLUSH  LOGSTR  LSHIFT  LSHIFT#  LSHIFT#DIV  LSHIFT#DIVR  LSHIFTDIV  LSHIFTDIVR  LTIME  Li>B  Lu>B  MAX  MIN  MINMAX  MOD  MODPOW2#  MUL  MULCONST  MULDIV  MULDIVMOD  MULDIVR  MULINT  MULRSHIFT  MULRSHIFT#  MULRSHIFTR  MULRSHIFTR#  MYADDR  NEGATE  NEQ  NEQINT  NEWC  NEWDICT  NIL  NIP  NOP  NOT  NOW  NULL  NULLROTRIF  NULLROTRIFNOT  NULLSWAPIF  NULLSWAPIFNOT  ONE  ONLYTOPX  ONLYX  OR  OVER  OVER2  PAIR  PARSEMSGADDR  PARSEMSGADDRQ  PFXDICTADD  PFXDICTCONSTGETJMP  PFXDICTDEL  PFXDICTGET  PFXDICTGETEXEC  PFXDICTGETJMP  PFXDICTGETQ  PFXDICTREPLACE  PFXDICTSET  PFXDICTSWITCH  PICK  PLDDICT  PLDDICTQ  PLDDICTS  PLDI  PLDILE4  PLDILE4Q  PLDILE8  PLDILE8Q  PLDIQ  PLDIX  PLDIXQ  PLDOPTREF  PLDREF  PLDREFIDX  PLDREFVAR  PLDSLICE  PLDSLICEQ  PLDSLICEX  PLDSLICEXQ  PLDU  PLDULE4  PLDULE4Q  PLDULE8  PLDULE8Q  PLDUQ  PLDUX  PLDUXQ  PLDUZ  POP  POPCTR  POPCTRSAVE  POPCTRX  POPROOT  POPSAVE  POW2  PREPARE  PREPAREDICT  PREPAREVAR  PRINT  PRINTSTR  PROC  PROC:<{  PROCREF  PROCREF:<{  PROGRAM{  PU2XC  PUSH  PUSH2  PUSH3  PUSHCONT  PUSHCTR  PUSHCTRX  PUSHINT  PUSHNAN  PUSHNEGPOW2  PUSHNULL  PUSHPOW2  PUSHPOW2DEC  PUSHREF  PUSHREFCONT  PUSHREFSLICE  PUSHROOT  PUSHSLICE  PUSHX  PUXC  PUXC2  PUXCPU  QADD  QAND  QDEC  QDIV  QDIVC  QDIVMOD  QDIVMODC  QDIVMODR  QDIVR  QFITS  QFITSX  QINC  QLSHIFT  QMOD  QMUL  QMULDIVMOD  QMULDIVR  QNEGATE  QNOT  QOR  QPOW2  QRSHIFT  QSUB  QSUBR  QTLEN  QUFITS  QUFITSX  QUIET  QXOR  RANDSEED  RAWRESERVE  RAWRESERVEX  REPEAT  REPEAT:  REPEAT:<{  REPEATEND  RET  RETALT  RETARGS  RETBOOL  RETDATA  RETFALSE  RETTRUE  RETURNARGS  RETURNVARARGS  RETVARARGS  REVERSE  REVX  REWRITESTDADDR  REWRITESTDADDRQ  REWRITEVARADDR  REWRITEVARADDRQ  ROLL  ROLLREV  ROLLREVX  ROLLX  ROT  ROT2  ROTREV  RSHIFT  RSHIFT#  RSHIFTR  RSHIFTR#  SAVE  SAVEALT  SAVEALTCTR  SAVEBOTH  SAVEBOTHCTR  SAVECTR  SBITREFS  SBITS  SCHKBITREFS  SCHKBITREFSQ  SCHKBITS  SCHKBITSQ  SCHKREFS  SCHKREFSQ  SCUTFIRST  SCUTLAST  SDBEGINS  SDBEGINS:imm  SDBEGINSQ  SDBEGINSQ:imm  SDBEGINSX  SDBEGINSXQ  SDCNTLEAD0  SDCNTLEAD1  SDCNTTRAIL0  SDCNTTRAIL1  SDCUTFIRST  SDCUTLAST  SDEMPTY  SDEQ  SDFIRST  SDLEXCMP  SDPFX  SDPFXREV  SDPPFX  SDPPFXREV  SDPSFX  SDPSFXREV  SDSFX  SDSFXREV  SDSKIPFIRST  SDSKIPLAST  SDSUBSTR  SECOND  SECONDQ  SEMPTY  SENDRAWMSG  SETALTCTR  SETCODE  SETCONT  SETCONTARGS  SETCONTCTR  SETCONTCTRX  SETCONTVARARGS  SETCP  SETCP0  SETCPX  SETEXITALT  SETEXITALT:<{  SETFIRST  SETFIRSTQ  SETGASLIMIT  SETGLOB  SETGLOBVAR  SETINDEX  SETINDEXQ  SETINDEXVAR  SETINDEXVARQ  SETNUMARGS  SETNUMVARARGS  SETRETCTR  SETSECOND  SETSECONDQ  SETTHIRD  SETTHIRDQ  SGN  SHA256U  SINGLE  SKIPDICT  SKIPOPTREF  SLICE  SPLIT  SPLITQ  SREFS  SREMPTY  SSKIPFIRST  SSKIPLAST  STB  STBQ  STBR  STBREF  STBREFQ  STBREFR  STBREFRQ  STBREFR_l  STBRQ  STDICT  STDICTS  STGRAMS  STI  STILE4  STILE8  STIQ  STIR  STIRQ  STIX  STIXQ  STIXR  STIXRQ  STI_l  STONE  STONES  STOPTREF  STRDUMP  STREF  STREF2CONST  STREFCONST  STREFQ  STREFR  STREFRQ  STREF_l  STRPRINT  STSAME  STSLICE  STSLICECONST  STSLICEQ  STSLICER  STSLICERQ  STSLICE_l  STU  STULE4  STULE8  STUQ  STUR  STURQ  STUX  STUXQ  STUXR  STUXRQ  STU_l  STVARINT16  STVARUINT16  STZERO  STZEROES  SUB  SUBCONST  SUBDICTGET  SUBDICTIGET  SUBDICTIRPGET  SUBDICTRPGET  SUBDICTUGET  SUBDICTURPGET  SUBINT  SUBR  SUBSLICE  SWAP  SWAP2  TEN  THENRET  THENRETALT  THIRD  THIRDQ  THROW  THROWANY  THROWANYIF  THROWANYIFNOT  THROWARG  THROWARGANY  THROWARGANYIF  THROWARGANYIFNOT  THROWARGIF  THROWARGIFNOT  THROWIF  THROWIFNOT  TLEN  TPOP  TPUSH  TRIPLE  TRUE  TRY  TRY:<{  TRYARGS  TUCK  TUPLE  TUPLEVAR  TVM_Asm  TWO  UBITSIZE  UFITS  UFITSX  UNCONS  UNPACKFIRST  UNPACKFIRSTVAR  UNPAIR  UNSINGLE  UNTIL  UNTIL:  UNTIL:<{  UNTILEND  UNTRIPLE  UNTUPLE  UNTUPLEVAR  WHILE  WHILE:<{  WHILEEND  X#  X#s  X.  X._  XC2PU  XCHG  XCHG0  XCHG2  XCHG3  XCHG3_l  XCHGX  XCPU  XCPU2  XCPUXC  XOR  ZERO  [  []  []=  [compile]  [forget]  ]  ` abort  abort" abs  add-lib  allot  and  anon  atom  atom>$  atom?  b#  b#s  b+  b.  b._  b>  b>idict!  b>idict!+  b>spec  b>udict!  b>udict!+  base  base#  base#s  base64>B  base64url>B  bbitrefs  bbits  binary  bl  boc+>B  boc>B  box  box?  brefs  brembitrefs  brembits  bremrefs  bye  b{ c()  c0  c1  c2  c3  c4  c5  c7  caddr  cadr  car  cddr  cdr  char  chr  cmod  cmp  cond  cons  constant  count  cr  crc16  crc32  crc32c  create  csr.  dbrunvm  dbrunvm-parallel  decimal  def?  depth  dict,  dict>s  dict@  dict@+  dictdiff  dictforeach  dictmap  dictmapext  dictmerge  dictnew  digit  does  drop  dup  ed25519_chksign  ed25519_sign  ed25519_sign_uint  emit  empty?  eq?  eqv?  exch  exch2  execute  explode  false  file-exists?  file>B  filepart>B  find  first  fits  forget  gasrunvm  gasrunvmcode  gasrunvmctx  gasrunvmdict  getenv  getenv?  halt  hash  hash>libref  hashB  hashu  hex  hold  hole  i,  i>B  i@  i@+  i@?  i@?+  idict!  idict!+  idict@  if  ifnot  include  integer?  library  library-version  list  max  min  minmax  mod  negate  newkeypair  nil  nil?  nip  nop  not  now  null  null!  null?  octal  or  over  pair  pair?  pfxdict!  pfxdict!+  pfxdict@  pick  priv>pub  quit  recursive  recv_external  recv_internal  ref,  ref@  ref@+  ref@?  ref@?+  remaining  reverse  rmod  roll  rot  run_ticktock  runvm  runvmcode  runvmctx  runvmdict  s()  s(-1)  s(-2)  s,  s-fits?  s0  s1  s10  s11  s12  s13  s14  s15  s2  s3  s4  s5  s6  s7  s8  s9  s>  s>c  sbitrefs  sbits  second  sgn  shash  si()  sign  single  single?  skip-ifdef  skip-to-eof  skipspc  smca>$  space  split_install  split_prepare  sr,  srefs  string?  swap  ten  third  times  tpop  triple  triple?  true  tuck  tuple  tuple-len?  tuple?  type  u,  u>B  u@  u@+  u@?  u@?+  udict!  udict!+  udict@  ufits  uncons  undef?  unpair  unsingle  until  untriple  untuple  variable  vmlibs  while  word  words  x#  x#s  x.  x._  x>B  x>B?  xor  x{ {  |  |+  |_  }  }>  }>AGAIN  }>ATEXIT  }>ATEXITALT  }>CATCH<{  }>CONT  }>DO:  }>DO<{  }>ELSE:  }>ELSE<{  }>IF  }>IFJMP  }>IFNOT  }>IFNOTJMP  }>REPEAT  }>SETEXITALT  }>UNTIL  }>c  }>s  }END  }END>  }END>c  }END>s
"""


def escape_split(s) -> set:
    return set(map(re.escape, s.split()))


def escape(s):
    return s.replace('"', '\\"') \
            .replace('/', '\\/')


def read_file(rel_path):
    with open(join(project_dir, rel_path)) as f:
        return f.read()


def write_file(rel_path, data):
    with open(join(project_dir, rel_path), 'w+') as f:
        return f.write(data)


def gen_kernel_js():
    builtin_set = escape_split(words)
    fift_set = escape_split(fift_words) - builtin_set
    asm_set = escape_split(asm_fift_words) - builtin_set - fift_set
    template = read_file('share/jupyter/kernels/fift/kernel.js.in')
    lines = template.split('\n')

    replacements = {
        '(builtin_words)': escape('|'.join(builtin_set)),
        '(fift_words)': escape('|'.join(fift_set)),
        '(asm_words)': escape('|'.join(asm_set)),
    }
    for placeholder, replacement in replacements.items():
        sub = next(line for line in lines if placeholder in line)
        template = template.replace(sub, sub.replace(placeholder, replacement))

    write_file('share/jupyter/kernels/fift/kernel.js', template)


if __name__ == '__main__':
    gen_kernel_js()