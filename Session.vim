let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
let VM_check_mappings =  1 
let VM_mouse_mappings =  0 
let VM_default_mappings =  1 
let VM_persistent_registers =  0 
let VM_highlight_matches = "underline"
let BufferlinePositions = "[\"/home/tcwhite/Projects/C-C++/shell/src/command.c\",\"/home/tcwhite/Projects/C-C++/shell/src/command.h\",\"/home/tcwhite/Projects/C-C++/shell/src/main.c\",\"/home/tcwhite/Projects/C-C++/shell/src/print.c\",\"/home/tcwhite/Projects/C-C++/shell/src/print.h\",\"/home/tcwhite/Projects/C-C++/shell/src/tokenize.c\",\"/home/tcwhite/Projects/C-C++/shell/src/tokenize.h\",\"/home/tcwhite/Projects/C-C++/shell/Makefile\"]"
silent only
silent tabonly
cd ~/Projects/C-C++/shell
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
if &shortmess =~ 'A'
  set shortmess=aoOA
else
  set shortmess=aoO
endif
badd +23 ~/Projects/C-C++/shell/Makefile
badd +55 ~/Projects/C-C++/shell/src/command.c
badd +17 ~/Projects/C-C++/shell/src/command.h
badd +13 ~/Projects/C-C++/shell/src/main.c
argglobal
%argdel
$argadd .
edit ~/Projects/C-C++/shell/src/command.c
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
argglobal
balt ~/Projects/C-C++/shell/src/command.h
setlocal foldmethod=expr
setlocal foldexpr=v:lua.vim.lsp.foldexpr()
setlocal foldmarker={{{,}}}
setlocal foldignore=#
setlocal foldlevel=99
setlocal foldminlines=1
setlocal foldnestmax=20
setlocal foldenable
26
sil! normal! zo
39
sil! normal! zo
let s:l = 55 - ((21 * winheight(0) + 23) / 47)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 55
normal! 015|
tabnext 1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
set hlsearch
nohlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
