begin_unit
begin_comment
comment|/*  * rev-parse.c  *  * Copyright (C) Linus Torvalds, 2005  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_decl_stmt
DECL|variable|def
specifier|static
name|char
modifier|*
name|def
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|no_revs
specifier|static
name|int
name|no_revs
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|single_rev
specifier|static
name|int
name|single_rev
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|revs_only
specifier|static
name|int
name|revs_only
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|do_rev_argument
specifier|static
name|int
name|do_rev_argument
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|output_revs
specifier|static
name|int
name|output_revs
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_function_decl
specifier|static
name|int
name|get_extended_sha1
parameter_list|(
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
function_decl|;
end_function_decl
begin_comment
comment|/*  * Some arguments are relevant "revision" arguments,  * others are about output format or other details.  * This sorts it all out.  */
end_comment
begin_function
DECL|function|is_rev_argument
specifier|static
name|int
name|is_rev_argument
parameter_list|(
specifier|const
name|char
modifier|*
name|arg
parameter_list|)
block|{
specifier|static
specifier|const
name|char
modifier|*
name|rev_args
index|[]
init|=
block|{
literal|"--max-count="
block|,
literal|"--max-age="
block|,
literal|"--min-age="
block|,
literal|"--merge-order"
block|,
name|NULL
block|}
decl_stmt|;
specifier|const
name|char
modifier|*
modifier|*
name|p
init|=
name|rev_args
decl_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
specifier|const
name|char
modifier|*
name|str
init|=
operator|*
name|p
operator|++
decl_stmt|;
name|int
name|len
decl_stmt|;
if|if
condition|(
operator|!
name|str
condition|)
return|return
literal|0
return|;
name|len
operator|=
name|strlen
argument_list|(
name|str
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|arg
argument_list|,
name|str
argument_list|,
name|len
argument_list|)
condition|)
return|return
literal|1
return|;
block|}
block|}
end_function
begin_function
DECL|function|show_rev
specifier|static
name|void
name|show_rev
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
if|if
condition|(
name|no_revs
condition|)
return|return;
name|output_revs
operator|++
expr_stmt|;
name|puts
argument_list|(
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_antirev
specifier|static
name|void
name|show_antirev
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
if|if
condition|(
name|no_revs
condition|)
return|return;
name|output_revs
operator|++
expr_stmt|;
name|printf
argument_list|(
literal|"^%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_rev_arg
specifier|static
name|void
name|show_rev_arg
parameter_list|(
name|char
modifier|*
name|rev
parameter_list|)
block|{
if|if
condition|(
name|no_revs
condition|)
return|return;
name|puts
argument_list|(
name|rev
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_norev
specifier|static
name|void
name|show_norev
parameter_list|(
name|char
modifier|*
name|norev
parameter_list|)
block|{
if|if
condition|(
name|revs_only
condition|)
return|return;
name|puts
argument_list|(
name|norev
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_arg
specifier|static
name|void
name|show_arg
parameter_list|(
name|char
modifier|*
name|arg
parameter_list|)
block|{
if|if
condition|(
name|do_rev_argument
operator|&&
name|is_rev_argument
argument_list|(
name|arg
argument_list|)
condition|)
name|show_rev_arg
argument_list|(
name|arg
argument_list|)
expr_stmt|;
else|else
name|show_norev
argument_list|(
name|arg
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_parent
specifier|static
name|int
name|get_parent
parameter_list|(
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|char
modifier|*
name|result
parameter_list|,
name|int
name|idx
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|ret
init|=
name|get_extended_sha1
argument_list|(
name|name
argument_list|,
name|sha1
argument_list|)
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
if|if
condition|(
name|ret
condition|)
return|return
name|ret
return|;
name|commit
operator|=
name|lookup_commit_reference
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|parse_commit
argument_list|(
name|commit
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|p
operator|=
name|commit
operator|->
name|parents
expr_stmt|;
while|while
condition|(
name|p
condition|)
block|{
if|if
condition|(
operator|!
operator|--
name|idx
condition|)
block|{
name|memcpy
argument_list|(
name|result
argument_list|,
name|p
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|p
operator|=
name|p
operator|->
name|next
expr_stmt|;
block|}
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_comment
comment|/*  * This is like "get_sha1()", except it allows "sha1 expressions",  * notably "xyz^" for "parent of xyz"  */
end_comment
begin_function
DECL|function|get_extended_sha1
specifier|static
name|int
name|get_extended_sha1
parameter_list|(
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|int
name|parent
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|name
argument_list|)
decl_stmt|;
name|parent
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|len
operator|>
literal|2
operator|&&
name|name
index|[
name|len
operator|-
literal|1
index|]
operator|>=
literal|'1'
operator|&&
name|name
index|[
name|len
operator|-
literal|1
index|]
operator|<=
literal|'9'
condition|)
block|{
name|parent
operator|=
name|name
index|[
name|len
operator|-
literal|1
index|]
operator|-
literal|'0'
expr_stmt|;
name|len
operator|--
expr_stmt|;
block|}
if|if
condition|(
name|len
operator|>
literal|1
operator|&&
name|name
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'^'
condition|)
block|{
name|int
name|ret
decl_stmt|;
name|name
index|[
name|len
operator|-
literal|1
index|]
operator|=
literal|0
expr_stmt|;
name|ret
operator|=
name|get_parent
argument_list|(
name|name
argument_list|,
name|sha1
argument_list|,
name|parent
argument_list|)
expr_stmt|;
name|name
index|[
name|len
operator|-
literal|1
index|]
operator|=
literal|'^'
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
return|return
literal|0
return|;
block|}
return|return
name|get_sha1
argument_list|(
name|name
argument_list|,
name|sha1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|show_default
specifier|static
name|void
name|show_default
parameter_list|(
name|void
parameter_list|)
block|{
name|char
modifier|*
name|s
init|=
name|def
decl_stmt|;
if|if
condition|(
name|s
condition|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|def
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
operator|!
name|get_extended_sha1
argument_list|(
name|s
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|show_rev
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
return|return;
block|}
name|show_arg
argument_list|(
name|s
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|as_is
init|=
literal|0
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
name|char
modifier|*
name|dotdot
decl_stmt|;
if|if
condition|(
name|as_is
condition|)
block|{
name|show_norev
argument_list|(
name|arg
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|*
name|arg
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--"
argument_list|)
condition|)
block|{
name|show_default
argument_list|()
expr_stmt|;
if|if
condition|(
name|revs_only
condition|)
break|break;
name|as_is
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--default"
argument_list|)
condition|)
block|{
name|def
operator|=
name|argv
index|[
name|i
operator|+
literal|1
index|]
expr_stmt|;
name|i
operator|++
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--revs-only"
argument_list|)
condition|)
block|{
name|revs_only
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--no-revs"
argument_list|)
condition|)
block|{
name|no_revs
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|arg
argument_list|,
literal|"--verify"
argument_list|)
condition|)
block|{
name|revs_only
operator|=
literal|1
expr_stmt|;
name|do_rev_argument
operator|=
literal|0
expr_stmt|;
name|single_rev
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|show_arg
argument_list|(
name|arg
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|dotdot
operator|=
name|strstr
argument_list|(
name|arg
argument_list|,
literal|".."
argument_list|)
expr_stmt|;
if|if
condition|(
name|dotdot
condition|)
block|{
name|unsigned
name|char
name|end
index|[
literal|20
index|]
decl_stmt|;
name|char
modifier|*
name|n
init|=
name|dotdot
operator|+
literal|2
decl_stmt|;
operator|*
name|dotdot
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|get_extended_sha1
argument_list|(
name|arg
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
operator|*
name|n
condition|)
name|n
operator|=
literal|"HEAD"
expr_stmt|;
if|if
condition|(
operator|!
name|get_extended_sha1
argument_list|(
name|n
argument_list|,
name|end
argument_list|)
condition|)
block|{
if|if
condition|(
name|no_revs
condition|)
continue|continue;
name|def
operator|=
name|NULL
expr_stmt|;
name|show_rev
argument_list|(
name|end
argument_list|)
expr_stmt|;
name|show_antirev
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
continue|continue;
block|}
block|}
operator|*
name|dotdot
operator|=
literal|'.'
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|get_extended_sha1
argument_list|(
name|arg
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
if|if
condition|(
name|no_revs
condition|)
continue|continue;
name|def
operator|=
name|NULL
expr_stmt|;
name|show_rev
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|*
name|arg
operator|==
literal|'^'
operator|&&
operator|!
name|get_extended_sha1
argument_list|(
name|arg
operator|+
literal|1
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
if|if
condition|(
name|no_revs
condition|)
continue|continue;
name|def
operator|=
name|NULL
expr_stmt|;
name|show_antirev
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|show_default
argument_list|()
expr_stmt|;
name|show_norev
argument_list|(
name|arg
argument_list|)
expr_stmt|;
block|}
name|show_default
argument_list|()
expr_stmt|;
if|if
condition|(
name|single_rev
operator|&&
name|output_revs
operator|!=
literal|1
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Needed a single revision\n"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
