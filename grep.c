begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"grep.h"
end_include
begin_include
include|#
directive|include
file|"xdiff-interface.h"
end_include
begin_function
DECL|function|append_grep_pattern
name|void
name|append_grep_pattern
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|pat
parameter_list|,
specifier|const
name|char
modifier|*
name|origin
parameter_list|,
name|int
name|no
parameter_list|,
name|enum
name|grep_pat_token
name|t
parameter_list|)
block|{
name|struct
name|grep_pat
modifier|*
name|p
init|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|p
argument_list|)
argument_list|)
decl_stmt|;
name|p
operator|->
name|pattern
operator|=
name|pat
expr_stmt|;
name|p
operator|->
name|origin
operator|=
name|origin
expr_stmt|;
name|p
operator|->
name|no
operator|=
name|no
expr_stmt|;
name|p
operator|->
name|token
operator|=
name|t
expr_stmt|;
operator|*
name|opt
operator|->
name|pattern_tail
operator|=
name|p
expr_stmt|;
name|opt
operator|->
name|pattern_tail
operator|=
operator|&
name|p
operator|->
name|next
expr_stmt|;
name|p
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
block|}
end_function
begin_function
DECL|function|compile_regexp
specifier|static
name|void
name|compile_regexp
parameter_list|(
name|struct
name|grep_pat
modifier|*
name|p
parameter_list|,
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|)
block|{
name|int
name|err
init|=
name|regcomp
argument_list|(
operator|&
name|p
operator|->
name|regexp
argument_list|,
name|p
operator|->
name|pattern
argument_list|,
name|opt
operator|->
name|regflags
argument_list|)
decl_stmt|;
if|if
condition|(
name|err
condition|)
block|{
name|char
name|errbuf
index|[
literal|1024
index|]
decl_stmt|;
name|char
name|where
index|[
literal|1024
index|]
decl_stmt|;
if|if
condition|(
name|p
operator|->
name|no
condition|)
name|sprintf
argument_list|(
name|where
argument_list|,
literal|"In '%s' at %d, "
argument_list|,
name|p
operator|->
name|origin
argument_list|,
name|p
operator|->
name|no
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|p
operator|->
name|origin
condition|)
name|sprintf
argument_list|(
name|where
argument_list|,
literal|"%s, "
argument_list|,
name|p
operator|->
name|origin
argument_list|)
expr_stmt|;
else|else
name|where
index|[
literal|0
index|]
operator|=
literal|0
expr_stmt|;
name|regerror
argument_list|(
name|err
argument_list|,
operator|&
name|p
operator|->
name|regexp
argument_list|,
name|errbuf
argument_list|,
literal|1024
argument_list|)
expr_stmt|;
name|regfree
argument_list|(
operator|&
name|p
operator|->
name|regexp
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"%s'%s': %s"
argument_list|,
name|where
argument_list|,
name|p
operator|->
name|pattern
argument_list|,
name|errbuf
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function_decl
specifier|static
name|struct
name|grep_expr
modifier|*
name|compile_pattern_or
parameter_list|(
name|struct
name|grep_pat
modifier|*
modifier|*
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|compile_pattern_atom
specifier|static
name|struct
name|grep_expr
modifier|*
name|compile_pattern_atom
parameter_list|(
name|struct
name|grep_pat
modifier|*
modifier|*
name|list
parameter_list|)
block|{
name|struct
name|grep_pat
modifier|*
name|p
decl_stmt|;
name|struct
name|grep_expr
modifier|*
name|x
decl_stmt|;
name|p
operator|=
operator|*
name|list
expr_stmt|;
switch|switch
condition|(
name|p
operator|->
name|token
condition|)
block|{
case|case
name|GREP_PATTERN
case|:
comment|/* atom */
case|case
name|GREP_PATTERN_HEAD
case|:
case|case
name|GREP_PATTERN_BODY
case|:
name|x
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|grep_expr
argument_list|)
argument_list|)
expr_stmt|;
name|x
operator|->
name|node
operator|=
name|GREP_NODE_ATOM
expr_stmt|;
name|x
operator|->
name|u
operator|.
name|atom
operator|=
name|p
expr_stmt|;
operator|*
name|list
operator|=
name|p
operator|->
name|next
expr_stmt|;
return|return
name|x
return|;
case|case
name|GREP_OPEN_PAREN
case|:
operator|*
name|list
operator|=
name|p
operator|->
name|next
expr_stmt|;
name|x
operator|=
name|compile_pattern_or
argument_list|(
name|list
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|x
condition|)
return|return
name|NULL
return|;
if|if
condition|(
operator|!
operator|*
name|list
operator|||
operator|(
operator|*
name|list
operator|)
operator|->
name|token
operator|!=
name|GREP_CLOSE_PAREN
condition|)
name|die
argument_list|(
literal|"unmatched parenthesis"
argument_list|)
expr_stmt|;
operator|*
name|list
operator|=
operator|(
operator|*
name|list
operator|)
operator|->
name|next
expr_stmt|;
return|return
name|x
return|;
default|default:
return|return
name|NULL
return|;
block|}
block|}
end_function
begin_function
DECL|function|compile_pattern_not
specifier|static
name|struct
name|grep_expr
modifier|*
name|compile_pattern_not
parameter_list|(
name|struct
name|grep_pat
modifier|*
modifier|*
name|list
parameter_list|)
block|{
name|struct
name|grep_pat
modifier|*
name|p
decl_stmt|;
name|struct
name|grep_expr
modifier|*
name|x
decl_stmt|;
name|p
operator|=
operator|*
name|list
expr_stmt|;
switch|switch
condition|(
name|p
operator|->
name|token
condition|)
block|{
case|case
name|GREP_NOT
case|:
if|if
condition|(
operator|!
name|p
operator|->
name|next
condition|)
name|die
argument_list|(
literal|"--not not followed by pattern expression"
argument_list|)
expr_stmt|;
operator|*
name|list
operator|=
name|p
operator|->
name|next
expr_stmt|;
name|x
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|grep_expr
argument_list|)
argument_list|)
expr_stmt|;
name|x
operator|->
name|node
operator|=
name|GREP_NODE_NOT
expr_stmt|;
name|x
operator|->
name|u
operator|.
name|unary
operator|=
name|compile_pattern_not
argument_list|(
name|list
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|x
operator|->
name|u
operator|.
name|unary
condition|)
name|die
argument_list|(
literal|"--not followed by non pattern expression"
argument_list|)
expr_stmt|;
return|return
name|x
return|;
default|default:
return|return
name|compile_pattern_atom
argument_list|(
name|list
argument_list|)
return|;
block|}
block|}
end_function
begin_function
DECL|function|compile_pattern_and
specifier|static
name|struct
name|grep_expr
modifier|*
name|compile_pattern_and
parameter_list|(
name|struct
name|grep_pat
modifier|*
modifier|*
name|list
parameter_list|)
block|{
name|struct
name|grep_pat
modifier|*
name|p
decl_stmt|;
name|struct
name|grep_expr
modifier|*
name|x
decl_stmt|,
modifier|*
name|y
decl_stmt|,
modifier|*
name|z
decl_stmt|;
name|x
operator|=
name|compile_pattern_not
argument_list|(
name|list
argument_list|)
expr_stmt|;
name|p
operator|=
operator|*
name|list
expr_stmt|;
if|if
condition|(
name|p
operator|&&
name|p
operator|->
name|token
operator|==
name|GREP_AND
condition|)
block|{
if|if
condition|(
operator|!
name|p
operator|->
name|next
condition|)
name|die
argument_list|(
literal|"--and not followed by pattern expression"
argument_list|)
expr_stmt|;
operator|*
name|list
operator|=
name|p
operator|->
name|next
expr_stmt|;
name|y
operator|=
name|compile_pattern_and
argument_list|(
name|list
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|y
condition|)
name|die
argument_list|(
literal|"--and not followed by pattern expression"
argument_list|)
expr_stmt|;
name|z
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|grep_expr
argument_list|)
argument_list|)
expr_stmt|;
name|z
operator|->
name|node
operator|=
name|GREP_NODE_AND
expr_stmt|;
name|z
operator|->
name|u
operator|.
name|binary
operator|.
name|left
operator|=
name|x
expr_stmt|;
name|z
operator|->
name|u
operator|.
name|binary
operator|.
name|right
operator|=
name|y
expr_stmt|;
return|return
name|z
return|;
block|}
return|return
name|x
return|;
block|}
end_function
begin_function
DECL|function|compile_pattern_or
specifier|static
name|struct
name|grep_expr
modifier|*
name|compile_pattern_or
parameter_list|(
name|struct
name|grep_pat
modifier|*
modifier|*
name|list
parameter_list|)
block|{
name|struct
name|grep_pat
modifier|*
name|p
decl_stmt|;
name|struct
name|grep_expr
modifier|*
name|x
decl_stmt|,
modifier|*
name|y
decl_stmt|,
modifier|*
name|z
decl_stmt|;
name|x
operator|=
name|compile_pattern_and
argument_list|(
name|list
argument_list|)
expr_stmt|;
name|p
operator|=
operator|*
name|list
expr_stmt|;
if|if
condition|(
name|x
operator|&&
name|p
operator|&&
name|p
operator|->
name|token
operator|!=
name|GREP_CLOSE_PAREN
condition|)
block|{
name|y
operator|=
name|compile_pattern_or
argument_list|(
name|list
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|y
condition|)
name|die
argument_list|(
literal|"not a pattern expression %s"
argument_list|,
name|p
operator|->
name|pattern
argument_list|)
expr_stmt|;
name|z
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|grep_expr
argument_list|)
argument_list|)
expr_stmt|;
name|z
operator|->
name|node
operator|=
name|GREP_NODE_OR
expr_stmt|;
name|z
operator|->
name|u
operator|.
name|binary
operator|.
name|left
operator|=
name|x
expr_stmt|;
name|z
operator|->
name|u
operator|.
name|binary
operator|.
name|right
operator|=
name|y
expr_stmt|;
return|return
name|z
return|;
block|}
return|return
name|x
return|;
block|}
end_function
begin_function
DECL|function|compile_pattern_expr
specifier|static
name|struct
name|grep_expr
modifier|*
name|compile_pattern_expr
parameter_list|(
name|struct
name|grep_pat
modifier|*
modifier|*
name|list
parameter_list|)
block|{
return|return
name|compile_pattern_or
argument_list|(
name|list
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|compile_grep_patterns
name|void
name|compile_grep_patterns
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|)
block|{
name|struct
name|grep_pat
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|opt
operator|->
name|all_match
condition|)
name|opt
operator|->
name|extended
operator|=
literal|1
expr_stmt|;
for|for
control|(
name|p
operator|=
name|opt
operator|->
name|pattern_list
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
switch|switch
condition|(
name|p
operator|->
name|token
condition|)
block|{
case|case
name|GREP_PATTERN
case|:
comment|/* atom */
case|case
name|GREP_PATTERN_HEAD
case|:
case|case
name|GREP_PATTERN_BODY
case|:
if|if
condition|(
operator|!
name|opt
operator|->
name|fixed
condition|)
name|compile_regexp
argument_list|(
name|p
argument_list|,
name|opt
argument_list|)
expr_stmt|;
break|break;
default|default:
name|opt
operator|->
name|extended
operator|=
literal|1
expr_stmt|;
break|break;
block|}
block|}
if|if
condition|(
operator|!
name|opt
operator|->
name|extended
condition|)
return|return;
comment|/* Then bundle them up in an expression. 	 * A classic recursive descent parser would do. 	 */
name|p
operator|=
name|opt
operator|->
name|pattern_list
expr_stmt|;
name|opt
operator|->
name|pattern_expression
operator|=
name|compile_pattern_expr
argument_list|(
operator|&
name|p
argument_list|)
expr_stmt|;
if|if
condition|(
name|p
condition|)
name|die
argument_list|(
literal|"incomplete pattern expression: %s"
argument_list|,
name|p
operator|->
name|pattern
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|free_pattern_expr
specifier|static
name|void
name|free_pattern_expr
parameter_list|(
name|struct
name|grep_expr
modifier|*
name|x
parameter_list|)
block|{
switch|switch
condition|(
name|x
operator|->
name|node
condition|)
block|{
case|case
name|GREP_NODE_ATOM
case|:
break|break;
case|case
name|GREP_NODE_NOT
case|:
name|free_pattern_expr
argument_list|(
name|x
operator|->
name|u
operator|.
name|unary
argument_list|)
expr_stmt|;
break|break;
case|case
name|GREP_NODE_AND
case|:
case|case
name|GREP_NODE_OR
case|:
name|free_pattern_expr
argument_list|(
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|left
argument_list|)
expr_stmt|;
name|free_pattern_expr
argument_list|(
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|right
argument_list|)
expr_stmt|;
break|break;
block|}
name|free
argument_list|(
name|x
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|free_grep_patterns
name|void
name|free_grep_patterns
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|)
block|{
name|struct
name|grep_pat
modifier|*
name|p
decl_stmt|,
modifier|*
name|n
decl_stmt|;
for|for
control|(
name|p
operator|=
name|opt
operator|->
name|pattern_list
init|;
name|p
condition|;
name|p
operator|=
name|n
control|)
block|{
name|n
operator|=
name|p
operator|->
name|next
expr_stmt|;
switch|switch
condition|(
name|p
operator|->
name|token
condition|)
block|{
case|case
name|GREP_PATTERN
case|:
comment|/* atom */
case|case
name|GREP_PATTERN_HEAD
case|:
case|case
name|GREP_PATTERN_BODY
case|:
name|regfree
argument_list|(
operator|&
name|p
operator|->
name|regexp
argument_list|)
expr_stmt|;
break|break;
default|default:
break|break;
block|}
name|free
argument_list|(
name|p
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|opt
operator|->
name|extended
condition|)
return|return;
name|free_pattern_expr
argument_list|(
name|opt
operator|->
name|pattern_expression
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|end_of_line
specifier|static
name|char
modifier|*
name|end_of_line
parameter_list|(
name|char
modifier|*
name|cp
parameter_list|,
name|unsigned
name|long
modifier|*
name|left
parameter_list|)
block|{
name|unsigned
name|long
name|l
init|=
operator|*
name|left
decl_stmt|;
while|while
condition|(
name|l
operator|&&
operator|*
name|cp
operator|!=
literal|'\n'
condition|)
block|{
name|l
operator|--
expr_stmt|;
name|cp
operator|++
expr_stmt|;
block|}
operator|*
name|left
operator|=
name|l
expr_stmt|;
return|return
name|cp
return|;
block|}
end_function
begin_function
DECL|function|word_char
specifier|static
name|int
name|word_char
parameter_list|(
name|char
name|ch
parameter_list|)
block|{
return|return
name|isalnum
argument_list|(
name|ch
argument_list|)
operator|||
name|ch
operator|==
literal|'_'
return|;
block|}
end_function
begin_function
DECL|function|show_line
specifier|static
name|void
name|show_line
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|bol
parameter_list|,
specifier|const
name|char
modifier|*
name|eol
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|lno
parameter_list|,
name|char
name|sign
parameter_list|)
block|{
if|if
condition|(
name|opt
operator|->
name|pathname
condition|)
name|printf
argument_list|(
literal|"%s%c"
argument_list|,
name|name
argument_list|,
name|sign
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|linenum
condition|)
name|printf
argument_list|(
literal|"%d%c"
argument_list|,
name|lno
argument_list|,
name|sign
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%.*s\n"
argument_list|,
call|(
name|int
call|)
argument_list|(
name|eol
operator|-
name|bol
argument_list|)
argument_list|,
name|bol
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fixmatch
specifier|static
name|int
name|fixmatch
parameter_list|(
specifier|const
name|char
modifier|*
name|pattern
parameter_list|,
name|char
modifier|*
name|line
parameter_list|,
name|regmatch_t
modifier|*
name|match
parameter_list|)
block|{
name|char
modifier|*
name|hit
init|=
name|strstr
argument_list|(
name|line
argument_list|,
name|pattern
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|hit
condition|)
block|{
name|match
operator|->
name|rm_so
operator|=
name|match
operator|->
name|rm_eo
operator|=
operator|-
literal|1
expr_stmt|;
return|return
name|REG_NOMATCH
return|;
block|}
else|else
block|{
name|match
operator|->
name|rm_so
operator|=
name|hit
operator|-
name|line
expr_stmt|;
name|match
operator|->
name|rm_eo
operator|=
name|match
operator|->
name|rm_so
operator|+
name|strlen
argument_list|(
name|pattern
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
block|}
end_function
begin_function
DECL|function|match_one_pattern
specifier|static
name|int
name|match_one_pattern
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
name|struct
name|grep_pat
modifier|*
name|p
parameter_list|,
name|char
modifier|*
name|bol
parameter_list|,
name|char
modifier|*
name|eol
parameter_list|,
name|enum
name|grep_context
name|ctx
parameter_list|)
block|{
name|int
name|hit
init|=
literal|0
decl_stmt|;
name|int
name|at_true_bol
init|=
literal|1
decl_stmt|;
name|regmatch_t
name|pmatch
index|[
literal|10
index|]
decl_stmt|;
if|if
condition|(
operator|(
name|p
operator|->
name|token
operator|!=
name|GREP_PATTERN
operator|)
operator|&&
operator|(
operator|(
name|p
operator|->
name|token
operator|==
name|GREP_PATTERN_HEAD
operator|)
operator|!=
operator|(
name|ctx
operator|==
name|GREP_CONTEXT_HEAD
operator|)
operator|)
condition|)
return|return
literal|0
return|;
name|again
label|:
if|if
condition|(
operator|!
name|opt
operator|->
name|fixed
condition|)
block|{
name|regex_t
modifier|*
name|exp
init|=
operator|&
name|p
operator|->
name|regexp
decl_stmt|;
name|hit
operator|=
operator|!
name|regexec
argument_list|(
name|exp
argument_list|,
name|bol
argument_list|,
name|ARRAY_SIZE
argument_list|(
name|pmatch
argument_list|)
argument_list|,
name|pmatch
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|hit
operator|=
operator|!
name|fixmatch
argument_list|(
name|p
operator|->
name|pattern
argument_list|,
name|bol
argument_list|,
name|pmatch
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|hit
operator|&&
name|opt
operator|->
name|word_regexp
condition|)
block|{
if|if
condition|(
operator|(
name|pmatch
index|[
literal|0
index|]
operator|.
name|rm_so
operator|<
literal|0
operator|)
operator|||
operator|(
name|eol
operator|-
name|bol
operator|)
operator|<=
name|pmatch
index|[
literal|0
index|]
operator|.
name|rm_so
operator|||
operator|(
name|pmatch
index|[
literal|0
index|]
operator|.
name|rm_eo
operator|<
literal|0
operator|)
operator|||
operator|(
name|eol
operator|-
name|bol
operator|)
operator|<
name|pmatch
index|[
literal|0
index|]
operator|.
name|rm_eo
condition|)
name|die
argument_list|(
literal|"regexp returned nonsense"
argument_list|)
expr_stmt|;
comment|/* Match beginning must be either beginning of the 		 * line, or at word boundary (i.e. the last char must 		 * not be a word char).  Similarly, match end must be 		 * either end of the line, or at word boundary 		 * (i.e. the next char must not be a word char). 		 */
if|if
condition|(
operator|(
operator|(
name|pmatch
index|[
literal|0
index|]
operator|.
name|rm_so
operator|==
literal|0
operator|&&
name|at_true_bol
operator|)
operator|||
operator|!
name|word_char
argument_list|(
name|bol
index|[
name|pmatch
index|[
literal|0
index|]
operator|.
name|rm_so
operator|-
literal|1
index|]
argument_list|)
operator|)
operator|&&
operator|(
operator|(
name|pmatch
index|[
literal|0
index|]
operator|.
name|rm_eo
operator|==
operator|(
name|eol
operator|-
name|bol
operator|)
operator|)
operator|||
operator|!
name|word_char
argument_list|(
name|bol
index|[
name|pmatch
index|[
literal|0
index|]
operator|.
name|rm_eo
index|]
argument_list|)
operator|)
condition|)
empty_stmt|;
else|else
name|hit
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|hit
operator|&&
name|pmatch
index|[
literal|0
index|]
operator|.
name|rm_so
operator|+
name|bol
operator|+
literal|1
operator|<
name|eol
condition|)
block|{
comment|/* There could be more than one match on the 			 * line, and the first match might not be 			 * strict word match.  But later ones could be! 			 */
name|bol
operator|=
name|pmatch
index|[
literal|0
index|]
operator|.
name|rm_so
operator|+
name|bol
operator|+
literal|1
expr_stmt|;
name|at_true_bol
operator|=
literal|0
expr_stmt|;
goto|goto
name|again
goto|;
block|}
block|}
return|return
name|hit
return|;
block|}
end_function
begin_function
DECL|function|match_expr_eval
specifier|static
name|int
name|match_expr_eval
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|o
parameter_list|,
name|struct
name|grep_expr
modifier|*
name|x
parameter_list|,
name|char
modifier|*
name|bol
parameter_list|,
name|char
modifier|*
name|eol
parameter_list|,
name|enum
name|grep_context
name|ctx
parameter_list|,
name|int
name|collect_hits
parameter_list|)
block|{
name|int
name|h
init|=
literal|0
decl_stmt|;
switch|switch
condition|(
name|x
operator|->
name|node
condition|)
block|{
case|case
name|GREP_NODE_ATOM
case|:
name|h
operator|=
name|match_one_pattern
argument_list|(
name|o
argument_list|,
name|x
operator|->
name|u
operator|.
name|atom
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|)
expr_stmt|;
break|break;
case|case
name|GREP_NODE_NOT
case|:
name|h
operator|=
operator|!
name|match_expr_eval
argument_list|(
name|o
argument_list|,
name|x
operator|->
name|u
operator|.
name|unary
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|,
literal|0
argument_list|)
expr_stmt|;
break|break;
case|case
name|GREP_NODE_AND
case|:
if|if
condition|(
operator|!
name|collect_hits
condition|)
return|return
operator|(
name|match_expr_eval
argument_list|(
name|o
argument_list|,
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|left
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|,
literal|0
argument_list|)
operator|&&
name|match_expr_eval
argument_list|(
name|o
argument_list|,
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|right
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|,
literal|0
argument_list|)
operator|)
return|;
name|h
operator|=
name|match_expr_eval
argument_list|(
name|o
argument_list|,
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|left
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|h
operator|&=
name|match_expr_eval
argument_list|(
name|o
argument_list|,
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|right
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|,
literal|0
argument_list|)
expr_stmt|;
break|break;
case|case
name|GREP_NODE_OR
case|:
if|if
condition|(
operator|!
name|collect_hits
condition|)
return|return
operator|(
name|match_expr_eval
argument_list|(
name|o
argument_list|,
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|left
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|,
literal|0
argument_list|)
operator|||
name|match_expr_eval
argument_list|(
name|o
argument_list|,
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|right
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|,
literal|0
argument_list|)
operator|)
return|;
name|h
operator|=
name|match_expr_eval
argument_list|(
name|o
argument_list|,
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|left
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|left
operator|->
name|hit
operator||=
name|h
expr_stmt|;
name|h
operator||=
name|match_expr_eval
argument_list|(
name|o
argument_list|,
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|right
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|,
literal|1
argument_list|)
expr_stmt|;
break|break;
default|default:
name|die
argument_list|(
literal|"Unexpected node type (internal error) %d\n"
argument_list|,
name|x
operator|->
name|node
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|collect_hits
condition|)
name|x
operator|->
name|hit
operator||=
name|h
expr_stmt|;
return|return
name|h
return|;
block|}
end_function
begin_function
DECL|function|match_expr
specifier|static
name|int
name|match_expr
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
name|char
modifier|*
name|bol
parameter_list|,
name|char
modifier|*
name|eol
parameter_list|,
name|enum
name|grep_context
name|ctx
parameter_list|,
name|int
name|collect_hits
parameter_list|)
block|{
name|struct
name|grep_expr
modifier|*
name|x
init|=
name|opt
operator|->
name|pattern_expression
decl_stmt|;
return|return
name|match_expr_eval
argument_list|(
name|opt
argument_list|,
name|x
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|,
name|collect_hits
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|match_line
specifier|static
name|int
name|match_line
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
name|char
modifier|*
name|bol
parameter_list|,
name|char
modifier|*
name|eol
parameter_list|,
name|enum
name|grep_context
name|ctx
parameter_list|,
name|int
name|collect_hits
parameter_list|)
block|{
name|struct
name|grep_pat
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|opt
operator|->
name|extended
condition|)
return|return
name|match_expr
argument_list|(
name|opt
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|,
name|collect_hits
argument_list|)
return|;
comment|/* we do not call with collect_hits without being extended */
for|for
control|(
name|p
operator|=
name|opt
operator|->
name|pattern_list
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
if|if
condition|(
name|match_one_pattern
argument_list|(
name|opt
argument_list|,
name|p
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|)
condition|)
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|grep_buffer_1
specifier|static
name|int
name|grep_buffer_1
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
name|int
name|collect_hits
parameter_list|)
block|{
name|char
modifier|*
name|bol
init|=
name|buf
decl_stmt|;
name|unsigned
name|long
name|left
init|=
name|size
decl_stmt|;
name|unsigned
name|lno
init|=
literal|1
decl_stmt|;
struct|struct
name|pre_context_line
block|{
name|char
modifier|*
name|bol
decl_stmt|;
name|char
modifier|*
name|eol
decl_stmt|;
block|}
modifier|*
name|prev
init|=
name|NULL
struct|,
modifier|*
name|pcl
struct|;
name|unsigned
name|last_hit
init|=
literal|0
decl_stmt|;
name|unsigned
name|last_shown
init|=
literal|0
decl_stmt|;
name|int
name|binary_match_only
init|=
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|hunk_mark
init|=
literal|""
decl_stmt|;
name|unsigned
name|count
init|=
literal|0
decl_stmt|;
name|enum
name|grep_context
name|ctx
init|=
name|GREP_CONTEXT_HEAD
decl_stmt|;
if|if
condition|(
name|buffer_is_binary
argument_list|(
name|buf
argument_list|,
name|size
argument_list|)
condition|)
block|{
switch|switch
condition|(
name|opt
operator|->
name|binary
condition|)
block|{
case|case
name|GREP_BINARY_DEFAULT
case|:
name|binary_match_only
operator|=
literal|1
expr_stmt|;
break|break;
case|case
name|GREP_BINARY_NOMATCH
case|:
return|return
literal|0
return|;
comment|/* Assume unmatch */
break|break;
default|default:
break|break;
block|}
block|}
if|if
condition|(
name|opt
operator|->
name|pre_context
condition|)
name|prev
operator|=
name|xcalloc
argument_list|(
name|opt
operator|->
name|pre_context
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|prev
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|pre_context
operator|||
name|opt
operator|->
name|post_context
condition|)
name|hunk_mark
operator|=
literal|"--\n"
expr_stmt|;
while|while
condition|(
name|left
condition|)
block|{
name|char
modifier|*
name|eol
decl_stmt|,
name|ch
decl_stmt|;
name|int
name|hit
decl_stmt|;
name|eol
operator|=
name|end_of_line
argument_list|(
name|bol
argument_list|,
operator|&
name|left
argument_list|)
expr_stmt|;
name|ch
operator|=
operator|*
name|eol
expr_stmt|;
operator|*
name|eol
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|(
name|ctx
operator|==
name|GREP_CONTEXT_HEAD
operator|)
operator|&&
operator|(
name|eol
operator|==
name|bol
operator|)
condition|)
name|ctx
operator|=
name|GREP_CONTEXT_BODY
expr_stmt|;
name|hit
operator|=
name|match_line
argument_list|(
name|opt
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|ctx
argument_list|,
name|collect_hits
argument_list|)
expr_stmt|;
operator|*
name|eol
operator|=
name|ch
expr_stmt|;
if|if
condition|(
name|collect_hits
condition|)
goto|goto
name|next_line
goto|;
comment|/* "grep -v -e foo -e bla" should list lines 		 * that do not have either, so inversion should 		 * be done outside. 		 */
if|if
condition|(
name|opt
operator|->
name|invert
condition|)
name|hit
operator|=
operator|!
name|hit
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|unmatch_name_only
condition|)
block|{
if|if
condition|(
name|hit
condition|)
return|return
literal|0
return|;
goto|goto
name|next_line
goto|;
block|}
if|if
condition|(
name|hit
condition|)
block|{
name|count
operator|++
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|status_only
condition|)
return|return
literal|1
return|;
if|if
condition|(
name|binary_match_only
condition|)
block|{
name|printf
argument_list|(
literal|"Binary file %s matches\n"
argument_list|,
name|name
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
if|if
condition|(
name|opt
operator|->
name|name_only
condition|)
block|{
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|name
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
comment|/* Hit at this line.  If we haven't shown the 			 * pre-context lines, we would need to show them. 			 * When asked to do "count", this still show 			 * the context which is nonsense, but the user 			 * deserves to get that ;-). 			 */
if|if
condition|(
name|opt
operator|->
name|pre_context
condition|)
block|{
name|unsigned
name|from
decl_stmt|;
if|if
condition|(
name|opt
operator|->
name|pre_context
operator|<
name|lno
condition|)
name|from
operator|=
name|lno
operator|-
name|opt
operator|->
name|pre_context
expr_stmt|;
else|else
name|from
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|from
operator|<=
name|last_shown
condition|)
name|from
operator|=
name|last_shown
operator|+
literal|1
expr_stmt|;
if|if
condition|(
name|last_shown
operator|&&
name|from
operator|!=
name|last_shown
operator|+
literal|1
condition|)
name|printf
argument_list|(
name|hunk_mark
argument_list|)
expr_stmt|;
while|while
condition|(
name|from
operator|<
name|lno
condition|)
block|{
name|pcl
operator|=
operator|&
name|prev
index|[
name|lno
operator|-
name|from
operator|-
literal|1
index|]
expr_stmt|;
name|show_line
argument_list|(
name|opt
argument_list|,
name|pcl
operator|->
name|bol
argument_list|,
name|pcl
operator|->
name|eol
argument_list|,
name|name
argument_list|,
name|from
argument_list|,
literal|'-'
argument_list|)
expr_stmt|;
name|from
operator|++
expr_stmt|;
block|}
name|last_shown
operator|=
name|lno
operator|-
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|last_shown
operator|&&
name|lno
operator|!=
name|last_shown
operator|+
literal|1
condition|)
name|printf
argument_list|(
name|hunk_mark
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|opt
operator|->
name|count
condition|)
name|show_line
argument_list|(
name|opt
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|name
argument_list|,
name|lno
argument_list|,
literal|':'
argument_list|)
expr_stmt|;
name|last_shown
operator|=
name|last_hit
operator|=
name|lno
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|last_hit
operator|&&
name|lno
operator|<=
name|last_hit
operator|+
name|opt
operator|->
name|post_context
condition|)
block|{
comment|/* If the last hit is within the post context, 			 * we need to show this line. 			 */
if|if
condition|(
name|last_shown
operator|&&
name|lno
operator|!=
name|last_shown
operator|+
literal|1
condition|)
name|printf
argument_list|(
name|hunk_mark
argument_list|)
expr_stmt|;
name|show_line
argument_list|(
name|opt
argument_list|,
name|bol
argument_list|,
name|eol
argument_list|,
name|name
argument_list|,
name|lno
argument_list|,
literal|'-'
argument_list|)
expr_stmt|;
name|last_shown
operator|=
name|lno
expr_stmt|;
block|}
if|if
condition|(
name|opt
operator|->
name|pre_context
condition|)
block|{
name|memmove
argument_list|(
name|prev
operator|+
literal|1
argument_list|,
name|prev
argument_list|,
operator|(
name|opt
operator|->
name|pre_context
operator|-
literal|1
operator|)
operator|*
sizeof|sizeof
argument_list|(
operator|*
name|prev
argument_list|)
argument_list|)
expr_stmt|;
name|prev
operator|->
name|bol
operator|=
name|bol
expr_stmt|;
name|prev
operator|->
name|eol
operator|=
name|eol
expr_stmt|;
block|}
name|next_line
label|:
name|bol
operator|=
name|eol
operator|+
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|left
condition|)
break|break;
name|left
operator|--
expr_stmt|;
name|lno
operator|++
expr_stmt|;
block|}
name|free
argument_list|(
name|prev
argument_list|)
expr_stmt|;
if|if
condition|(
name|collect_hits
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|opt
operator|->
name|status_only
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|opt
operator|->
name|unmatch_name_only
condition|)
block|{
comment|/* We did not see any hit, so we want to show this */
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|name
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
comment|/* NEEDSWORK: 	 * The real "grep -c foo *.c" gives many "bar.c:0" lines, 	 * which feels mostly useless but sometimes useful.  Maybe 	 * make it another option?  For now suppress them. 	 */
if|if
condition|(
name|opt
operator|->
name|count
operator|&&
name|count
condition|)
name|printf
argument_list|(
literal|"%s:%u\n"
argument_list|,
name|name
argument_list|,
name|count
argument_list|)
expr_stmt|;
return|return
operator|!
operator|!
name|last_hit
return|;
block|}
end_function
begin_function
DECL|function|clr_hit_marker
specifier|static
name|void
name|clr_hit_marker
parameter_list|(
name|struct
name|grep_expr
modifier|*
name|x
parameter_list|)
block|{
comment|/* All-hit markers are meaningful only at the very top level 	 * OR node. 	 */
while|while
condition|(
literal|1
condition|)
block|{
name|x
operator|->
name|hit
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|x
operator|->
name|node
operator|!=
name|GREP_NODE_OR
condition|)
return|return;
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|left
operator|->
name|hit
operator|=
literal|0
expr_stmt|;
name|x
operator|=
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|right
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|chk_hit_marker
specifier|static
name|int
name|chk_hit_marker
parameter_list|(
name|struct
name|grep_expr
modifier|*
name|x
parameter_list|)
block|{
comment|/* Top level nodes have hit markers.  See if they all are hits */
while|while
condition|(
literal|1
condition|)
block|{
if|if
condition|(
name|x
operator|->
name|node
operator|!=
name|GREP_NODE_OR
condition|)
return|return
name|x
operator|->
name|hit
return|;
if|if
condition|(
operator|!
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|left
operator|->
name|hit
condition|)
return|return
literal|0
return|;
name|x
operator|=
name|x
operator|->
name|u
operator|.
name|binary
operator|.
name|right
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|grep_buffer
name|int
name|grep_buffer
parameter_list|(
name|struct
name|grep_opt
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|char
modifier|*
name|buf
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
comment|/* 	 * we do not have to do the two-pass grep when we do not check 	 * buffer-wide "all-match". 	 */
if|if
condition|(
operator|!
name|opt
operator|->
name|all_match
condition|)
return|return
name|grep_buffer_1
argument_list|(
name|opt
argument_list|,
name|name
argument_list|,
name|buf
argument_list|,
name|size
argument_list|,
literal|0
argument_list|)
return|;
comment|/* Otherwise the toplevel "or" terms hit a bit differently. 	 * We first clear hit markers from them. 	 */
name|clr_hit_marker
argument_list|(
name|opt
operator|->
name|pattern_expression
argument_list|)
expr_stmt|;
name|grep_buffer_1
argument_list|(
name|opt
argument_list|,
name|name
argument_list|,
name|buf
argument_list|,
name|size
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|chk_hit_marker
argument_list|(
name|opt
operator|->
name|pattern_expression
argument_list|)
condition|)
return|return
literal|0
return|;
return|return
name|grep_buffer_1
argument_list|(
name|opt
argument_list|,
name|name
argument_list|,
name|buf
argument_list|,
name|size
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
end_unit
