begin_unit
name|'import'
name|'os'
newline|'\n'
name|'import'
name|'subprocess'
newline|'\n'
nl|'\n'
name|'from'
name|'git_remote_helpers'
op|'.'
name|'util'
name|'import'
name|'check_call'
op|','
name|'check_output'
newline|'\n'
nl|'\n'
nl|'\n'
DECL|class|GitImporter
name|'class'
name|'GitImporter'
op|'('
name|'object'
op|')'
op|':'
newline|'\n'
indent|'    '
string|'"""An importer for testgit repositories.\n\n    This importer simply delegates to git fast-import.\n    """'
newline|'\n'
nl|'\n'
DECL|member|__init__
name|'def'
name|'__init__'
op|'('
name|'self'
op|','
name|'repo'
op|')'
op|':'
newline|'\n'
indent|'        '
string|'"""Creates a new importer for the specified repo.\n        """'
newline|'\n'
nl|'\n'
name|'self'
op|'.'
name|'repo'
op|'='
name|'repo'
newline|'\n'
nl|'\n'
DECL|member|get_refs
dedent|''
name|'def'
name|'get_refs'
op|'('
name|'self'
op|','
name|'gitdir'
op|')'
op|':'
newline|'\n'
indent|'        '
string|'"""Returns a dictionary with refs.\n        """'
newline|'\n'
name|'args'
op|'='
op|'['
string|'"git"'
op|','
string|'"--git-dir="'
op|'+'
name|'gitdir'
op|','
string|'"for-each-ref"'
op|','
string|'"refs/heads"'
op|']'
newline|'\n'
name|'lines'
op|'='
name|'check_output'
op|'('
name|'args'
op|')'
op|'.'
name|'strip'
op|'('
op|')'
op|'.'
name|'split'
op|'('
string|"'\\n'"
op|')'
newline|'\n'
name|'refs'
op|'='
op|'{'
op|'}'
newline|'\n'
name|'for'
name|'line'
name|'in'
name|'lines'
op|':'
newline|'\n'
indent|'            '
name|'value'
op|','
name|'name'
op|'='
name|'line'
op|'.'
name|'split'
op|'('
string|"' '"
op|')'
newline|'\n'
name|'name'
op|'='
name|'name'
op|'.'
name|'strip'
op|'('
string|"'commit\\t'"
op|')'
newline|'\n'
name|'refs'
op|'['
name|'name'
op|']'
op|'='
name|'value'
newline|'\n'
dedent|''
name|'return'
name|'refs'
newline|'\n'
nl|'\n'
DECL|member|do_import
dedent|''
name|'def'
name|'do_import'
op|'('
name|'self'
op|','
name|'base'
op|')'
op|':'
newline|'\n'
indent|'        '
string|'"""Imports a fast-import stream to the given directory.\n\n        Simply delegates to git fast-import.\n        """'
newline|'\n'
nl|'\n'
name|'dirname'
op|'='
name|'self'
op|'.'
name|'repo'
op|'.'
name|'get_base_path'
op|'('
name|'base'
op|')'
newline|'\n'
name|'if'
name|'self'
op|'.'
name|'repo'
op|'.'
name|'local'
op|':'
newline|'\n'
indent|'            '
name|'gitdir'
op|'='
name|'self'
op|'.'
name|'repo'
op|'.'
name|'gitpath'
newline|'\n'
dedent|''
name|'else'
op|':'
newline|'\n'
indent|'            '
name|'gitdir'
op|'='
name|'os'
op|'.'
name|'path'
op|'.'
name|'abspath'
op|'('
name|'os'
op|'.'
name|'path'
op|'.'
name|'join'
op|'('
name|'dirname'
op|','
string|"'.git'"
op|')'
op|')'
newline|'\n'
dedent|''
name|'path'
op|'='
name|'os'
op|'.'
name|'path'
op|'.'
name|'abspath'
op|'('
name|'os'
op|'.'
name|'path'
op|'.'
name|'join'
op|'('
name|'dirname'
op|','
string|"'git.marks'"
op|')'
op|')'
newline|'\n'
nl|'\n'
name|'if'
name|'not'
name|'os'
op|'.'
name|'path'
op|'.'
name|'exists'
op|'('
name|'dirname'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'os'
op|'.'
name|'makedirs'
op|'('
name|'dirname'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'refs_before'
op|'='
name|'self'
op|'.'
name|'get_refs'
op|'('
name|'gitdir'
op|')'
newline|'\n'
nl|'\n'
name|'args'
op|'='
op|'['
string|'"git"'
op|','
string|'"--git-dir="'
op|'+'
name|'gitdir'
op|','
string|'"fast-import"'
op|','
string|'"--quiet"'
op|','
string|'"--export-marks="'
op|'+'
name|'path'
op|']'
newline|'\n'
nl|'\n'
name|'if'
name|'os'
op|'.'
name|'path'
op|'.'
name|'exists'
op|'('
name|'path'
op|')'
op|':'
newline|'\n'
indent|'            '
name|'args'
op|'.'
name|'append'
op|'('
string|'"--import-marks="'
op|'+'
name|'path'
op|')'
newline|'\n'
nl|'\n'
dedent|''
name|'check_call'
op|'('
name|'args'
op|')'
newline|'\n'
nl|'\n'
name|'refs_after'
op|'='
name|'self'
op|'.'
name|'get_refs'
op|'('
name|'gitdir'
op|')'
newline|'\n'
nl|'\n'
name|'changed'
op|'='
op|'{'
op|'}'
newline|'\n'
nl|'\n'
name|'for'
name|'name'
op|','
name|'value'
name|'in'
name|'refs_after'
op|'.'
name|'iteritems'
op|'('
op|')'
op|':'
newline|'\n'
indent|'            '
name|'if'
name|'refs_before'
op|'.'
name|'get'
op|'('
name|'name'
op|')'
op|'=='
name|'value'
op|':'
newline|'\n'
indent|'                '
name|'continue'
newline|'\n'
nl|'\n'
dedent|''
name|'changed'
op|'['
name|'name'
op|']'
op|'='
name|'value'
newline|'\n'
nl|'\n'
dedent|''
name|'return'
name|'changed'
newline|'\n'
dedent|''
dedent|''
endmarker|''
end_unit
