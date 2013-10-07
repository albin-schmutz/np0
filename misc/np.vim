" Vim syntax file

syn keyword npKeyword ARRAY BEGIN CONST DO ELSE ELSIF
syn keyword npKeyword END IF MODULE OF OR PROC
syn keyword npKeyword RECORD THEN TYPE VAR WHILE

syn keyword npType BOOL CHAR INT

syn keyword npStdConst FALSE TRUE

syn keyword npStdFunc ADDR CHR ORD SIZE SYS

syn region npComment start="{" end="}" contains=npComment,npTodo

command -nargs=+ HiLink hi def link <args>

HiLink npStdConst Boolean
HiLink npStdFunc Boolean
HiLink npType Identifier
HiLink npKeyword Statement
HiLink npComment Comment

delcommand HiLink
